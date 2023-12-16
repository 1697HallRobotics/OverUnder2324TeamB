#include "vex.h"

brain Brain = brain(); // make sure the robot is not braindead
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

motor intakeMotor = motor(PORT2, true);

pneumatics Pneumatics = pneumatics(Brain.ThreeWirePort.A);

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

void pre_auton() {
    intakeMotor.setVelocity(100, pct);

    cataMotor.setMaxTorque(100, pct);
    cataMotor.setVelocity(75, pct);

    leftMotors.setVelocity(100, pct);
    rightMotors.setVelocity(100, pct);

    Brain.Screen.drawImageFromBuffer(logo_blue_map, 0, 0, sizeof(logo_blue_map));
}

void autonomous_red() {
    leftMotors.spinFor(reverse, 800, deg, false);
    rightMotors.spinFor(reverse, 800, deg, false);
    this_thread::sleep_for(2 * 1000);
    leftMotors.spinFor(fwd, 200, deg, false);
    rightMotors.spinFor(fwd, 200, deg, false);
}
void autonomous_blu() {
    intakeMotor.spinFor(fwd, 5, sec);
}

void driver_control() {
    Controller.ButtonA.pressed(spinCatapult);

    cataMotor.setPosition(0, deg);

    while (true) {
        leftPower = Controller.Axis3.position();
        rightPower = Controller.Axis1.position();

        if(abs(leftPower) <= deadzone) leftPower = 0;
        if(abs(rightPower) <= deadzone) rightPower = 0;

        if (leftPower != 0 || rightPower != 0) {
            spinRightFwd((leftPower - rightPower) * speed);
            spinLeftFwd((leftPower + rightPower) * speed);
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

        if (Controller.ButtonR2.pressing()) {
            cataMotor.spin(fwd);
        } else {
            cataMotor.stop(hold);
        }
        
    }
}

int main() {
    Competition.autonomous(autonomous_red);
    Competition.drivercontrol(driver_control);
    

    // Run the pre-autonomous function. 
    pre_auton();

    while (true)
    {
        wait(100, msec);
    }
}