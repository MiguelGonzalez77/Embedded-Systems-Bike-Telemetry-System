#include <stdint.h>
#include "display.h"

// Timer4A
#include "/../inc//Timer4A.h"

#include "ReedSW.h"
 // 128 wide and 160 height for the LCD

#include "DPS310.h" 
#include "Heartbeat.h"
#include "./inc//UART.h"
 

#include <string.h>

#define GRAPH_WIDTH 128  // 128x128 graph
#define TEXTDATA 32 // banner 
#define GRAPH_HEIGHT 128
#define MAX_PULSE_FREQ 8000 // 100000 cycles refers to the period of tach signal
// given that then from calculate_Speed (really should be calculate_RPS, but dumb me,MAX rps = 80MHz/100000 = 8000 (Max RPS 
// freq in 0.1Hz

extern int Diameter;
uint16_t maxYpixel, minYpixel, X_coor; // x goes from 0-127 (128)
int32_t Y_coor_range;
uint64_t time;
int counter;
void formatNumber(int32_t n, int32_t numberLength, int decimalSpot, char* decimalString);
void displayUpdateTask(void){
		updateDisplay();
 }
 // Period for Lab 10: // low priority  80MHz /10 = 8000000
	// Runs every 0.1 second.
void displayInit(int period, int priority){
	
	// Initialize ST7735.
	ST7735_InitR(INITR_REDTAB);
	ST7735_SetTextColor(ST7735_BLUE);
	ST7735_FillScreen(ST7735_BLACK);
	
	// Initialize Graph.
	ST7735_DrawFastHLine(0, TEXTDATA - 1, GRAPH_WIDTH, ST7735_WHITE);
	ST7735_FillRect(0, TEXTDATA, GRAPH_WIDTH, GRAPH_HEIGHT, ST7735_WHITE); // not sure if I use my defined variables
	// GRAPH_WIDTH and HEIGHT or the LCD boundaries. This is meant to separate the banner and the graph.
	
	ST7735_SetCursor(0, 1);														// MPH VIEW
	ST7735_OutString("MPH: ");
	ST7735_SetCursor(7,2);
	ST7735_OutString("C        M");
	
	Timer4A_Init(&displayUpdateTask, period, priority);
 }
 
void ST7735_DrawGraph(int16_t vertical, uint16_t color){
		// Continuously update graph.
		X_coor = (X_coor + 1) % GRAPH_WIDTH; // increment X _coor for continuous graph update,
	
		// If at graph end, update graph screen.
		if (X_coor == 0){
			ST7735_FillRect(0, TEXTDATA, GRAPH_WIDTH, GRAPH_HEIGHT, ST7735_WHITE); // not sure if I use my defined variables
		}
	 ST7735_DrawPixel(X_coor, vertical, color);
	 ST7735_DrawPixel(X_coor + 1, vertical, color);
	 ST7735_DrawPixel(X_coor, vertical + 1, color);
	 ST7735_DrawPixel(X_coor + 1, vertical + 1, color);
 }
int scaleGraph(uint32_t currentRPSValue, int scalefactor){ // don't need to scale X coordinate because it is time, so 
	// it scales height to translate RPS values into pixel positions on the graph's vertical axis
	int pixelPos = (currentRPSValue * (GRAPH_HEIGHT - 1)) / scalefactor; // scales RPS value
	// to a pixel position on graph, multiplies the RPS value by height of graph then divides by max rps frequency, -1 ensures
	// pixel positions stays within 0 to GRAPH_HEIGHT - 1 (or 0-127).
	if(pixelPos > (GRAPH_HEIGHT - 1)){
		pixelPos = GRAPH_HEIGHT - 1;
	}
	else if (pixelPos < 0){
		pixelPos = 0;
	}
	int adjustedPos = TEXTDATA + (GRAPH_HEIGHT - pixelPos - 1); // adjusts pixel position to account for actual position of graph
	// on display. Since graph starts at TEXTDATA pixels down from the top of the screen, need to add TEXTDATA to calculated postion
	// also as vertical position is measure from bottom of graph area, subtract pixel position from Graph height - 1 for correct
	// 0-127
	return adjustedPos;
	
}
 
