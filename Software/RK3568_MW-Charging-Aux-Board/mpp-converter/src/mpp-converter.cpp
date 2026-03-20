#include "mpp-converter.h"

MppConverter::MppConverter(USBHikvisioner* camera):
uhv_(camera) 
{
    bool ret = true;

        // 1. 初始化 ZLM 的推流频道 (适配 ZLM 最新版 API)
        // mediakit::MediaTuple tuple;
        // tuple.vhost = "__defaultVhost__"; 
        // tuple.app = "live";               
        // tuple.stream = "test_720p";       


        // zlmChannel720p_ = std::make_shared<mediakit::DevChannel>(tuple, 0.0f);
        // auto track = std::make_shared<mediakit::H265Track>();
        
        // // 把轨道注册进频道
        // zlmChannel720p_->addTrack(track);
        // // 【关键】：通知频道轨道添加完毕，频道才会开始对外注册流！！！
        // zlmChannel720p_->addTrackCompleted();

        // ==========================================================
        // 1. 初始化 720p 推流频道 (强行派发到 ZLM 后台线程，保证绝对安全)
        // ==========================================================
        poller720p_ = toolkit::EventPollerPool::Instance().getPoller();
        poller720p_->sync([this]() {
            mediakit::MediaTuple tuple;
            tuple.vhost = "__defaultVhost__"; 
            tuple.app = "live";               
            tuple.stream = "test_720p";       

            this->zlmChannel720p_ = std::make_shared<mediakit::DevChannel>(tuple, 0.0f);
            this->zlmChannel720p_->addTrack(std::make_shared<mediakit::H265Track>());
            this->zlmChannel720p_->addTrackCompleted();
        });

        // ==========================================================
        // 2. 初始化 1080p 录制频道 (同样派发到后台线程)
        // 注意：这里【不再】直接调用 setupRecord，而是等用户手动调用 start_record()
        // ==========================================================
        poller1080p_ = toolkit::EventPollerPool::Instance().getPoller();
        poller1080p_->sync([this]() {
            mediakit::MediaTuple tuple1080;
            tuple1080.vhost = "__defaultVhost__"; 
            tuple1080.app = "record";             
            tuple1080.stream = "test_1080p";       

            this->zlmChannel1080p_ = std::make_shared<mediakit::DevChannel>(tuple1080, 0.0f);
            this->zlmChannel1080p_->addTrack(std::make_shared<mediakit::H265Track>());
            this->zlmChannel1080p_->addTrackCompleted();
        });

        auto on_h265_encoded = [this](int streamId, const uint8_t* h265Data, size_t dataSize) {
            uint32_t currentStamp = toolkit::getCurrentMillisecond();

            if (streamId == 0) {
                // 【1080p MP4 录制打帧】和 720p 是一模一样的零拷贝方式！
                if (this->zlmChannel1080p_) {
                    auto frame = std::make_shared<mediakit::H265FrameNoCacheAble>(
                        (char*)h265Data, dataSize, currentStamp, currentStamp, 4 
                    );
                    this->zlmChannel1080p_->inputFrame(frame);
                }
            } 
            else if (streamId == 1) {
                // std::cout << "[720p] Got H.265 frame, size: " << dataSize << " bytes\n";
                
                // 【关键】：这里不要直接塞给 Track，而是塞给 Channel！
                // Channel 会自动解析 H265 NALU，提取 SPS/PPS 并分发给 Track
                if (this->zlmChannel720p_) {
                    auto frame = std::make_shared<mediakit::H265FrameNoCacheAble>(
                        (char*)h265Data, 
                        dataSize, 
                        currentStamp, 
                        currentStamp, 
                        4 
                    );
                    // 把帧塞给推流频道，频道会自动激活直播流！
                    this->zlmChannel720p_->inputFrame(frame);
                }
            }
        };

    ret = add_stream_task(
        1,                  // streamId: 1
        1280,               // imageWidth
        720,                // imageHeight
        30,                 // frameRate: 30fps
        2000000,            // targetBps: 2Mbps
        &uhv_->pushFlowTaskQueue,         // pushFlowTaskQueue: 绑定的输入队列
        &uhv_->pushFlowPool,
        on_h265_encoded     // encodedCallback: 绑定的输出回调
    );
    if (!ret) {
        std::cerr << "Failed to start MPP streams!" << std::endl;
        return;
    }

    ret = add_stream_task(
        0,                  // streamId: 0
        1920,               // imageWidth
        1088,                // imageHeight
        30,                 // frameRate: 30fps
        2000000,            // targetBps: 2Mbps
        &uhv_->v1080pTaskQueue,         // pushFlowTaskQueue: 绑定的输入队列
        &uhv_->v1080pPool,
        on_h265_encoded     // encodedCallback: 绑定的输出回调
    );
    if (!ret) {
        std::cerr << "Failed to start MPP streams!" << std::endl;
        return;
    }
}

