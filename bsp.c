
#include "qpn_port.h"
#include "bsp.h"
#include "xintc.h"
#include "xspi.h"
#include "xspi_l.h"
#include "xil_exception.h"

#include <stdio.h>
#include <stdlib.h>
#include <mb_interface.h>

#include "xparameters.h"
#include <xil_types.h>
#include <xil_assert.h>

#include <xio.h>
#include "xtmrctr.h"
#include "fft.h"
#include "note.h"
#include "trig.h"
#include "stream_grabber.h"
#include <stdbool.h>

#include "lab2a.h"



#define GPIO_CHANNEL1 1
#define SAMPLES 128
#define M 7 //2^m=samples
#define CLOCK 100000000.0 //clock speed

extern bool inHomeScreen;
float frequency;
float sample_f;
int m = 5;
int incr = 32;
int sample_size = 1;
int octave = 2;
int error = 0;
int a4 = 440;

int octaveOffsets[10] = {5, 5, 5, 5, 4, 4, 2, 2, 1, 0};

static float q[SAMPLES];
static float w[SAMPLES];
static float zero[SAMPLES];

void read_fsl_values(float *q, int n);

XIntc sys_intc;
static XGpio btn;
static XGpio encoder;
static XSpi spi;

// int to use for determining the direction of twist
int state = 0;

/*..........................................................................*/
void BSP_init(void) {

/* Setup interrupts and reference to interrupt handler function(s)  */

	XStatus Status;
	XStatus btnStatus;
	XStatus encoderStatus;
	XStatus spiStatus;

	Status = XST_SUCCESS;
	Status = XIntc_Initialize(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);

	if (Status == XST_SUCCESS)
		xil_printf("Interrupt controller initialized!\n\r");
	else
		xil_printf("Interrupt controller not initialized!\n\r");

	// button
	btnStatus = XIntc_Connect(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR,
			(XInterruptHandler)BtnHandler, &btn);
	if (btnStatus == XST_SUCCESS)
		xil_printf("Button handler connected\n\r");
	else
		xil_printf("Button handler not connected\n\r");

	btnStatus = XGpio_Initialize(&btn, XPAR_AXI_GPIO_BTN_DEVICE_ID);
	if (btnStatus == XST_SUCCESS)
			xil_printf("Button initialized\n\r");
		else
			xil_printf("Button not initialized\n\r");

	// rotary encoder
	encoderStatus = XIntc_Connect(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_ENCODER_IP2INTC_IRPT_INTR,
			(XInterruptHandler)TwistHandler, &encoder);
	if (encoderStatus == XST_SUCCESS)
		xil_printf("Encoder handler connected\n\r");
	else
		xil_printf("Encoder handler not connected\n\r");
		
	encoderStatus = XGpio_Initialize(&encoder, XPAR_GPIO_0_DEVICE_ID);
	if (encoderStatus == XST_SUCCESS)
		xil_printf("Encoder initialized\n\r");
	else
		xil_printf("Encoder not initialized\n\r");

	// SPI driver
	XSpi_Config *spiConfig = XSpi_LookupConfig(XPAR_SPI_DEVICE_ID);
	if (spiConfig == NULL) {
		xil_printf("Can't find spi device!\n");
		return XST_DEVICE_NOT_FOUND;
	}

	spiStatus = XSpi_CfgInitialize(&spi, spiConfig, spiConfig->BaseAddress);
	if (spiStatus == XST_SUCCESS) {
		xil_printf("Initialized spi!\n");
	}

	XSpi_Reset(&spi);

	u32 controlReg = XSpi_GetControlReg(&spi);
	XSpi_SetControlReg(&spi,
			(controlReg | XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK) &
			(~XSP_CR_TRANS_INHIBIT_MASK));

	XSpi_SetSlaveSelectReg(&spi, ~0x01);
}
/*..........................................................................*/
void QF_onStartup(void) {                 /* entered with interrupts locked */

/* Enable interrupts */

	XStatus Status;

	// Enable interrupt controller
	XIntc_Enable(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR);
	XIntc_Enable(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_ENCODER_IP2INTC_IRPT_INTR);

	// Start interrupt controller
	Status = XIntc_Start(&sys_intc, XIN_REAL_MODE);

	if (Status == XST_SUCCESS)
		xil_printf("Interrupt controller started\n\r");
	else
		xil_printf("Interrupt controller failed to start\n\r");

	// register handler with Microblaze
	microblaze_register_handler((XInterruptHandler)XIntc_DeviceInterruptHandler,
					(void*)XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);

	// Global enable of interrupt
	microblaze_enable_interrupts();

	// Enable interrupt on the GPIO
	XGpio_SetDataDirection(&btn, 1, 0xFFFFFFFF);
	XGpio_InterruptEnable(&btn, 1);
	XGpio_InterruptGlobalEnable(&btn);

	XGpio_SetDataDirection(&encoder, 1, 0xFFFFFFFF);
	XGpio_InterruptEnable(&encoder, 1);
	XGpio_InterruptGlobalEnable(&encoder);

	xil_printf("Finished Initialization\r\n");

	read_fsl_values(q, 4096); //start pipeline
	stream_grabber_start();
	makeTrigLUT(SAMPLES);


}


