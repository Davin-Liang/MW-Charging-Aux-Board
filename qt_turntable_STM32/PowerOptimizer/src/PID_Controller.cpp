#include "PID_Controller.h"
#include "turntable_controller.h"
#include <cmath>
#include <QMessageBox>

PIDController::PIDController(YawOrPitch_t  axis, TurntableController *controller)
    : axis_(axis), controller_(controller)
{
    gains_.kp = 0.0;
    gains_.ki = 0.0;
    gains_.kd = 0.0;
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
    // 确保 controller 可用
    if (!controller_) return false;

    // 读取当前角度
    float current = 0.0f;
    // bool ok = false;
    // if (axis_ == YawOrPitch_t::Yaw) {
    //     ok = controller_->read_axis_angle(Yaw, &current);
    // } else {
    //     ok = controller_->read_axis_angle(Pitch, &current);
    // }
    bool ok = controller_->read_axis_angle(axis_, &current);
    if (!ok) return false;
    // PID 计算
    double error = target - static_cast<double>(current);

    // 积分（注意：没有 anti-windup 机制，这里简单实现）
    {
        std::lock_guard<std::mutex> lk(mtx_);
        state_.integral += error * controlPeriodSec; // 时间尺度
        double derivative = (error - state_.lastError) / controlPeriodSec;

        // 控制量
        double u = gains_.kp * error + gains_.ki * state_.integral + gains_.kd * derivative;      
         // 限制输出幅度（避免发送过大速度）
        const double maxSpeed = 10.0; // deg/s，演示值
        if (u > maxSpeed) u = maxSpeed;
        if (u < -maxSpeed) u = -maxSpeed;

        // 将控制量转换为速度 + 方向并下发给转台
        double speed = -u;
        if ( speed>= 0){
            controller_->set_axis_speed(axis_, static_cast<float>(speed));
            controller_->set_manual_rotation(axis_, Left);
        }    
        else{
            controller_->set_axis_speed(axis_, static_cast<float>(-speed));
            controller_->set_manual_rotation(axis_, Right);
        }  
        state_.lastError = error;
    }
     
    // 判断是否到达目标（绝对误差）
    if (std::fabs(error) <= threshold) {
        controller_->stop_manual_rotation(axis_);
        return true;
    }
    return false;
}

