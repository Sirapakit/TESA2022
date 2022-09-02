#include <Arduino.h>
#include <M5StickCPlus.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <analogWrite.h>
#include <Adafruit_PWMServoDriver.h>

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


const char *outtopic_voltage = "Voltage";
const char *outtopic_accX = "acc/X";
const char *outtopic_accY = "acc/Y";
const char *outtopic_accZ = "acc/Z";
const char *outtopic_gyroX = "gyro/X";
const char *outtopic_gyroY = "gyro/Y";
const char *outtopic_gyroZ = "gyro/Z";
const char *intopic = "TEST/TESA";

#define MSG_BUFFER_SIZE (50)
char msg_voltage[MSG_BUFFER_SIZE];
char msg_accX[MSG_BUFFER_SIZE];
char msg_accY[MSG_BUFFER_SIZE];
char msg_accZ[MSG_BUFFER_SIZE];
char msg_gyroX[MSG_BUFFER_SIZE];
char msg_gyroY[MSG_BUFFER_SIZE];
char msg_gyroZ[MSG_BUFFER_SIZE];

float accX, accY, accZ;
float gyroX, gyroY, gyroZ;

int Voltage = 50;
int angle_servo_1, angle_servo_2;

bool wifi_status = false;
bool sub_status = false;

char array[50];

int servomin1, servomin2;
int servomax1, servomax2; 

void setupWifi();
void callback(char *intopic, byte *payload, unsigned int length);
void reConnect();

