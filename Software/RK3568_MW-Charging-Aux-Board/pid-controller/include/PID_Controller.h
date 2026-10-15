/**
 * @file PID_Controller.h
 * @brief PID 控制器头文件，用于闭环位置追踪与速度控制
 */

#pragma once
#include <mutex>
#include "turntable_controller.h"

/**
 * @class PIDController
 * @brief 针对单轴（Yaw 或 Pitch）的 PID 闭环位置控制器
 */
class PIDController
{
public:
    /**
     * @brief PID 增益参数结构体
     */
    struct Gains {
        double kp = 0.0;
        double ki = 0.0;
        double kd = 0.0;
    };

    /**
     * @brief 构造函数
     * @param axis 被控轴类型 (Yaw 或 Pitch)
     * @param controller 绑定的硬件控制器实例指针（默认为 nullptr，必须在使用前设置）
     */
    explicit PIDController(YawOrPitch_t axis, TurntableController *controller = nullptr);

    /**
     * @brief 设置 PID 增益参数
     * @param g 包含 kp, ki, kd 的结构体
     */
    void setGains(const Gains &g);

    /**
     * @brief 获取当前 PID 增益参数
     * @return 当前的 Gains 结构体
     */
    Gains getGains() const;

    /**
     * @brief 重置内部状态（清空积分器和历史误差）
     */
    void reset();

    /**
     * @brief 将 PID 与底层硬件控制器绑定
     * @param controller 转台硬件控制器实例指针
     */
    void setController(TurntableController *controller);

    /**
     * @brief 执行单次 PID 控制环（读取当前状态 -> 计算输出 -> 下发指令给硬件）
     * @param target 目标角度 [°]
     * @param threshold 容差阈值，绝对误差小于此值判定为到位，默认 0.01 [°]
     * @param controlPeriodSec 控制周期，用于积分反走样的时间尺度，默认 0.05 [s]
     * @return true 表示该轴控制已到位 (|error| <= threshold)；false 表示仍在调节或读取失败
     */
    bool controlLoop(double target, double threshold = 0.01, double controlPeriodSec = 0.05);

private:
    /**
     * @brief 私有状态结构体，记录系统运行过程中的中间变量
     */
    struct State {
        double lastError = 0.0;
        double integral = 0.0;
    };

    YawOrPitch_t axis_;
    TurntableController *controller_; ///< 非拥有型指针（外部管理生命周期）
    Gains gains_;
    State state_;

    mutable std::mutex mtx_; ///< 保证多线程读写增益与状态的安全性
};
