#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <cmath>
#include <mutex>

#include "turntable_controller.h"

class TurntableController; // 前置声明（实际在 turntable_controller.h 中定义）

class PIDController
{
public:
    // 公有参数结构体（可由 UI 读取/写入）
    struct Gains {
        double kp;
        double ki;
        double kd;
    };

    // 构造：需要指定被控轴和对应的 TurntableController 实例指针（可以为 nullptr，必须在使用前设置）
    PIDController(YawOrPitch_t axis, TurntableController *controller = nullptr);

    // 设置/获取参数
    void setGains(const Gains &g);
    Gains getGains() const;

    // 重置内部状态（积分和历史误差）
    void reset();

    // 将 PID 与底层硬件控制器绑定（替换模拟/真实控制器）
    void setController(TurntableController *controller);

    // 控制环（类内部做：读取当前、计算控制量并发送到硬件）
    // 返回 true 表示该轴控制已到位（|error| <= threshold）
    // controlPeriodSec 用于积分反走样时刻度（在本 demo 未使用，但保留接口）
    bool controlLoop(double target, double threshold = 0.01, double controlPeriodSec = 0.05);

private:
    // 私有状态结构体（外部不能访问）
    struct State {
        double lastError;
        double integral;
    };

    YawOrPitch_t  axis_;
    TurntableController *controller_; // 非拥有指针（外部管理生命周期）
    Gains gains_;

    mutable std::mutex mtx_; // 保证线程安全（若未来控制器在不同线程）
    State state_;
};

#endif // PID_CONTROLLER_H
