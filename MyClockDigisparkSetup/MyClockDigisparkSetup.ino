
#include <TinyWireM.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDR  0x3F
#define RTC_ADDR  0x68

LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

// Implementation due to Tomohiko Sakamoto
byte DayOfWeek(int y, int m, int d) {   // y > 1752, 1 <= m <= 12
  static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

  y -= m < 3;
  return ((y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7) + 1; // 01 - 07, 01 = Sunday
}

void setup() {

  char months[][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  byte m = 0;
  for (; m < 12; m++) {
    if (!strncmp(months[m], __DATE__, 3))
      break;
  }
  m++;

  TinyWireM.begin();

  TinyWireM.beginTransmission(RTC_ADDR);
  TinyWireM.send(0);
  TinyWireM.send((__TIME__[6] - '0') << 4 | (__TIME__[7] - '0') & 0x0F);
  TinyWireM.send((__TIME__[3] - '0') << 4 | (__TIME__[4] - '0') & 0x0F);
  TinyWireM.send((__TIME__[0] - '0') << 4 | (__TIME__[1] - '0') & 0x0F);

  TinyWireM.send(DayOfWeek((__DATE__[7] - '0') * 1000 + (__DATE__[8] - '0') * 100 + (__DATE__[9] - '0') * 10 + (__DATE__[10] - '0'),
                           m,
                           (__DATE__[4] - '0') * 10 + (__DATE__[5] - '0'))); // DoW

  TinyWireM.send( ((__DATE__[4] - '0') << 4) | ((__DATE__[5] - '0') & 0x0F));
  TinyWireM.send(m);
  TinyWireM.send( ((__DATE__[9] - '0') << 4) | ((__DATE__[10] - '0') & 0x0F));
  TinyWireM.endTransmission();

  analogWrite(1, 255);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.print(__TIME__);
  lcd.setCursor(0, 1);
  lcd.print(__DATE__);




}

void loop() {

}
