#define ENABLE_GxEPD2_GFX 1

#include <GxEPD2_4G.h> // needs be first include
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include "futura.h"
//#include "Bitmaps4g800x480.h"
//#include "Bitmaps4g400x300.h"
//#include "poster.h"
#include "graffiti.h"
#include "heyo.h"

GxEPD2_4G<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT / 2> display(GxEPD2_750_T7(/*CS=5*/ SS, /*DC=*/ 4, /*RST=*/ 21, /*BUSY=*/ 19));

const char hour[] = "SIX";
const char tenth[] = "TWENTY";
const char minute[] = "FIVE";

const char verse[] = "Commit to the Lord whatever you do, and he will establish your plans.";

//const char *hour[12] = {"ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN", "ELEVEN", "TWELVE"};
//const char *tenth[4] = {"O'", "TWENTY", "THIRTY", "FORTY", "FIFTY"};
//const char *minute[10] = {"ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN"};
//const char *teen[10] = {"ELEVEN", "TWELVE", "THIRTEEN", "FOURTEEN", "FIFTEEN", "SIXTEEN", "SEVENTEEN", "EIGHTEEN", "NINETEEN"};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  delay(100);
  display.init(115200);

  //refreshDisplay(1);
  helloWorld();
  delay(4000);
  
  
  //showGreyLevels();
  
//  delay(1000);
  //display.epd2.clearScreen();
//  delay(1000);
  //display.epd2.drawImage_4G(gImage_triangles, 4, 0, 0, 400, 300, false, false, true);
  //display.epd2.writeImage_4G(gImage_heyo, 2, 0, 0, 800, 480, true, false, true);
  //display.epd2.refresh();
  //display.drawBitmap(0, 0, gImage_poster, display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_BLACK);
  //delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void refreshDisplay(int refreshes){
  display.setFullWindow();
  for(int i = 0; i < refreshes; i++){
    display.firstPage();
    do{
      display.fillRect(0, 0, display.width(), display.height(), GxEPD_WHITE);
    }while (display.nextPage());
    delay(100);
//    display.firstPage();
//    do{
//      display.fillRect(0, 0, display.width(), display.height(), GxEPD_BLACK);
//    }while (display.nextPage());
  }
}

void showGreyLevels()
{
  
  display.setRotation(0);
  uint16_t h = display.height() / 4;
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.fillRect(0, 0, display.width(), h, GxEPD_WHITE);
    display.fillRect(0, h, display.width(), h, GxEPD_LIGHTGREY);
    display.fillRect(0, 2 * h, display.width(), h, GxEPD_DARKGREY);
    display.fillRect(0, 3 * h, display.width(), h, GxEPD_BLACK);
  }
  while (display.nextPage());
  delay(2000);
}

int xPos(const char* string){
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(string, 0, 0, &tbx, &tby, &tbw, &tbh);
  Serial.println(((display.width() - tbw) / 2) - tbx);
  return ((display.width() - tbw) / 2) - tbx;
}

int yPos(const char* string){
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(string, 0, 0, &tbx, &tby, &tbw, &tbh);
  return ((display.height() - tbh) / 2) - tby;
}


void helloWorld()
{
  Serial.println("helloWorld");
  display.setRotation(3);
  display.setFont(&FuturaBookfont40pt7b);
  display.setTextColor(GxEPD_BLACK);
  //int16_t tbx, tby; uint16_t tbw, tbh;
  //display.getTextBounds(hour, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center bounding box by transposition of origin:
  //uint16_t x = ((display.width() - tbw) / 2) - tbx;
  //uint16_t y = ((display.height() - tbh) / 2) - tby;
  
  display.setFullWindow();
  display.setPartialWindow(0, 280, display.width(), 240);

  delay(2000);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    //display.setCursor(x, y);
    display.setCursor(xPos(hour), yPos(hour) - 80);
    display.print(hour);
    display.setCursor(xPos(tenth), yPos(tenth));
    display.print(tenth);
    display.setCursor(xPos(minute), yPos(minute) + 80);
    display.print(minute);
  }
  while (display.nextPage());

  display.setPartialWindow(0, 540, display.width(), 240);
  display.firstPage();
  do
  {
    //display.fillScreen(GxEPD_WHITE);
    display.setFont(&FreeSans9pt7b);
    //display.setCursor(x, y);
    display.setCursor(10, 560);
    display.print(verse);
  }
  while (display.nextPage());
  
  //Serial.println("helloWorld done");
}
