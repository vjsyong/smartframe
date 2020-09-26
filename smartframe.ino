#define ENABLE_GxEPD2_GFX 1

//#include <GxEPD2_4G.h> // needs be first include
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include "futura.h"
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

const int hOffset = -80;
const int tOffset = 0;
const int mOffset = 80;

const char verse[] = "Commit to the Lord whatever you do, and he will establish your plans.";

const char *hour[12] = {"ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN", "ELEVEN", "TWELVE"};
const char *tenth[5] = {"O'", "TWENTY", "THIRTY", "FORTY", "FIFTY"};
const char *minute[10] = {"ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN"};
const char *teen[10] = {"TEN", "ELEVEN", "TWELVE", "THIRTEEN", "FOURTEEN", "FIFTEEN", "SIXTEEN", "SEVENTEEN", "EIGHTEEN", "NINETEEN"};
const char sharp[] = "SHARP"; //Special case for when minute is at 00

//Time Struct
ts t;

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

    int16_t tbx, tby; uint16_t tbw, tbh;
    //Set Up Window
    display.getTextBounds(string, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setPartialWindow(0, ((display.height() - tbh) / 2) + offset, display.width(), tbh);
    display.setCursor(xPos(string), yPos(string) + offset);
    //Render box and text
    display.firstPage();
    do
    {
      display.fillScreen(GxEPD_WHITE);
      display.setFont(&FuturaBookfont40pt7b);
      display.setTextColor(GxEPD_BLACK);
      display.print(string);
    }while (display.nextPage());
}

void drawTimePartial(int _hour, int _minute){
    
    //Draw Hour
    const char* hString = hour[_hour%12-1];
    renderPartialTextBox(hString, hOffset);

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
    if (mString != NULL){
      renderPartialTextBox(mString, mOffset);
    }
    
    
}


void drawTime(int _hour, int _minute){
    
    //Set up Screen
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FuturaBookfont40pt7b);
    display.setTextColor(GxEPD_BLACK);

    //Draw Hour
    display.setCursor(xPos(hour[_hour%12-1]), yPos(hour[_hour%12-1]) - 80);
    display.print(hour[_hour%12-1]);

    if(_minute == 0){
      display.setCursor(xPos("SHARP"), yPos("SHARP"));
      display.print("SHARP");
    }
    else if (_minute > 10 && _minute <20){
      display.setCursor(xPos(teen[_minute%10-1]), yPos(teen[_minute%10-1]));
      display.print(teen[_minute%10-1]);     
    }else{
      if (_minute < 10 ){
        display.setCursor(xPos(tenth[0]), yPos(tenth[0]));
        display.print(tenth[0]); 
      }else{
        display.setCursor(xPos(tenth[_minute/10-1]), yPos(tenth[_minute/10-1]));
        display.print(tenth[_minute/10-1]); 
      }
      
      display.setCursor(xPos(minute[_minute%10-1]), yPos(minute[_minute%10-1]) + 80);
      display.print(minute[_minute%10-1]);
    }

    
    
}

void drawFrame()
{
  DS3231_get(&t);
  Serial.println("Draw Time");
  display.setRotation(3);
  
  display.setFullWindow();
  //display.setPartialWindow(0, 240, display.width(), 280);
  display.fillScreen(GxEPD_WHITE);
  display.firstPage();
  do
  {
    //drawTimePartial(t.hour, t.min);
    drawTime(t.hour, t.min);
    //Draw verse
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(10, 560);
    //display.print(verse);
  }
  while (display.nextPage());
  
  Serial.println("Draw Time Complete");
}

void testPartial(char* string, int offset){
  //DS3231_get(&t);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.setRotation(3);
  display.setFont(&FuturaBookfont40pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.getTextBounds(string, 0, 0, &tbx, &tby, &tbw, &tbh);
  Serial.println(((display.width() - tbw) / 2) - tbx);
  Serial.println(((display.height() - tbh) / 2) - tby);
  Serial.println(tbw);
  Serial.println(tbh);
  //display.setPartialWindow(((display.width() - tbw) / 2) - tbx, ((display.height() - tbh) / 2) + offset, tbw, tbh);
  display.setPartialWindow(((display.width() - tbw) / 2), ((display.height() - tbh) / 2) + offset, display.width(), tbh);
  
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_BLACK);
    display.setCursor(((display.width() - tbw) / 2) - tbx, ((display.height() - tbh) / 2) - tby + offset);
    display.setFont(&FuturaBookfont40pt7b);
    display.setTextColor(GxEPD_WHITE);
    display.print(string);
  }while (display.nextPage());
}

 
void setup() {
  // put your setup code here, to run once:
  
  delay(100);
  display.init(115200);
  
  //display.epd2.clearScreen();
//  testPartial("TWELVE", 0);
//  delay(5000);
//  testPartial("TEN", 100);
//
//  delay(5000);
//  testPartial("ELEVEN", 0);
//  delay(5000);
//  testPartial("FIVE", 100);
  
  Wire.begin(22, 15);
  DS3231_init(DS3231_INTCN);
}

void loop() {
  // put your main code here, to run repeatedly:
  drawFrame();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}