MppConverter::~MppConverter() 
{
    stop_all_streams();
    stop_media_servers();
}

bool MppConverter::add_stream_task(int streamId, 
                                   int imageWidth, 
                                   int imageHeight, 
                                   int frameRate, 
                                   int targetBps,
                                   ThreadSafeQueue<DmaBuffer_t*>* taskQueue,
                                   DmaBufferPool* bufferPool,
                                   OnEncodedCallback encodedCallback) 
{
    if (!taskQueue || !encodedCallback) {
        std::cerr << "[MppConverter] Error: Queue or callback is null!" << std::endl;
        return false;
    }

    auto workerContext = std::make_shared<StreamWorkerContext>();
    workerContext->streamId_ = streamId;
    workerContext->imageWidth_ = imageWidth;
    workerContext->imageHeight_ = imageHeight;
    
    /* 强制 16 字节对齐 */
    workerContext->horStride_ = MPP_ALIGN(imageWidth, 16);
    workerContext->verStride_ = MPP_ALIGN(imageHeight, 16);
    workerContext->frameSize_ = workerContext->horStride_ * workerContext->verStride_ * 3 / 2;
    
    workerContext->taskQueue_ = taskQueue;
    workerContext->bufferPool_ = bufferPool;
    workerContext->onEncodedCallback_ = encodedCallback;
    workerContext->isRunning_ = false;
    workerContext->enableStream_ = false;

    /* 1. 初始化 MPP 硬件编码器 */
    MPP_RET ret = mpp_create(&workerContext->mppCtx_, &workerContext->mpi_);
    if (ret != MPP_OK) {
        std::cerr << "[MppConverter] Failed to create mpp context for stream " << streamId << std::endl;
        return false;
    }

    ret = mpp_init(workerContext->mppCtx_, MPP_CTX_ENC, MPP_VIDEO_CodingHEVC); // 指定 H.265
    if (ret != MPP_OK) {
        std::cerr << "[MppConverter] Failed to init mpp for stream " << streamId << std::endl;
        return false;
    }

    /* 2. 配置编码参数 (CBR 固定码率模式) */
    MppEncCfg mppCfg;
    mpp_enc_cfg_init(&mppCfg);

    mpp_enc_cfg_set_s32(mppCfg, "prep:width", workerContext->imageWidth_);
    mpp_enc_cfg_set_s32(mppCfg, "prep:height", workerContext->imageHeight_);
    mpp_enc_cfg_set_s32(mppCfg, "prep:hor_stride", workerContext->horStride_);
    mpp_enc_cfg_set_s32(mppCfg, "prep:ver_stride", workerContext->verStride_);
    mpp_enc_cfg_set_s32(mppCfg, "prep:format", MPP_FMT_YUV420SP); // NV12 格式

    mpp_enc_cfg_set_s32(mppCfg, "rc:mode", MPP_ENC_RC_MODE_CBR);
    mpp_enc_cfg_set_s32(mppCfg, "rc:bps_target", targetBps);
    mpp_enc_cfg_set_s32(mppCfg, "rc:bps_max", targetBps * 1.5);
    mpp_enc_cfg_set_s32(mppCfg, "rc:bps_min", targetBps * 0.5);
    mpp_enc_cfg_set_s32(mppCfg, "rc:fps_in_flex", 0);
    mpp_enc_cfg_set_s32(mppCfg, "rc:fps_in_num", frameRate);
    mpp_enc_cfg_set_s32(mppCfg, "rc:fps_in_denorm", 1);
    mpp_enc_cfg_set_s32(mppCfg, "rc:fps_out_num", frameRate);
    mpp_enc_cfg_set_s32(mppCfg, "rc:fps_out_denorm", 1);

    mpp_enc_cfg_set_s32(mppCfg, "h265:profile", 1); // Main Profile

    ret = workerContext->mpi_->control(workerContext->mppCtx_, MPP_ENC_SET_CFG, mppCfg);
    mpp_enc_cfg_deinit(mppCfg);

    if (ret != MPP_OK) {
        std::cerr << "[MppConverter] Failed to set config for stream " << streamId << std::endl;
        return false;
    }

    /* 3. 启动处理线程 */
    workerContext->isRunning_ = true;
    workerContext->workerThread_ = std::thread(&MppConverter::encoding_thread_loop, this, workerContext);
    
    streamWorkers_.push_back(workerContext);
    
    std::cout << "[MppConverter] Stream " << streamId << " added and thread started." << std::endl;
    return true;
}

