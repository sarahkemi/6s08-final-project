#include <math.h>
#include <Wire.h>
#include <SPI.h>
#include <ESP8266.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN_G 2
#define PIN_C 7
#define PIN_E 10
#define PIN_A 12

#define wifiSerial Serial1          // for ESP chip

// Wifi options, constants, and variables
#define VERBOSE_WIFI true          // Verbose ESP8266 output
#define IOT true
#define IOT_UPDATE_INTERVAL 10000  // How often to send/pull from cloud (ms)
#define SSID "MIT"               // PUT SSID HERE
#define PASSWORD ""         // PUT PASSWORD HERE
uint32_t tLastIotReq = 0;       // time of last send/pull
uint32_t tLastIotResp = 0;      // time of last response
String MAC = "";
String resp = "";

ESP8266 wifi = ESP8266(true);  //Change to "true" or nothing for verbose serial output

//MusicBuddy username & password
String mb_user = "sarah";
String mb_pass = "loves_uke";

//MusicBuddy action - for song tutor, action is "song-tutor"
String action = "song-tutor";

Adafruit_NeoPixel string_g = Adafruit_NeoPixel(16, PIN_G, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel string_c = Adafruit_NeoPixel(16, PIN_C, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel string_e = Adafruit_NeoPixel(16, PIN_E, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel string_a = Adafruit_NeoPixel(16, PIN_A, NEO_GRB + NEO_KHZ800);

void setup() {

  // Serial setup
  Serial.begin(9600);

  Serial.println("Setting up strings...");
  
  string_g.begin();
  string_g.show(); // Initialize all pixels to 'off'

  string_c.begin();
  string_c.show(); // Initialize all pixels to 'off'
  
  string_e.begin();
  string_e.show(); // Initialize all pixels to 'off'
  
  string_a.begin();
  string_a.show(); // Initialize all pixels to 'off'

  string_g.setBrightness(1);
  string_c.setBrightness(1);
  string_e.setBrightness(1);
  string_a.setBrightness(1);

  if (IOT) {
  wifi.begin();
  wifi.connectWifi(SSID, PASSWORD);
  while (!wifi.isConnected()); //wait for connection
  MAC = wifi.getMAC();
  Serial.println("Set up wifi..");
  }
 

}

void loop() {
   if (IOT && wifi.hasResponse()) {
    Serial.println("IOT");
    resp = wifi.getResponse();
    Serial.println(resp);
    Serial.println("Got wifi response!");
    tLastIotResp = millis();
    Serial.println("blah blah");
    //Serial.println("tLastIotResp:" + tLastIotResp);
Serial.println("blah blah2");

    int start_pattern = resp.indexOf("<p>");
    Serial.println("blah blah3");
    int end_pattern = resp.indexOf("</p>", start_pattern);
    Serial.println("blah blah4");
    String pattern_string = resp.substring(start_pattern+3, end_pattern);
    Serial.println("blah blah5");

    Serial.println("pattern_string:" + pattern_string);

    Serial.print((int)pattern_string[0]-48);
    Serial.print((int)pattern_string[1]-48);
    Serial.print((int)pattern_string[2]-48);
    Serial.println((int)pattern_string[3]-48);


    chord((int)pattern_string[0]-48,(int)pattern_string[1]-48,(int)pattern_string[2]-48,(int)pattern_string[3]-48);
    Serial.println("CHORD FINISHED");
  }

  if (IOT && (millis() - tLastIotReq >= IOT_UPDATE_INTERVAL)) {
    if (wifi.isConnected() && !wifi.isBusy()) { //Check if we can send request
        Serial.println("Starting request to server..");
      String domain = "iesc-s2.mit.edu";
      int port = 80;
      String path = "/student_code/aladetan/dev1/sb4.py";
      String param = "username=" + mb_user + "&password=" + mb_pass + "&action=" + action;

      Serial.println("blah blah sending");
      wifi.sendRequest(GET, domain, port, path, param);
      Serial.println("blah blah sent");
      tLastIotReq = millis();
      Serial.println("Sent request to server!");
//      String thing = wifi.getResponse();
      Serial.println("got wifi response to update");
    } 
//    else {
//      Serial.println("Wifi maybenot connected: " + String(wifi.isConnected()));
//      Serial.println("Wifi maybe busy: " + String(wifi.isBusy()));
//      Serial.println(String((millis() - tLastIotReq)));
//      }
  }

delay(200);
}

void chord(int g, int c, int e, int a) {
Serial.println("CHORD");


int pattern[4] = {g,c,e,a};

int frets[5][3] = {{100,100,100},{0,1,2},{5,6,7},{8,9,10},{13,14,15}};
Adafruit_NeoPixel strings[4] = {string_g,string_c ,string_e,string_a};

//clear all the strings
Serial.println("String: clear");
for(int i=0; i < 16; i++){
  string_g.setPixelColor(i, 0, 0, 0);
  string_c.setPixelColor(i, 0, 0, 0);
  string_e.setPixelColor(i, 0, 0, 0);
  string_a.setPixelColor(i, 0, 0, 0);
  }

//Serial.println("String: show");
//string_g.show();
//string_c.show();
//string_e.show();
//string_a.show();

Serial.println("String: complete");

//create fret seperators
int seperators[] = {3,4,11,12};
//
for(int x=0; x < 4;x++){
  for(int i=0; i < 4; i++){
    strings[x].setPixelColor(seperators[i],0,0,255);
  }
}

Serial.println("String: fret sep created");

//for loop iterates through the pattern and looks for string that has a numbered fret, and then lights up that fret according to our matrix of leds
  for(int i=0; i < 4; i++){
    if((int)pattern[i] > 0 && (int)pattern[i] < 5){
  
      for(int x=0; x < 3; x++){
         strings[i].setPixelColor(frets[(int)pattern[i]][x], 255, 0, 0);
        }
  
      }     
    } 

    Serial.println("String: numbered frets done");
    string_g.show();
    string_c.show();
    string_e.show();
    string_a.show();

    Serial.println("String: show all");
}

