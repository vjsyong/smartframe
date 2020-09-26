#define ENABLE_GxEPD2_GFX 1

//#include <GxEPD2_4G.h> // needs be first include
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include "futura.h"
#include "futura8.h"
#include "futura12.h"
#include "header.h"
#include <ds3231.h>
#include <Wire.h>

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60        /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR int bootCount = 0;


//4G doesn't support partial refresh on 750_T7
//GxEPD2_4G<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT/2> display(GxEPD2_750_T7(/*CS=5*/ SS, /*DC=*/ 4, /*RST=*/ 21, /*BUSY=*/ 19));
GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT/2> display(GxEPD2_750_T7(/*CS=5*/ SS, /*DC=*/ 4, /*RST=*/ 21, /*BUSY=*/ 19));

/* Place Holder for hardcoded
const char hourCoded[] = "EIGHT";
const char tenthCoded[] = "FORTY";
const char minuteCoded[] = "SIX"; */

const int hOffset = -60;
const int tOffset = 20;
const int mOffset = 100;

const int rotation = 3;

String verse1 = "Commit to the Lord whatever you do,";
String verse2 = "and he will establish your plans.";
String reference = "Proverbs 16:3";

const char *hour[12] = {"ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN", "ELEVEN", "TWELVE"};
const char *tenth[5] = {"O'", "TWENTY", "THIRTY", "FORTY", "FIFTY"};
const char *minute[9] = {"ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE"};
const char *teen[10] = {"TEN", "ELEVEN", "TWELVE", "THIRTEEN", "FOURTEEN", "FIFTEEN", "SIXTEEN", "SEVENTEEN", "EIGHTEEN", "NINETEEN"};
const char sharp[] = "SHARP"; //Special case for when minute is at 00

//Time Struct
ts t;
int lastMinute = 0;

int xPos(const char* string){
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(string, 0, 0, &tbx, &tby, &tbw, &tbh);
  return ((display.width() - tbw) / 2) - tbx;
}

int yPos(const char* string){
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(string, 0, 0, &tbx, &tby, &tbw, &tbh);
  return ((display.height() - tbh) / 2) - tby;
}

void renderPartialTextBox(const char* string,  int offset){
    //Set up Screen
    display.setRotation(rotation);
    int16_t tbx, tby; uint16_t tbw, tbh;
    //Set Up Window
    display.getTextBounds(string, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setPartialWindow(0, ((display.height() - tbh) / 2) + offset, display.width(), tbh);
    
    //Render box and text
    display.firstPage();
    do
    {
      display.setCursor(xPos(string), yPos(string) + offset);
      display.fillScreen(GxEPD_WHITE);
      display.setFont(&FuturaBookfont40pt7b);
      display.setTextColor(GxEPD_BLACK);
      display.print(string);
    }while (display.nextPage());
}

void drawTimePartial(int _hour, int _minute, bool fullUpdate = false){

    //Do a full refresh every 5 minutes
    if(_minute % 5 == 0){
      drawTimeFull();
      return;
    }
    
    //Draw Hour
    const char* hString = hour[_hour%12-1];
    //Draw Tenth, Teen or Sharp
    const char* tString;
    if(_minute == 0){ //Set Sharp
      tString = sharp;
    }else if (_minute < 10){ //Set "O'" Special Case
      tString = tenth[0];
    }else if (_minute >= 10 && _minute <20){//Set Teen
      tString = teen[_minute%10];
    }else{
      tString = tenth[_minute/10-1];
    }
    renderPartialTextBox(tString, tOffset);

    //Draw Minute
    const char* mString;
    if (_minute%10 == 0 || (_minute > 10 && _minute <20)){ //null cases
      mString = NULL; //null string
    }else{
      mString = minute[_minute%10-1];
    }

    renderPartialTextBox(hString, hOffset);
    renderPartialTextBox(tString, tOffset);
    if (mString != NULL){
      renderPartialTextBox(mString, mOffset);
    }
}


void drawTimeFull()
{
  DS3231_get(&t);
  int _hour = t.hour;
  int _minute = t.min;
//  int _hour = 9;
//  int _minute = 30;
  display.setRotation(rotation);
  
  display.setFullWindow();
  display.fillScreen(GxEPD_WHITE);
  display.firstPage();
  do
  {
    
    //Set up Screen
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FuturaBookfont40pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.drawBitmap(0,80, gImage_header, 480, 181, GxEPD_BLACK);  // Print Subscribers symbol (POSITION_X, POSITION_Y, IMAGE_NAME, IMAGE_WIDTH, IMAGE_HEIGHT, COLOR);
    display.drawLine(120, 280,   360, 280,   GxEPD_BLACK);  // Draw line (x0,y0,x1,y1,color)
    display.drawLine(120, 560,   360, 560,   GxEPD_BLACK);  // Draw line (x0,y0,x1,y1,color)
    //Draw Hour
    display.setCursor(xPos(hour[_hour%12-1]), yPos(hour[_hour%12-1]) + hOffset);
    display.print(hour[_hour%12-1]);

    if(_minute == 0){
      display.setCursor(xPos("SHARP"), yPos("SHARP") + tOffset);
      display.print("SHARP");
    }
    else if (_minute > 10 && _minute <20){
      display.setCursor(xPos(teen[_minute%10-1]), yPos(teen[_minute%10-1]) + tOffset);
      display.print(teen[_minute%10-1]);     
    }else{
      if (_minute < 10 ){
        display.setCursor(xPos(tenth[0]), yPos(tenth[0]) + tOffset);
        display.print(tenth[0]); 
      }else{
        display.setCursor(xPos(tenth[_minute/10-1]), yPos(tenth[_minute/10-1]) + tOffset);
        display.print(tenth[_minute/10-1]); 
      }
      if(_minute%10 != 0){
        display.setCursor(xPos(minute[_minute%10-1]), yPos(minute[_minute%10-1]) + mOffset);
        display.print(minute[_minute%10-1]);
      }
    }
    
    display.setFont(&futuralight12pt7b);
    display.setCursor(60, 620);
    display.print(verse1);
    display.setCursor(85, 650);
    display.print(verse2);
    display.setCursor(175, 680);
    display.print(reference);
  }
  while (display.nextPage());
  
  Serial.println("Draw Time Complete");
}

 
void setup() {
  delay(100);
  display.init(115200);

  
  
  //Draw frame header
  display.setRotation(rotation);


  Wire.begin(22, 15);
  DS3231_init(DS3231_INTCN);
  DS3231_get(&t);

  //Draw a full frame on startup
  lastMinute = t.min;
  drawTimeFull();
  delay(10000);
}

void loop() {
  // put your main code here, to run repeatedly:
  DS3231_get(&t);
  if (t.min != lastMinute){
    //drawFrame();
    drawTimePartial(t.hour, t.min);
    lastMinute = t.min;
  }
  delay(5000);
//  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
//  esp_deep_sleep_start();
}
