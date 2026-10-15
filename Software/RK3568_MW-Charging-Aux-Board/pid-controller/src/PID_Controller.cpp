#include "PID_Controller.h"
#include <cmath>

PIDController::PIDController(YawOrPitch_t axis, TurntableController *controller)
    : axis_(axis), controller_(controller)
{
    reset();
}

void PIDController::setGains(const Gains &g)
{
    std::lock_guard<std::mutex> lk(mtx_);
    gains_ = g;
}

PIDController::Gains PIDController::getGains() const
{
    std::lock_guard<std::mutex> lk(mtx_);
    return gains_;
}

void PIDController::reset()
{
    std::lock_guard<std::mutex> lk(mtx_);
    state_.integral = 0.0;
    state_.lastError = 0.0;
}

void PIDController::setController(TurntableController *controller)
{
    std::lock_guard<std::mutex> lk(mtx_);
    controller_ = controller;
}

bool PIDController::controlLoop(double target, double threshold, double controlPeriodSec)
{
    if (!controller_) return false;

    float current = 0.0f;
    if (!controller_->read_axis_angle(axis_, &current)) {
        return false;
    }

    // 误差计算
    double error = target - static_cast<double>(current);

    {
        std::lock_guard<std::mutex> lk(mtx_);
        
        // 积分计算（注：当前为基础积分，实际工业应用中建议后续加入 Anti-windup 积分限幅机制）
        state_.integral += error * controlPeriodSec; 
        
        // 微分计算
        double derivative = (error - state_.lastError) / controlPeriodSec;

        // PID 输出控制量计算
        double u = gains_.kp * error + gains_.ki * state_.integral + gains_.kd * derivative;      
        
        // 运动输出限幅 (避免发送过大速度引发机械剧烈抖动)
        constexpr double MAX_SPEED = 10.0; // deg/s
        if (u > MAX_SPEED) u = MAX_SPEED;
        if (u < -MAX_SPEED) u = -MAX_SPEED;

        // 控制量解码为具体的转动方向和速度并下发
        double speed = -u;
        if (speed >= 0) {
            controller_->set_axis_speed(axis_, static_cast<float>(speed));
            controller_->set_manual_rotation(axis_, LeftOrRight_t::Left);
        } else {
            controller_->set_axis_speed(axis_, static_cast<float>(-speed));
            controller_->set_manual_rotation(axis_, LeftOrRight_t::Right);
        }  
        
        // 状态迭代
        state_.lastError = error;
    }
     
    // 目标到达判定（绝对误差满足死区阈值则停机）
    if (std::fabs(error) <= threshold) {
        controller_->stop_manual_rotation(axis_);
        return true;
    }
    
    return false;
}