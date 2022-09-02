#include <Arduino.h>
#include <M5StickCPlus.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <analogWrite.h>

WiFiClient espClient;
PubSubClient client(espClient);

void mqtt_wifi_init()
{

    const char *mqtt_server = "10.13.8.163";

    const char *outtopic_voltage = "Voltage";
    const char *outtopic_accX = "acc/X";
    const char *outtopic_accY = "acc/Y";
    const char *outtopic_accZ = "acc/Z";
    const char *outtopic_gyroX = "gyro/X";
    const char *outtopic_gyroY = "gyro/Y";
    const char *outtopic_gyroZ = "gyro/Z";
    const char *intopic = "TEST/TESA";
    /// add commit
    M5.Lcd.printf("Connecting to %s", ssid);
    WiFi.mode(WIFI_STA);        // Set the mode to WiFi station mode.
    WiFi.begin(ssid, password); // Start Wifi connection.

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        M5.Lcd.print(".");
    }
    M5.Lcd.printf("\nSuccess\n");

    setupWifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void setupWifi()
{
    const char *ssid = "catsvn";
    const char *password = "catsvn2000";
    delay(10);
    M5.Lcd.printf("Connecting to %s", ssid);
    WiFi.mode(WIFI_STA);        // Set the mode to WiFi station mode.  设置模式为WIFI站模式
    WiFi.begin(ssid, password); // Start Wifi connection.  开始wifi连接

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        M5.Lcd.print(".");
    }
    M5.Lcd.printf("\nSuccess\n");
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
    M5.Lcd.setCursor(10, 110);
    M5.Lcd.println(oltage);

    M5.Lcd.setCursor(10, 110);
    M5.Lcd.println(accX);

    M5.Lcd.setCursor(10, 110);
    M5.Lcd.println(ccY);

    M5.Lcd.setCursor(10, 110);
    M5.Lcd.println(accZ);

    M5.Lcd.setCursor(10, 110);
    M5.Lcd.println(gyroX);

    M5.Lcd.setCursor(10, 110);
    M5.Lcd.println(gyroY);

    M5.Lcd.setCursor(10, 110);
    M5.Lcd.println(gyroZ);

    M5.Lcd.setCursor(10, 110);
    M5.Lcd.println(wifi_stated);

    M5.Lcd.setCursor(10, 110);
    M5.Lcd.println(mqtt_stated);
}

void mqtt_task()
{
    while (!client.connected())
    {
        M5.Lcd.print("Attempting MQTT connection...");
        // Create a random client ID.
        String clientId = "MM5St-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect.
        if (client.connect(clientId.c_str()))
        {
            M5.Lcd.printf("\nSuccess\n");
            // Once connected, publish an announcement to the topic.
            // client.publish("M5Stack", "hello world");
            // ... and resubscribe.
            client.subscribe(intopic);

            if (String("TEST/TESA") == "esp32/output")
            {
                Serial.print("Changing output to ");
            }
        }
        else
        {
            M5.Lcd.print("failed, rc=");
            M5.Lcd.print(client.state());
            M5.Lcd.println("try again in 5 seconds");
            delay(5000);
        }
    }
}
void callback(char *intopic, byte *payload, unsigned int length) // use to get message from publisher
{
    char array[50];

    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
        array[i] = ((char)payload[i]);
    }
    Serial.println();
}

void adc_read_task()
{
    static int pot_value = analogRead(36);
}

void setup()
{
    imu_init();
    lcd_init();
    mqtt_wifi_init();
}

void loop()
{
    imu_task();
    lcd_task();
    mqtt_task();
    adc_read_task();
    // servo_task();
    // button_task();
    //
}