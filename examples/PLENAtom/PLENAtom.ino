/*
    This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
*/
#include "M5Atom.h"
#include <PLEN5Stack.h>

// for M5Atom: Use Grove
//#define SCL 32//22
//#define SDA 26//21
// for M5Atom: Use Pin
#define SCL 21//22
#define SDA 25//21


PLEN5Stack plen5stack;

extern const unsigned char AtomImageData[375 + 2];
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

void setup()
{
    M5.begin(true, false, true);
    delay(10);
    setBuff(0xff, 0x00, 0x00);
    M5.dis.displaybuff(DisBuff);

  Wire.begin(SDA, SCL);
  plen5stack.servoInitialSet();
}

uint8_t FSM = 0;

void loop()
{
    if (M5.Btn.wasPressed())
    {
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
        if (FSM >= 4)
        {
            FSM = 0;
        }
    }

    delay(50);
    M5.update();
}
