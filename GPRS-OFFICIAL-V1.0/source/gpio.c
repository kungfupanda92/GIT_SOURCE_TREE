#include "gpio.h"
//--------------------------------------------------------------------------------------
void GPIO_Output(GPIOx GPIO, uint32_t GPIO_Pin) {
	if (GPIO == GPIO_P0) {
		IODIR0 |= GPIO_Pin;
	} else {
		IODIR1 |= GPIO_Pin;
	}
}
//---------------------------------------------------------------------------------------
void GPIO_Input(GPIOx GPIO, uint32_t GPIO_Pin) {
	if (GPIO == GPIO_P0) {
		IODIR0 &= !(GPIO_Pin);
	} else {
		IODIR1 &= !(GPIO_Pin);
	}
}
//--------------------------------------------------------------------------------------
void GPIO_WriteBit(GPIOx GPIO, uint32_t GPIO_Pin, bool BitVal) {
	if (GPIO == GPIO_P0) {
		if (BitVal != Bit_RESET) {
			IOSET0 |= GPIO_Pin;
		} else {
			IOCLR0 |= GPIO_Pin;
		}
	} else {
		if (BitVal != Bit_RESET) {
			IOSET1 |= GPIO_Pin;
		} else {
			IOCLR1 |= GPIO_Pin;
		}
	}
}
//--------------------------------------------------------------------------------------
void GPIO_Toggled(GPIOx GPIO, uint32_t GPIO_Pin) {
	static bool tog=true;
	tog = !tog;
	if (GPIO == GPIO_P0) {
		if (tog != Bit_RESET) {
			IO0SET |= GPIO_Pin;
		} else {
			IO0CLR |= GPIO_Pin;
		}
	} else {
		if (tog != Bit_RESET) {
			IO1SET |= GPIO_Pin;
		} else {
			IO1CLR |= GPIO_Pin;
		}
	}
}
//--------------------------------------------------------------------------------------