void mqtt_wifi_init()
{
    const char *mqtt_server = "10.13.8.163";

    setupWifi();
    // const char *mqtt_server = "tcp://0.tcp.ap.ngrok.io:17656";
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void setupWifi()
{
    const char *ssid = "catsvn";
    // const char *ssid = "BCI LAB 2.4";
    // const char *password = "bcimemberonly";
    const char *password = "catsvn2000";

    delay(10);
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

    wifi_status = true;
}

void imu_init()
{
    M5.IMU.Init();
}

void imu_task()
{
    M5.IMU.getAccelData(&accX, &accY, &accZ);
    M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
}

void lcd_init()
{
    M5.begin();
    M5.lcd.fillScreen(BLACK);
    M5.Lcd.setRotation(3);
    M5.Lcd.setTextSize(1);

    M5.Lcd.drawRect(5, 5, 220, 120, BLUE);
    M5.Lcd.drawRect(2, 2, 220, 120, PINK);

    M5.Lcd.setCursor(80, 10);
    M5.Lcd.println("BME RANGER 12");

    M5.Lcd.setCursor(10, 30);
    M5.Lcd.println("Acclerometer");
    M5.Lcd.setCursor(10, 40);
    M5.Lcd.println("   X       Y       Z");

    M5.Lcd.setCursor(10, 55);
    M5.Lcd.println("Gyrometer");
    M5.Lcd.setCursor(10, 65);
    M5.Lcd.println("   X       Y       Z");

    M5.Lcd.setCursor(10, 80);
    M5.Lcd.print("Voltage:  ");

    M5.Lcd.setCursor(10, 95);
    M5.Lcd.print("Wifi-Status :  ");

    M5.Lcd.setCursor(10, 110);
    M5.Lcd.print("MQTT-Status :  ");
}

void lcd_task()
{

    // UI-voltage
    M5.Lcd.setCursor(65, 80);
    M5.Lcd.println(Voltage);

    // UI-acc
    M5.Lcd.setCursor(40, 40);
    M5.Lcd.print(accX);

    M5.Lcd.setCursor(90, 40);
    M5.Lcd.print(accY);

    M5.Lcd.setCursor(140, 40);
    M5.Lcd.print(accZ);

    // UI-gyro
    M5.Lcd.setCursor(40, 65);
    M5.Lcd.print(gyroX);

    M5.Lcd.setCursor(90, 65);
    M5.Lcd.print(gyroY);


    M5.Lcd.setCursor(140, 65);
    M5.Lcd.print(gyroZ);

    delay(1000);
}

void mqtt_reconnect()
{
    while (!client.connected())
    {
        M5.Lcd.setCursor(95, 110);
        M5.Lcd.println("Attempting");

        String clientId = "MM5St-";
        clientId += String(random(0xffff), HEX);

        if (client.connect(clientId.c_str()))
        {
            M5.Lcd.setCursor(95, 110);
            client.subscribe(intopic);
            M5.Lcd.printf("Success");

            if (String("TEST/TESA") == "esp32/output")
            {
                Serial.print("Changing output to ");
            }
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
    static long now = millis();
    if (millis() - now >= 1000)
    {
        snprintf(msg_voltage, MSG_BUFFER_SIZE, " %d", Voltage);
        snprintf(msg_accX, MSG_BUFFER_SIZE, " %7.2f", accX);
        snprintf(msg_accY, MSG_BUFFER_SIZE, " %7.2f", accY);
        snprintf(msg_accZ, MSG_BUFFER_SIZE, " %7.2f", accZ);
        snprintf(msg_gyroX, MSG_BUFFER_SIZE, " %7.2f", gyroX);
        snprintf(msg_gyroY, MSG_BUFFER_SIZE, " %7.2f", gyroY);
        snprintf(msg_gyroZ, MSG_BUFFER_SIZE, " %7.2f", gyroZ);

        client.publish(outtopic_voltage, msg_voltage);
        client.publish(outtopic_accX, msg_accX);
        client.publish(outtopic_accY, msg_accY);
        client.publish(outtopic_accZ, msg_accZ);
        client.publish(outtopic_gyroX, msg_gyroX);
        client.publish(outtopic_gyroY, msg_gyroY);
        client.publish(outtopic_gyroZ, msg_gyroZ);

        client.loop();
        now = millis();
    }
}

void callback(char *intopic, byte *payload, unsigned int length)
{

    Serial.println("Callback");

    for (int i = 0; i < length; i++)
    {
        array[i] = ((char)payload[i]);
        //Serial.println((char)payload[i]);
        //Serial.println(array[i]);
    }
    sub_status = true;
}

void mqtt_sub()
{
    if (sub_status = true)
    {
    // Serial.println("MQTT_SUB");
    // Serial.println(array);
    sscanf(array, "%d%d", &angle_servo_1, &angle_servo_2);
    // Serial.print("Angle1: ");
    // Serial.println(angle_servo_1);
    // Serial.print("Angle2: ");
    // Serial.println(angle_servo_2);
    // do smth
    // decode
    sub_status = false;
    } 
}

void servo_motor_init()
{
    Wire.begin(32,33);
    pwm.begin();
    pwm.setPWMFreq(60);

}

void servo_motor()
{
    servomin1 = 125;
    servomin2 = 0;
    servomax1 = angle_servo_1;
    Serial.print("servomax1: ");
    Serial.println(servomax1);
    servomax2 = angle_servo_2;
    Serial.print("servomax2: ");
    Serial.println(servomax2);
    // for (int pulselen = servomin1; pulselen <= servomax1; pulselen++) {
        // pwm.setPWM(0, 0, pulselen);
        pwm.setPWM(0, 0, servomax1);

    // }

    // for (int pulselen = servomin2; pulselen <= servomax2; pulselen++) {
        // pwm.setPWM(1, 0, pulselen);
    pwm.setPWM(1, 0, servomax2);
    // }

  delay(500);
}

void adc_read_task()
{
    static int pot_value = analogRead(36);
    long now;
    if (millis() - now >= 100)
    {
        // do something
        Serial.println("Now = ");
        now = millis();
    }
}

void setup()
{
    lcd_init();
    mqtt_wifi_init();
    imu_init();
    servo_motor_init();
    // client.setCallback(callback);
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
        mqtt_sub();
        break;
    }
    case 2:
    {
        servo_motor();
        break;
    }
    case 3:
    {
        imu_task();
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
    // case 6:
    // {
    // adc_read_task();
    // break;
    // }
    default:
    {
        state = 0;
        break;
    }
    }
    // }
    state++;
    // reset_button_task();
}