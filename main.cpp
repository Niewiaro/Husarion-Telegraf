#include "hFramework.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h> /* abs */

using namespace hFramework;

// GLOBAL VARS
// input_wheel
int input_wheel_start_state = 0;
int input_wheel_curent_state = 0;
static const int input_wheel_offset = 69;
static const int input_wheel_tolerance = 5;
static const int input_wheel_encoder_dalay = 777;
static const int input_wheel_home_position_delay = 1000;

// binary_array
bool binary_array[4] = {1, 1, 1, 1};
int binary_array_index = 0;
static const int binary_array_size = 4;

// binary
static const int border_top = 9;

// output
int output_number = -1;

// RUNTIME VARS
bool input_wheel_encoder_run = true;
bool input_wheel_home_position_run = false;

int binaryToDecimal(const bool *binary, int size)
{
	int decimal = 0;
	for (int i = 0; i < size; ++i)
	{
		decimal += binary[i] * (1 << (size - 1 - i)); // (1 << n) to przesunięcie bitowe, które działa jak 2^n
	}
	if (decimal > border_top)
	{
		Serial.printf("ERROR value %d\r out of border %d\r\n", decimal, border_top);
		return 0;
	}
	return decimal;
}

void input_wheel_home_position()
{
	input_wheel_home_position_run = true;
	hLED2.on();
	Serial.printf("Go Home from: %d\r\n", input_wheel_curent_state);
	// hMot1.rotAbs(input_wheel_start_state, 500, false, INFINITE); // rotate to "0" ticks absolute position, and NOT block program until task finishes
	hMot1.rotRel(input_wheel_start_state - input_wheel_curent_state, 500, false, INFINITE); // rotate to "0" ticks absolute position, and NOT block program until task finishes

	while (true)
	{
		if (abs(input_wheel_start_state - input_wheel_curent_state) > input_wheel_tolerance)
		{
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
		Serial.printf("input_wheel_curent_state: %d\r\n", input_wheel_curent_state); // print the current position of Motor 1 (no. of encoder ticks)
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
			Serial.printf("Recived: %d\r\n", binary_array[binary_array_index]);
			binary_array_index++;
		}
		sys.delay(input_wheel_encoder_dalay);
	}
}

void welcome()
{
	Serial.printf("Husarion Telegraf\nhttps://github.com/Niewiaro/Husarion-Telegraf");
}

void init()
{
	welcome();

	// GLOBAL VARS
	// input_wheel_start_state
	input_wheel_start_state = hMot1.getEncoderCnt();
	input_wheel_curent_state = input_wheel_start_state;
	Serial.printf("input_wheel_state: %d\r\n", input_wheel_start_state);

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
	hMot1.setMotorPolarity(Polarity::Reversed);	  // changing motor polarity
}

void hMain()
{
	init();
	sys.taskCreate(input_wheel_encoder); // this creates a task that will execute `encoder` concurrently

	while (true)
	{
		// check if all bits recived
		if (binary_array_index == 4)
		{
			Serial.printf("binary_array_index: %d\r\n", binary_array_index);
			output_number = binaryToDecimal(binary_array, binary_array_size);
			Serial.printf("output_number: %d\r\n", output_number);

			for (int i = 0; i < binary_array_size; ++i)
			{
				binary_array[i] = 1;
			}
			binary_array_index = 0;
		}
	}
}
