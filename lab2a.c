
#define AO_LAB2A

#include <stdlib.h>
#include "qpn_port.h"
#include "bsp.h"
#include "lab2a.h"
#include "lcd.h"
#include <string.h>
#include <stdbool.h>

bool inHomeScreen = false;
char fr[9] = "Note: ";
char note[3] = "";

extern float frequency;
extern int octave;
extern int m;
extern int incr;
extern int sample_size;
extern int a4;
extern int error;
extern int binspacing;
extern int maxfreq;

void adjust_fft_func_values(int octave);

//Lab2A State machine
typedef struct Lab2ATag  {
	QActive super;
}  Lab2A;

/* Setup state machines */
static QState Lab2A_initial (Lab2A *me);
static QState Lab2A_on      (Lab2A *me);
static QState homeMenu  (Lab2A *me);
static QState octaveMenu  (Lab2A *me);
static QState a4Menu	(Lab2A *me);
static QState settingMenu	(Lab2A *me);
static QState histogramMenu	(Lab2A *me);
//static QState spectrogramMenu	(Lab2A *me);

Lab2A AO_Lab2A;

void Lab2A_ctor(void)  {
	Lab2A *me = &AO_Lab2A;
	QActive_ctor(&me->super, (QStateHandler)&Lab2A_initial);
}

QState Lab2A_initial(Lab2A *me) {
	xil_printf("\n\rInitialization");
    return Q_TRAN(&Lab2A_on);
}

QState Lab2A_on(Lab2A *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			xil_printf("\n\rOn");
			}
			
		case Q_INIT_SIG: {
			initLCD();
			clrScr();
			drawBackGround();
			return Q_TRAN(&homeMenu);
			}
	}
	
	return Q_SUPER(&QHsm_top);
}

QState homeMenu(Lab2A *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			inHomeScreen = true;
			strcat(fr, note);
			drawHomeScreen(fr, frequency, error);
			strcpy(fr, "Note: ");
			xil_printf("\n\rStartup homeMenu\n\r");
			return Q_HANDLED();
		}
		
		case ENCODER_UP: {
			return Q_HANDLED();
		}

		case ENCODER_DOWN: {
			return Q_HANDLED();
		}

		case IDLE_SIG: {
			strcat(fr, note);
			drawNote(fr);
			drawOctave();
			drawFrequency(frequency);
			drawError(error);
			drawErrorBar(error);
			strcpy(fr, "Note: ");
			return Q_HANDLED();
		}

		/*case OCTAVE_CLICK:  {
			xil_printf("Changing to Octave Menu\n\r");
			clearHomeScreen();
			inHomeScreen = false;
			return Q_TRAN(&octaveMenu);
		}

		case A4_CLICK: {
			xil_printf("Changing to A4 Menu\n\r");
			clearHomeScreen();
			inHomeScreen = false;
			return Q_TRAN(&a4Menu);
		}

		case SET_CLICK: {
			return Q_HANDLED();
		}*/

		case OCTAVE_CLICK:  {
			return Q_HANDLED();
		}

		case A4_CLICK: {
			return Q_HANDLED();
		}

		case SET_CLICK: {
			xil_printf("Changing to Settings Menu\n\r");
			clearHomeScreen();
			inHomeScreen = false;
			return Q_TRAN(&settingMenu);
		}
		case HISTOGRAM_CLICK:  {
			return Q_HANDLED();
		}

		case SPECTROGRAM_CLICK: {
			return Q_HANDLED();
		}

	}

	return Q_SUPER(&Lab2A_on);

}

QState settingMenu(Lab2A *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			xil_printf("Entering Setting Menu\n\r");
			//drawOctaveSelection(octave);
			drawSettingScreen();
			return Q_HANDLED();
		}

		case ENCODER_UP: {
			return Q_HANDLED();
		}

		case ENCODER_DOWN: {
			return Q_HANDLED();
		}

		case OCTAVE_CLICK:  {
			xil_printf("Changing to Octave Menu\n\r");
			clearHomeScreen();

			return Q_TRAN(&octaveMenu);
		}

		case A4_CLICK: {
			xil_printf("Changing to A4 Menu\n\r");
			clearHomeScreen();

			return Q_TRAN(&a4Menu);
		}

		case SPECTROGRAM_CLICK: {
			xil_printf("Changing to Home Menu\n\r");
			clearHomeScreen();

			return Q_TRAN(&homeMenu);
		}
		case HISTOGRAM_CLICK:  {
			xil_printf("Changing to Histogram Menu\n\r");

			clearHomeScreen();

			return Q_TRAN(&histogramMenu);
		}

	//	case SPECTROGRAM_CLICK: {
	//		xil_printf("No Spectrogram Menu Yet\n\r");
		//	return Q_HANDLED();
			//clearHomeScreen();


			//return Q_TRAN(&spectrogramMenu);
		//}
	}

	return Q_SUPER(&Lab2A_on);

}

