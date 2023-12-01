/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       alsubaioma                                                */
/*    Created:      9/29/2023, 2:44:10 PM                                     */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#pragma region Includes
#include "vex.h"
#pragma endregion Includes

#pragma region Variable Definitions

// A global instance of competition
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

// define your global instances of motors and other devices here

brain Brain = brain(); // make sure the robot is not braindead

#pragma endregion Global Definitions

void switchInputScheme(void) {
  switch (currentScheme)
  {
  case inputScheme::TANK:
    currentScheme = inputScheme::CLASSIC_RC;
    printToScreen("Input Scheme changed to classic RC");
    leftPower = 0;
    rightPower = 0;
    break;
  case inputScheme::CLASSIC_RC:
    currentScheme = inputScheme::TANK;
    printToScreen("Input Scheme changed to TANK");
    leftPower = 0;
    rightPower = 0;
    break;
  default:
    break;
  }

}

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/


void pre_auton(void) {
  Controller.ButtonY.pressed(switchInputScheme);
  cataMotor.setMaxTorque(100, pct);
  cataMotor.setVelocity(75, pct);

  Brain.Screen.drawImageFromBuffer(logo_red_map, 0, 0, sizeof(logo_red_map));

}

bool autonomousFailed = false;

void finishAutonomous(void) {
  autonomousFailed = true;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  dramatic bold text                                                       */
/*---------------------------------------------------------------------------*/
void autonomous(void) {
  // testing code, in order to simulate the competition timer.
  if (testingAutonomous) {
    Brain.resetTimer();
    Brain.Timer.event(finishAutonomous, 15000);
    printToScreen("Autonomous test started");
  }

  //..........................................................................
  // Start autonomous code here
  //..........................................................................

  cataMotor.spinFor(directionType::fwd, 10, timeUnits::sec);

  //..........................................................................
  // Finish autonomous code here
  //..........................................................................
  if (testingAutonomous) {
    if (autonomousFailed) {
      printToScreen("darn it :(");
      Brain.Screen.print("You failed the autonomous while being ");
      Brain.Screen.print((15000 - Brain.Timer.time()) / -1000);
      Brain.Screen.print(" seconds too slow.");
    }
    else {
      printToScreen("hooray :)");
      Brain.Screen.print("You completed the autonomous with ");
      Brain.Screen.print((15000 - Brain.Timer.time()) / 1000);
      Brain.Screen.print(" seconds to spare.");
    }
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

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

void driverControl(void) {
    Controller.ButtonA.pressed(spinCatapult);

    cataMotor.setPosition(0, deg);

    while (true) {
        if (currentScheme == inputScheme::TANK) {
            leftPower = Controller.Axis3.position() * speed;
            rightPower = Controller.Axis2.position() * speed;
            
            if(abs(leftPower) <= deadzone) leftPower = 0;
            if(abs(rightPower) <= deadzone) rightPower = 0;

            if(leftPower != 0 || rightPower != 0) {
                spinRightFwd(rightPower);
                spinLeftFwd(leftPower);
            } else {
                leftMotors.stop(brake);
                rightMotors.stop(brake);
            }
        }
        else if (currentScheme == inputScheme::CLASSIC_RC) {
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
        }
    }
}


//
// Main will set up the competition functions and callbacks.
//
int maindisabled() {

  // Make sure all motors are set up correctly. If any are not installed, the program immediately exits.
  //if (!frontLeftMotor.installed() || !frontRightMotor.installed() || !backLeftMotor.installed() || !backRightMotor.installed()) return -1;

  if (!testingAutonomous && !testingDriverControl) {
    // Set up callbacks for autonomous and driver control periods.
    Competition.autonomous(autonomous);
    Competition.drivercontrol(driverControl);
  }

  // Run the pre-autonomous function. 
  pre_auton();

  // If we are currently testing autonomous or driver control, run the respective function.
  if (testingAutonomous) autonomous();
  if (testingDriverControl) driverControl();


  /*
  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
  */
}