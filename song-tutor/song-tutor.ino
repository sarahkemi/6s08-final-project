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
#include <Snooze.h>

//Our power saving options and constants, to the rescue!!

#define DORMANTINTERVAL 60000 * 20
uint32_t last_button_press = 0;

// Time for teensy sleep or deepsleep (ms). Change to whatever you need!
#define TEENSY_SLEEP_TIME 48  
SnoozeBlock config;

#define BUTTON_WHITE 23
#define BUTTON_GREEN 22

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    14  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// I2C Setup for IMU
#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

#define wifiSerial Serial1          // for ESP chip
#define WIFI_CH_PD_PIN 17


// Wifi options, constants, and variables
#define VERBOSE_WIFI true          // Verbose ESP8266 output
#define IOT true
#define SSID "6S08C"               // PUT SSID HERE
#define PASSWORD "6S086S08"         // PUT PASSWORD HERE
//#define SSID "MIT"               // PUT SSID HERE
//#define PASSWORD ""         // PUT PASSWORD HERE
uint32_t tLastIotReq = 0;       // time of last send/pull
String MAC = "";
String resp = "";

//concept: stop device from refreshing screen and moving to another song by turning on and off boolean to update selector class
bool movement = true;

String titles;
String chords;
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

class PowerMonitor {
  const float BATTERY_CAPACITY = 1000;     // battery capacity in mA-h
  const float BATTERY_VOLTAGE = 5.0;       // V
  
  // ALL CURRENTS IN MILLIAMPS! Fill in with your values
  const float OLED_ON_CURRENT = 24.4*0.85; // Make assumption about % of pixels lit
  const float OLED_SLEEP_CURRENT = 0.01;

  const float ACCEL_ON_CURRENT = 1.57;
  const float ACCEL_SLEEP_CURRENT = 1.29;
  const float GYRO_ON_CURRENT = 5.70;
  const float GYRO_SLEEP_CURRENT = 1.57;
  const float MAG_ON_CURRENT = 1.29;
  const float MAG_SLEEP_CURRENT = 0.77;
  
  const float GPS_ON_CURRENT = 24.5;
  const float GPS_SLEEP_CURRENT = 1.62;
  
  const float WIFI_NOTCONNECTED_CURRENT = 68.0;
  const float WIFI_DEEPSLEEP_CURRENT = 0.001;
  const float WIFI_MODEM_SLEEP_CURRENT = 15.0;

  const float TEENSY_ON_CURRENT = 3.83;       
  const float TEENSY_SLEEP_CURRENT = 0.11;
  const float TEENSY_DEEPSLEEP_CURRENT = 0.02;

  // All VOLTAGES IN VOLTS!  Fill in with your values 
  const float TEENSY_SUPPLY_VOLTAGE = 3.3;
  const float GPS_SUPPLY_VOLTAGE = 5.0;
  const float WIFI_SUPPLY_VOLTAGE = 3.3;
  const float OLED_SUPPLY_VOLTAGE = 3.3;
  const float IMU_SUPPLY_VOLTAGE = 3.3;

  // These hold state of the various components
  uint8_t state_teensy, state_gps, state_wifi, state_oled, state_imu;
  bool iswifi, isgps, isoled, isimu;    // do these exist (1) or not (0)
  long int timestamp;                   // current time (in millis)
  long int time_offset;                 // holds interval for when 
                                        // Teensy is sleeping, since millis() 
                                        // doesn't run during Teensy sleep

  float energyConsumed;     // running total of energy consumed, in mJ
  float batteryCapacity;    // calculated battery capacity, in mJ
  float percentEnergyLeft;  // percent of battery energy left

  public:

  /* Constructor. Takes in one 8-bit input, c, that determines which
  components are being used in our system. We assume Teensy is always present!
  Other components each take 1 bit, as shown below:   
        c: 0 0 0 0 IMU OLED GPS WIFI
        So 00001101 = 0x0D would mean GPS is not present */   
  PowerMonitor(uint8_t c)
  {     

    timestamp = millis();
    energyConsumed = 0;
    batteryCapacity = BATTERY_CAPACITY * BATTERY_VOLTAGE * 3600; // mJoules
    percentEnergyLeft = 100;  // Start out at 100%!


    iswifi = isgps = isoled = isimu = false;    // Assume no components
    if (c | 0x01 << 0) iswifi = true;     // Add components depending on bits in c
    if (c | 0x01 << 2) isoled = true;
    if (c | 0x01 << 3) isimu = true;    

    // initially, all components are on
    state_teensy = 2;   //0=deepsleep, 1 = sleep, 2 = normal
    state_wifi = 1;     //0=sleep, 1 = on aka modem-sleep
    state_oled = 1;     //0=sleep, 1 = on
    state_imu = 0x05; //[accel,gyro,mag], 0=sleep, 1 =on  - apart of our strategy!!
  }

