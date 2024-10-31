#include "hFramework.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h> /* abs */

using namespace hFramework;

// GLOBAL VARS
int input_wheel_start_state = 0;
int input_wheel_offset = 10;
int input_wheel_curent_state = 0;
bool binary_array[4] = {1, 1, 1, 1};
int binary_array_index = 0;
int binary_array_size = 4;
int number_output = 0;
int input_wheel_encoder_dalay = 100;
int input_wheel_home_position_delay = 1000;

int binaryToDecimal(const bool *binary, int size)
{
	int decimal = 0;
	for (int i = 0; i < size; ++i)
	{
		decimal += binary[i] * (1 << (size - 1 - i)); // (1 << n) to przesunięcie bitowe, które działa jak 2^n
	}
	if (decimal > 9)
	{
		return 0;
	}
	return decimal;
}

void input_wheel_home_position()
{
	hLED2.on();
	hMot1.rotAbs(input_wheel_start_state, 200, false, INFINITE); // rotate to "0" ticks absolute position, and NOT block program until task finishes
	hLED2.off();
}

void input_wheel_encoder()
{
	while (true)
	{
		input_wheel_curent_state = hMot1.getEncoderCnt();
		Serial.printf("input_wheel_curent_state: %d\r\n", input_wheel_curent_state); // print the current position of Motor 1 (no. of encoder ticks)
		hLED1.toggle();

		if (abs(input_wheel_start_state - input_wheel_curent_state) > input_wheel_offset)
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

			binary_array_index++;

			// check if all bits recived
			if (binary_array_index == 4)
			{
				Serial.printf("binary_array_index: %d\r\n", binary_array_index);
				number_output = binaryToDecimal(binary_array, binary_array_size);
				Serial.printf("number_output: %d\r\n", number_output);

				for (int i = 0; i < binary_array_size; ++i)
				{
					binary_array[i] = 1;
				}
				binary_array_index = 0;
			}
		}
		sys.delay(input_wheel_encoder_dalay);
	}
}

void init()
{
	input_wheel_start_state = hMot1.getEncoderCnt();
	input_wheel_offset = 10;
	input_wheel_curent_state = input_wheel_start_state;

	for (int i = 0; i < binary_array_size; ++i)
	{
		binary_array[i] = 1;
	}

	binary_array_index = 0;
	Serial.printf("input_wheel_state: %d\r\n", input_wheel_start_state);
	sys.taskCreate(input_wheel_encoder); // this creates a task that will execute `encoder` concurrently
}

void hMain()
{
	init();
}
