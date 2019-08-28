// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>

#define DEBUG 1
#ifndef DEBUG
  #define DEBUG_SETUP
  #define ENTER(f)
  #define PRINT1(v1)
  #define PRINT2(v1,v2)
  #define PRINT3(v1,v2,v3)
  #define LEAVE
  #define RETURN(v1,v2) return v2;
  //#define DEBUG_STACK
#else
  String DEBUG_STACK[50];
  uint16_t DEBUG_STACK_PTR=0;
  #define DEBUG_SETUP Serial.begin(9600);Serial.print("DEBUG Setup:");Serial.println("Complete");
  #define ENTER(f) Serial.println((String("Enter:")+f).c_str());DEBUG_STACK[DEBUG_STACK_PTR++]=String(f);
  #define PRINT1(v1) Serial.println(DEBUG_STACK[DEBUG_STACK_POINTER]+":"+v1);
  #define PRINT2(v1,v2) Serial.println((DEBUG_STACK[DEBUG_STACK_PTR]+":"+v1+v2).c_str());
  #define PRINT3(v1,v2,v3) Serial.println(DEBUG_STACK[DEBUG_STACK_PTR]+":"+v1+v2+v3);
  #define LEAVE Serial.println((String("Leave:")+DEBUG_STACK[--DEBUG_STACK_PTR]).c_str());
  #define RETURN(v1,v2) Serial.println(DEBUG_STACK[--DEBUG_STACK_PTR]+":Return="+v1);return v2;
#endif


class CabConfigData{
  public:
  uint16_t _pin;
  uint16_t _nPixels;
  uint16_t _nShelves;
  uint16_t* _shelfSizes;
  
  CabConfigData(uint16_t pin,uint16_t nShelves,uint16_t* shelfSizes) :
                                        _pin(pin),
                                        _nPixels(0),
                                        _nShelves(nShelves),
                                        _shelfSizes(shelfSizes){
    ENTER("CabConfigData")
    _nPixels=0;
    for(uint16_t i=0; i<_nShelves; ++i){
      _nPixels += 2*_shelfSizes[i];                                    
    }
    LEAVE
  }
};

CabConfigData* cabConfigData[2];

class CabConfig{
  private:
  uint16_t _nShelves;
  uint16_t* _shelfSizes;
  Adafruit_NeoPixel* _strip;
  
  public:
  CabConfig(CabConfigData* cabConfigData) :
                                        _nShelves(cabConfigData->_nShelves),
                                        _shelfSizes(cabConfigData->_shelfSizes),
                                        _strip(new Adafruit_NeoPixel(cabConfigData->_nPixels,cabConfigData->_pin,NEO_GRB+NEO_KHZ800)){
    ENTER("CabConfig")
    PRINT2("nPixels=",cabConfigData->_nPixels)
    PRINT2("nShelves=",_nShelves)
    PRINT2("pin=",cabConfigData->_pin)
    //_strip.begin();
    //_strip.show();
    //_strip.setBrightness(100);
      //_strip.setPixelColor(0,_strip.Color(255,255,255));
      //_strip.show();
      //delay(2);
    LEAVE
  }
  
  Adafruit_NeoPixel* strip(){
    ENTER("strip")
    RETURN("_strip",_strip);
  }
  void init(){
    _strip->begin();
    _strip->show();    
  }
  void test(){
    ENTER("test")
    teststrand();
    testshelves();
    LEAVE
  }
  private:
  void testshelves(){
    testshelves(_strip->Color(255,0,0));    
    testshelves(_strip->Color(0,255,0));    
    testshelves(_strip->Color(0,0,255));    
  }
  void setShelfPixels(uint32_t c,uint16_t offset,uint16_t s){
      for(int j=0;j<s;++j){
        _strip->setPixelColor(offset+j,c);
        _strip->setPixelColor(_strip->numPixels()-1-offset-j,c);
      }
  }
  void testshelves(uint32_t c){
    int16_t offset=0;
    for(int i=0;i<_nShelves;++i){
      const int16_t s=_shelfSizes[i];
      setShelfPixels(c,offset,s);
      _strip->show();
      delay(2000);
      setShelfPixels(0,offset,s);
      _strip->show();
      offset+=s;
    }
  }
  void teststrand(){    
    ENTER("testinsequence");
    teststrand(_strip->Color(255,0,0));
    teststrand(_strip->Color(0,255,0));
    teststrand(_strip->Color(0,0,255));
    LEAVE
  }
  void teststrand(uint32_t c){
    ENTER("testinsequence")    
    for(int j=0;j<1;++j){
      for(int i=0;i<_strip->numPixels();++i){
        PRINT2("i=",i);
        _strip->setPixelColor(i,c);
        _strip->show();
        delay(100);
        _strip->setPixelColor(i,_strip->Color(0,0,0));
        _strip->show();
      }
    }
    LEAVE
  }
};


