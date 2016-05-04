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

#define BUTTON_WHITE 23
#define BUTTON_GREEN 22

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
#define SSID "MIT"               // PUT SSID HERE
#define PASSWORD ""         // PUT PASSWORD HERE
uint32_t tLastIotReq = 0;       // time of last send/pull
uint32_t tLastIotResp = 0;      // time of last response
String MAC = "";
String resp = "";

//concept: stop device from refreshing screen and moving to another song by turning on and off boolean to update selector class
bool movement = true;

String body;
String titles;
String chords;
int start;
int endhtml;
int start_titles;
int end_titles;
int start_chords;
int end_chords;
int start_length;
int end_length;
int db_length;

String title_list[50];
String chord_list[50];
int last_chord_null = -1;
int last_title_null = -1;


//MusicBuddy username & password
String mb_user = "sarah";
String mb_pass = "loves_uke";

//MusicBuddy action - for song finder, action is "song-finder"
String previous_song_title = "None";
String song_title = "Something Else";
String current_song = "Random";
int ind = 0;


//Initialize library classes
ESP8266 wifi = ESP8266(true);  //Change to "true" or nothing for verbose serial output
Adafruit_SSD1306 display(4);
LSM9DS1 imu;

class Selector
{
  int limit;
  float hi_threshold = 15;
  float lo_threshold = 15;
  int i;
  
  
  public:
  Selector(){
    i = 0;
  }
  void update(float pitch, int list_length){
    limit = list_length;
    tft.setCursor(0, 40);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextWrap(false);

    for(int i = 0; i < list_length; i++){
      tft.println(title_list[i]);
    };
    
    tft.println("=>");
    tft.print(title_list[i]);
    current_song=title_list[i];   //Sets the global variable to the song currently on the screen
    
    //scroll through the song choices

    if(pitch > lo_threshold){
//      if(i> limit){
//        i = 0;
//       }
//      else{
//        i +=1;
//      
//      }
      i = (i+1)%list_length;            //replaced with lines above cause more readable
      delay(50);
      tft.fillScreen(ST7735_BLACK);
    }
    
    if(pitch < -lo_threshold){
      if(i < 0){
      i = limit;
      }
      else{
      i -=1;
      }      
      delay(50);
      tft.fillScreen(ST7735_BLACK);
    }
  }
 
  void view_chords(){
      tft.fillScreen(ST7735_BLACK);
      tft.setCursor(0, 5);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextWrap(false);
//      tft.print(current_song);
      tft.print(title_list[i]);
      tft.println(":");
      tft.println(chord_list[i]);
//      delay(5000);
//      tft.fillScreen(ST7735_BLACK);

      
    }
};

class Angle
{
  float ax_cal;
  float ay_cal;
  float az_cal;
  float gx_cal;
  float gy_cal;
  float gz_cal;

  //float accel_scale = 32768.0//not needed!
  float gyro_scale = 245.0/32768.0;
  
  float acc_pitch;
  float gyro_pitch;
  float acc_roll;
  float gyro_roll;
  float predicted_pitch;
  float predicted_roll;
  unsigned long last_time;
  unsigned long new_time;
  float dt;
  float alpha = 0.95;

  public:
  Angle(){
    ax_cal=0;
    ay_cal=0;
    az_cal=0;
    gx_cal=0;
    gy_cal=0;
    gz_cal=0;
    acc_pitch=0;
    gyro_pitch=0;
    acc_roll=0;
    gyro_roll=0;
    last_time = 0;
    new_time = 0;
    dt = 0;
    predicted_pitch=0;
    predicted_roll=0;
    
  }

  void calibrate(){
//    display.clearDisplay();
//    display.setCursor(0,0);
//    display.setTextSize(1);
//    display.println("Don't touch");
//    display.println("");
//    display.print("Calibrating...");
//    display.display();

    
    for (int i = 0; i<100; i++){
      imu.readAccel();
      imu.readGyro();
      ax_cal+=0.01*imu.ax;
      ay_cal+=0.01*imu.ay;
      az_cal+=0.01*imu.az;  
      gx_cal+=0.01*imu.gx;
      gy_cal+=0.01*imu.gy;
      gz_cal+=0.01*imu.gz;
      delay(30);
    }
    az_cal-=16384.0; //gravitycorrection assuming 2g full scale!!!
    
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(ax_cal);
    display.println(ay_cal);
    display.println(az_cal);
    display.println(gx_cal);
    display.println(gy_cal);
    display.println(gz_cal);
    display.display();
    delay(800); 
  }

