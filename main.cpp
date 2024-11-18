#include "hFramework.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h> /* abs */

using namespace hFramework;

// GLOBAL VARS
// input_wheel
int input_wheel_start_state = 0;
int input_wheel_curent_state = 0;
static const bool input_wheel_curent_state_show = false;
static const int input_wheel_offset = 69;
static const int input_wheel_tolerance = 5;
static const int input_wheel_encoder_dalay = 777;
static const int input_wheel_home_position_delay = 1000;
static const int input_wheel_power = 500; // <0; 1000>

// output_wheel
int output_wheel_start_state = 0;
static const int output_wheel_digits = 10;
static const int output_wheel_angle = 720;
static const int output_wheel_step = output_wheel_angle / output_wheel_digits;
static const int output_wheel_power = 700; // <0; 1000>

// actuator
int actuator_start_state = 0;
static const int actuator_target_state = 360;
static const int actuator_power = 1000; // <0; 1000>

// binary_array
bool binary_array[4] = {1, 1, 1, 1};
int binary_array_index = 0;
static const int binary_array_size = 4;

// binary
static const int border_top = 9;

// output
int output_number = -1;

// clear_button
int clear_buuton_delay = 200;

// debug
bool debug = true;
int debug_info_delay = 300;

// RUNTIME VARS
bool input_wheel_encoder_run = true;
bool input_wheel_home_position_run = false;

void debug_info()
{
	while (true)
	{
		Serial.printf("hMot1: %d\thMot2: %d\thMot3: %d\r\n", hMot1.getEncoderCnt(), hMot2.getEncoderCnt(), hMot3.getEncoderCnt());
		sys.delay(debug_info_delay);
	}
}

int binaryToDecimal(const bool *binary, int size)
{
	int decimal = 0;
	for (int i = 0; i < size; ++i)
	{
		decimal += binary[i] * (1 << (size - 1 - i)); // (1 << n) to przesunięcie bitowe, które działa jak 2^n
	}
	if (decimal > border_top)
	{
		Serial.printf("ERROR:\r\nValue %d out of border %d\r\nReturn 0\r\n\r\n", decimal, border_top);
		return 0;
	}
	return decimal;
}

void input_wheel_home_position()
{
	input_wheel_home_position_run = true;
	hLED2.on();
	Serial.printf("MOTOR:\r\ninput_wheel from: %d\tto: %d\r\n", input_wheel_curent_state, input_wheel_start_state);
	hMot1.rotAbs(input_wheel_start_state, input_wheel_power, true, INFINITE); // rotate to "0" ticks absolute position, and NOT block program until task finishes
	// hMot1.rotRel(input_wheel_start_state - input_wheel_curent_state, 500, false, INFINITE);

	while (true)
	{
		if (abs(input_wheel_start_state - input_wheel_curent_state) > input_wheel_tolerance)
		{
			hMot1.stop();
			hMot1.stopRegulation();
			break;
		}
	}

	input_wheel_home_position_run = false;
	hLED2.off();
}

void input_wheel_encoder()
{
	while (input_wheel_encoder_run)
	{
		input_wheel_curent_state = hMot1.getEncoderCnt();
		if (input_wheel_curent_state_show)
		{
			Serial.printf("input_wheel_curent_state: %d\r\n", input_wheel_curent_state); // print the current position of Motor 1 (no. of encoder ticks)
		}
		hLED1.toggle();

		if (!input_wheel_home_position_run &&
			abs(input_wheel_start_state - input_wheel_curent_state) > input_wheel_offset &&
			binary_array_index < 4)
		{
			sys.delay(input_wheel_home_position_delay);
			sys.taskCreate(input_wheel_home_position);

			// set bit
			if (input_wheel_curent_state > input_wheel_start_state)
			{
				binary_array[binary_array_index] = 1;
			}
			else
			{
				binary_array[binary_array_index] = 0;
			}
			Serial.printf("------- Recived -------\r\n%d\r\n", binary_array[binary_array_index]);
			binary_array_index++;
		}
		sys.delay(input_wheel_encoder_dalay);
	}
}

void output_wheel_show()
{
	hLED3.on();

	Serial.printf("MOTOR:\r\actuator from: %d\tto: %d\r\n", hMot3.getEncoderCnt(), actuator_start_state);
	hMot3.rotAbs(actuator_start_state, actuator_power, true, INFINITE); // rotate to "0" ticks absolute position, and NOT block program until task finishes

	int position = output_number * output_wheel_step;
	Serial.printf("MOTOR:\r\noutput_wheel from: %d\tto: %d\r\n", hMot2.getEncoderCnt(), position);
	hMot2.rotAbs(position, output_wheel_power, true, INFINITE); // rotate to "0" ticks absolute position, and NOT block program until task finishes

	Serial.printf("MOTOR:\r\actuator from: %d\tto: %d\r\n", hMot3.getEncoderCnt(), actuator_target_state);
	hMot3.rotAbs(actuator_target_state, actuator_power, true, INFINITE); // rotate to "0" ticks absolute position, and NOT block program until task finishes

	hLED3.off();
}

