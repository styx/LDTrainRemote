#include <Lpf2Hub.h>      //legoino
#include <Lpf2HubConst.h> //legoino
#include <Bounce2.h>      //bounce2

Lpf2Hub myHub;
byte port = (byte)PoweredUpHubPort::A;

//Pin declaration
#define BTN_MUSIC 27
#define BTN_LIGHT 26
#define BTN_WATER 14
#define BTN_STOP 25
#define PTI_SPEED 12

Bounce pbMusic = Bounce();
Bounce pbLight = Bounce();
Bounce pbWater = Bounce();
Bounce pbStop = Bounce();
int gLastStatePtiSpeed = 0;

static int gLightOn = 0;
static short gColor = NONE;
static int gSpeed =0;

void handlePoti()
{
  int ptiSpeed = analogRead(PTI_SPEED);
  gLastStatePtiSpeed = ptiSpeed;
  // Serial.println(ptiSpeed);
  int speed = 0;
  // if ( ptiSpeed > 2600) speed = 64;      // fast foreward
  // else if (ptiSpeed > 2400) speed = 32;  // normal forweard
  // else if (ptiSpeed > 2100) speed = 16;  // slow forefard (might not work on low battery)
  // else if (ptiSpeed < 1800) speed = -16; // slow backward
  // else if (ptiSpeed < 1400) speed = -32; // fast backward

  if (ptiSpeed < 1200) speed = 64;
  else if (ptiSpeed < 1400) speed = 32;
  else if (ptiSpeed < 1650) speed = 16;
  else if (ptiSpeed > 2100) speed = -32;
  else if (ptiSpeed > 1850) speed = -16;
  else speed = 0;                        // stop

  if(speed != gSpeed)
  {
    if(gSpeed == 0 && speed > 0)
    {
        myHub.playSound((byte)DuploTrainBaseSound::STATION_DEPARTURE);
        delay(100);
    }
    gSpeed =speed;
    myHub.setBasicMotorSpeed(port, speed);
  }
}


Color getNextColor()
{
    if(gColor == 255)
    {
        gColor = 0;
        return (Color)0;
    }

    gColor++;

    if(gColor == NUM_COLORS)
    {
        gColor =255;
    }
    return (Color) gColor;
}

void handleButtons()
{
  if(pbMusic.update())
  {
    if(pbMusic.fell())
    {
      myHub.playSound((byte)DuploTrainBaseSound::HORN);
      delay(100);
    }
  }
  if(pbLight.update())
  {
    if(pbLight.fell())
    {
      myHub.setLedColor(getNextColor());
      delay(100);
    }
  }
  if(pbWater.update())
  {
    if(pbWater.fell())
    {
      myHub.playSound((byte)DuploTrainBaseSound::WATER_REFILL);
      delay(100);
    }
  }
  if(pbStop.update())
  {
    if(pbStop.fell())
    {
      myHub.setBasicMotorSpeed(port, 0);
      delay(100);
      myHub.playSound((byte)DuploTrainBaseSound::BRAKE);
    }
  }
}

void setup() {
  Serial.begin(115200);
  //define Pin Modes
  pbMusic.attach(BTN_MUSIC, INPUT_PULLUP);
  pbLight.attach(BTN_LIGHT, INPUT_PULLUP);
  pbWater.attach(BTN_WATER, INPUT_PULLUP);
  pbStop.attach(BTN_STOP, INPUT_PULLUP);

  myHub.init();
}

void loop() {

    if (myHub.isConnecting()) {
        myHub.connectHub();
        if (myHub.isConnected()) {
            Serial.println("We are now connected to the HUB");
        } else {
            Serial.println("We have failed to connect to the HUB");
        }
    }
    if (myHub.isConnected()) {
        handleButtons();
        handlePoti();
    }
    delay(50);
}
