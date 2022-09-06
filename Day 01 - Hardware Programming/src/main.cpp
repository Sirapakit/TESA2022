#include <Arduino.h>
#include <M5StickCPlus.h>
#include <analogWrite.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wifi.h>
#include <Wire.h>
#include <Adafruit_ADS1x15.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Adafruit_PWMServoDriver myServo = Adafruit_PWMServoDriver();
Adafruit_ADS1015 ads; 

int PWM_FREQ = 60;
int SERVO_MAX = 400;
int SERVO_MIN = 200;
int PULSE;

int delay_IMU_acc_task = 2000;
int delay_IMU_gyro_task = 3000;
int delay_adc_read = 3000;

float accX, accY, accZ;
float gyroX, gyroY, gyroZ;
int Voltage;

bool lcd_state = false;

#define MSG_BUFFER_SIZE (50)
char msg_poten_0[MSG_BUFFER_SIZE];
char msg_poten_1[MSG_BUFFER_SIZE];
char msg_poten_2[MSG_BUFFER_SIZE];
char msg_poten_3[MSG_BUFFER_SIZE];

char msg_accZ[MSG_BUFFER_SIZE];


const char outtopic_pot0 = "Pot0";
const char outtopic_pot1= "acc/X";
const char outtopic_pot2 = "acc/Y";
const char outtopic_pot3 = "acc/Z";
const char *outtopic_accZ = "acc/Z";

void setupWifi()
{
    const char ssid = "Wifi ของ korrawiz";
    const char password = "korrawiz";

    delay(50);
    M5.Lcd.setCursor(95, 95);
    M5.Lcd.print(ssid);
    WiFi.mode(WIFI_STA); // Set the mode to WiFi station mode.

    WiFi.begin(ssid, password); // Start Wifi connection.

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        M5.Lcd.print(".");
    }
    M5.Lcd.setCursor(95, 95);
    M5.Lcd.printf("Success");

}

void mqtt_reconnect()
{
    const char *SECRET_MQTT_CLIENT_ID = "Dzc0JB4oOCkJBhE2MgwfCic";
    const char *mqttUser = "Dzc0JB4oOCkJBhE2MgwfCic";
    const char *mqttPassword = "uAPsxtWtFq8YkWU1gXodMpIF";
    while (!client.connected())
    {
        M5.Lcd.setCursor(95, 110);
        M5.Lcd.println("Attempting");

        if (client.connect(SECRET_MQTT_CLIENT_ID, mqttUser, mqttPassword))
        {
            M5.Lcd.setCursor(95, 110);
            // client.subscribe(intopic);
            M5.Lcd.printf("Success");

        char topic[40];
char payload[100];
sprintf(msg_poten_0,"%s/fingers",MQTT_TOPIC_PREFIX);
sprintf(msg_poten_1,"%s/fingers",MQTT_TOPIC_PREFIX);
sprintf(msg_poten_2,"%s/fingers",MQTT_TOPIC_PREFIX);
sprintf(msg_poten_3,"%s/fingers",MQTT_TOPIC_PREF
IX);
sprintf(msg_accZ,"%s/fingers",MQTT_TOPIC_PREFIX);
client.publish(outtopic_pot0, msg_poten_0);
client.publish(outtopic_pot1, msg_poten_1);
client.publish(outtopic_pot2, msg_poten_2);
client.publish(outtopic_pot3, msg_poten_3);
client.publish(outtopic_accZ, msg_accZ);
Serial.println("PUBLISH");


        }
        else
        {
            M5.Lcd.setCursor(95, 110);
            M5.Lcd.print("Failed");
            delay(5000);

        }
    }
}

void ads_init(){
  if (!ads.begin()) {
    Serial.println("Fail to initialzize ADS1115");
    while (1);
  }
}

void IMU_init()
{
  M5.begin();
  M5.IMU.Init();
  M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
  M5.IMU.getAccelData(&accX, &accY, &accZ);
  Serial.begin(115200);
}

void IMU_acc_task()
{
  static long now_acc;
  if (millis() - now_acc >= delay_IMU_acc_task){
  Serial.print("AccX:");
  Serial.println(accX);
  Serial.print("AccY:");
  Serial.println(accY);
  Serial.print("AccZ:");
  Serial.println(accZ);

  now_acc = millis();
  }
}

void IMU_gyro_task()
{
  static long now_gyro;
  if (millis() - now_gyro >= delay_IMU_gyro_task){
  Serial.print("gyroX:");
  Serial.println(gyroX);
  Serial.print("gyroY:");
  Serial.println(gyroY);
  Serial.print("gyroZ:");
  Serial.println(gyroZ);

  now_gyro = millis();
  }
}

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

void adc_read()
{
    static long now;
    if (millis() - now >= delay_adc_read)
    {

    int16_t pot_value_0 = ads.readADC_SingleEnded(0);
    int16_t pot_value_1 = ads.readADC_SingleEnded(1);
    int16_t pot_value_2 = ads.readADC_SingleEnded(2);
    int16_t pot_value_3 = ads.readADC_SingleEnded(3);

    int percentage_0 = map(pot_value_0, 180, 231, 0, 100);
    int percentage_1 = map(pot_value_1, 0, 1200, 0, 100);
    int percentage_2 = map(pot_value_2, 170, 261, 0, 100);
    int percentage_3 = map(pot_value_3, 180, 220, 0, 100);

        Serial.print("Voltage_0:");
        Serial.println(percentage_0);

        Serial.print("Voltage_1:");
        Serial.println(percentage_1);

        Serial.print("Voltage_2:");
        Serial.println(percentage_2);

        Serial.print("Voltage_3:");
        Serial.println(percentage_3);

        now = millis();
    }
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

void servo_lock_algo(){
  

  M5.Lcd.setTextSize(2);

  M5.Lcd.setCursor(10, 45);
  M5.Lcd.println(accZ);
  
  
  if ( accZ < 2.0 ){
      Serial.println("Normal");
      delay(1000);
  }
  else {
    // for (int i = 0; i < 100; i++){
    Serial.println("Failing downward");
         myServo.setPWM(3, 0, 600);
         myServo.setPWM(4, 0, 600);
         myServo.setPWM(5, 0, 600);
         myServo.setPWM(6, 0, 600);
         delay(1000);

      // Serial.print(i);
  //}
    delay(2000);

  }
}

void setup()
{
  ads_init();
  lcd_init();
  IMU_init();
  servo_motor_init();
}

void loop()
{
  // static int state = 0;
  // switch (state)
  //   {
  //     case 0:
  //     {
        IMU_acc_task();
    //     break;
    // }
    
    //   case 1:
    // {
        IMU_gyro_task();
    //     break;
    // }
    
    //   case 2:
    //   {
        adc_read();
    //     break;
    //   }
    
    //   case 3:
    // {
      servo_lock_algo();
  //     break;
  //   }
    
  //   default:
  //   {
  //     state = 0;
  //     break;
  //   }
  //   }
  //   state++;
        
  // reset_button();
}