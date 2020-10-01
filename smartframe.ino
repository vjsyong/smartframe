#define ENABLE_GxEPD2_GFX 1

//#include <GxEPD2_4G.h> // needs be first include
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include "futura.h"
#include "futura8.h"
#include "futura12.h"
#include "header.h"
#include "nowifi.h"
#include "message.h"
#include <ds3231.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "credentials.h"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60        /* Time ESP32 will go to sleep (in seconds) */  
RTC_DATA_ATTR int bootCount = 0;


//4G doesn't support partial refresh on 750_T7
//GxEPD2_4G<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT/2> display(GxEPD2_750_T7(/*CS=5*/ SS, /*DC=*/ 4, /*RST=*/ 21, /*BUSY=*/ 19));
GxEPD2_BW < GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT / 2 > display(GxEPD2_750_T7(/*CS=5*/ SS, /*DC=*/ 4, /*RST=*/ 21, /*BUSY=*/ 19));

//Wifi multiconnection object
WiFiMulti wifiMulti;

//Manna API for random bible verses
const char* serverName = "http://beta.ourmanna.com/api/v1/get/?format=json&order=random";
String retrievedVerse;

const int hOffset = -60;
const int tOffset = 20;
const int mOffset = 100;
 
const int rotation = 3;

const int lineLimit = 32; //Max characters per line for bible verse
const int characterLimit = lineLimit * 4;

String strings[100]; // Max amount of strings anticipated
String renderContainer[5];
int renderCountainerCharCount[5];

String verseFull = "Here is a trustworthy saying that deserves full acceptance: Christ Jesus came into the world to save sinners—of whom I am the worst. ";
String verseReference = "1 Timothy 1:15";

const char *hour[12] = {"TWELVE", "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN", "ELEVEN"};
const char *tenth[5] = {"O'", "TWENTY", "THIRTY", "FORTY", "FIFTY"};
const char *minute[9] = {"ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE"};
const char *teen[10] = {"TEN", "ELEVEN", "TWELVE", "THIRTEEN", "FOURTEEN", "FIFTEEN", "SIXTEEN", "SEVENTEEN", "EIGHTEEN", "NINETEEN"};
const char sharp[] = "SHARP"; //Special case for when minute is at 00

//Time Struct
ts t;
int lastMinute = 0;


//Split String helper function
int split(String string, char c)
{
  String data = "";
  int bufferIndex = 0;

  for (int i = 0; i < string.length(); ++i) {
    char c = string[i];
    if (c != ' ')
      data += c;
    else {
      //data += '\0';
      strings[bufferIndex++] = data;
      data = "";
    }
  }
  return bufferIndex;
}

void parseVerse() {
  Serial.println("Splitting String Start");
  int count = split(verseFull,  ' ');
  Serial.println("Splitting String Complete");
  int charCount = 0;
  int i = 0;
  for (int j = 0; j < count; ++j)
  {
    charCount += strings[j].length();
    if (charCount >= lineLimit + i * lineLimit) { 
      i++;
      Serial.println("Breaking String into next array");
    }
    renderContainer[i] = renderContainer[i] + ' ' + strings[j];
  }
  Serial.println("Breaking String Complete");
//  for(int i = 0; i < 5; i++){
//    Serial.println(renderContainer[i]);
//  }

}

int xPos(const char* string) {
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(string, 0, 0, &tbx, &tby, &tbw, &tbh);

  return ((display.width() - tbw) / 2) - tbx;
}

int yPos(const char* string) {
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(string, 0, 0, &tbx, &tby, &tbw, &tbh);
  return ((display.height() - tbh) / 2) - tby;
}