  /* Determine the energy consumed since the last update. Updates 
  class variables energyConsumed and percentEnergyLeft */
  void updateEnergy() {
    long int dT = millis() + time_offset - timestamp; //Time since last update
    float pw = 0;   // power usage during this interval

    // Depending on state of each component and whether it is present, 
    // choose appropriate power setting
    if (state_teensy==2) {
      pw += TEENSY_ON_CURRENT * TEENSY_SUPPLY_VOLTAGE;
    } else if (state_teensy==1) {
      pw += TEENSY_SLEEP_CURRENT * TEENSY_SUPPLY_VOLTAGE;
    } else if (state_teensy==0) {
      pw += TEENSY_DEEPSLEEP_CURRENT * TEENSY_SUPPLY_VOLTAGE;
    }
    if (isoled) pw += (state_oled) ? OLED_ON_CURRENT*OLED_SUPPLY_VOLTAGE : OLED_SLEEP_CURRENT*OLED_SUPPLY_VOLTAGE;    
    if (isimu) {
      pw += (state_imu & 0x01) ? ACCEL_ON_CURRENT*IMU_SUPPLY_VOLTAGE : ACCEL_SLEEP_CURRENT*IMU_SUPPLY_VOLTAGE;    
      pw += (state_imu & (0x01 << 1)) ? GYRO_ON_CURRENT*IMU_SUPPLY_VOLTAGE : GYRO_SLEEP_CURRENT*IMU_SUPPLY_VOLTAGE;    
      pw += (state_imu & (0x01 << 2)) ? MAG_ON_CURRENT*IMU_SUPPLY_VOLTAGE : MAG_SLEEP_CURRENT*IMU_SUPPLY_VOLTAGE;    
    }

    // WiFi power depends on whether we are connected to AP, and based on 
    // how many transmit and receive operations we have undertaken
    if (iswifi) {
      if (state_wifi) {
        if (wifi.isConnected()) {
          pw += WIFI_MODEM_SLEEP_CURRENT*WIFI_SUPPLY_VOLTAGE;
          } else {
          pw += WIFI_NOTCONNECTED_CURRENT*WIFI_SUPPLY_VOLTAGE;
          }
      } else {
        pw += WIFI_DEEPSLEEP_CURRENT*WIFI_SUPPLY_VOLTAGE;
      }
    }
    float deltaEnergy = dT / 1000.0 * pw;
    energyConsumed += deltaEnergy;  // mJ
    percentEnergyLeft -= deltaEnergy / batteryCapacity * 100;
    timestamp = millis() + time_offset;
  }
  
