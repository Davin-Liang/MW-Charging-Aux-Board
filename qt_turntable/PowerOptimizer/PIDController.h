#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include "TurntableController.h"
#include <cmath>

class PIDController
{
public:

    // --- PID参数结构体 ---
    struct PIDParam {
        double kp;
        double ki;
        double kd;
    };

private:
    // --- PID内部状态结构体（不能外部访问） ---
    struct PIDState {
        double lastError;
        double integral;
    };

public:
    PIDController(TurntableAxis axis, TurntableController *controller);

    void setParameters(double kp, double ki, double kd);
    void reset();

    // 单步 PID 计算
    double compute(double target, double current);

    // 由 PID 内部执行完整闭环控制
    bool controlLoop(double target, double current, double threshold = 0.01);

    PIDParam param;   // 参数结构体（允许 UI 访问）

private:
    PIDState state;   // 内部状态结构体（UI 不能访问）

    TurntableAxis axis;                 // 控制哪个轴（Yaw / Pitch）
    TurntableController *controller;    // 底层硬件控制器
};

#endif