class Controller{
  private:
  uint16_t _nCabs;
  CabConfig** _cabs;
  void clear(){
    for(int16_t i=0;i<_nCabs;++i){
      _cabs[i]->strip()->clear();
    }
  }
  void show(){
    for(int16_t i=0;i<_nCabs;++i){
      _cabs[i]->strip()->show();
    }
  }
  /*void setAllPixels(int32_t r, int32_t g, int32_t b) {
    for(uint16_t i=0; i<_nCabs; ++i){
      Adafruit_NeoPixel strip = _cabs[i]->strip();  
      for(int i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(r,g,b));
      }
    }
    show();
  }*/
  
  public:
  Controller(uint16_t nCabs, CabConfigData** cabConfigData) : _nCabs(nCabs),_cabs(new CabConfig*[nCabs]){
    ENTER("Controller")
    for(int i=0; i<_nCabs; ++i){
      _cabs[i] = new CabConfig(cabConfigData[i]);
    }
    LEAVE
  }
  void illuminateNextShelf(){
    for(int i=0;i<_nCabs;++i){
      CabConfig* cab = _cabs[i];
      //cab
    }
  }
  void test(){
    for(uint16_t i=0;i<_nCabs;++i){
      _cabs[i]->test();
    }
  }
  void init(){
    for(uint16_t i=0; i<_nCabs; ++i){
      _cabs[i]->init();  
      //Adafruit_NeoPixel* strip = _cabs[i]->strip();  
      //strip->begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
      //strip->show();            // Turn OFF all pixels ASAP
    }
  }
};

Controller* cntr;

// setup() function -- runs once at startup --------------------------------

void setup() {
  DEBUG_SETUP
  ENTER("setup")
  cabConfigData[0] = new CabConfigData(7,2,new uint16_t[2]{4,3});
  //cabConfigData[1] = new CabConfigData(8,2,new uint16_t[2]{3,4});
  cntr = new Controller(1,cabConfigData);
  cntr->init();
  cntr->test();
  LEAVE
}


// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
  //ENTER("loop");
  cntr->illuminateNextShelf();
  //LEAVE
}

/*
// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
  // Fill along the length of the strip in various colors...
  colorWipe(strip.Color(255,   0,   0), 50); // Red
  colorWipe(strip.Color(  0, 255,   0), 50); // Green
  colorWipe(strip.Color(  0,   0, 255), 50); // Blue

  // Do a theater marquee effect in various colors...
  theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
  theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
  theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness

  rainbow(10);             // Flowing rainbow cycle along the whole strip
  theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
}

// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(uint16_t i=0; i<cntr.nCabs(); ++i){
    Adafruit_NeoPixel strip = cntr.cabs()[i].strip();  
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    }
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      cntr.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(uint16_t i=0; i<cntr.nCabs(); ++i){
        Adafruit_NeoPixel strip = cntr.cabs()[i].strip();  
        for(int c=b; c<strip.numPixels(); c += 3) {
          strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
        }
      }
      cntr.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
      for(uint16_t i=0; i<cntr.nCabs(); ++i){
        Adafruit_NeoPixel strip = cntr.cabs()[i].strip();  
        for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
          // Offset pixel hue by an amount to make one full revolution of the
          // color wheel (range of 65536) along the length of the strip
          // (strip.numPixels() steps):
          int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
          // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
          // optionally add saturation and value (brightness) (each 0 to 255).
          // Here we're using just the single-argument hue variant. The result
          // is passed through strip.gamma32() to provide 'truer' colors
          // before assigning to each pixel:
          strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
        }
    }
    cntr.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      cntr.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(uint16_t i=0; i<cntr.nCabs(); ++i){
        Adafruit_NeoPixel strip = cntr.cabs()[i].strip();  
        for(int c=b; c<strip().numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
    }
    cntr.show();                // Update strip with new contents
    delay(wait);                 // Pause for a moment
    firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
  }
}
*/
