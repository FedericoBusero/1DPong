#include "FastLED.h"
#include "Button.h"

// TODO: button class eruit

#define BUTTON_A     1
#define BUTTON_B     2
#define BUTTON_RESET 3

Button b1(BUTTON_A, 2, 'A');
Button b2(BUTTON_B, 3, 'B');
Button b_reset(BUTTON_RESET, 4, 'R');

// How many leds in your strip?
#define NUMLEDPIXELS  30 // 30, 60
#define NUM_LED_CLICK  7 // 7, 13

#define WS2812B_PIN 5

// Define the array of leds
CRGB leds[NUMLEDPIXELS];

long next_tick;
#define LED_BRIGHTNESS       45
#define LED_BRIGHTNESS_FIX   30
#define TIME_STARTUP       1000
#define TIME_ENDRUN        1500
#define TIME_SHOWWINNER    1500
#define TIME_MOVE_DEFAULT (3000/NUMLEDPIXELS)
#define TIME_MOVE_PIXEL    (350/NUMLEDPIXELS)
#define MAX_SCORE            10

int pos = 0;
int time_move = TIME_MOVE_DEFAULT;
int score1 = 0;
int score2 = 0;


enum
{
  STATE_RESET,
  STATE_STARTUP,
  STATE_MOVEUP,
  STATE_MOVEDOWN,
  STATE_SHOWWINNER,
  STATE_ENDRUN,
  STATE_ENDGAME,
};

int currentstate = STATE_RESET;


void setup() {
  Serial.begin(115200);
  Serial.println("1D Pong + Score setup started");
  FastLED.addLeds<NEOPIXEL, WS2812B_PIN>(leds, NUMLEDPIXELS);
  // FastLED.setDither( 0 );
}

void show_score()
{
  Serial.println("show score");
  Serial.print(score1);
  Serial.print("-");
  Serial.println(score2);
  fill_solid (leds, NUMLEDPIXELS, CRGB(0, 0, 0));

  for (int count = 0; count < score1; ++count)
  {
    leds[count] = CRGB(0, LED_BRIGHTNESS_FIX, LED_BRIGHTNESS_FIX);
  }
  for (int count = 0; count < score2; ++count)
  {
    leds[NUMLEDPIXELS - count - 1] = CRGB(0, LED_BRIGHTNESS_FIX, LED_BRIGHTNESS_FIX);
  }

  FastLED.show();
}

void show_winner(int w)
{
  Serial.println("end run");
  fill_solid (leds, NUMLEDPIXELS, CRGB(0, 0, 0));

  // TODO verkorten

  if (w == 0)
  {
    ++score1;
    for (int count = 0; count < NUMLEDPIXELS / 2; ++count)
    {
      leds[count] = CRGB(0, LED_BRIGHTNESS_FIX, 0);
    }
    for (int count = NUMLEDPIXELS / 2; count < NUMLEDPIXELS; ++count)
    {
      leds[count] = CRGB(LED_BRIGHTNESS_FIX, 0, 0);
    }
  }
  else
  {
    ++score2;
    for (int count = 0; count < NUMLEDPIXELS / 2; ++count)
    {
      leds[count] = CRGB(LED_BRIGHTNESS_FIX, 0, 0);
    }
    for (int count = NUMLEDPIXELS / 2; count < NUMLEDPIXELS; ++count)
    {
      leds[count] = CRGB(0, LED_BRIGHTNESS_FIX, 0);
    }
  }

  FastLED.show();
}

void updateState()
{
  switch (currentstate)
  {
    case STATE_RESET:
      pos = 0;
      score1 = 0;
      score2 = 0;
      next_tick = millis() + TIME_STARTUP;
      time_move = TIME_MOVE_DEFAULT;
      fill_solid (leds, NUMLEDPIXELS, CRGB(LED_BRIGHTNESS_FIX, LED_BRIGHTNESS_FIX, LED_BRIGHTNESS_FIX));
      FastLED.show();

      currentstate = STATE_STARTUP;
      break;

    case STATE_STARTUP:
      if (millis() > next_tick)
      {
        fill_solid (leds, NUMLEDPIXELS, CRGB(0, 0, 0));

        currentstate = STATE_MOVEUP;
        pos = 0;
        next_tick = millis() + time_move;
      }
      break;

    case STATE_MOVEUP:
      if (millis() > next_tick)
      {
        for (int count = 0; count < NUMLEDPIXELS; ++count)
        {
          leds[count] /= 2;
        }
        // fill_solid (leds, NUMLEDPIXELS, CRGB(0, 0, 0));
        ++pos;
        if (pos < NUMLEDPIXELS)
        {
          leds[pos] = CRGB(0, 0, LED_BRIGHTNESS);

          next_tick = millis() + time_move;
        }
        else
        {
          show_winner(0);

          currentstate = STATE_SHOWWINNER;
          next_tick = millis() + TIME_SHOWWINNER;
        }
      }
      break;

    case STATE_MOVEDOWN:
      if (millis() > next_tick)
      {
        for (int count = 0; count < NUMLEDPIXELS; ++count)
        {
          leds[count] /= 2;
        }
        // fill_solid (leds, NUMLEDPIXELS, CRGB(0, 0, 0));
        --pos;
        if (pos >= 0)
        {
          leds[pos] = CRGB(0, 0, LED_BRIGHTNESS);

          next_tick = millis() + time_move;
        }
        else
        {
          show_winner(1);

          currentstate = STATE_SHOWWINNER;
          next_tick = millis() + TIME_SHOWWINNER;
        }
      }
      break;

    case STATE_SHOWWINNER:
      if (millis() > next_tick)
      {
        if ((score1 == MAX_SCORE) || (score2 == MAX_SCORE))
        {
          currentstate = STATE_ENDGAME;
        }
        else
        {
          currentstate = STATE_ENDRUN;
        }
        next_tick = millis() + TIME_ENDRUN;
        show_score();
      }

    case STATE_ENDRUN:
      if (millis() > next_tick)
      {
        fill_solid (leds, NUMLEDPIXELS, CRGB(0, 0, 0));
        time_move = TIME_MOVE_DEFAULT;

        if (pos >= NUMLEDPIXELS)
        {
          currentstate = STATE_MOVEDOWN;
        }
        else
        {
          currentstate = STATE_MOVEUP;
        }

        next_tick = millis();
      }
      break;

    case STATE_ENDGAME:
      break;
  }
}

void onButtonPressed(void *self)
{
  Button *button = (Button*)self;

  Serial.print("Pressed: ");
  Serial.println(button->getCaption());
  switch (button->getId())
  {
    case BUTTON_A:
      if ((currentstate == STATE_MOVEDOWN) && (pos < NUM_LED_CLICK))
      {
        currentstate = STATE_MOVEUP;
        time_move = TIME_MOVE_PIXEL * (pos + 1);
      }
      break;

    case BUTTON_B:
      if ((currentstate == STATE_MOVEUP) && (pos >= NUMLEDPIXELS - NUM_LED_CLICK))
      {
        currentstate = STATE_MOVEDOWN;
        time_move = TIME_MOVE_PIXEL * (NUMLEDPIXELS - pos);
      }
      break;

    case BUTTON_RESET:
      currentstate = STATE_RESET;
      next_tick = millis() ;
      break;


  }
}

void onButtonReleased(void *self)
{
  Button *button = (Button*)self;

  Serial.print("Released: ");
  Serial.println(button->getCaption());
}

void loop() {
  updateState();
  b1.checkStatus();
  b2.checkStatus();
  b_reset.checkStatus();

  FastLED.show();
  FastLED.delay(4);
}