void QF_onIdle(void) {        /* entered with interrupts locked */

//
    QF_INT_UNLOCK();                       /* unlock interrupts */

    {

		if (inHomeScreen) {

			sample_f = (int)100000000 >> (11 + octaveOffsets[octave]);

			//zero w array
			memcpy(w, zero, sizeof(float)*SAMPLES);

			frequency = fft(q, w, SAMPLES, M, sample_f);
			if (octave <= 5)
				frequency += 14;

			if (octave == 7){

			if (frequency >= 2180 && frequency <= 2200)
				frequency += 30;
			if (frequency >= 2465 && frequency <= 2475)
				frequency += 20;
			if (frequency >= 2758 && frequency <= 2775)
				frequency += 30;
			if (frequency >= 2940 && frequency <= 2950)
				frequency += 15;
			if (frequency >= 3700 && frequency <= 3710)
				frequency += 22;
			if (frequency >= 3920 && frequency <= 3940)
				frequency += 20;
			}


			if (octave == 6){

			if (frequency >= 1110 && frequency <= 1127)
				frequency -= 12;
			if (frequency >= 1325 && frequency <= 1333)
				frequency -= 10;
			if (frequency >= 1145 && frequency <= 1155)
				frequency += 25;
			if (frequency >= 1230 && frequency <= 1240)
				frequency += 10;
			if (frequency >= 1400 && frequency <= 1415)
				frequency -= 13;
			if (frequency >= 1430 && frequency <= 1465)
				frequency += 30;
			if (frequency >= 1639 && frequency <= 1650)
				frequency += 16;

			if (frequency >= 1980)
				frequency -= 12;

			}


			if (octave == 5){
				frequency += 10;
			if (frequency >= 523 && frequency <= 531)
				frequency -= 7;
			if (frequency >= 591 && frequency <= 596)
				frequency -= 7;
			if (frequency >= 660 && frequency <= 666)
				frequency -= 5;
			if (frequency >= 690 && frequency <= 697)
				frequency += 5;
			if (frequency >= 870)
				frequency += 6;
			if (frequency >= 915)
				frequency += 5;
			}

			if (octave == 4){
			if (frequency >= 334 && frequency <= 358)
				frequency -= 5;
			if (frequency >= 360 && frequency <= 369)
				frequency += 5;
			if (frequency >= 263 && frequency <= 287)
				frequency -= 5;
			}

			if (octave <= 2){
				frequency -= 10;
			if (frequency >= 103)
				frequency += 7;
			}
			if (octave == 3){
				frequency -= 3;
			if (frequency >= 176)
				frequency += 5;
			}
			findNote(frequency);

			//pipeline
			stream_grabber_wait_enough_samples(4096/sample_size);
			fill_samples(q, 4096, m, incr, sample_size);
			stream_grabber_start();

			QActive_postISR((QActive *)&AO_Lab2A, IDLE_SIG);
		}

    }
}

/* Do not touch Q_onAssert */
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_LOCK();
    for (;;) {
    }
}



 // interrupt handlers

void BtnHandler(void *CallbackRef) {
	XGpio_InterruptClear(&btn, 1);

	u32 data = XGpio_DiscreteRead(&btn, 1);

	if (data == 1) { //octave button
		QActive_postISR((QActive *)&AO_Lab2A, OCTAVE_CLICK);
	}
	else if (data == 2) { //A4 button
		QActive_postISR((QActive *)&AO_Lab2A, A4_CLICK);
	}
	else if (data == 16) { //set button
		QActive_postISR((QActive *)&AO_Lab2A, SET_CLICK);
	}
	else if (data == 4) { //hist button
		QActive_postISR((QActive *)&AO_Lab2A, HISTOGRAM_CLICK);
	}
	else if (data == 8) { //spec button
		QActive_postISR((QActive *)&AO_Lab2A, SPECTROGRAM_CLICK);
	}
}

void GpioHandler(void *CallbackRef) {

}

void TwistHandler(void *CallbackRef) {
	//XGpio_DiscreteRead( &twist_Gpio, 1);
	XGpio_InterruptClear(&encoder, 1);

	u32 data = XGpio_DiscreteRead( &encoder, 1);

	switch (state) {
	case (0):
			switch (data) {
			case (1):
					state = 1;
					break;
			case (2):
					state = 2;
					break;
			case (3):
					state = 0;
					break;
			}
			break;
	case (1):
			switch (data) {
			case (0):
					state = 3;
					break;
			case (1):
					state = 1;
					break;
			case (3):
					state = 0;
					break;
			}
			break;
	case (2):
			switch (data) {
			case (0):
					state = 4;
					break;
			case (2):
					state = 2;
					break;
			case (3):
					state = 0;
					break;
			}
			break;
	case (3):
			switch (data) {
			case (0):
					state = 3;
					break;
			case (1):
					state = 1;
					break;
			case (2):
					state = 5;
					break;
			}
			break;
	case (4):
			switch (data) {
			case (0):
					state = 4;
					break;
			case (1):
					state = 6;
					break;
			case (2):
					state = 2;
					break;
			}
			break;
	case (5):
			switch (data) {
			case (0):
					state = 3;
					break;
			case (2):
					state = 5;
					break;
			case (3):
					state = 0;
					QActive_postISR((QActive *)&AO_Lab2A, ENCODER_UP);
					break;
			}
			break;
	case (6):
			switch (data) {
			case (0):
					state = 4;
					break;
			case (1):
					state = 6;
					break;
			case (3):
					state = 0;
					QActive_postISR((QActive *)&AO_Lab2A, ENCODER_DOWN);
					break;
			}
	}
}

void read_fsl_values(float* q, int n) {
	stream_grabber_start();
	stream_grabber_wait_enough_samples(n);

	fill_samples(q, n, m, incr, sample_size);
}

void fill_samples(float *q, int n, int m, int incr, int sample_size) {
	int i;
	for (i = 0; i < n/sample_size; i+=incr) {
		int avg = 0;
		int j = 0;

		for (j = 0; j < incr; j++) {
			avg += stream_grabber_read_sample(i + j);
		}

		avg = avg >> m;

		q[i >> m] = 3.3 * avg/67108864.0;
	}
}

