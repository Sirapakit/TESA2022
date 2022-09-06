#include <Arduino.h>
#include <M5StickCPlus.h>
#include <analogWrite.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int PWM_FREQ = 60;
int SERVO_MAX = 400;
int SERVO_MIN = 200;

uint16_t PULSE;
bool lcd_state = false;
void lcd_init()
{
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextSize(1);

  M5.Lcd.drawRect(5, 5, 220, 120, BLUE);
  M5.Lcd.drawRect(2, 2, 220, 120, PINK);

  M5.Lcd.setCursor(80, 10);
  M5.Lcd.println("BME RANGER 12");
}

void lcd_task()
{
  // static long now;
  // if (millis() - now >= 500)
  // {
  M5.Lcd.setCursor(10, 80);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print(PULSE);
  // now = millis();
  // }
}

void servo_motor_init()
{
  Wire.begin(32, 33);
  pwm.begin();
  pwm.setPWMFreq(PWM_FREQ);
  // servo should rotate first to clarify that the servo is usable
}

void servo_motor()
{
  lcd_state = true;
  for (PULSE = 0; PULSE <= SERVO_MAX; PULSE++)
  {
    pwm.setPWM(0, 0, PULSE);
    M5.Lcd.setCursor(10, 80);
    M5.Lcd.setTextSize(2);
    M5.Lcd.print(PULSE);
    // pwm.setPWM(1, 0, SERVO_2);
  }
  delay(300);
  for (PULSE = SERVO_MAX; PULSE >= SERVO_MIN; PULSE--)
  {
    pwm.setPWM(0, 0, PULSE);
    M5.Lcd.setCursor(10, 80);
    M5.Lcd.setTextSize(2);
    M5.Lcd.print(PULSE);
    // pwm.setPWM(1, 0, SERVO_2);
  }
  delay(300);
  // lcd_state = false;
}

void setup()
{
  lcd_init();
  servo_motor_init();
}

void loop()
{
  servo_motor();
  // if (lcd_state == false)
  // {
  // return;
  // }
  // else
  // {
  // lcd_task();
  // lcd_state = false;
  // }
}