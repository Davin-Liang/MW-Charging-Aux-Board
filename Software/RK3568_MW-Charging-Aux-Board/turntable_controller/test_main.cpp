#include "turntable_controller.h"
#include <unistd.h>

int main()
{
    TurntableController TTC = TurntableController("/dev/ttyUSB0");

    while (TTC.get_connect_status() == false){
        if (TTC.connect(9600, 'N', 8, 1))
            break;
        std::cout << "尝试连接" << std::endl;
        sleep(2);
    };

    std::cout << "连接上了" << std::endl;
        

    while (TTC.get_connect_status() == true){
        TTC.set_manual_rotation(Yaw, Left);
        sleep(2);
        TTC.set_manual_rotation(Pitch, Right);
        sleep(2);
        std::cout << "1" << std::endl;

        TTC.stop_manual_rotation(Yaw);
        sleep(2);
        TTC.stop_manual_rotation(Pitch);
        sleep(2);

        TTC.reset_axis_coord(Yaw);
        sleep(2);
        TTC.reset_axis_coord(Pitch);
        sleep(2);

        TTC.set_axis_speed(Yaw, 10.0);
        sleep(2);
        TTC.set_axis_speed(Pitch, 5.0);
        sleep(2);

        // float speed;
        // TTC.read_axis_speed(Yaw, &speed);
        // std::cout << yaw << ":speed"  << speed << std::endl;
        // sleep(2);
        // TTC.read_axis_speed(Pitch, &speed);
        // std::cout << Pitch << ":speed"  << speed << std::endl;
        // sleep(2);

        TTC.set_axis_angle(Yaw, 2.5);
        sleep(2);
        TTC.set_axis_angle(Pitch, 2.5);
        sleep(2);

        // float angle;
        // TTC.read_axis_angle(Yaw, &angle);
        // std::cout << yaw << ":angle"  << angle << std::endl;
        // sleep(2);
        // TTC.read_axis_angle(Pitch, &angle);
        // std::cout << Pitch << ":angle"  << angle << std::endl;
        // sleep(2);

        TTC.set_axis_relative_motion(Yaw, 10.0);
        sleep(2);
        TTC.set_axis_relative_motion(Pitch, -10.0);
        sleep(2);

        break;
    }

    return 0;
}