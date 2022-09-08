#include <Arduino.h>
#include <M5StickCPlus.h>
#include <analogWrite.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wifi.h>
#include <Wire.h>
#include <Adafruit_ADS1x15.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Adafruit_ADS1115 ads;

int PWM_FREQ = 60;
int SERVO_MAX = 400;
int SERVO_MIN = 200;
int PULSE;

int delay_IMU_acc_task = 2000;
int delay_IMU_gyro_task = 3000;
int delay_adc_read = 3000;

int percentage_0, percentage_1, percentage_2, percentage_3;

float accX, accY, accZ;
float gyroX, gyroY, gyroZ;
int Voltage;

bool lcd_state = false;

const char *outtopic = "channels/1850319/publish";

#define MSG_BUFFER_SIZE (200)
char msg_poten_0[MSG_BUFFER_SIZE];
// char msg_poten_1[MSG_BUFFER_SIZE];
// char msg_poten_2[MSG_BUFFER_SIZE];
// char msg_poten_3[MSG_BUFFER_SIZE];
char msg_accZ[MSG_BUFFER_SIZE];
char msg_outtopic[MSG_BUFFER_SIZE];

// const char *outtopic_pot0 = "channels/1850319/publish/Pot_0";
// const char *outtopic_pot1 = "channels/1850319/publish/Pot_1";
// const char *outtopic_pot2 = "channels/1850319/publish/Pot_2";
// const char *outtopic_pot3 = "channels/1850319/publish/Pot_3";
// const char *outtopic_accZ = "channels/1850319/publish/accZ";

void setupWifi();
// void callback(char *intopic, byte *payload, unsigned int length);
void reConnect();

void mqtt_wifi_init()
{
  const char *mqtt_server = "mqtt3.thingspeak.com";
  // const char *mqtt_server = "10.13.8.163";
  // const char *mqtt_server = "10.13.8.180";
  // const char *mqtt_server = "broker.hivemq.com";
  // const char *mqtt_server = "tcp://0.tcp.ap.ngrok.io:17656";
  setupWifi();
  client.setServer(mqtt_server, 1883);
  // client.setCallback(callback);
}

void setupWifi()
{
  // const char *ssid = "Wifi ของ korrawiz";
  // const char *password = "korrawiz";
  const char *ssid = "Redmi999";
  const char *password = "11111111";
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
  // const char *SECRET_MQTT_CLIENT_ID = "Dzc0JB4oOCkJBhE2MgwfCic";
  // const char *mqttUser = "Dzc0JB4oOCkJBhE2MgwfCic";
  // const char *mqttPassword = "uAPsxtWtFq8YkWU1gXodMpIF";
  while (!client.connected())
  {
    M5.Lcd.setCursor(95, 110);
    M5.Lcd.println("Attempting");

    // if (client.connect(SECRET_MQTT_CLIENT_ID, mqttUser, mqttPassword))
    if (client.connect("Dzc0JB4oOCkJBhE2MgwfCic", "Dzc0JB4oOCkJBhE2MgwfCic", "uAPsxtWtFq8YkWU1gXodMpIF"))
    {
      M5.Lcd.setCursor(95, 110);
      // client.subscribe(intopic);
      M5.Lcd.printf("Success");

      // if (String("TEST/TESA") == "esp32/output")
      // {
      //   Serial.print("Changing output to ");
      // }
    }
    else
    {
      M5.Lcd.setCursor(95, 110);
      M5.Lcd.print("Failed");
      delay(5000);
    }
  }
}

void mqtt_pub()
{
  static long now;
  if (millis() - now >= 5000)
  {
    // snprintf(msg_poten_0, MSG_BUFFER_SIZE, " %d", percentage_0);
    // snprintf(msg_poten_1, MSG_BUFFER_SIZE, " %d", percentage_1);
    // snprintf(msg_poten_2, MSG_BUFFER_SIZE, " %d", percentage_2);
    // snprintf(msg_poten_3, MSG_BUFFER_SIZE, " %d", percentage_3);
    // snprintf(msg_accZ, MSG_BUFFER_SIZE, " %7.2f", accZ);

    // snprintf(msg_poten_0, MSG_BUFFER_SIZE, " %d", 100);
    // snprintf(msg_poten_1, MSG_BUFFER_SIZE, " %d", 111);
    // snprintf(msg_poten_2, MSG_BUFFER_SIZE, " %d", 222);
    // snprintf(msg_poten_3, MSG_BUFFER_SIZE, " %d", 333);
    // char payload[100];

    sprintf(msg_outtopic, "field1=100&field2=111&field3=222&field4=333&field5=%7.2f&status=MQTTPUBLISH", accZ);
    client.publish(outtopic, msg_outtopic);

    // client.publish(outtopic_pot0, msg_poten_0);
    // client.publish(outtopic_pot1, msg_poten_1);
    // client.publish(outtopic_pot2, msg_poten_2);
    // client.publish(outtopic_pot3, msg_poten_3);
    // client.publish(outtopic_accZ, msg_accZ);

    client.loop();
    now = millis();
  }
}

