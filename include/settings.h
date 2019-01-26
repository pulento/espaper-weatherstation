/**The MIT License (MIT)
Copyright (c) 2017 by Daniel Eichhorn
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
See more at http://blog.squix.ch
*/

#include <simpleDSTadjust.h>

// see text.h for available text resources. Also change WUNDERGRROUND_LANGUAGE below to fetch weather data in different language
#define LANG 'EN'
#include "text.h"

// Config mode SSID
const String CONFIG_SSID = "ESPaperConfig";

// Setup
String WIFI_SSID = "";
String WIFI_PASS = "";

const int UPDATE_INTERVAL_SECS = 20 * 60; // Update every 20 minutes

// OpenWeatherMap Settings as per https://docs.thingpulse.com/guides/espaper-plus-kit/#configuration-customization
// Sign up here to get an API key: https://docs.thingpulse.com/how-tos/openweathermap-key/
String DISPLAYED_CITY_NAME = "Santiago";
String OPEN_WEATHER_MAP_APP_ID = "";
/*
Go to https://openweathermap.org/find?q= and search for a location. Go through the
result set and select the entry closest to the actual location you want to display 
data for. It'll be a URL like https://openweathermap.org/city/2657896. The number
at the end is what you assign to the constant below.
 */
String OPEN_WEATHER_MAP_LOCATION_ID = "3871336";
/*
Arabic -> ar, Bulgarian -> bg, Catalan -> ca, Czech -> cz, German -> de, Greek -> el,
English -> en, Persian (Farsi) -> fa, Finnish -> fi, French -> fr, Galician -> gl,
Croatian -> hr, Hungarian -> hu, Italian -> it, Japanese -> ja, Korean -> kr,
Latvian -> la, Lithuanian -> lt, Macedonian -> mk, Dutch -> nl, Polish -> pl,
Portuguese -> pt, Romanian -> ro, Russian -> ru, Swedish -> se, Slovak -> sk,
Slovenian -> sl, Spanish -> es, Turkish -> tr, Ukrainian -> ua, Vietnamese -> vi,
Chinese Simplified -> zh_cn, Chinese Traditional -> zh_tw.
*/
String OPEN_WEATHER_MAP_LANGUAGE = "en";

#define UTC_OFFSET - 4
struct dstRule StartRule = {"CLST", Last, Sun, Dec, 2, 3600}; // Chile Summer Time = UTC/GMT -3 hours
struct dstRule EndRule = {"CLT", Last, Sun, Mar, 2, 0};       // Chile Time = UTC/GMT -4 hours

// Settings for Boston
// #define UTC_OFFSET -5
// struct dstRule StartRule = {"EDT", Second, Sun, Mar, 2, 3600}; // Eastern Daylight time = UTC/GMT -4 hours
// struct dstRule EndRule = {"EST", First, Sun, Nov, 1, 0};       // Eastern Standard time = UTC/GMT -5 hour

// values in metric or imperial system?
bool IS_METRIC = true;

// Change for 12 Hour/ 24 hour style clock
bool IS_STYLE_12HR = true;

// change for different ntp (time servers)
#define NTP_SERVERS "0.cl.pool.ntp.org", "1.cl.pool.ntp.org", "2.cl.pool.ntp.org"
// #define NTP_SERVERS "us.pool.ntp.org", "time.nist.gov", "pool.ntp.org"



/*
 * BUSY---->gpio4
 * RST---->gpio2
 * DC---->gpio5
 * CS---->gpio15
 * CLK---->gpio14
 * DIN---->gpio13
 * Buttons : Reset ( RST pins on esp ) ,
 * Flash ( GPIO-0 10K pull up )
 * User button ( GPIO-12 10K pull up )
 */
 /*
 Connect the following pins:
 Display  NodeMCU
 BUSY     D1
 RST      D2
 DC       D8
 CS       D3
 CLK      D5
 DIN      D7
 GND      GND
 3.3V     3V3
*/
/*
 * BUSY>gpio4 RST>gpio2 DC>gpio5 CS>gpio15 CLK>gpio14 DIN>gpio13
 */
#define CS 15  // D8
#define RST 5  // D4
#define DC 4   // D1
#define BUSY 16 // D2
#define USR_BTN D0 // D6


/***************************
 * End Settings
 **************************/
