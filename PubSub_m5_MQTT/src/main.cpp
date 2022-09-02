#include <Arduino.h>
#include <M5StickCPlus.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <analogWrite.h>

WiFiClient espClient;
PubSubClient client(espClient);

// Configure the name and password of the connected wifi and your MQTT Serve
// host.

// const char* mqtt_server = "broker.hivemq.com";

const char *outtopic_voltage = "Voltage";
const char *outtopic_accX = "acc/X";
const char *outtopic_accY = "acc/Y";
const char *outtopic_accZ = "acc/Z";
const char *outtopic_gyroX = "gyro/X";
const char *outtopic_gyroY = "gyro/Y";
const char *outtopic_gyroZ = "gyro/Z";

const char *intopic = "TEST/TESA";

int analog_value;
int analog_value_sub;
float accX, accY, accZ;
float gyroX, gyroY, gyroZ;

const int freq = 60;
const int ledChannel = 0;
const int resolution = 8;

int GPIO_pwm = 25;
float angle_sub;
float curr_angle;
float error;
float prev_error;
float pwm;
float curr_time;
float prev_time;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg_voltage[MSG_BUFFER_SIZE];
char msg_accX[MSG_BUFFER_SIZE];
char msg_accY[MSG_BUFFER_SIZE];
char msg_accZ[MSG_BUFFER_SIZE];
char msg_gyroX[MSG_BUFFER_SIZE];
char msg_gyroY[MSG_BUFFER_SIZE];
char msg_gyroZ[MSG_BUFFER_SIZE];
int value = 0;

float floatMap(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setupWifi();
void callback(char *topic, byte *payload, unsigned int length);
void reConnect();

void setup()
{
    Serial.begin(115200);
    M5.begin();
    M5.IMU.Init();
    Wire.begin(32, 33);
    pinMode(GPIO_pwm, OUTPUT);
    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(GPIO_pwm, ledChannel);
    M5.Lcd.setRotation(3);
    setupWifi();
    client.setServer(mqtt_server,
                     1883); // Sets the server details.
    client.setCallback(
        callback); // Sets the message callback function.
}

void loop()
{
    int analog_value = analogRead(36);
    int voltage = floatMap(analog_value, 0, 4095, 0, 360);
    M5.IMU.getAccelData(&accX, &accY, &accZ);
    M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
    M5.Lcd.printf("Voltage= %ld", voltage);
    M5.Lcd.println();
    M5.Lcd.printf("accX:%7.2f accY:%7.2f accZ:%7.2f ", accX, accY, accZ);
    M5.Lcd.println();
    M5.Lcd.printf("gyroX:%7.2f gyroY:%7.2f gyroZ:%7.2f ", gyroX, gyroY, gyroZ);
    M5.Lcd.println();

    if (!client.connected())
    {
        reConnect();
    }
    client.loop(); // This function is called periodically to allow clients to
                   // process incoming messages and maintain connections to the
                   // server.
    unsigned long now =
        millis(); // Obtain the host startup duration.
    if (now - lastMsg > 2000)
    {
        lastMsg = now;
        ++value;
        snprintf(msg_voltage, MSG_BUFFER_SIZE, " %ld", voltage);
        snprintf(msg_accX, MSG_BUFFER_SIZE, " %7.2f", accX);
        snprintf(msg_accY, MSG_BUFFER_SIZE, " %7.2f", accY);
        snprintf(msg_accZ, MSG_BUFFER_SIZE, " %7.2f", accZ);
        snprintf(msg_gyroX, MSG_BUFFER_SIZE, " %7.2f", gyroX);
        snprintf(msg_gyroY, MSG_BUFFER_SIZE, " %7.2f", gyroY);
        snprintf(msg_gyroZ, MSG_BUFFER_SIZE, " %7.2f", gyroZ);

        if (M5.BtnA.wasReleasefor(100))
        {
            esp_restart();
        }
        M5.update(); // Detect whether the keystroke state has changed.
        delay(100);
    }

    void setupWifi()
    {
        delay(10);
        M5.Lcd.printf("Connecting to %s", ssid);
        WiFi.mode(
            WIFI_STA);              // Set the mode to WiFi station mode.
        WiFi.begin(ssid, password); // Start Wifi connection.

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            M5.Lcd.print(".");
        }
        M5.Lcd.printf("\nSuccess\n");
    }
    void callback(char *intopic, byte *payload, unsigned int length)
    {
        // M5.Lcd.print("Message arrived [");
        // M5.Lcd.print(intopic);
        // M5.Lcd.print("] ");
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
        // float ki = 0.03;
        // float kd = 0.5;
        int curr_angle = analogRead(36);

        Serial.print("angle_sub: ");
        for (int i = 0; i < length; i++)
        {
            // int test;
            // test = atoi(array);
            // Serial.printf("%ld", test);
            Serial.print((int(array[i]) + 0) - 48);
        }
        Serial.println();
        // float degree = (curr_angle*360)/4096;
        // error = angle_sub - degree;
        curr_time = millis();
        // float ed = (error - prev_error)/(curr_time - prev_time)*0.001;
        // float eint = eint + error*(curr_time - prev_time)*0.001;
        float u = kp * error; //+ ki*eint+ kd*ed;
        if (u > 1023)
        {
            u = 1023;
        }

        if (u < 1023)
        {
            u = 1023;
        }

        if (error > 0)
        {
            pwm = abs(u);
            ledcWrite(ledChannel, pwm);
        }

        prev_time = curr_time;
        prev_error = error;
        Serial.println("Motor Testing");
        delay(1000);
    }

    void reConnect()
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