// void callback(char *intopic, byte *payload, unsigned int length)
// {
//   for (int i = 0; i < length; i++)
//   {
//     array_from_payload[i] = ((char)payload[i]);
//   }
//   sub_status = true;
// }

void ads_init()
{
  if (!ads.begin())
  {
    Serial.println("Fail to initialzize ADS1115");
    while (1)
      ;
  }
}

void IMU_init()
{
  M5.IMU.Init();
  Serial.begin(115200);
}

void IMU_acc_task()
{
  M5.IMU.getAccelData(&accX, &accY, &accZ);
  // static long now_acc;
  // if (millis() - now_acc >= delay_IMU_acc_task)
  // {
  //   Serial.print("AccX:");
  //   Serial.println(accX);
  //   Serial.print("AccY:");
  //   Serial.println(accY);
  //   Serial.print("AccZ:");
  //   Serial.println(accZ);

  //   now_acc = millis();
  // }
}

void IMU_gyro_task()
{
  M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
  // static long now_gyro;
  // if (millis() - now_gyro >= delay_IMU_gyro_task)
  // {
  //   // Serial.print("gyroX:");
  //   // Serial.println(gyroX);
  //   // Serial.print("gyroY:");
  //   // Serial.println(gyroY);
  //   // Serial.print("gyroZ:");
  //   // Serial.println(gyroZ);
  //   // M5.Lcd.setCursor(45, 45);
  //   // M5.Lcd.printf("Gyro: %7.2f, %7.2f, %7.2f", gyroX, gyroY, gyroZ);

  //   now_gyro = millis();
  // }
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
  if (millis() - now >= 1500)
  {
    M5.Lcd.setCursor(65, 80);
    M5.Lcd.println(percentage_0);

    M5.Lcd.setCursor(40, 40);
    M5.Lcd.print(accX);

    M5.Lcd.setCursor(90, 40);
    M5.Lcd.print(accY);

    M5.Lcd.setCursor(140, 40);
    M5.Lcd.print(accZ);

    M5.Lcd.setCursor(40, 65);
    M5.Lcd.print(gyroX);

    M5.Lcd.setCursor(90, 65);
    M5.Lcd.print(gyroY);

    M5.Lcd.setCursor(140, 65);
    M5.Lcd.print(gyroZ);

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
  if (millis() - now >= 2000)
  {

    int pot_value_0 = ads.readADC_SingleEnded(0);
    int pot_value_1 = ads.readADC_SingleEnded(1);
    int pot_value_2 = ads.readADC_SingleEnded(2);
    int pot_value_3 = ads.readADC_SingleEnded(3);

    percentage_0 = map(pot_value_0, 180, 231, 0, 100);
    percentage_1 = map(pot_value_1, 0, 1200, 0, 100);
    percentage_2 = map(pot_value_2, 170, 261, 0, 100);
    percentage_3 = map(pot_value_3, 180, 220, 0, 100);

    // Serial.print("Voltage_0:");
    // Serial.println(percentage_0);

    Serial.print("Voltage_1:");
    Serial.println(pot_value_1);

    Serial.print("Voltage_2:");
    Serial.println(pot_value_2);

    // Serial.print("Voltage_3:");
    // Serial.println(percentage_3);

    // M5.Lcd.setCursor(45, 65);
    // M5.Lcd.printf("Percentage: %d, %d, %d, %d", percentage_0, percentage_1, percentage_2, percentage_3);
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

void servo_lock_algo()
{

  M5.Lcd.setTextSize(2);

  M5.Lcd.setCursor(10, 45);
  M5.Lcd.println(accZ);

  if (accZ < 2.0)
  {
    Serial.println("Normal");
    delay(1000);
  }
  else
  {
    // for (int i = 0; i < 100; i++){
    Serial.println("Failing downward");
    pwm.setPWM(3, 0, 600);
    pwm.setPWM(4, 0, 600);
    pwm.setPWM(5, 0, 600);
    pwm.setPWM(6, 0, 600);
    delay(1000);

    // Serial.print(i);
    //}
    delay(2000);
  }
}

void setup()
{
  lcd_init();
  mqtt_wifi_init();
  // ads_init();
  IMU_init();
  // servo_motor_init();
}

void loop()
{
  static int state = 0;

  switch (state)
  {
  case 0:
  {
    mqtt_reconnect();
    break;
  }
  case 1:
  {
    IMU_acc_task();
    break;
  }
  case 2:
  {
    IMU_gyro_task();
    break;
  }
  case 3:
  {
    // adc_read();
    // servo_lock_algo();
    break;
  }
  case 4:
  {
    lcd_task();
    break;
  }
  case 5:
  {
    mqtt_pub();
    break;
  }
  default:
  {
    state = 0;
    break;
  }
  }
  state++;
  // reset_button();
}