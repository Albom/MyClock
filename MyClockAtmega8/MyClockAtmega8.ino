
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>

#define LCD_ADDR  0x3F
#define RTC_ADDR  0x68
#define SENSOR_ADDR 0x5C

#define SHOW_SECONDS 0
#define SERIAL_ENABLE 0

LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);


#define SPACE  ' '

byte displayTime[7];
byte currentTime[7];
byte currentMode = 0;
unsigned long t_prev;


void setup() {

  Wire.begin();

  lcd.begin(16, 2);
  lcd.backlight();

  buildFont3();

  currentMode = 0;

  t_prev = millis();

#if SERIAL_ENABLE
  Serial.begin(9600);
#endif

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
                                2, 5, 1, 3, 6, 4, // 9
                                SPACE, SPACE, SPACE, SPACE, SPACE, SPACE // 10 - white
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
  Wire.beginTransmission(RTC_ADDR);
  Wire.write(0);
  Wire.endTransmission();

  if (Wire.requestFrom(RTC_ADDR, 7) == 7) {

    for (int8_t i = 0; i < 7; i++) {
      currentTime[i] = Wire.read();
    }

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


void readSensor(int* temp, int* hum) {

  Wire.beginTransmission(SENSOR_ADDR);
  Wire.write(0);
  Wire.endTransmission();
  delay(10);

  Wire.beginTransmission(SENSOR_ADDR);
  Wire.write(3);
  Wire.write(0);
  Wire.write(4);
  Wire.endTransmission();
  delay(2);

  if (Wire.requestFrom(SENSOR_ADDR, 8) == 8) {

    byte c[8];
    for (int8_t i = 0; i < 8; i++) {
      c[i] = Wire.read();
    }

    *hum = c[2] * 256 + c[3];
    *temp = c[4] * 256 + c[5];

  }
}


const char days[] PROGMEM = {"  SUMOTUWETHFRSA"};

int bl = 0;
void loop() {

  unsigned long times[] = {15000, 5000, 3000, 3000};
  unsigned long t = millis();
  int hum, temp;

  if (t_prev > t) {
    t_prev = t;
    return;
  };

  if (t - t_prev > times[currentMode]) {
    t_prev = t;
    //   lcd.clear();

    currentMode++;
    if (currentMode > 3) {
      currentMode = 0;
    }
  }

  getTime();

  if (!compareTime()) {

    int lumin = analogRead(0);
#if SERIAL_ENABLE
    Serial.println(lumin);
#endif
    analogWrite(10, lumin < 400  ? 30 : 150); // 30 - night, 150 - day

    if (currentMode == 0) {

      printDigit3(0, displayTime[2] >> 4);
      printDigit3(4, displayTime[2] & 0x0F);
      printDigit3(9, displayTime[1] >> 4);
      printDigit3(13, displayTime[1] & 0x0F);

#if SHOW_SECONDS
      lcd.setCursor(7, 1);
      lcd.print(displayTime[0] >> 4);
      lcd.setCursor(8, 1);
      lcd.print(displayTime[0] & 0x0F);
#else
      lcd.setCursor(8, 0);
      lcd.write(displayTime[0] % 2 ? 0xA1 : SPACE);
      lcd.setCursor(7, 1);
      lcd.write(SPACE);
      lcd.write(displayTime[0] % 2 ? 0xA1 : SPACE);
#endif
    }

    if (currentMode == 1) {

      printDigit3(0, displayTime[4] >> 4);
      printDigit3(4, displayTime[4] & 0x0F);
      printDigit3(9, displayTime[5] >> 4);
      printDigit3(13, displayTime[5] & 0x0F);

      lcd.setCursor(7, 1);
      int offset = displayTime[3]  * 2 ;
      lcd.write(pgm_read_byte_near(& days[offset]));
      lcd.write(pgm_read_byte_near(& days[offset + 1]));

#if SHOW_SECONDS
#else
      lcd.setCursor(8, 0);
      lcd.write(SPACE);
#endif

    }

    if (currentMode == 2) {

      readSensor(&temp, &hum);

      printDigit3(0, temp / 100);
      printDigit3(4, temp % 100 / 10);
      printDigit3(9, temp % 10);
      printDigit3(13, 10);

      lcd.setCursor(7, 1);
      lcd.write(0xA1);
      lcd.setCursor(8, 1);
      lcd.write(SPACE);

      lcd.setCursor(13, 0);
      lcd.write(0x6F);

      lcd.setCursor(13, 1);
      lcd.write(SPACE);

      delay(2000);

    }

    if (currentMode == 3) {

      readSensor(&temp, &hum);

      printDigit3(0, hum / 100);
      printDigit3(4, hum % 100 / 10);
      printDigit3(9, hum % 10);
      printDigit3(13, 10);

      lcd.setCursor(7, 1);
      lcd.write(0xA1);
      lcd.setCursor(8, 1);
      lcd.write(SPACE);

      lcd.setCursor(13, 0);
      lcd.write(SPACE);

      lcd.setCursor(13, 1);
      lcd.write(0x25);

      delay(2000);

    }



  }

}
