/*
    This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
*/
#include "M5Atom.h"
#include <PLEN5Stack.h>
#include <Adafruit_NeoPixel.h> //ver 1.3.4

//#include "utility/Button.h"

// for M5Atom: Use Grove
//#define SCL 32//22
//#define SDA 26//21
// for M5Atom: Use Pin
#define SCL 21//22
#define SDA 25//21
#define LEDEYE 22
#define BUTTON_A_PIN 23
#define BUTTON_B_PIN 19
#define ANALOGI_IN 33

PLEN5Stack plen5stack;

Button myBtnA = Button(BUTTON_A_PIN, true, 10);
Button myBtnB = Button(BUTTON_B_PIN, true, 10);

//extern const unsigned char AtomImageData[375 + 2];
uint8_t DisBuff[2 + 5 * 5 * 3];

void setBuff(uint8_t Rdata, uint8_t Gdata, uint8_t Bdata)
{
  DisBuff[0] = 0x05;
  DisBuff[1] = 0x05;
  for (int i = 0; i < 25; i++)
  {
    DisBuff[2 + i * 3 + 0] = Rdata;
    DisBuff[2 + i * 3 + 1] = Gdata;
    DisBuff[2 + i * 3 + 2] = Bdata;
  }
}

const CRGB COLOR[8] = {
  0xf00000,//Green
  0x00f000,//Red
  0x0000f0,//Blue
  0xf0f000,//Yellow
  0x50f000,//Orange
  0x00f0f0,//Purple
  0x707070,//White
  0x000000//Black
};

int dot[25] = {0, 0, 0, 0, 0,
               0, 1, 0, 1, 0,
               0, 0, 0, 0, 0,
               1, 0, 0, 0, 1,
               0, 1, 1, 1, 0
              };

void displayDraw( int dot[25] ) {
  for (int i = 0; i < 25; i++) {
    if (0 == dot[24 - i]) {
      M5.dis.drawpix(i, COLOR[3]);
    } else {
      M5.dis.drawpix(i, COLOR[4]);
    }
  }
}

void setup()
{
  M5.begin(true, false, true);
  delay(10);
  setBuff(0xff, 0x00, 0x00);
  M5.dis.displaybuff(DisBuff);

  pinMode(ANALOGI_IN, INPUT);
  pinMode(LEDEYE, OUTPUT);
  digitalWrite(LEDEYE, LOW);
  Wire.begin(SDA, SCL);
  plen5stack.servoInitialSet();
  displayDraw(dot);
}

uint8_t FSM = 0;

void loop()
{
  if (M5.Btn.wasPressed())
  {
    plenBlink();
    switch (FSM)
    {
      case 0:
        setBuff(0x40, 0x00, 0x00);
        plen5stack.motion(0x29);//Patapata: Shake arms
        break;
      case 1:
        setBuff(0x00, 0x40, 0x00);
        plen5stack.motion(38);//Dance Twist
        break;
      case 2:
        setBuff(0x00, 0x00, 0x40);
        plen5stack.motion(0x46);//Walk
        break;
      case 3:
        setBuff(0x20, 0x20, 0x20);
        plen5stack.motion(8);//Highfive
        break;
      default:
        break;
    }
    M5.dis.displaybuff(DisBuff);
    FSM++;
    if (FSM >= 4) {
      FSM = 0;
    }
    plenBlink();
  }
  else if (!digitalRead(BUTTON_A_PIN)) 
  {
    for (int i = 0; i < 25; i++) {
      Serial.println(dot[i]);
    }
    displayDraw(dot);

  } else if (!digitalRead(BUTTON_B_PIN)) 
  {
    plenBlink();
  }

  delay(50);
  M5.update();
}

void plenBlink()
{
  digitalWrite(LEDEYE, HIGH);
  delay(100);
  digitalWrite(LEDEYE, LOW);
  delay(100);
}
