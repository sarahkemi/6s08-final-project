#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN_G 2
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

  chord(g);
}

void chord(int pattern[]) {

int frets[5][3] = {{100,100,100},{0,1,2},{5,6,7},{8,9,10},{13,14,15}};
int stringsLength = 4;
Adafruit_NeoPixel strings[4] = {string_g,string_c ,string_e,string_a};

//clear all the strings

for(int i=0; i < 16; i++){
  string_g.setPixelColor(i, 0, 0, 0);
  string_c.setPixelColor(i, 0, 0, 0);
  string_e.setPixelColor(i, 0, 0, 0);
  string_a.setPixelColor(i, 0, 0, 0);
  }
string_g.show();
string_c.show();
string_e.show();
string_a.show();

//create fret seperators
int seperators[] = {3,4,11,12};
//
for(int x=0; x < 4;x++){
  for(int i=0; i < 4; i++){
    strings[x].setPixelColor(seperators[i],0,0,255);
  }
}
//strings[0].setPixelColor(seperators[0],0,0,255);
//strings[0].setPixelColor(seperators[1],0,0,255);
//for(int x = 0;x < sizeof(strings);x++) {
//  strings[x].setPixelColor(seperators[0],0,0,255);
//}
//int index =0;
//while(index < stringsLength){
//  strings[index].setPixelColor(seperators[0],0,0,255);
//  index++;
//}
    

//for loop iterates through the pattern and looks for string that has a numbered fret, and then lights up that fret according to our matrix of leds
  for(int i=0; i < sizeof(pattern); i++){
    if(pattern[i] > 0){
  
      for(int x=0; x < 3; x++){
         strings[i].setPixelColor(frets[pattern[i]][x], 255, 0, 0);
        }
  
      }     
    } 

    string_g.show();
    string_c.show();
    string_e.show();
    string_a.show();
}

