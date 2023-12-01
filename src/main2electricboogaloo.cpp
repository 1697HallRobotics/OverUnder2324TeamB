#include "vex.h"

competition Competition = competition();
controller Controller = controller();

motor frontLeftMotor = motor(PORT12, true);
motor backLeftMotor = motor(PORT5, true);

motor frontRightMotor = motor(PORT11, false);
motor backRightMotor = motor(PORT8, false);

motor_group leftMotors = motor_group(frontLeftMotor, backLeftMotor);
motor_group rightMotors = motor_group(frontRightMotor, backRightMotor);

motor cataMotorL = motor(PORT10, true);
motor cataMotorR = motor(PORT6, false);
motor_group cataMotor = motor_group(cataMotorL, cataMotorR);

motor intakeMotor = motor(PORT22, true);

// define your global instances of motors and other devices here
brain Brain = brain(); // make sure the robot is not braindead

bool spinning = false;

void spinCatapult() {
  if (spinning) {
    cataMotor.stop(brakeType::hold);
  } 
  else
  {
    cataMotor.spin(directionType::fwd);
  }
  spinning = !spinning;
}

void driver_control() {
    Controller.ButtonA.pressed(spinCatapult);

    cataMotor.setPosition(0, deg);

    while (true) {
        leftPower = Controller.Axis3.position() * speed;
        rightPower = Controller.Axis1.position() * speed;

        if(abs(leftPower) <= deadzone) leftPower = 0;
        if(abs(rightPower) <= deadzone) rightPower = 0;

        if (leftPower != 0 || rightPower != 0) {
            spinRightFwd(leftPower - rightPower);
            spinLeftFwd(leftPower + rightPower);
        }
        else {
            leftMotors.stop(brake);
            rightMotors.stop(brake);
        }

        if (Controller.ButtonL1.pressing()) {
            intakeMotor.spin(fwd);
        } else if (Controller.ButtonL2.pressing()) {
            intakeMotor.spin(reverse);
        } else {
            intakeMotor.stop(hold);
        }
        
    }
}

int main() {
    
}