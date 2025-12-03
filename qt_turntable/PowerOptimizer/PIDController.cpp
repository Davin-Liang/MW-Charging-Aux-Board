#include "pid_controller.h"

PIDController::PIDController(TurntableAxis axis, TurntableController *controller)
    : axis(axis), controller(controller)
{
    param = {0, 0, 0};  // kp ki kd
    reset();
}

void PIDController::setParameters(double kp, double ki, double kd)
{
    param.kp = kp;
    param.ki = ki;
    param.kd = kd;
}

void PIDController::reset()
{
    state.lastError = 0.0;
    state.integral = 0.0;
}

double PIDController::compute(double target, double current)
{
    double error = target - current;
    state.integral += error;
    double derivative = error - state.lastError;

    state.lastError = error;

    return param.kp * error + param.ki * state.integral + param.kd * derivative;
}

/**
 * 返回值：
 *   true  → 控制已完成（误差<=阈值）
 *   false → 控制未完成（继续控制）
 */
bool PIDController::controlLoop(double target, double current, double threshold)
{
    double u = compute(target, current);

    // u > 0 → 正转，u < 0 → 反转
    if (u > 0)
        controller->set_manual_rotation(axis, Left);
    else
        controller->set_manual_rotation(axis, Right);

    controller->set_axis_speed(axis, std::fabs(u));

    // 停止条件
    if (std::fabs(target - current) <= threshold)
    {
        controller->stop_manual_rotation(axis);
        return true;
    }

    return false;
}