void output_show()
{
	Serial.printf("------- OUTPUT -------\r\nbinary_array_index: %d\r\nbinary_array: [ ", binary_array_index);
	for (int i = 0; i < binary_array_size; ++i)
	{
		Serial.printf("%d; ", binary_array[i]);
	}
	Serial.printf("]\r\noutput_number: %d\r\n", output_number);
}

void clear_button_thread_loop()
{
	while (true)
	{
		hBtn1.waitForPressed(); // waiting for press hBtn1
		printf("clear_button PRESSED\r\n");
		sys.delay(clear_buuton_delay);
		hBtn1.waitForReleased(); // waiting for released hBtn1
		printf("clear_button RELEASED\r\n");
		clear_buffor();
	}
}

void clear_buffor(bool all = false)
{
	if (all)
	{
		for (int i = 0; i < binary_array_size; ++i)
		{
			binary_array[i] = 1;
		}
		binary_array_index = 0;
	}
	else
	{
		if (binary_array_index == 0)
		{
			binary_array[0] = 1;
		}
		else
		{
			binary_array[--binary_array_index] = 1;
		}
	}
}

void welcome()
{
	Serial.printf("Husarion Telegraf\r\nhttps://github.com/Niewiaro/Husarion-Telegraf\r\n");
	Serial.printf(" __  __           _           _                   _               \r\n");
	Serial.printf("|  \\/  |         | |         | |                 (_)             \r\n");
	Serial.printf("| \\  / | ___  ___| |__   __ _| |_ _ __ ___  _ __  _  ___         \r\n");
	Serial.printf("| |\\/| |/ _ \\/ __| '_ \\ / _` | __| '__/ _ \\| '_ \\| |/ __|    \r\n");
	Serial.printf("| |  | |  __/ (__| | | | (_| | |_| | | (_) | | | | | (__          \r\n");
	Serial.printf("|_|__|_|\\___|\\___|_| |_|\\__,_|\\__|_|  \\___/|_| |_|_|\\___|   \r\n");
	Serial.printf("|  __ \\          (_)                                             \r\n");
	Serial.printf("| |  | | ___  ___ _  __ _ _ __                                    \r\n");
	Serial.printf("| |  | |/ _ \\/ __| |/ _` | '_ \\                                 \r\n");
	Serial.printf("| |__| |  __/\\__ \\ | (_| | | | |                                \r\n");
	Serial.printf("|_____/ \\___||___/_|\\__, |_| |_|                                \r\n");
	Serial.printf("                     __/ |                                        \r\n");
	Serial.printf("                    |___/                                         \r\n");
}

void init()
{
	welcome();

	// GLOBAL VARS
	// input_wheel_start_state
	input_wheel_start_state = hMot1.getEncoderCnt();
	input_wheel_curent_state = input_wheel_start_state;
	Serial.printf("input_wheel_start_state: %d\r\n", input_wheel_start_state);

	// output_wheel_start_state
	output_wheel_start_state = hMot2.getEncoderCnt();
	Serial.printf("output_wheel_start_state: %d\r\n", output_wheel_start_state);

	// actuator_start_state
	actuator_start_state = hMot3.getEncoderCnt();
	Serial.printf("actuator_start_state: %d\r\n", actuator_start_state);

	// binary_array
	for (int i = 0; i < binary_array_size; ++i)
	{
		binary_array[i] = 1;
	}
	binary_array_index = 0;

	// other
	output_number = -1;

	// RUNTIME VARS
	input_wheel_encoder_run = true;
	input_wheel_home_position_run = false;

	// LEGO
	hMot1.setEncoderPolarity(Polarity::Normal); // changing encoder polarity (Polarity::Normal is default)
	hMot1.setMotorPolarity(Polarity::Reversed); // changing motor polarity

	hMot2.setEncoderPolarity(Polarity::Normal);
	hMot2.setMotorPolarity(Polarity::Reversed);

	hMot3.setEncoderPolarity(Polarity::Normal);
	hMot3.setMotorPolarity(Polarity::Reversed);
}

void hMain()
{
	init();
	sys.taskCreate(input_wheel_encoder); // this creates a task that will execute `encoder` concurrently
	sys.taskCreate(clear_button_thread_loop);

	if (debug)
	{
		sys.taskCreate(debug_info);
	}

	while (true)
	{
		// check if all bits recived
		if (binary_array_index == 4)
		{
			output_number = binaryToDecimal(binary_array, binary_array_size);
			output_show();
			sys.taskCreate(output_wheel_show);
			clear_buffor(true);
		}
	}
}