void MppConverter::encoding_thread_loop(std::shared_ptr<StreamWorkerContext> workerContext) 
{
    while (workerContext->isRunning_) {
        {
            std::unique_lock<std::mutex> lock(workerContext->pauseMutex_);
            // 如果 enableStream_ 为 false，线程会在这里彻底挂起，不占任何 CPU！
            workerContext->pauseCv_.wait(lock, [this, workerContext] { 
                return workerContext->enableStream_ || !workerContext->isRunning_; 
            });
        }
        /* 1. 阻塞式获取任务 (消费者模型) */
        DmaBuffer_t* srcBuffer = workerContext->taskQueue_->pop();
        
        /* 【退出机制】收到 nullptr (毒丸)，说明外部要求安全停止线程 */
        if (srcBuffer == nullptr) break;

        if (srcBuffer->dmaFd <= 0) {
            std::cerr << "[MppConverter] Invalid DMA fd received in stream " << workerContext->streamId_ << std::endl;
            // 处理完您的 srcBuffer (如将其放回空闲池)，然后继续
            continue;
        }

        MPP_RET ret = MPP_OK;
        MppBuffer frmBuffer = nullptr;

        /* 2. 导入 DMA Fd 实现零拷贝 */
        MppBufferInfo mppBufInfo;
        memset(&mppBufInfo, 0, sizeof(MppBufferInfo));
        mppBufInfo.type = MPP_BUFFER_TYPE_EXT_DMA;
        mppBufInfo.fd = srcBuffer->dmaFd;
        mppBufInfo.size = workerContext->frameSize_;
        
        ret = mpp_buffer_import(&frmBuffer, &mppBufInfo);
        if (ret != MPP_OK) {
            std::cerr << "[MppConverter] Failed to import DMA fd!" << std::endl;
            workerContext->bufferPool_->release_buffer(srcBuffer);
            continue; // 这里视您的业务逻辑，可能需要回收 srcBuffer
        }

        /* 3. 构造 MppFrame 并发送给硬件 */
        MppFrame mppFrame = nullptr;
        mpp_frame_init(&mppFrame);
        mpp_frame_set_width(mppFrame, workerContext->imageWidth_);
        mpp_frame_set_height(mppFrame, workerContext->imageHeight_);
        mpp_frame_set_hor_stride(mppFrame, workerContext->horStride_);
        mpp_frame_set_ver_stride(mppFrame, workerContext->verStride_);
        mpp_frame_set_fmt(mppFrame, MPP_FMT_YUV420SP);
        mpp_frame_set_buffer(mppFrame, frmBuffer);

        ret = workerContext->mpi_->encode_put_frame(workerContext->mppCtx_, mppFrame);

        /* 4. 获取硬件编码结果 */
        MppPacket mppPacket = nullptr;
        ret = workerContext->mpi_->encode_get_packet(workerContext->mppCtx_, &mppPacket);
        if (ret == MPP_OK && mppPacket != nullptr) {
            void* h265Data = mpp_packet_get_pos(mppPacket);
            size_t dataSize = mpp_packet_get_length(mppPacket);
            
            /* 触发异步回调，将数据抛给上层 (如 ZLMediaKit 或 MP4 录制器) */
            if (h265Data && dataSize > 0)
                workerContext->onEncodedCallback_(workerContext->streamId_, static_cast<const uint8_t*>(h265Data), dataSize);

            mpp_packet_deinit(&mppPacket);
        }

        /* 5. 清理本帧 MPP 资源 (注意：这不会 close 真正的 dmaFd，由内存池负责管理) */
        mpp_frame_deinit(&mppFrame);
        mpp_buffer_put(frmBuffer);
        
        workerContext->bufferPool_->release_buffer(srcBuffer);
    }
}

