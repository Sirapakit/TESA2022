#include <Arduino.h>
#include <M5StickCPlus.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <analogWrite.h>

WiFiClient espClient;
PubSubClient client(espClient);

// Requirement
// 6 IMU --> Acc, Gyro X, Y, Z
// Wifi, MQTT Status

const char *outtopic_voltage = "Voltage";
const char *outtopic_accX = "acc/X";
const char *outtopic_accY = "acc/Y";
const char *outtopic_accZ = "acc/Z";
const char *outtopic_gyroX = "gyro/X";
const char *outtopic_gyroY = "gyro/Y";
const char *outtopic_gyroZ = "gyro/Z";
const char *intopic = "TEST/TESA";

unsigned long lastMsg = 0;
int value = 0;

void setup()
{
    Serial.begin(115200);
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.setTextSize(1);

    M5.Lcd.drawRect(5, 5, 220, 120, BLUE);

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
    M5.Lcd.println("350");

    M5.Lcd.setCursor(10, 95);
    M5.Lcd.print("Wifi-Status :  ");
    // M5.Lcd.setCursor(10, 70);
    M5.Lcd.println("Connected");

    M5.Lcd.setCursor(10, 110);
    M5.Lcd.print("MQTT-Status :  ");
    // M5.Lcd.setCursor(10, 70);
    M5.Lcd.println("Connected");
}

void loop()
{
    for (int i = 110; i <= 114; i++)
    {
        // Serial.println(i);

        M5.Lcd.setCursor(40, 40);
        M5.Lcd.print(i);

        M5.Lcd.setCursor(90, 40);
        M5.Lcd.print(i);

        M5.Lcd.setCursor(140, 40);
        M5.Lcd.print(i);

        delay(500);
    }

    char charvalue = '5';
    int number = ((int(charvalue) + 0) - 48);

    // M5.Lcd.printf("Voltage= 999");
    // M5.Lcd.println();
    // M5.Lcd.printf("accX: 0.5 accY: 0.5 accZ: 0.5 ");
    // M5.Lcd.println();
    // M5.Lcd.printf("gyroX: 0.5 gyroY: 0.5 gyroZ: 0.5 ");
    // M5.Lcd.println();

    // unsigned long now = millis();
    // if (now - lastMsg > 2000)
    // {
    //     lastMsg = now;
    //     ++value;

    //     M5.Lcd.print("Publish message: ");
    //     M5.Lcd.println("msg_voltage");

    //     M5.Lcd.print("Publish message: ");
    //     M5.Lcd.println("msg_accX");

    //     M5.Lcd.print("Publish message: ");
    //     M5.Lcd.println("msg_accY");

    //     M5.Lcd.print("Publish message: ");
    //     M5.Lcd.println("msg_accZ");

    //     M5.Lcd.print("Publish message: ");
    //     M5.Lcd.println("msg_gyroX");

    //     M5.Lcd.print("Publish message: ");
    //     M5.Lcd.println("msg_gyroY");

    //     M5.Lcd.print("Publish message: ");
    //     M5.Lcd.println("msg_gyroZ");

    //     delay(500);

    //     if (value % 7 == 0)
    //     {
    //         M5.Lcd.fillScreen(BLACK);
    //         M5.Lcd.setCursor(0, 0);
    //     }
    // }

    // if (M5.BtnA.wasReleasefor(100))
    // {
    //     esp_restart();
    // }
    // M5.update(); // Detect whether the keystroke state has changed.
    // delay(100);
}

void callback(char *intopic, byte *payload, unsigned int length)
{

    const int max_array = 50;
    const char *angle_sub[max_array];
    int count = 0;
    char array[50];

    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
        array[i] = ((char)payload[i]);
    }
    Serial.println();

    float kp = 7;

    int curr_angle = analogRead(36);

    Serial.print("angle_sub: ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((int(array[i]) + 0) - 48);
    }
    Serial.println();

    delay(1000);
}
