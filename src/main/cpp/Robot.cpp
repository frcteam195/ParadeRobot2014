// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

#include <frc/smartdashboard/SmartDashboard.h>
#include <frc/livewindow/LiveWindow.h>
#include <math.h>

template <typename T>
inline int signum(T val)
{
    return (T(0) < val) - (val < T(0));
}

double normalizeJoystickWithDeadband(double val, double deadband) {
    val = (abs(val) > abs(deadband)) ? val : 0.0;

    if (val != 0)
	val = signum(val) * ((abs(val) - deadband) / (1.0 - deadband));

    return (abs(val) > abs(deadband)) ? val : 0.0;
}


void Robot::RobotInit() {
    frc::LiveWindow::GetInstance()->DisableAllTelemetry();
    frc::LiveWindow::GetInstance()->SetEnabled(false);


    cat_winch.Set(ControlMode::PercentOutput, 0);

}

void Robot::RobotPeriodic() {
}


void Robot::AutonomousInit() {}
void Robot::AutonomousPeriodic() {}

void Robot::TeleopInit() {
    cat_state = CATAPULT_STATE::WINDING;
}


void Robot::TeleopPeriodic() {

    switch(cat_state){
    case CATAPULT_STATE::WINDING:

	cat_solenoid.Set(false);
	cat_winch.Set( ControlMode::PercentOutput, 0.1 );

        if( cat_limit_switch.Get() ) {
	    cat_state = CATAPULT_STATE::UNWINDING;
	}

	break;

    case CATAPULT_STATE::UNWINDING:
	cat_solenoid.Set(false);
	cat_winch.Set( ControlMode::PercentOutput, -0.1 );

	if( cat_winch.GetSelectedSensorPosition() > 125  ) {
	    cat_state = CATAPULT_STATE::PRIMED;
	}

	break;

    case CATAPULT_STATE::PRIMED:
	cat_solenoid.Set(false);
	cat_winch.Set( ControlMode::PercentOutput, 0 );
	// input
	if( joystick.GetTriggerPressed() ) {
	    firing_timer.start();
	    cat_state = CATAPULT_STATE::FIRING;
	}
	break;

    case CATAPULT_STATE::FIRING:
	cat_solenoid.Set(true);
	// wait some amount of time
	if( firing_timer.hasElapsed() > 3.0 ){
	    cat_state = CATAPULT_STATE::WINDING;
	}
	break;
    default:
	std::cout << "Warning: Uknown Catapult state\n";
	break;
    }
}

void Robot::DisabledInit() {}
void Robot::DisabledPeriodic() {}

void Robot::TestInit() {}
void Robot::TestPeriodic() {}

#ifndef RUNNING_FRC_TESTS
int main() {
    return frc::StartRobot<Robot>();
}
#endif
