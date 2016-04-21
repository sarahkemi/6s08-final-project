#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN_G 0
#define PIN_C 7
#define PIN_E 10
#define PIN_A 12

Adafruit_NeoPixel string_g = Adafruit_NeoPixel(16, PIN_G, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel string_c = Adafruit_NeoPixel(16, PIN_C, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel string_e = Adafruit_NeoPixel(16, PIN_E, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel string_a = Adafruit_NeoPixel(16, PIN_A, NEO_GRB + NEO_KHZ800);

void setup() {
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


}

void loop() {
  // put your main code here, to run repeatedly:
  int c[] = {0,0,0,3};
  int g[] = {0,2,3,2};
  int am[] = {2,0,0,0};
  int f[] = {2,1,0,0};
//  chord(g);
  chord(am);
//  chord(c);
//  chord(g);
//  chord(f);
}

void chord(int pattern[]) {

int frets[5][3] = {{400,500,500},{0,1,2},{5,6,7},{8,9,10},{13,14,15}};
Adafruit_NeoPixel strings[] = {string_g,string_c ,string_e,string_a};

//string_g.show();
//string_c.show();
//string_e.show();
//string_a.show();

  for(int i=0; i < sizeof(pattern); i++){

  if(pattern[i] > 0){

      if(pattern[i] == 1){
      strings[i].setPixelColor(0, 255, 0, 0);
      strings[i].setPixelColor(1, 255, 0, 0);
      strings[i].setPixelColor(2, 255, 0, 0);
      }

      if(pattern[i] == 2){
      strings[i].setPixelColor(5, 255, 0, 0);
      strings[i].setPixelColor(6, 255, 0, 0);
      strings[i].setPixelColor(7, 255, 0, 0);
      }

     if(pattern[i] == 3){
      strings[i].setPixelColor(8, 255, 0, 0);
      strings[i].setPixelColor(9, 255, 0, 0);
      strings[i].setPixelColor(10, 255, 0, 0);
      }

      if(pattern[i] == 4){
      strings[i].setPixelColor(13, 255, 0, 0);
      strings[i].setPixelColor(14, 255, 0, 0);
      strings[i].setPixelColor(15, 255, 0, 0);
      }


      }     
    } 

    string_g.show();
    string_c.show();
    string_e.show();
    string_a.show();

  delay(2000);
}

