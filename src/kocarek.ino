#include <FastLED.h>
#include <ezButton.h>
#include <EEPROM.h>


#define LED_PIN     3
#define NUM_LEDS   60
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define MODES_COUNT 8
#define LONG_PRESS_TIME 650
#define DOUBLE_PRESS_TIME 230
#define BRIGHTNESS_LEVEL_COUNT 4
#define UPDATES_PER_SECOND 100
static uint8_t brightnessArray [BRIGHTNESS_LEVEL_COUNT] = {15, 45, 150, 255};
CRGB leds[NUM_LEDS];
ezButton button(2);  // create ezButton object that attach to pin 2;
CRGBPalette16 currentPalette;
uint8_t indexIncrement = 3;
TBlendType    currentBlending;
bool  moving = false;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
unsigned long count;
static uint8_t longPressCount = 0;
static uint8_t brightnessIndex = 0;
static uint8_t startIndex = 0;
unsigned long pressedAt = 0;
bool longPress = false;
bool doublePress = false;
int countInit;

#define NUM_COLORS2 6
#define NUM_COLORS 5
static const CRGB TwinkleColors [NUM_COLORS] =
{
    CRGB::Red,
    CRGB::Blue,
    CRGB::Purple,
    CRGB::Green,
    CRGB::Yellow
};
static const CRGB TwinkleColors2 [NUM_COLORS2] =
{
    CRGB::Lavender,
    CRGB::LightBlue,
    CRGB::PapayaWhip,
    CRGB::HotPink,
    CRGB::DarkOrchid,
    CRGB::MidnightBlue
};

void setup() {
    delay( 2000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    button.setDebounceTime(50); // set debounce time to 50 milliseconds
    button.setCountMode(COUNT_RISING);
    brightnessIndex = EEPROM.read(0);
    countInit = EEPROM.read(1);
    FastLED.setBrightness(brightnessArray[brightnessIndex]);

    ChangePalette();

    //Serial.begin(9600);

}


void loop()
{
    static uint8_t oldCount;

    button.loop(); // MUST call the loop() function first

    if(button.isPressed() && (millis() - pressedAt) < DOUBLE_PRESS_TIME){ //double click
      doublePress = true;
    }

    if(button.isPressed()){
      pressedAt = millis();
    }
    
    if(button.getState() == LOW && (millis() - pressedAt) > LONG_PRESS_TIME){
      brightnessIndex = (brightnessIndex + 1) % BRIGHTNESS_LEVEL_COUNT;
      FastLED.setBrightness(brightnessArray[brightnessIndex]);
      longPress = true;
      pressedAt = millis();
    }

    if(longPress && button.isReleased()){
      longPress = false;
      longPressCount++;
      EEPROM.write(0, brightnessIndex);
    }
    
    if(doublePress && button.isReleased()){ //double click resets counters
      button.resetCount();
      longPressCount = 0;
      doublePress = false;
    }
    
    count = (countInit + button.getCount() - longPressCount) % MODES_COUNT;

    if(count != oldCount)
      EEPROM.write(1, count);
    
    ChangePalette();
    
    if(moving){
      startIndex = startIndex + 1; /* motion speed */
    }else{
      startIndex = 0;
    }



    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);

    //Serial.println(button.getCount());
    //Serial.println(longPressCount);
    oldCount = count;
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{

    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, 255, currentBlending);
        colorIndex += indexIncrement;
        //colorIndex = colorIndex%(NUM_LEDS*4);
    }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalette()
{
      if(millis() < 2200){
        if( count ==  0)  {AllGreen(); currentBlending = LINEARBLEND; moving = false; indexIncrement = 0;}
        FastLED.setBrightness(255);
      }else{
        FastLED.setBrightness(brightnessArray[brightnessIndex]);
        //if( count == 0)  { AllBlack(); currentBlending = NOBLEND; moving = false; indexIncrement = 4;}
        if( count ==  0)  {AllWhite();}
        if( count ==  1)  { Back(); currentBlending = NOBLEND; indexIncrement = 4;}
        if( count ==  2)  { SetupRedGreen();}
        if( count ==  3)  { SetupTwinkle2(); moving = true; indexIncrement = 1;}
        if( count == 4)  { currentPalette = RainbowStripeColors_p; currentBlending = NOBLEND; moving = true; indexIncrement = 3;  FillLEDsFromPaletteColors(startIndex);}
        if( count == 5)  { SetupTwinkle(); moving = true; indexIncrement = 1;}
        //if( count == 7)  { SetupPurpleAndGreenPalette(); currentBlending = LINEARBLEND; moving = true; indexIncrement = 3;}
        if( count == 6)  { SetupCommette(); moving = true; indexIncrement = 1;}
        //if( count == 9)  { SetupBlackAndWhiteStripedPalette(); currentBlending = LINEARBLEND; moving = true; indexIncrement = 3;}
        //if( count == 7)  { currentPalette = CloudColors_p; currentBlending = LINEARBLEND; moving = true; indexIncrement = 3;}
        if( count == 7)  { currentPalette = PartyColors_p; currentBlending = LINEARBLEND; moving = true; indexIncrement = 1;  FillLEDsFromPaletteColors(startIndex);}
        //if( count == 11)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  moving = true; indexIncrement = 3;}
        //if( count == 12)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; moving = true; indexIncrement = 3;}
        //if( count == 8)  { currentPalette = RainbowColors_p; currentBlending = LINEARBLEND; moving = true; indexIncrement = 3;  FillLEDsFromPaletteColors(startIndex);}
      }
}

