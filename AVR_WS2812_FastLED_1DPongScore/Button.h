#ifndef _BUTTON_H_
#define _BUTTON_H_

#define DEBOUNCE_MAX 2

void onButtonPressed(void *self);
void onButtonReleased(void *self);

#define BUTTON_MODE_MANUAL 0
#define BUTTON_MODE_OFF    1
#define BUTTON_MODE_AUTO   2

class Button
{
  public:
    Button(int id, int buttonpin, char caption)
    {
      myid = id;
      pinMode(buttonpin, INPUT_PULLUP);
      mybuttonstate = 0;
      debounce = 0;
      mybuttonpin = buttonpin;
      buttonmode = BUTTON_MODE_MANUAL;

      mycaption = caption;
    }


    char getCaption()
    {
      return mycaption;
    }

    void setCaption(char caption)
    {
      mycaption = caption;
    }

    void setMode(int newmode)
    {
      buttonmode = newmode;
    }

    int getId()
    {
      return myid;
    }


    bool isOn()
    {
      return (!digitalRead(mybuttonpin));
    }

    void checkStatus()
    {
      int dread = !digitalRead(mybuttonpin);

      int newstate = mybuttonstate;

      if (dread > 0)
      {
        ++debounce;
      }
      else
      {
        --debounce;
      }
      if (debounce < 0)
      {
        debounce = 0;
      }
      if (debounce > DEBOUNCE_MAX)  {
        debounce = DEBOUNCE_MAX;
      }

      if (debounce == 0) {
        newstate = 0;
      }
      if (debounce == DEBOUNCE_MAX)
      {
        newstate = 1;
      }


      if (newstate != mybuttonstate)
      {
        if (buttonmode != BUTTON_MODE_OFF)
        {
          if (newstate)
          {
            onButtonPressed((void *)this);
          }
          else
          {
            onButtonReleased((void *)this);
          }
        }
        mybuttonstate = newstate;
      }
    }

  private:
    int mybuttonpin;
    int debounce;
    int myid;
    int mybuttonstate;
    char mycaption;
    int buttonmode;
};

#endif
