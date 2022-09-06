#include <Arduino.h>
#include <M5StickCPlus.h>
#include <analogWrite.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int PWM_FREQ = 60;
int SERVO_MAX = 400;
int SERVO_MIN = 200;
int PULSE;

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
  static long now;
  if (millis() - now >= 500)
  {
    M5.Lcd.setCursor(10, 80);
    M5.Lcd.setTextSize(2);
    M5.Lcd.print(PULSE);

    now = millis();
  }
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
    // pwm.setPWM(1, 0, PULSE);
    // pwm.setPWM(2, 0, PULSE);
    // pwm.setPWM(3, 0, PULSE);
  }
  delay(300);
  for (PULSE = SERVO_MAX; PULSE >= SERVO_MIN; PULSE--)
  {
    pwm.setPWM(0, 0, PULSE);
    // pwm.setPWM(1, 0, PULSE);
    // pwm.setPWM(2, 0, PULSE);
    // pwm.setPWM(3, 0, PULSE);
  }
  delay(300);
  // lcd_state = false;
}

void reset_button()
{
  if (M5.BtnB.pressedFor(1000))
  {
    Serial.println("Restart in 3 seconds...");
    M5.Lcd.fillScreen(PINK);
    delay(3000);
    esp_restart();
  }
  M5.update();
}

void setup()
{
  lcd_init();
  servo_motor_init();
}

void loop()
{
  static int state = 0;
  switch (state)
  {
    {
    case 0:
      servo_motor();
      break;
    }
    {
    case 1:
      lcd_task();
      break;
    }
  default:
    break;
  }
  state++;
  reset_button();
}