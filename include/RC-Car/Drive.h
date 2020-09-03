#pragma once
#include <cstdint>

/**
 * Motor Controller Class
 * 
 * @brief A class with represents a DC motor controller.
 * 
 * The MotorController class represents an abstraction layer from the actual motor controller used in the project.
 * 
 * The values are only updated by the requests made the master command, there is no feedback from the MC.
 * 
 * @param enabled a boolean value keeping track of the state of the motor (0: disabled/ 1: enabled)
 * @param valPWM an int16_t value containing the PWM signal to the motor in the range of -1023 to 1023
 * ! Please note that values out of range can result in undocumented behaviour
 */
class MotorController
{
private:
	int enabled;
	int16_t valPWM;

protected:
	MotorController();

public:
	void virtual enable();
	void virtual disable();

	void virtual brake();
	void virtual setPWM(int16_t value);

	int isEnabled();
	int16_t getPWM();
};


/**
 * L298N Class
 * 
 * @brief a class representing the L298N Motor Controller
 * 
 * The L298N motor controller is a basic dual H-Bridge motor driver which 
 * allows speed and direction control of two DC motors at the same time.
 * For more info: http://www.handsontec.com/dataspecs/L298N%20Motor%20Driver.pdf
 * 
 * The module has two groups of 4 pins: 
	+---------+---------+--------------------------------+
	| Motor A | Motor B | Description                    |
	+---------+---------+--------------------------------+
	| ENA     | ENB     | Enable input pins              |
	+---------+---------+--------------------------------+
	| IN1     | IN3     | TTL Inputs for forward motion  |
	+---------+---------+--------------------------------+
	| IN2     | IN4     | TTL Inputs for backward motion |
	+---------+---------+--------------------------------+
	| PWM(A)  | PWM(B)  | PWM Inputs for speed of motor  |
	+---------+---------+--------------------------------+
 *  
 * @param MotorA a L298N::Motor representing Motor A
 * @param MotorB a L298N::Motor representing Motor B
 */
class L298N : public MotorController
{
public:
	/**
	 * L298N::Motor Class
	 * 
	 * @brief A class representing control over a single motor in a L298N Motor Controller
	 */
	class Motor : public MotorController
	{
	private:
		uint8_t IN_F; // TTL Input for forward motion
		uint8_t IN_B; // TTL Input for backward motion
		uint8_t EN; // Enable input pin
		uint8_t PWM; // PWM input pin

	public:
		Motor();
		Motor(uint8_t IN_F, uint8_t IN_B, uint8_t EN, uint8_t PWM);

		void enable();
		void disable();
		void setPWM(int16_t);
		void brake();
	};

	Motor MotorA;
	Motor MotorB;

	L298N(uint8_t IN1, uint8_t IN2, uint8_t IN3, uint8_t IN4, uint8_t ENA, uint8_t ENB, uint8_t PWMA, uint8_t PWMB);
	
	// Virtual functions
	void enable();
	void disable();
	void setPWM(int16_t);
	void brake();
};


/**
 * BTS7960 Class
 * 
 * @brief A class representing the BTS7960 Motor Controller
 * 
 * The BTS 7960 is a fully integrated high current half, 
 * bridge for motor drive applications.
 * 
 * The bridge has 8 pins:
	+--------+----------+------------------------------------------------------+
	| Pin No | Function | Description                                          |
	+--------+----------+------------------------------------------------------+
	| 1      | RPWM     | Forward Level or PWM signal, Active High             |
	+--------+----------+------------------------------------------------------+
	| 2      | LPWM     | Reverse Level or PWM signal, Active High             |
	+--------+----------+------------------------------------------------------+
	| 3      | R_EN     | Forward Drive Enable Input, Active High/ Low Disable |
	+--------+----------+------------------------------------------------------+
	| 4      | L_EN     | Reverse Drive Enable Input, Active High/Low Disable  |
	+--------+----------+------------------------------------------------------+
	| 5      | R_IS     | Forward Drive, Side current alarm output             |
	+--------+----------+------------------------------------------------------+
	| 6      | L_IS     | Reverse Drive, Side current alarm output             |
	+--------+----------+------------------------------------------------------+
 * The additional two pins are VCC and GND for logic.
 * 
 * TODO: Currently pins R_IS and L_IS are not used, implement a way for feedback data.
 */
class BTS7960 : public MotorController
{
private:
	uint8_t R_EN;
	uint8_t L_EN;
	uint8_t R_IS;
	uint8_t L_IS;
	uint8_t RPWM;
	uint8_t LPWM;

public:
	BTS7960(uint8_t R_EN, uint8_t L_EN, uint8_t R_IS, uint8_t L_IS, uint8_t RPWM, uint8_t LPWM);

	// Virtual functions
	void enable();
	void disable();
	void setPWM(int16_t);
	void brake();
};

// class Steering
// {
// public:
// 	Steering(uint8_t pinEnable, uint8_t pinPWM, uint8_t IN1, uint8_t IN2)
// 		: L298N(pinEnable, pinPWM, IN1, IN2){};

// 	void steer(int16_t direction);
// 	void lock();
// 	void On();
// 	void Off();

// 	int16_t getSteer();
// 	double getAngle();
// 	int isEnabled();
// };