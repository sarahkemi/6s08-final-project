// The SFE_LSM9DS1 library requires both Wire and SPI be
// included BEFORE including the 9DS1 library.
#include <math.h>
#include <Wire.h>
#include <SPI.h>
#include <ESP8266.h>
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SparkFunLSM9DS1.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    14  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     9

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// I2C Setup for IMU
#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

#define wifiSerial Serial1          // for ESP chip

// Wifi options, constants, and variables
#define VERBOSE_WIFI true          // Verbose ESP8266 output
#define IOT true
#define IOT_UPDATE_INTERVAL 10000  // How often to send/pull from cloud (ms)
#define SSID "6S08B"               // PUT SSID HERE
#define PASSWORD "6S086S08"         // PUT PASSWORD HERE
uint32_t tLastIotReq = 0;       // time of last send/pull
uint32_t tLastIotResp = 0;      // time of last response
String MAC = "";
String resp = "";

String body;
int start;
int endhtml;

//MusicBuddy username & password
String mb_user = "sarah";
String mb_pass = "loves_uke";

//MusicBuddy action - for song finder, action is "song-finder"
String action = "song-finder";

//Initialize library classes
ESP8266 wifi = ESP8266(true);  //Change to "true" or nothing for verbose serial output
Adafruit_SSD1306 display(4);
LSM9DS1 imu;


void setup() {
   // Display setup
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);

  
  // Serial setup
  Serial.begin(115200);
 
  // IMU setup
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  if (!imu.begin())
  {
    Serial.println("Failed to communicate with LSM9DS1.");
    Serial.println("Double-check wiring.");
    while (1);
  }

    // Wifi setup
  if (IOT) {
    wifi.begin();
    wifi.connectWifi(SSID, PASSWORD);
    while (!wifi.isConnected()); //wait for connection
    MAC = wifi.getMAC();
    delay(1000);

 //sending the request - if this doesn't work, put it back in the loop and go from there

    //maybe try changing the while to an if statement
    while(!wifi.hasResponse()){
  
    if(wifi.isConnected() && !wifi.isBusy()){
    String domain = "iesc-s2.mit.edu";
    int port = 80;
    String path = "/student_code/aladetan/dev1/sb4.py";
    String param = "username=" + mb_user + "&password=" + mb_pass + "&action=" + action;
    wifi.sendRequest(GET, domain, port, path, param); 
    delay(5000);
    }
    }

    if(wifi.hasResponse()){
  
    String resp;
  
    resp = wifi.getResponse();
  
    start = resp.indexOf("<html>");
    endhtml = resp.indexOf("</html>", start);
    body = resp.substring(start+7, endhtml-1);  
  
    }
   

    
  }

}

void loop() {
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextWrap(true);
  tft.print(body); 
}
