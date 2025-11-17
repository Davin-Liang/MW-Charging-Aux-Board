#include "turntable_controller.h"
#include <unistd.h>

int main()
{
    TTC = TurntableController();

    while (TTC.get_connect_status() != false){
        if (TTC.connect(9600, 'N', 8, 1))
            break;
        sleep(2);
    };
        

    while (TTC.get_connect_status() == true){
        TTC.set_manual_rotation(Yaw, Left);
        sleep(2);
        TTC.set_manual_rotation(Pitch, Right);
        sleep(2);

        TTC.stop_manual_rotation(Yaw);
        sleep(2);
        TTC.stop_manual_rotation(Pitch);
        sleep(2);

        TTC.reset_axis_coord(Yaw);
        sleep(2);
        TTC.reset_axis_coord(Pitch);
        sleep(2);

        TTC.set_axis_speed(Yaw, 2.5);
        sleep(2);
        TTC.set_axis_speed(Pitch, 2.5);
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
    }

    return 0;
}