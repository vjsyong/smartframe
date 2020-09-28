# Smartframe - An ESP32 Powered EPaper Photoframe

Smartframe is a wifi connected e paper display insided a photo frame to make it blend in with other desk decorations.

## Introduction
So you're an aspiring engineer, and you meet a girl you really like. You may not be the best at verbally expressing your feelings or affections for her, what do you do? You develop a product for her. Nothing says "I really like you" more than  designing a custom built smart photo frame that has fancy wifi functions that pulls inspirations from the web but at the same time blends in with the rest of the table decor.  

### Features

  - Personalizable  header to gift to your significant other ;) 
  - Dynamically updated word clock that looks like paper that moves!
  - Automatically connect to wifi and pull a random quote/bible verse every 10 minutes for daily inspiration!

### Build

Wire the pins as such
| ESP32 | Good Display Driver Board |
| ------ | ------ |
|21 | Busy|
|19| RST|
| 5 | CS |
| 4 | D/C|
|15 | SCK|
|23| SDI|
|GND|GND|
|3.3v|3.3v|

| ESP32 | DS3231 Module|
| ------ | ------ |
| GND | GND |
|3.3v | VCC |
|27 | SDA |
|26| SCL|

### Setup

While this  project was purpose built for only one person in  mind, that  shouldn't stop you from attempting to build one yourself. The code in this project contains many hard-coded elements that rely on creating your own "C-array" bitmaps and rendering them. 

If you want to adapt this project for yourself. You need to do the following. 

1. Create a 480 * 181 pixel sized custom bitmap banner which will contain whatever message you wish to contain as part of the header. If your significant other happens to be named Amber too then you can probably skip this step.
2. Go to the  "credentials.h" page and set up the wifi networks you wish to connect to. Note that you can set up multiple networks in case the frame will move around. 
3. If you are not religeous or don't fancy having bible verses on  your frame, then comment out the getVerseFromAPI() function call in the setup and main loop. If you're savvy enough, you can modify the function to pull json files from wherever you wish.

### Dependencies 
Please install these libraries from the Arduino library manager or the Platform.io manager 
1. GxEPD2 by ZinggJM - HUGE shoutout to him. This  project would definitely not have been posssible without his work.
2. Adafruit_GFX
3. ArduinoJson - `YOU MUST INSTALL VERSION 5 AND UNDER.`

### Notes
-  EPaper displays are  known to have ghosting problems inbetween refreshes, so don't be alarmed if you see ghosting on your display. I have tried to strike a fine balance between user friendliness and performance by performing partial updates between minutes and a full update every 10 minutes.
-  If your device loses WiFi connection or cannot successfully connect to a network, an icon will display on the top right hand corner informing you of such. To remedy the issue, simply reset the microcontroller until the icon goes away.



License
----
GNU General Public License v3.0

**Go make your guy or girl happy now.**

