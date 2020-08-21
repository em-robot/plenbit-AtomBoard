/*
    This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
*/

#include "M5Atom.h"
#include <PLEN5Stack.h>
#include <Adafruit_NeoPixel.h> //ver 1.3.4

#include "DHT12.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// for M5Atom: Use Grove
#define SCL 32//22
#define SDA 26//21
//for M5Atom: Use Pin (Wire1)
#define SCL1 21//22
#define SDA1 25//21

// I2c reference : M5ATOM unofficial Doc
// https://raspberrypi.mongonta.com/spec-of-m5atom-matrix-lite/

#define LEDEYE 22
#define BUTTON_A_PIN 23
#define BUTTON_B_PIN 19
#define ANALOGI_IN 33

PLEN5Stack plen5stack;

DHT12 dht12; //Preset scale CELSIUS and ID 0x5c.
Adafruit_BMP280 bme;

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
  Wire.begin(SDA1, SCL1);
  Wire1.begin(SDA, SCL);
  plen5stack.servoInitialSet();
  displayDraw(dot);

  while (!bme.begin(0x76))
  {
    delay(100);
  }
}

uint8_t FSM = 0;

unsigned long time1;

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
  else if (!digitalRead(BUTTON_A_PIN)) {
    //Serial.print("A:");

    //plenBlink();

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
  
  if(millis()-time1 >= 1000)
  {
    time1 = millis();

    float temp = dht12.readTemperature();
    float humidity = dht12.readHumidity();
    float pressure = bme.readPressure();
//    if(temp >= 30)
//    {
//      M5.Lcd.setTextColor(RED, BLACK);
//    }else
//    {
//      M5.Lcd.setTextColor(WHITE, BLACK);
//    }
    Serial.printf("\r\nTemp    : %2.1f\r\n", temp);
    Serial.printf("Humidity: %2.0f%%\r\n", humidity);
    Serial.printf("Pressure: %.2fhPa\r\n", pressure/100.0);
//    if(temp >= 30)
//    {
//      plen5stack.motion(0x29);//patapata
//    }
  }// 1 tick timer for sensor
}

void plenBlink()
{
  digitalWrite(LEDEYE, HIGH);
  delay(100);
  digitalWrite(LEDEYE, LOW);
  delay(100);
}
