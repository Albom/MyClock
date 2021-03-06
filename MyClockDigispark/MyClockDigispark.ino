
#include <TinyWireM.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>

#define LCD_ADDR  0x3F
#define RTC_ADDR  0x68

LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

#define SPACE  ' '

byte displayTime[7];
byte currentTime[7];
byte currentMode = 0;
unsigned long t_prev;


void setup() {

  TinyWireM.begin();

  lcd.init();
  lcd.backlight();

  buildFont3();

  analogWrite(1, 255);

  currentMode = 0;

  t_prev = millis();

}


const uint8_t custom[] PROGMEM = {
  B11111, B11111, B11111, B00000, B00000, B00000, B00000, B00000 ,
  B11100, B11110, B11111, B11111, B11111, B11111, B11111, B11111 ,
  B11111, B11111, B11111, B11111, B11111, B11111, B01111, B00111 ,
  B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111 ,
  B11111, B11111, B11111, B11111, B11111, B11111, B11110, B11100 ,
  B11111, B11111, B11111, B00000, B00000, B00000, B11111, B11111 ,
  B11111, B00000, B00000, B00000, B00000, B11111, B11111, B11111 ,
  B00111, B01111, B11111, B11111, B11111, B11111, B11111, B11111
};


void buildFont3() {

  for (int8_t i = 0; i < 8; i++) {
    uint8_t c[8];
    for (int8_t j = 0; j < 8; j++) {
      c[j] = pgm_read_byte_near(&custom[i * 8 + j]);
    }
    lcd.createChar(i, c);
  }

}


const char digits[] PROGMEM  = {7, 0, 1, 2, 3, 4, // 0
                                SPACE, 1, SPACE, SPACE, 2, SPACE, // 1
                                5, 5, 1, 2, 6, 6, // 2
                                0, 5, 1, 3, 6, 4,  // 3
                                2, 3, 1, SPACE, SPACE, 2, // 4
                                7, 5, 5, 3, 6, 4, // 5
                                7, 5, 5, 2, 6, 4, // 6
                                0, 5, 1, SPACE, 2, SPACE, // 7
                                7, 5, 1, 2, 6, 4, // 8
                                2, 5, 1, 3, 6, 4 // 9
                               };

void printDigit3(byte posX, byte digit) {

  byte offset = digit * 6;
  lcd.setCursor(posX, 0);
  for (byte i = 0; i < 3; i++) {
    lcd.write(pgm_read_byte_near(&digits[offset]));
    offset++;
  }
  lcd.setCursor(posX, 1);
  for (byte i = 0; i < 3; i++) {
    lcd.write(pgm_read_byte_near(&digits[offset]));
    offset++;
  }

}


void getTime() {
  TinyWireM.beginTransmission(RTC_ADDR);
  TinyWireM.send(0);
  TinyWireM.endTransmission();

  TinyWireM.requestFrom(RTC_ADDR, 7);

  for (int8_t i = 0; i < 7; i++) {
    currentTime[i] = TinyWireM.receive();
  }
}

bool compareTime() {
  for (int8_t i = 0; i < 7; i++) {
    if (displayTime[i] != currentTime[i]) {
      for (int8_t j = 0; j < 7; j++) {
        displayTime[j] = currentTime[j];
      }
      return false;
    }
  }
  return true;
}


void pt(byte b1, byte b2) {
  printDigit3(0, b1 >> 4);
  printDigit3(4, b1 & 0x0F);
  printDigit3(9, b2 >> 4);
  printDigit3(13, b2 & 0x0F);

  lcd.setCursor(7, 1);
  
}

const char days[] PROGMEM = { "  SUMOTUWETHFRSA"};

void loop() {

  unsigned long t = millis();

  if (t_prev > t) {
    t_prev = t;
    return;
  };

  if (t - t_prev > 5000) {
    t_prev = t;
    currentMode++;
    if (currentMode > 1) {
      currentMode = 0;
    }
  }

  getTime();

  if (!compareTime()) {

    if (currentMode == 0) {

      pt(displayTime[2], displayTime[1]);

      lcd.print(displayTime[0] >> 4);
      lcd.setCursor(8, 1);
      lcd.print(displayTime[0] & 0x0F);
    }

    if (currentMode == 1) {

      pt(displayTime[4], displayTime[5]);

      int offset = ((displayTime[3] & 0x0F) - 1) * 2 ;
      lcd.write(pgm_read_byte_near(& days[offset]));
      lcd.write(pgm_read_byte_near(& days[offset + 1]));

    }

  }

}
