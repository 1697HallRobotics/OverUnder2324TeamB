#include "vex.h"

brain Brain = brain(); // make sure the robot is not braindead
competition Competition = competition();
controller Controller = controller();

motor frontLeftMotor = motor(PORT11, true);
motor backLeftMotor = motor(PORT1, true);

motor frontRightMotor = motor(PORT20, false);
motor backRightMotor = motor(PORT10, false);

motor_group leftMotors = motor_group(frontLeftMotor, backLeftMotor);
motor_group rightMotors = motor_group(frontRightMotor, backRightMotor);

motor cataMotor = motor(PORT9, true);

motor intakeMotor = motor(PORT12, true);

motor wingMotor = motor(PORT2, false);

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

    wingMotor.setVelocity(100, pct);
    wingMotor.setMaxTorque(100, pct);

    Brain.Screen.drawImageFromBuffer(logo_blue_map, 0, 0, sizeof(logo_blue_map));
}

void autonomous_atk() {
    cataMotor.spinFor(fwd, 700, deg, false);
    
    leftMotors.spinFor(fwd, 800, deg, false);
    rightMotors.spinFor(fwd, 800, deg, false);
    this_thread::sleep_for(1.5 * 1000);
    leftMotors.spinFor(reverse, 200, deg, false);
    rightMotors.spinFor(reverse, 200, deg, false);
    
}
void autonomous_def() {
    intakeMotor.spinFor(fwd, 5, sec);
}

bool blockingR2Function = false;
bool wingsOut = false;

void driver_control() {
    Controller.ButtonR1.pressed([]() {
        blockingR2Function = true;
        cataMotor.spinFor(fwd, 720, deg, true);
        blockingR2Function = false;
    });
    Controller.ButtonA.pressed([]() {
        if (wingsOut) {
            wingMotor.spinFor(reverse, 190, deg, false);
        } else {
            wingMotor.spinFor(fwd, 190, deg, std::false_type::value);
        }
        wingsOut = !wingsOut;
    });

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

        if (Controller.ButtonR2.pressing() && !blockingR2Function) {
            cataMotor.spin(fwd);
        } else if (!blockingR2Function) {
            cataMotor.stop(hold);
        }
    }
}

int main() {
    Competition.autonomous(autonomous_atk);
    Competition.drivercontrol(driver_control);
    
    // Run the pre-autonomous function. 
    pre_auton();

    while (true)
    {
        wait(100, msec);
    }
}