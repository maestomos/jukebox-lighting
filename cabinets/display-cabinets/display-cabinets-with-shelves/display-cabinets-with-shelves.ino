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

//#define DEBUG 1
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
  #define PRINT1(v1) Serial.println(DEBUG_STACK[DEBUG_STACK_PTR-1]+":"+v1);
  #define PRINT2(v1,v2) Serial.println((DEBUG_STACK[DEBUG_STACK_PTR-1]+":"+v1+v2).c_str());
  #define PRINT3(v1,v2,v3) Serial.println(DEBUG_STACK[DEBUG_STACK_PTR-1]+":"+v1+v2+v3);
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

class RGB{
  public:
  uint8_t _r,_g,_b;
  public:
  RGB(uint8_t r,uint8_t g,uint8_t b):_r(r),_g(g),_b(b){    
  }
  uint32_t color(Adafruit_NeoPixel* strip) const{return strip->Color(_r,_g,_b);}
};

static const uint16_t NUM_SHELVES=6;
static const uint16_t NUM_CABINETS=2;
static const uint16_t NUM_COLORS=11;
static const RGB COLORS[NUM_COLORS]={
                                RGB(255,0,0),
                                RGB(0,255,0),
                                RGB(0,0,255),
                                RGB(0,255,255),
                                RGB(255,255,0),
                                RGB(255,0,255),
                                RGB(200,150,190),
                                RGB(40,170,240),
                                RGB(90,140,230),
                                RGB(50,220,190),
                                RGB(150,90,160)};
CabConfigData* cabConfigData[NUM_CABINETS];

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
  uint16_t nShelves(){
    return _nShelves;
  }
  void setShelfPixels(uint32_t c,uint16_t n){
      setPixels(c,toOffset(n),_shelfSizes[n],false,0);
  }
  private:
  uint16_t toOffset(uint16_t n){
    uint16_t offset=0;
    for(uint16_t i=0;i<n;++i) offset+=_shelfSizes[i];
    return offset;
  }
  void setPixels(uint32_t c,uint16_t offset, uint16_t s, boolean show,uint32_t wait){
      ENTER("setPixels")
      PRINT2("show=",show)
      for(int j=0;j<s;++j){
        _strip->setPixelColor(offset+j,c);
        _strip->setPixelColor(_strip->numPixels()-1-offset-j,c);
      }
      if(show)_strip->show();
      delay(wait);
      LEAVE
  }
  void testshelves(){
    for(int i=0;i<NUM_COLORS;++i){
      testshelves(COLORS[i].color(_strip));
    }
  }
  void testshelves(uint32_t c){
    int16_t offset=0;
    for(int i=0;i<_nShelves;++i){
      const int16_t s=_shelfSizes[i];
      setPixels(c,offset,s,true,100);
      setPixels(0,offset,s,true,0);
      offset+=s;
    }
  }
  void teststrand(){    
    ENTER("testinsequence");
    for(int i=0;i<NUM_COLORS;++i){
      teststrand(COLORS[i].color(_strip));
    }
    LEAVE
  }
  void teststrand(uint32_t c){
    ENTER("testinsequence")    
    for(int j=0;j<1;++j){
      for(int i=0;i<_strip->numPixels();++i){
        PRINT2("i=",i);
        _strip->setPixelColor(i,c);
        _strip->show();
        delay(6);
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
  uint16_t _nextShelf;
  uint16_t _nextColor;

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
  
  public:
  Controller(uint16_t nCabs, CabConfigData** cabConfigData) :
                          _nCabs(nCabs),_cabs(new CabConfig*[nCabs]),_nextShelf(0),_nextColor(0){
    ENTER("Controller")
    for(int i=0; i<_nCabs; ++i){
      _cabs[i] = new CabConfig(cabConfigData[i]);
    }
    LEAVE
  }
  private:
  void setPixels(uint8_t r,uint8_t g,uint8_t b){
      for(int i=0;i<_nCabs;++i){
        _cabs[i]->setShelfPixels(_cabs[i]->strip()->Color(r,g,b),_nextShelf%_cabs[i]->nShelves());
      }    
  }
  void illuminate(uint8_t r,uint8_t g,uint8_t b,uint16_t w){
      setPixels(r,g,b);
      show();
      delay(w);    
  }
  void fadeIn(){
    const RGB rgb=COLORS[_nextColor];
    for(int i=30;i>=1;--i){
      illuminate(rgb._r/i,rgb._g/i,rgb._b/i,50);
    }
  }
  void fadeOut(){
    const RGB rgb=COLORS[_nextColor];
    for(int i=1;i<=30;++i){
      illuminate(rgb._r/i,rgb._g/i,rgb._b/i,50);
    }
    illuminate(0,0,0,0);
  }
  void illuminateAllShelvesInSequence()
  {
    const RGB rgb=COLORS[_nextColor];
    for(int i=0;i<NUM_SHELVES;++i){
      illuminate(rgb._r,rgb._g,rgb._b,500);
      illuminate(0,0,0,0);
      _nextShelf++;
    }
    _nextShelf-=2;
    for(int i=0;i<NUM_SHELVES-1;++i){
      illuminate(rgb._r,rgb._g,rgb._b,500);
      illuminate(0,0,0,0);
      _nextShelf--;
    }
      _nextShelf++; // Ensures that _nexShelf%NUM_SHELVES==0
  }
  void illuminateNextShelf(){
    ENTER("illuminateNextShelf")
    PRINT2("_nextColor=",_nextColor);
    PRINT2("_nextShelf=",_nextShelf);
    fadeIn();
    delay(12000);
    fadeOut();
    _nextShelf++;
    if(_nextShelf%NUM_SHELVES==0){
      _nextColor++;
      if(_nextColor%NUM_COLORS==0){
        _nextColor=0;
      }
    }
    LEAVE
  }
  public:
  void test(){
    for(uint16_t i=0;i<_nCabs;++i){
      _cabs[i]->test();
    }
  }
  void init(){
    for(uint16_t i=0; i<_nCabs; ++i){
      _cabs[i]->init();
    }
  }
  void nextAction(){
    if(_nextShelf%NUM_SHELVES==0){
      illuminateAllShelvesInSequence();
    }
    illuminateNextShelf();
  }
};

Controller* cntr;

// setup() function -- runs once at startup --------------------------------

void setup() {
  DEBUG_SETUP
  ENTER("setup")
  cabConfigData[0] = new CabConfigData(7,NUM_SHELVES,new uint16_t[NUM_SHELVES]{5,7,8,10,8,6});
  cabConfigData[1] = new CabConfigData(8,NUM_SHELVES,new uint16_t[NUM_SHELVES]{7,8,7,8,8,6});
  cntr = new Controller(NUM_CABINETS,cabConfigData);
  cntr->init();
  cntr->test();
  LEAVE
}


// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
  ENTER("loop");
  cntr->nextAction();
  LEAVE
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