void updateDisplay(void){
	
		// Initialize values here.
		int reed_period;
		ReedSW_hasPressed(&reed_period);
		int mph = (Diameter * 3141593)/(reed_period * 176); // 24in wheel 0.1 mph units in17.6
	
		int heartbeat = get_heartbeat();
	
		// Scale values.
		int mphsc = scaleGraph(mph, 4000);
		int graph_heartbeat = scaleGraph(heartbeat, 4100);
		
		// Draw values here.
		ST7735_DrawGraph(mphsc, ST7735_RED);
		ST7735_DrawGraph(graph_heartbeat, ST7735_GREEN);
		
		// Output String statements here.
		
		ST7735_SetCursor(5,1);
		char speed[7];
		formatNumber(mph, 4, 3, speed);
		ST7735_OutString(speed+1);
	
		ST7735_SetCursor(0, 2);														// TEMP VIEW
		char temp[7];
		formatNumber(DPS310_getTemp(), 4, 3, temp);
		ST7735_OutString(temp+1);
		
		ST7735_SetCursor(8, 2);
		char alt[8];
		formatNumber(DPS310_altitudeApprox(), 5, 4, temp);
		ST7735_OutString(temp);
		

		ST7735_SetCursor(12,1);
		char hb[7];
		formatNumber(Heartbeat_getBPM(), 4, 4, hb);
		ST7735_OutString(hb+1);
 }
void Output_Value(int value, int y_cord, int x_cord, char* String){
	// String Output
	ST7735_SetCursor(x_cord, y_cord);
	ST7735_OutString(String);
	
	int string_length = strlen(String);
	
	// Value Output
	ST7735_SetCursor(x_cord + string_length + 1, y_cord);
	ST7735_OutUDec(value);
	
	// Value Clear
//	ST7735_SetCursor(x_cord + string_length, y_cord);
//	ST7735_OutString("    ");
}
 // Prints number into decimal on display
 // n - number to be printed
 // numberLength - # of digits that should output to display, not including sign or decimal point.
 // decimalSpot - location of decimal in printed number, corresponds to physical location
 #define NUM_CHARS (numberLength + 2)		// digits of number + sign + decimal
 void formatNumber(int32_t n, int32_t numberLength, int decimalSpot, char* decimalString) {
	 char signChar = ' ';
	 decimalString[NUM_CHARS] = 0;												// null termination
	 
		if (n < 0) {																// negative number correction, sign marked and number will be processed as positive
			signChar = '-';
			n = -n;																
		}
		
		int32_t maxNumber = 1;											// maxNumber = 10^numberLength
		for (int i = 0; i < numberLength; i++) {
			maxNumber *= 10;
		}
		
		if (n < maxNumber) {

			for (int i = NUM_CHARS - 1; i > decimalSpot; i--) {		// determine decimal digits
					decimalString[i] = n%10 + 0x30;
					n /= 10;
				}
			
			decimalString[decimalSpot] = '.';											// place point
				
			if (n == 0) {																					// determine whole digits
				decimalString[--decimalSpot] = '0';
			}
			else while (n > 0) {																				
					decimalString[--decimalSpot] = n%10 + 0x30;
					n /= 10;
					}
			
			decimalString[--decimalSpot] = signChar;							// place sign character
			
			for (int i = 0; i < decimalSpot; i++) {								// fill remaining spots with spaces
				decimalString[i] = ' ';
			}
		}
		else {																									// generate error string
			decimalString[0] = signChar;
			for (int i = 1; i < NUM_CHARS; i++) {
				decimalString[i] = (i == decimalSpot) ? '.' : '*';
			}
		}	
}
 

 