QState histogramMenu(Lab2A *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			xil_printf("Entering Histogram Menu\n\r");

			drawHistogramScreen();
			return Q_HANDLED();
		}

		case ENCODER_UP: {
			drawHistogramScreen();
			return Q_HANDLED();
		}

		case ENCODER_DOWN: {
			drawHistogramScreen();
			return Q_HANDLED();
		}

		case SPECTROGRAM_CLICK: {
			xil_printf("Changing to Home Menu\n\r");
			clearHomeScreen();

			return Q_TRAN(&homeMenu);
		}

	}

	return Q_SUPER(&Lab2A_on);

}
QState octaveMenu(Lab2A *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			xil_printf("Entering Octave Menu\n\r");
			drawOctaveSelection(octave);
			return Q_HANDLED();
		}
		
		case ENCODER_UP: {
			xil_printf("Octave up\n\r");
			if (octave < 7) {
				octave++;
				updateOctave(octave);
				adjust_fft_func_values(octave);
			}
			return Q_HANDLED();
		}

		case ENCODER_DOWN: {
			xil_printf("Octave down\n\r");
			if (octave > 2) {
				octave--;
				updateOctave(octave);
				adjust_fft_func_values(octave);
			}
			return Q_HANDLED();
		}

		case SPECTROGRAM_CLICK:  {
			xil_printf("Changing to home screen\n\r");
			clearOctaveScreen();
			return Q_TRAN(&homeMenu);
		}

		case A4_CLICK: {
			return Q_HANDLED();
		}

		case OCTAVE_CLICK:
			return Q_HANDLED();

	}

	return Q_SUPER(&Lab2A_on);

}

QState a4Menu(Lab2A *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			xil_printf("Entering A4 Final\n\r");
			tuneA4(a4);
			return Q_HANDLED();
		}

		case ENCODER_UP: {
			xil_printf("A4 up\n\r");
			if (a4 < 460) {
				a4++;
				updateA4(a4);
			}
			return Q_HANDLED();
		}

		case ENCODER_DOWN: {
			xil_printf("A4 down\n\r");
			if (a4 > 420) {
				a4--;
				updateA4(a4);
			}
			return Q_HANDLED();
		}

		case SPECTROGRAM_CLICK:  {
			xil_printf("Changing to home screen\n\r");
			clearA4Screen();
			return Q_TRAN(&homeMenu);
		}

		case OCTAVE_CLICK: {
			return Q_HANDLED();
		}
		case A4_CLICK:
			return Q_HANDLED();

		}

	return Q_SUPER(&Lab2A_on);

}

void adjust_fft_func_values(int octave) {
	switch(octave) {
		case 2:
		case 3: {
			m = 5;
			incr = 32;
			sample_size = 1;
			break;
		}
		case 4:
		case 5: {
			m = 4;
			incr = 16;
			sample_size = 2;
			break;
		}
		case 6:
		case 7: {
			m = 2;
			incr = 4;
			sample_size = 8;
			break;
		}
	}
}

/* set up functions */
void setBackGroundColor() {
	setColor(64, 64, 64);
}

void setDrawingBgColor() {
	setColorBg(64, 64, 64);
}

void drawBackGround() {
	setBackGroundColor();
	fillRect(20, 20, 220, 300);
}
/* set up functions */

/* homeMenu draw functions */
void drawHomeScreen(char* note, float frequency, int error) {
	drawFrequencyLabel();
	drawFrequency(frequency);
	drawNote(note);
	drawError(error);
}
void drawHistogramScreen(){
	setFont(BigFont);
	setDrawingBgColor();
	setColor(255, 255, 255);
	char frqStr[6] = "0000";
	char binStr[2] = "00";
	itoa(maxfreq, frqStr, 10);
	itoa(binspacing, binStr, 10);
	lcdPrint(frqStr, 150, 50);
	lcdPrint(binStr, 180, 80);
	lcdPrint("MaxFreq:", 25, 50);
	lcdPrint("BinSpace:", 25, 80);
	setColor(0, 255, 0);
	//fillRect(120, 195, 120, 235);
	for(int i = 0; i < 60; i++){
		if(i < 10){
			fillRect(i + 60, 260, i + 61, 260 - i);
		}
		else if(10 <= i < 14){
					fillRect(i + 60, 260, i + 61, (260 - i) - (i - 9)*3);
				}
	}

	//xil_printf("%d\n\r", maxfreq);
	//xil_printf("%d", binspacing);

//

}
void drawSettingScreen() {
	setFont(BigFont);
	setDrawingBgColor();
	setColor(255, 255, 255);
	lcdPrint("Settings", 55, 35);
	lcdPrint("Menu", 85, 55);
	lcdPrint("Up: Octave", 45, 100);
	lcdPrint("Left: A4Tune", 25, 140);
	lcdPrint("Right: Debug", 25, 180);
	lcdPrint("Down: Back", 40, 220);

	setBackGroundColor();


}