// Mypalette.
void AllWhite()
{
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::White;
    }
}

// Mypalette.
void AllGreen()
{
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Green;
    }
}


// Mypalette.
void AllBlack()
{
        for( int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Black;
        }
}

// Mypalette.
void Forward()
{
    currentPalette[0] = CRGB::OrangeRed;
    currentPalette[1] = CRGB::OrangeRed;
    currentPalette[2] = CRGB::White;
    currentPalette[3] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[5] = CRGB::OrangeRed;
    currentPalette[6] = CRGB::OrangeRed;
    currentPalette[7] = CRGB::OrangeRed;
    currentPalette[8] = CRGB::OrangeRed;
    currentPalette[9] = CRGB::Red;
    currentPalette[10] = CRGB::Red;
    currentPalette[11] = CRGB::Red;
    currentPalette[12] = CRGB::OrangeRed;
    currentPalette[13] = CRGB::OrangeRed;

    FillLEDsFromPaletteColors(startIndex);

}

// Mypalette.
void Back()
{
    currentPalette[0] = CRGB::OrangeRed;
    currentPalette[1] = CRGB::OrangeRed;
    currentPalette[2] = CRGB::Red;
    currentPalette[3] = CRGB::Red;
    currentPalette[4] = CRGB::Red;
    currentPalette[5] = CRGB::OrangeRed;
    currentPalette[6] = CRGB::OrangeRed;
    currentPalette[7] = CRGB::OrangeRed;
    currentPalette[8] = CRGB::OrangeRed;
    currentPalette[9] = CRGB::White;
    currentPalette[10] = CRGB::White;
    currentPalette[11] = CRGB::White;
    currentPalette[12] = CRGB::OrangeRed;
    currentPalette[13] = CRGB::OrangeRed;
    currentPalette[14] = CRGB::OrangeRed;

    FillLEDsFromPaletteColors(0);

}

// Mypalette.
void Warning()
{
  if(millis()/500 % 2){
    fill_solid( currentPalette, 16, CRGB::Black);
  }else{
    fill_solid( currentPalette, 16, CRGB::OrangeRed);
  }
  FillLEDsFromPaletteColors(startIndex);
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupCommette()
{
    startIndex = startIndex%(NUM_LEDS*4);
    int pos = startIndex /4;
    static byte hue = HUE_RED;
    hue += 4;
  for( int i = 0; i < NUM_LEDS; i++) {
      leds[i].fadeToBlackBy(16);
  }
  for( int i = pos; i < pos+6; i++) {
      leds[i%NUM_LEDS].setHue(hue);;
  }
}

void SetupTwinkle()
{
    static byte hue = HUE_RED;
    hue += 4;
  for( int i = 0; i < NUM_LEDS; i++) {
      leds[i].fadeToBlackBy(4);
  }
  if(startIndex % 6 == 0){
    leds[random(NUM_LEDS)] = TwinkleColors[random(0, NUM_COLORS)];
    }
}

void SetupTwinkle2()
{
    int pos = startIndex /4;
    static byte hue = HUE_RED;
    hue += 4;
  for( int i = 0; i < NUM_LEDS; i++) {
      leds[i].fadeToBlackBy(16);
  }
    leds[random(NUM_LEDS)] = TwinkleColors2[random(0, NUM_COLORS2)];
}

void SetupRedGreen()
{
  for( int i = 0; i < NUM_LEDS; i++) {
      if(i%2)
        leds[i] = CRGB::Red;
      else
        leds[i] = CRGB::Green;
  }

}