  void update(){
    new_time = millis();
    dt = (new_time-last_time)*0.001; //in seconds
    last_time = new_time;
    imu.readAccel();
    imu.readGyro();
    float ax=imu.ax-ax_cal;
    float ay=imu.ay-ay_cal;
    float az=imu.az-az_cal;
    //Gyro:
    float gx=(imu.gx-gx_cal)*gyro_scale;//into dps
    float gy=(imu.gy-gy_cal)*gyro_scale;
    float gz=(imu.gz-gz_cal)*gyro_scale;
    acc_pitch = -atan2(ay,az)*180/PI;
    acc_roll = -atan2(ax,az)*180/PI;
    predicted_pitch = alpha*(predicted_pitch + gx*dt)+(1-alpha)*acc_pitch;
    predicted_roll = alpha*(predicted_roll - gy*dt) + (1-alpha)*acc_roll; 

//    Serial.println(predicted_pitch);
//    Serial.println(predicted_roll);
  }
  float pitch(){
    return predicted_pitch;
  }
  float roll(){
    return predicted_roll;
  }
  
};

Angle angle;
Selector select;


void setup() {
   // Display setup
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);

  //button setup

  pinMode(BUTTON_GREEN, INPUT);
  pinMode(BUTTON_WHITE, INPUT);

  pinMode(BUTTON_GREEN, INPUT_PULLUP);
  pinMode(BUTTON_WHITE, INPUT_PULLUP);
  
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
    String path = "/student_code/matiash/dev1/sb2.py";                                                //Changed from aladetan's path

    //We need to define song-title and index variables
    String param = "username=" + mb_user + "&password=" + mb_pass + "&action=" + "song-finder";
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

    start_titles = resp.indexOf("<h1>");
    end_titles = resp.indexOf("</h1>", start_titles);
    titles = resp.substring(start_titles+5, end_titles-1);  

    start_chords = resp.indexOf("<h2>");
    end_chords = resp.indexOf("</h2>", start_chords);
    chords = resp.substring(start_chords+5, end_chords-1); 

    start_length = resp.indexOf("<p>");
    end_length = resp.indexOf("</p>", start_length);
    db_length = resp.substring(start_length+4, end_length-1).toInt();
    
    for(int i = 0; i < db_length; i++){
    title_list[i] = titles.substring(last_title_null +1, titles.indexOf("\n", last_title_null +1));
//    Serial.println("Title:");
//    Serial.println(title_list[i]);
    last_title_null = titles.indexOf("\n", last_title_null +1 );

    chord_list[i] = chords.substring(last_chord_null+1, chords.indexOf("\n", last_chord_null + 1));
//    Serial.println("Chord:");
//    Serial.println(chord_list[i]);
    last_chord_null = chords.indexOf("\n", last_chord_null + 1);
    }
    }
   

    
  }

}

void loop() {
  angle.update();
  if(movement){
      select.update(angle.pitch(),db_length);
    }


  //do some green button magic

    bool select_button = !digitalRead(BUTTON_GREEN);

    Serial.println("Button:");
    Serial.println(select_button);
    
    if(select_button){
      previous_song_title = song_title; //Careful - there could be some problems with the next 2 lines...
      song_title=current_song;
      if(previous_song_title==song_title) {
        ind=ind+1; //We'll account for going out of bounds in the requests.py file, unless we can find a way to parse the string of chords given by chord_list[i]
      }    
      else {
        ind=0; //means that we switched songs, so return ind to 0
      }

      if(wifi.isConnected() && !wifi.isBusy()){
          String domain = "iesc-s2.mit.edu";
          int port = 80;
          String path = "/student_code/matiash/dev1/sb2.py";                                                //Changed from aladetan's path
      
          String param = "username=" + mb_user + "&password=" + mb_pass + "&action=" + "send-pattern" + "&song-title=" + song_title + "&index=" + ind;
          wifi.sendRequest(POST, domain, port, path, param,true);
          delay(100);
      
      
      select.view_chords();
      movement = false;
      }
    }

  //do some white button magic

    bool back_button = !digitalRead(BUTTON_WHITE);
    
      if(back_button){
      movement = true;
      }
      
        
  delay(50);
}