  /* Set power mode of any particular component 
  Inputs:
   Arduino String p: can be "teensy", "wifi", "oled", "imu", "gps"
   8-bit integer m: value depends on component */
  void setPowerMode(String p, uint8_t m){
    // Before changing power mode, update the energy consumed for last interval
    updateEnergy();   

    // Teensy modes are 2: Normal, 1: Sleep, 0: DeepSleep
    if (p=="teensy") {
      if (m != state_teensy) {    // Only set mode if different from current state
        if (m==1) {
          state_teensy = m;
          Snooze.sleep(config);
          // Need to add offset b/c millis doesn't work during sleep
          time_offset += TEENSY_SLEEP_TIME; 
          updateEnergy();   // update energy again to capture energy during sleep
          state_teensy = 2;         
          } 
        else if (m==0) {
          state_teensy = m;
          Snooze.deepSleep(config);
          time_offset += TEENSY_SLEEP_TIME;
          updateEnergy();
          state_teensy = 2;
        }
      }
    }
    // OLED modes are 1: Normal, 0: Sleep
    else if (isoled && (p=="oled")) {
      if (m != state_oled) {
        if (m==1) {
          display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
        } else if (m==0) {
          display.ssd1306_command(SSD1306_DISPLAYOFF);
        }
        state_oled = m;
      }
    }
    // Wifi modes are 1: On, 0: DeepSleep
    else if (iswifi && (p=="wifi")) {
      if (m != state_wifi) {
        if (m==1) {
          digitalWrite(WIFI_CH_PD_PIN, HIGH);
        } else if (m==0) {
          digitalWrite(WIFI_CH_PD_PIN, LOW);
        }
        state_wifi = m;
      }
      
    }
    /*  IMU modes are more complicated because IMU has 3 peripherals. Thus we
      use the three least-significant bits in m to hold the desired state of
      each sensor: [0][0][0][0][0][accel][gyro][mag].  So 0x05 means accel and mag on, 
      gyro sleep */
    else if (isimu && (p=="imu")) {
      // Cannot have gyro ON and accel OFF.  Right now this is not
      // explicitly forbidden, but will not result in intended behavior,
      // and will create inaccuracy in estimated current.
      if (m != state_imu) {
        uint8_t tempRegValue=0;   // Hold register value
        uint8_t dbits = ~ (m ^ state_imu);    // Find which bits are different
        if  (dbits & (0x01 << 2) ) {    // If accel bit is diff
          tempRegValue = imu.xgReadByte(CTRL_REG6_XL);  // read accel register
          if (m & (0x01 << 2)) {    // if we want to turn ON,
            tempRegValue |= 0xE0;   // set bits 7:5 to 1, keep other bits
            imu.xgWriteByte(CTRL_REG6_XL, tempRegValue);
          } else {                  // turn off
            tempRegValue &= 0x1F;   // set bits 7:5 to 0, keep other bits
            imu.xgWriteByte(CTRL_REG6_XL, tempRegValue);
          }
        }
        if  (dbits & (0x01 << 1) ) {  // If gyro bit is different
          tempRegValue = imu.xgReadByte(CTRL_REG1_G);  
          if (m & (0x01 << 1)) {
            tempRegValue |= 0xE0;   //set bits 7:5 to 1, keep other bits
            imu.xgWriteByte(CTRL_REG1_G, tempRegValue);
          } else {
            tempRegValue &= 0x1F;   //set bits 7:5 to 0, keep other bits
            imu.xgWriteByte(CTRL_REG1_G, tempRegValue);
          }
        }
        if  (dbits & 0x01) {        // If mag bit is different
          tempRegValue = imu.mReadByte(CTRL_REG3_M);  
          if (m & 0x01) {
            tempRegValue &= 0xFC;   //set bits 1:0 to 0, keep other bits
            imu.mWriteByte(CTRL_REG3_M, tempRegValue);
          } else {
            tempRegValue |= 0x03;   //set bits 1:0 to 1, keep other bits
            imu.mWriteByte(CTRL_REG3_M, tempRegValue);
          }
        }
        state_imu = m;
      }
    }
  }
  // Returns energyConsumed for use by outside world
  float getEnergyConsumed() {
      return energyConsumed;
  }
// Returns percentEnergyLeft for use by outside world
  float getBatteryLeft() {
      return percentEnergyLeft;
  }
  float getTotalElapsedTime() {
    return millis() + time_offset;
  }
};

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
    tft.setTextSize(1);
    tft.setTextWrap(false);

    for(int i = 0; i < list_length; i++){
      tft.println(title_list[i]);
    };
    
    tft.println("=>");
    tft.print(title_list[i]);
    current_song=title_list[i];   //Sets the global variable to the song currently on the screen
    
    //scroll through the song choices

    if(pitch > lo_threshold){
      i = (i+1)%list_length;
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
      tft.setTextSize(2);
      tft.setTextWrap(true);
      tft.print(title_list[i]);
      tft.println(":");
      tft.println(chord_list[i]);

//    Printing the index to tell the players what chord they are on
      int spaces=0;
      int starting_index=0;
      while(chord_list[i].indexOf(" ",starting_index) != -1) {
        spaces++;
        starting_index=chord_list[i].indexOf(" ",starting_index)+1;
      }
      int chord_index=ind%(spaces+1);      //TODO finish this
      tft.println("Chord#: " + String(chord_index+1));
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
PowerMonitor pm(0x0F);


void setup() {
   // Display setup
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);

  pinMode(WIFI_CH_PD_PIN, OUTPUT);
  digitalWrite(WIFI_CH_PD_PIN, HIGH);
  config.setTimer(TEENSY_SLEEP_TIME);   


  //Button setup

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
    last_title_null = titles.indexOf("\n", last_title_null +1 );

    chord_list[i] = chords.substring(last_chord_null+1, chords.indexOf("\n", last_chord_null + 1));
    last_chord_null = chords.indexOf("\n", last_chord_null + 1);
    }
    }    
  }

}

void loop() {

      bool select_button = !digitalRead(BUTTON_GREEN);
      bool back_button = !digitalRead(BUTTON_WHITE);


  angle.update();
  if(movement){
      select.update(angle.pitch(),db_length);
    }

  if(millis() - last_button_press > DORMANTINTERVAL){
    //do sleep things here
      pm.setPowerMode("wifi",0);
      pm.setPowerMode("teensy",1);
      tft.fillScreen(ST7735_BLACK);
      movement = false;


      if((millis() - last_button_press > DORMANTINTERVAL) && (select_button || back_button)){
          pm.setPowerMode("wifi",1);
          pm.setPowerMode("teensy",2);
          tft.fillScreen(ST7735_BLACK);
          movement = true;
          delay(5000);

          pinMode(BUTTON_GREEN, INPUT);
          pinMode(BUTTON_WHITE, INPUT);

          pinMode(BUTTON_GREEN, INPUT_PULLUP);
          pinMode(BUTTON_WHITE, INPUT_PULLUP);
        }
    }  


  //do some green button magic
    
    if(select_button){
      last_button_press = millis();
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
      if(back_button){
      tft.fillScreen(ST7735_BLACK);
      movement = true;
      last_button_press = millis();
      }
      
        
  delay(50);
}

