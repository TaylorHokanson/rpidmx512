/**
 * https://github.com/sparkfun/L6470-AutoDriver/blob/master/Libraries/Arduino/examples/SparkFunGetSetParamTest/SparkFunGetSetParamTest.ino
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "bcm2835.h"

#include "autodriver.h"

#include "l6470constants.h"

#define GPIO_BUSY_IN	RPI_V2_GPIO_P1_35
#define GPIO_RESET_OUT 	RPI_V2_GPIO_P1_38

// printf(); requires that you enumerate data types like so:
// printf("%s%d\n", "happy", kVal);
// the first statement indicates incoming string and int data, then a hard return

// What percentage of voltage are we passing to the motor?
// As expressed on a scale of 0 - 255
static uint8_t AccK = 50;
static uint8_t DecK = 50;
static uint8_t RunK = 50;
static uint8_t HoldK = 30;

void pv(float v);
void pv(bool v);
void pv(uint8_t v);
void pv(unsigned long v);
void pv(int v);

void test(float v1, float v2);
void test(int v1, int v2);
void test(bool v1, bool v2);

static char name[255];
static bool pass = true;

int main(int argc, char **argv) {
	unsigned long temp;
	bool tempBool;
	//uint8_t tempByte;
	//float tempFloat;
	int tempInt;
	int tempInt2;

	/****************************************************************************************************/
	/*                                                                                                  */
	if (bcm2835_init() == 0) {
		fprintf(stderr, "Not able to init the bmc2835 library\n");
		return -1;
	}

	// Start by setting up the pins and the SPI peripheral.
	//  The library doesn't do this for you!
	bcm2835_spi_begin();

	bcm2835_gpio_fsel(GPIO_RESET_OUT, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_set(GPIO_RESET_OUT);

	// This low/high is a reset of the L6470 chip on the Autodriver board, and is a good thing to do at
	// the start of any Autodriver sketch, to be sure you're starting the Autodriver from a known state.
	bcm2835_gpio_clr(GPIO_RESET_OUT);
	bcm2835_delayMicroseconds(10000);
	bcm2835_gpio_set(GPIO_RESET_OUT);
	bcm2835_delayMicroseconds(10000);
	/*                                                                                                  */
	/****************************************************************************************************/

	AutoDriver board(0, BCM2835_SPI_CS0, GPIO_RESET_OUT);
	
	
	// first check  board config register, should be 0x2E88 on bootup
	temp = board.getParam(L6470_PARAM_CONFIG);
	printf("Board Config: Expected 0x2E88, got 0x%.4X\n", (int) temp);

	// Now check the status of the board. Should be 0x7c03	
	temp = board.getStatus();
	printf("Board Status: Expected 0x7C03, got 0x%.4X\n", (int) temp);
	
	//board.setLoSpdOpt(1);
	//printf("%s%d\n", "Low Speed Optimization: ", board.getLoSpdOpt());
	
	strcpy(name, "LoSpdOpt");
	tempBool = board.getLoSpdOpt();
	pv(tempBool);
	tempBool = ~tempBool;
	board.setLoSpdOpt(tempBool);
	test(tempBool, board.getLoSpdOpt());

	// Step Mode
	// Input Values: 0 - 7
	// Represents: Full, 1/2, 1/4, 1/8, 1/16, 1/32, 1/64, 1/128 (datasheet pg. 48)
	board.configStepMode(0);
	printf("%s%d\n", "Step Mode: ", board.getStepMode());

	// Min Speed
	// 0 to 976.3 step/s with a resolution of 0.238 step/s (datasheet pg. 43)
	//board.setMinSpeed(23.8418788909);
	board.setMinSpeed(23.8);
	printf("%s%.1f\n", "Min Speed: ", board.getMinSpeed());

	// Max Speed
	// 15.25 to 15610 step/s with a resolution of 15.25 step/s (datasheet pg. 43)
	//board.setMaxSpeed(152.587890625);
	board.setMaxSpeed(152.5);
	printf("%s%.1f\n", "Max Speed: ", board.getMaxSpeed());
	
	// Full Speed
	// 7.63 to 15625 step/s with a resolution of 15.25 step/s (datasheet pg. 44)
	//board.setFullSpeed(160.21728515625);
	board.setFullSpeed(152.5);
	printf("%s%.1f\n", "Full Speed: ", board.getFullSpeed());

	// Acceleration
	// 14.55 to 59590 step/s2 with a resolution of 14.55 step/s2 (datasheet pg. 42)
	//board.setAcc(72.76008090920998);
	board.setAcc(73);
	printf("%s%.2f\n", "Acceleration: ", board.getAcc());

	// Deceleration
	// 14.55 to 59590 step/s2 with a resolution of 14.55 step/s2 (datasheet pg. 43)
	//board.setDec(72.76008090920998);
	board.setDec(73);
	printf("%s%.2f\n", "Deceleration: ", board.getDec());

	// Overcurrent Threshold
	// 375 mA to 6 A, in steps of 375 mA (datasheet pg. 47)
	board.setOCThreshold(L6470_OCD_TH_2250mA);
	printf("%s%d%s\n", "Overcurrent Threshold: ", ((board.getOCThreshold() + 1) * 375), "mA");

	strcpy(name, "PWMFreqDivisor");
	tempInt = board.getPWMFreqDivisor();
	tempInt2 = board.getPWMFreqMultiplier();
	pv(tempInt);
	tempInt = (tempInt == L6470_CONFIG_PWM_INT_DIV_1) ? L6470_CONFIG_PWM_INT_DIV_2 : L6470_CONFIG_PWM_INT_DIV_1;
	board.setPWMFreq(tempInt, tempInt2);
	test(tempInt, board.getPWMFreqDivisor());

	strcpy(name, "PWMFreqMultiplier");
	pv(tempInt2);
	tempInt2 = (tempInt2 == L6470_CONFIG_PWM_DEC_MUL_1) ? L6470_CONFIG_PWM_DEC_MUL_2 : L6470_CONFIG_PWM_DEC_MUL_1;
	board.setPWMFreq(tempInt, tempInt2);
	test(tempInt2, board.getPWMFreqMultiplier());
	
	// Relates to motor keeping up with drive pulses? (datasheet pg. 31) 
	board.setSlewRate(L6470_CONFIG_POW_SR_110V_us);

	// Enable overcurrent shutdown (datasheet pg. 29)
	board.setOCShutdown(L6470_CONFIG_OC_SD_ENABLE);

	// Enable/disable voltage compensation (datasheet pg. 28)
	// I think this relates to the variable resistor on the EVAL board
	board.setVoltageComp(TL6470_CONFIG_VS_COMP_ENABLE);

	// Enable/disable switch as HardStop interrupt (datasheet pg. 51)
	board.setSwitchMode(TL6470_CONFIG_SW_MODE_USER);

	// Use internal/external oscillator (datasheet pg. 28)
	board.setOscMode(L6470_CONFIG_OSC_INT_16MHZ);

	// K Values
	// Input Values: 0 - 255
	// Represents: Percentage of driver voltage passed to motors (datasheet pg. 44)
	board.setAccKVAL(AccK);
	printf("%s%d\n", "AccK:  ", board.getAccKVAL());

	board.setDecKVAL(DecK);
	printf("%s%d\n", "DecK:  ", board.getDecKVAL());

	board.setRunKVAL(RunK);
	printf("%s%d\n", "RunK:  ", board.getRunKVAL());

	board.setHoldKVAL(HoldK);
	printf("%s%d\n", "HoldK: ", board.getHoldKVAL());
}

void pv(float v) {
	printf("%s %f\n", name, v);
}

void pv(bool v) {
	printf("%s %s\n", name, v ? "True" : "False");
}

void pv(uint8_t v) {
	printf("%s %d\n", name, (int) v);
}

void pv(unsigned long v) {
	printf("%s %lu\n", name, v);
}

void pv(int v) {
	printf("%s %d\n", name, v);
}

void test(float v1, float v2) {
	if (abs(v1 - v2) > 0.1) {
		printf("!!! %s failed\n", name);
		printf("Expected %f Got %f\n", v1, v2);
		pass = false;
	} else {
		printf("%s passed r/w test!\n", name);
	}
}

void test(int v1, int v2) {
	if (v1 != v2) {
		printf("!!! %s failed\n", name);
		printf("Expected %d Got %d\n", v1, v2);
		pass = false;
	} else {
		printf("%s passed r/w test!\n", name);
	}
}

void test(bool v1, bool v2) {
	if (v1 != v2) {
		printf("!!! %s failed\n", name);
		printf("Expected %s Got %s\n", v1 ? "True" : "False", v2 ? "True" : "False");
		pass = false;
	} else {
		printf("%s passed r/w test!\n", name);
	}
}