void MppConverter::stop_all_streams() 
{
    for (auto& workerContext : streamWorkers_) {
        if (workerContext->isRunning_) {
            /* 唤醒因条件变量而睡眠的线程 */
            if (!workerContext->enableStream_)
                ctrl_stream(workerContext->streamId_, true);

            if (workerContext->enableRecord_)
                ctrl_record(workerContext->streamId_, false);

            workerContext->isRunning_ = false;
            
            /* 注入“毒丸” (Poison Pill) 唤醒阻塞在 top() 上的线程 */
            // 假设您的 ThreadSafeQueue 有 push 方法
            if (workerContext->taskQueue_)
                workerContext->taskQueue_->push(nullptr);
            
            if (workerContext->workerThread_.joinable())
                workerContext->workerThread_.join();

            /* 销毁 MPP 硬件资源 */
            if (workerContext->mppCtx_) {
                mpp_destroy(workerContext->mppCtx_);
                workerContext->mppCtx_ = nullptr;
                workerContext->mpi_ = nullptr;
            }
        }
    }
    streamWorkers_.clear();

    if (zlmChannel1080p_) {
        zlmChannel1080p_.reset(); // 释放频道对象
        std::cout << "[MppConverter] 1080p 录制通道已关闭，正在封装 MP4..." << std::endl;
    }
    
    if (zlmChannel720p_) {
        zlmChannel720p_.reset();
    }

    std::cout << "[MppConverter] All streams stopped and resources released." << std::endl;
}

void MppConverter::ctrl_stream(int streamId, bool status) 
{
    for (auto& workerContext : streamWorkers_) {
        if (streamId != workerContext->streamId_) continue;

        if (workerContext->enableStream_ == status) return;

        if (status) {
            {
                std::lock_guard<std::mutex> lock(workerContext->pauseMutex_);
                workerContext->enableStream_ = true;
            }
            workerContext->pauseCv_.notify_all(); // 【关键】：唤醒正在深度睡眠的编码线程！
            std::cout << "[MppConverter] 阀门打开，已唤醒编码线程！" << std::endl;
        } else {
            {
                std::lock_guard<std::mutex> lock(workerContext->pauseMutex_);
                workerContext->enableStream_ = false;
            }
            std::cout << "[MppConverter] 阀门关闭，编码线程即将休眠！" << std::endl;
        }
    }
}

void MppConverter::ctrl_record(int streamId, bool status)
{
    for (auto& workerContext : streamWorkers_) {
        if (streamId != workerContext->streamId_) continue;

        if (workerContext->enableRecord_ == status) return;

        if (status) {
            if (streamId == 0) {
                if (zlmChannel1080p_ && poller1080p_) {
                    workerContext->enableRecord_ = true;
                    poller1080p_->async([this]() {
                        this->zlmChannel1080p_->setupRecord(mediakit::Recorder::type_mp4, true, "./video_records", 0);
                    });
                    ctrl_stream(streamId, true);
                    std::cout << "[MppConverter] 已开始 1080p MP4 录像!" << std::endl;
                }
            } else if (streamId == 1) {

            }
        } else {
            if (streamId == 0) {
                if (zlmChannel1080p_ && poller1080p_) {
                    workerContext->enableRecord_ = false;
                    auto channel_copy = zlmChannel1080p_; 
                    poller1080p_->async([channel_copy]() {
                        if (channel_copy) {
                            channel_copy->setupRecord(mediakit::Recorder::type_mp4, false, "", 0);
                        }
                    });
                    ctrl_stream(streamId, false);
                    std::cout << "[MppConverter] 已暂停 1080p MP4 录像!" << std::endl;
                }
            } else if (streamId == 1) {

            }
        }
    }
}

void MppConverter::init_zlm_env() 
{
    toolkit::Logger::Instance().add(std::make_shared<toolkit::ConsoleChannel>());
    toolkit::Logger::Instance().setWriter(std::make_shared<toolkit::AsyncLogWriter>());
    toolkit::WorkThreadPool::setPoolSize(4); // 启动四核线程池
    std::cout << "[MppConverter] ZLM 全局环境初始化完成." << std::endl;
}

bool MppConverter::start_media_servers() 
{
    // 实例化服务器对象
    rtspSrv_ = std::make_shared<toolkit::TcpServer>();
    rtmpSrv_ = std::make_shared<toolkit::TcpServer>();

    try {
        rtspSrv_->start<mediakit::RtspSession>(8554); 
        rtmpSrv_->start<mediakit::RtmpSession>(1935); 
        
        std::cout << "[系统提示] ZLM 流媒体服务器已启动 (RTSP: 8554, RTMP: 1935)!" << std::endl;
        return true;
    } catch (std::exception &ex) {
        std::cerr << "[严重错误] 流媒体服务器启动失败: " << ex.what() << std::endl;
        // 启动失败的话，清理掉刚才 new 出来的对象
        rtspSrv_.reset();
        rtmpSrv_.reset();
        return false;
    }
}

void MppConverter::stop_media_servers() 
{
    if (rtspSrv_) {
        rtspSrv_.reset(); // 释放端口，踢掉客户端
    }
    if (rtmpSrv_) {
        rtmpSrv_.reset();
    }
    std::cout << "[MppConverter] ZLM 网络服务器已安全关闭." << std::endl;
}
