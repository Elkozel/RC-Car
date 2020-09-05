#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>
#include <RC-Car/Drive.h>

#define SOFTPWM

/**
 * Coppied from server.cpp
 * A guard function which looks for errors returned from system calls.
 * 
 * @param check the code returned by the system call
 * @param msg the message to be displayed if the return code indicates an error
 */
int guard(int check, const char *msg)
{
	if (check < 0)
	{
		std::cerr << msg << std::endl;
		exit(errno);
	}
	return check;
}

MotorController::MotorController()
{
	this->valPWM = 0;
	this->enabled = 0;
}

void MotorController::enable()
{
	this->enabled = 1;
	this->setPWM(0);
}

void MotorController::disable()
{
	this->setPWM(0);
	this->enabled = 0;
}

void MotorController::setPWM(int16_t value)
{
	if (isEnabled())
	{
		uint16_t mask = 0b0000001111111111;
		this->valPWM = (value & mask);
		if (value < 0)
			this->valPWM *= -1;
	}
}

void MotorController::brake() { this->valPWM = 0; }

int MotorController::isEnabled() { return this->enabled; }

int16_t MotorController::getPWM() { return this->valPWM; }

L298N::Motor::Motor() {};

L298N::Motor::Motor(uint8_t IN_F, uint8_t IN_B, uint8_t EN, uint8_t PWM) : MotorController()
{
	this->IN_F = IN_F;
	this->IN_B = IN_B;
	this->EN = EN;
	this->PWM = PWM;

	wiringPiSetup();

	// Set the pinMode of each pin
	#ifdef SOFTPWM
	int err = softPwmCreate (PWM, 0, 1024);
	if(err != 0){
		std::cerr << "Error initializing PWM" << std::endl;
		exit(errno);
	}
	#else
	pinMode(PWM, OUTPUT);
	#endif
	pinMode(EN, OUTPUT);
	pinMode(IN_F, OUTPUT);
	pinMode(IN_B, OUTPUT);

	// Bring controller to a start state
	#ifndef SOFTPWM
	digitalWrite(PWM, LOW);
	#endif
	digitalWrite(EN, LOW);
	digitalWrite(IN_F, LOW);
	digitalWrite(IN_B, LOW);
}

void L298N::Motor::enable()
{
	MotorController::enable();
	digitalWrite(IN_F, LOW);
	digitalWrite(IN_B, LOW);
	digitalWrite(EN, HIGH);
}

void L298N::Motor::disable()
{
	MotorController::disable();
	digitalWrite(IN_F, LOW);
	digitalWrite(IN_B, LOW);
	digitalWrite(EN, LOW);
}

void L298N::Motor::setPWM(int16_t value)
{
	MotorController::setPWM(value);
	if (value < 0)
	{
		digitalWrite(IN_F, LOW);
		digitalWrite(IN_B, HIGH);
	}
	else
	{
		digitalWrite(IN_F, HIGH);
		digitalWrite(IN_B, LOW);
	}
	#ifdef SOFTPWM
	softPwmWrite (PWM, (int)getPWM());
	#else
	pwmWrite(PWM, (int)getPWM());
	#endif
}

void L298N::Motor::brake()
{
	MotorController::brake();
	digitalWrite(IN_F, HIGH);
	digitalWrite(IN_B, HIGH);
	#ifdef SOFTPWM
	softPwmWrite (PWM, 0);
	#else
	pwmWrite(PWM, 0);
	#endif
}

L298N::L298N(uint8_t IN1, uint8_t IN2, uint8_t IN3, uint8_t IN4, uint8_t ENA, uint8_t ENB, uint8_t PWMA, uint8_t PWMB)
{
	MotorA = L298N::Motor(IN1, IN2, ENA, PWMA);
	MotorB = L298N::Motor(IN3, IN4, ENB, PWMB);
}

void L298N::enable()
{
	MotorA.enable();
	MotorB.enable();
}
void L298N::disable()
{
	MotorA.disable();
	MotorB.disable();
}
void L298N::setPWM(int16_t value)
{
	MotorA.setPWM(value);
	MotorB.setPWM(value);
}
void L298N::brake()
{
	MotorA.brake();
	MotorB.brake();
}

BTS7960::BTS7960(uint8_t R_EN, uint8_t L_EN, uint8_t R_IS, uint8_t L_IS, uint8_t RPWM, uint8_t LPWM) : MotorController()
{
	this->R_EN = R_EN;
	this->L_EN = L_EN;
	this->R_IS = R_IS;
	this->L_IS = L_IS;
	this->RPWM = RPWM;
	this->LPWM = LPWM;

	wiringPiSetup();

	// Set the pinMode of each pin
	pinMode(R_EN, OUTPUT);
	pinMode(L_EN, OUTPUT);
	pinMode(R_IS, INPUT);
	pinMode(L_IS, INPUT);
	pinMode(RPWM, OUTPUT);
	pinMode(LPWM, OUTPUT);

	// Bring controller to a start state
	digitalWrite(R_EN, LOW);
	digitalWrite(L_EN, LOW);
	digitalWrite(RPWM, LOW);
	digitalWrite(LPWM, LOW);
}

void BTS7960::enable()
{
	MotorController::enable();
	digitalWrite(R_EN, HIGH);
	digitalWrite(L_EN, HIGH);
}

void BTS7960::disable()
{
	MotorController::disable();
	digitalWrite(R_EN, LOW);
	digitalWrite(L_EN, LOW);
}

void BTS7960::setPWM(int16_t value)
{
	MotorController::setPWM(value);
	if (this->getPWM() < 0)
	{
		pwmWrite(RPWM, (int)getPWM());
		pwmWrite(LPWM, 0);
	}
	else
	{
		pwmWrite(RPWM, 0);
		pwmWrite(LPWM, (int)getPWM());
	}
}

void BTS7960::brake()
{
	MotorController::brake();
	digitalWrite(RPWM, HIGH);
	digitalWrite(LPWM, HIGH);
}

// Steering::Steering(uint8_t pinEnable, uint8_t pinPWM, uint8_t IN1, uint8_t IN2) : L298N(pinEnable, pinPWM, IN1, IN2)
// {
// 	this->brake();
// };

// void Steering::steer(int16_t direction)
// {
// 	uint16_t mask = 0b0000001111111111;
// 	this->valPWM = (value & mask);
// 	pwmWrite(PWM, (int)valPWM);

// 	if (value < 0)
// 	{
// 		digitalWrite(IN1, LOW);
// 		digitalWrite(IN2, HIGH);
// 		this->valPWM *= -1;
// 	}
// 	else
// 	{
// 		digitalWrite(IN1, HIGH);
// 		digitalWrite(IN2, LOW);
// 	}
// }

// void Steering::lock() { brake(); }

// void Steering::On() { enable(); }

// void Steering::Off() { disable(); }

// int16_t Steering::getSteer() { return getPWM(); }

// double Steering::getAngle() { return (double)180 * (valPWM / 1024); }

// int Steering::isEnabled() { return L298N::isEnabled(); }