void renderPartialTextBox(const char* string,  int offset) {
  //Set up Screen
  display.setRotation(rotation);
  display.setFont(&FuturaBookfont40pt7b);
  int16_t tbx, tby; uint16_t tbw, tbh;
  //Set Up Window
  display.getTextBounds(string, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setPartialWindow(0, ((display.height() - tbh) / 2) + offset, display.width(), tbh);
  int x = xPos(string);
  int y = yPos(string);
  //Render box and text
  display.firstPage();
  do
  {
    display.setCursor(x, y + offset);
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FuturaBookfont40pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.print(string);
  } while (display.nextPage());
}

void drawTimePartial(int _hour, int _minute, bool fullUpdate = false) {

  //Do a full refresh every 5 minutes
  if (_minute % 10 == 0) {
    getVerseFromAPI();
    drawTimeFull();
    return;
  }

  if(wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected");
    display.setPartialWindow(420, 60, 30, 30);
    display.firstPage();
    do{
        display.drawBitmap(420, 60, gImage_nowifi, 30, 30, GxEPD_BLACK);
    }while (display.nextPage());
  }else{
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    display.setPartialWindow(420, 60, 30, 30);
    display.firstPage();
    do{
        display.drawBitmap(420, 60, gImage_nowifi, 30, 30, GxEPD_WHITE);
    }while (display.nextPage());
  }

  //Draw Hour
  const char* hString = hour[_hour % 12];
  //Draw Tenth, Teen or Sharp
  const char* tString;
  if (_minute == 0) { //Set Sharp
    tString = sharp;
  } else if (_minute < 10) { //Set "O'" Special Case
    tString = tenth[0];
  } else if (_minute >= 10 && _minute < 20) { //Set Teen
    tString = teen[_minute % 10];
  } else {
    tString = tenth[_minute / 10 - 1];
  }
  

  //Draw Minute
  const char* mString;
  if (_minute % 10 == 0 || (_minute > 10 && _minute < 20)) { //null cases
    mString = NULL; //null string
  } else {
    mString = minute[_minute % 10 - 1];
  }

  //renderPartialTextBox(hString, hOffset);
  renderPartialTextBox(tString, tOffset);
  if (mString != NULL) {
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
    display.drawBitmap(0, 80, gImage_header, 480, 181, GxEPD_BLACK); // Print Subscribers symbol (POSITION_X, POSITION_Y, IMAGE_NAME, IMAGE_WIDTH, IMAGE_HEIGHT, COLOR);
    if(wifiMulti.run() != WL_CONNECTED) {
      Serial.println("WiFi not connected!");
      display.drawBitmap(420, 60, gImage_nowifi, 30, 30, GxEPD_BLACK); // Print Subscribers symbol (POSITION_X, POSITION_Y, IMAGE_NAME, IMAGE_WIDTH, IMAGE_HEIGHT, COLOR);
    }
    display.drawLine(120, 280,   360, 280,   GxEPD_BLACK);  // Draw line (x0,y0,x1,y1,color)
    display.drawLine(120, 560,   360, 560,   GxEPD_BLACK);  // Draw line (x0,y0,x1,y1,color)
    //Draw Hour
    display.setCursor(xPos(hour[_hour % 12]), yPos(hour[_hour % 12]) + hOffset);
    display.print(hour[_hour % 12]);

    if (_minute == 0) {
      display.setCursor(xPos("SHARP"), yPos("SHARP") + tOffset);
      display.print("SHARP");

    }
    else if (_minute >= 10 && _minute < 20) {
      display.setCursor(xPos(teen[_minute % 10]), yPos(teen[_minute % 10]) + tOffset);
      display.print(teen[_minute % 10]);

    } else {
      if (_minute < 10 ) {
        display.setCursor(xPos(tenth[0]), yPos(tenth[0]) + tOffset);
        display.print(tenth[0]);

      } else {
        display.setCursor(xPos(tenth[_minute / 10 - 1]), yPos(tenth[_minute / 10 - 1]) + tOffset);
        display.print(tenth[_minute / 10 - 1]);

      }
      if (_minute % 10 != 0) {
        display.setCursor(xPos(minute[_minute % 10 - 1]), yPos(minute[_minute % 10 - 1]) + mOffset);
        display.print(minute[_minute % 10 - 1]);

      }

    }

    display.setFont(&futuralight12pt7b);

    
    int i = 0;
    while(renderContainer[i] != ""){
      display.setFont(&futuralight12pt7b);
      display.setCursor(xPos(renderContainer[i].c_str()), yPos(renderContainer[i].c_str()) + 200 + i*30);
      display.print(renderContainer[i].c_str());
      i++;
    }
    display.setCursor(xPos(verseReference.c_str()), yPos(verseReference.c_str()) + 200 + i*30);
    display.print(verseReference); 

  }
  while (display.nextPage());
}

void getVerseFromAPI(){
  bool withinCharLimit = true;
  do{
    if (WiFi.status() == WL_CONNECTED) {
      //Clear container
      for(int i = 0; i < 5; i++){
          renderContainer[i] =  "";
      }
      
      HTTPClient http;  //Object of class HTTPClient
      http.begin(serverName);
      int httpCode = http.GET();
      //Check the returning code                                                                  
      if (httpCode > 0) {
        // Parsing
        const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;
        DynamicJsonBuffer jsonBuffer(bufferSize);
        JsonObject& root = jsonBuffer.parseObject(http.getString());
        const char* verse = root["verse"]["details"]["text"];
        verseFull = String(verse);
        verseFull = verseFull + ' '; //pad an extra space
        const char* reference =  root["verse"]["details"]["reference"];
        verseReference = reference;
        if(strlen(verse) > characterLimit)
          withinCharLimit = false;
        else
          withinCharLimit = true;
      }
      http.end();   //Close connection
    }
  }while(!withinCharLimit);
  parseVerse();
}


void setup() {
  delay(1000);
  display.init(115200);

  

  //Draw message
  display.setRotation(rotation);

  display.setFullWindow();
  display.fillScreen(GxEPD_WHITE);
  display.firstPage();
  do
  {
    display.drawBitmap(0, 150, gImage_message, 480, 468, GxEPD_BLACK); 
  }while(display.nextPage());
  
  Wire.begin(27, 26);
  DS3231_init(DS3231_INTCN);
  DS3231_get(&t);

  //Draw a full frame on startup
  lastMinute = t.min;

//  //Connect to list of wifi networks
//  for(int i = 0; i < numberOfNetworks; i++){
//    wifiMulti.addAP(ssid[i], password[i]);
//    Serial.println(ssid[i]);
//    Serial.println(password[i]);
//  }

  wifiMulti.addAP("Yongs", "eb33aef3f6");
  Serial.println("Connecting Wifi...");
  if(wifiMulti.run() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
  }
  delay(10000);


  getVerseFromAPI();
  drawTimeFull();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  DS3231_get(&t);
  if (t.min != lastMinute) {
    drawTimePartial(t.hour, t.min);
    lastMinute = t.min;
  }
  delay(1000);
}