void drawNote(char* note) {
	setFont(BigFont);
	setDrawingBgColor();
	setColor(255, 255, 255);
	lcdPrint(note, 40, 80);
	setBackGroundColor();
}

void drawOctave() {
	setFont(BigFont);
	setDrawingBgColor();
	setColor(255, 255, 255);
	char octStr;
	itoa(octave, octStr, 10);
	lcdPrint("Octave:", 40, 50);
	lcdPrint(octStr, 160, 50);
	setBackGroundColor();
}

void drawFrequency(float frequency) {
	setFont(SevenSegNumFont);
	setColor(255, 255, 255);
	setColorBg(64, 64, 64);
	char frequencyStr[6] = "0000";
	char temp = '0';
	int i;
	for (i = 3; i > -1; i--) {
		temp = (int)frequency%10 + 48;
		frequencyStr[i] = temp;
		frequency /= 10;
	}
	lcdPrint(frequencyStr, 40, 135);
	setBackGroundColor();

}

void drawFrequencyLabel() {
	setFont(BigFont);
	setColor(255, 255, 255);
	setDrawingBgColor();
	lcdPrint("Frequency", 40, 110);
	lcdPrint("Hz", 170, 168);
}

void drawError(int error) {
	setFont(BigFont);
	setColor(255, 255, 255);
	setDrawingBgColor();
	char errStr[8];
	if (error >= 0 ){
		lcdPrint("+", 50, 245);
		itoa(error, errStr, 10);
	}
	else{
		lcdPrint("-", 50, 245);
		itoa(-1*error, errStr, 10);
	}
	strcat(errStr, " ");
	if (errStr[1] == ' ')
		strcat(errStr, " cents");
	else
		strcat(errStr, "cents");
	lcdPrint(errStr, 70, 245);
	setBackGroundColor();
}

void drawErrorBar(int error) {
	setColor(255, 255, 0);
	fillRect(120, 195, 120, 235);
	if (error < 0) {
		setColor(255, 0, 0);
		fillRect(119 + (error), 205, 119, 225);
		setBackGroundColor();
		fillRect(121, 205, 170, 225);
		fillRect(118 + error, 205, 69, 225);
	} else {
		setColor(255, 0, 0);
		fillRect(121, 205, 121 + (error), 225);
		setBackGroundColor();
		fillRect(70, 205, 119, 225);
		fillRect(122 + error, 205, 171, 225);
	}
}

void clearHomeScreen() {
	setBackGroundColor();
	setDrawingBgColor();
	fillRect(20, 35, 220, 270);
}
/* homeMenu draw functions */

/* octaveMenu draw functions */
void drawOctaveSelection(int octave) {
	setFont(BigFont);
	setColor(255, 255, 255);
	setDrawingBgColor();
	lcdPrint("Octave:", 70, 100);
	setFont(SevenSegNumFont);
	char octaveStr[2];
	itoa(octave, octaveStr, 10);
	lcdPrint(octaveStr, 100, 130);
	setBackGroundColor();
}

void updateOctave(int octave) {
	setDrawingBgColor();
	setFont(SevenSegNumFont);
	setColor(255, 255, 255);
	char octaveStr[2];
	itoa(octave, octaveStr, 10);
	lcdPrint(octaveStr, 100, 130);
	setBackGroundColor();
}

void clearOctaveScreen() {
	setBackGroundColor();
	setDrawingBgColor();
	fillRect(70, 100, 220, 200);
}
/* octaveMenu draw functions */

/* tuneA4Menu draw functions */
void tuneA4(int frequency) {
	setFont(BigFont);
	setColor(255, 255, 255);
	setDrawingBgColor();
	lcdPrint("Tune A4:", 70, 100);
	setFont(SevenSegNumFont);
	char A4Str[4];
	itoa(frequency, A4Str, 10);
	lcdPrint(A4Str, 70, 130);
	setBackGroundColor();
}

void updateA4(int frequency) {
	setFont(SevenSegNumFont);
	setColor(255, 255, 255);
	setDrawingBgColor();
	char A4Str[4];
	itoa(frequency, A4Str, 10);
	lcdPrint(A4Str, 70, 130);
	setBackGroundColor();
}

void clearA4Screen() {
	setBackGroundColor();
	setDrawingBgColor();
	fillRect(70, 100, 220, 200);
}
/* tunea4Menu draw functions */
