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

unsigned long lastMsg = 0;
int value = 0;

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    for (int i = 110; i <= 114; i++)
    {
        // Serial.println(i);

        M5.Lcd.setCursor(40, 40);
        M5.Lcd.print(i + 57);

        M5.Lcd.setCursor(90, 40);
        M5.Lcd.print(i + 67);

        M5.Lcd.setCursor(140, 40);
        M5.Lcd.print(i - 4);

        M5.Lcd.setCursor(40, 65);
        M5.Lcd.print(i + 45);

        M5.Lcd.setCursor(90, 65);
        M5.Lcd.print(i + 569);

        M5.Lcd.setCursor(140, 65);
        M5.Lcd.print(i + 137);

        // Voltage
        M5.Lcd.setCursor(65, 80);
        M5.Lcd.println(i);

        // Wifi-Status
        M5.Lcd.setCursor(95, 95);
        M5.Lcd.println(i);

        // MqTT Status
        M5.Lcd.setCursor(95, 110);
        M5.Lcd.println(i);

        delay(500);
    }

    char charvalue = '5';
    int number = ((int(charvalue) + 0) - 48);

    int numVal[] = {1, 3, 7, 9, '\0'};

    char text[] = "135 987";

    int x, y;
    sscanf(text, "%d%d", &x, &y);
    Serial.print("text to int: ");
    // Serial.println(x);
    // Serial.println(y);

    // int x = atoi(numVal);

    // int numValue = 1379;
    // int *pointNum;

    // pointNum = &numValue;

    // Serial.print("Number value: ");
    // Serial.println(*pointNum);

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
