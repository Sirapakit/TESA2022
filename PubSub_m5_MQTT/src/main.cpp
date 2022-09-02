#include <Arduino.h>
#include <M5StickCPlus.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid        = "catsvn";
const char* password    = "catsvn2000";
const char* mqtt_server = "10.13.8.180";
// const char* mqtt_server = "broker.hivemq.com";

const char* outtopic_voltage = "Voltage"; 
const char* outtopic_accX = "acc/X";
const char* outtopic_accY = "acc/Y";
const char* outtopic_accZ = "acc/Z";
const char* outtopic_gyroX = "gyro/X";
const char* outtopic_gyroY = "gyro/Y";
const char* outtopic_gyroZ = "gyro/Z";

const char* intopic = "TEST/TESA";

float accX, accY, accZ;
float gyroX, gyroY, gyroZ;

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


float floatMap(float x, float in_min, float in_max, float out_min, float out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setupWifi();
void callback(char* topic, byte* payload, unsigned int length);
void reConnect();
void restart();

void setup() {
    Serial.begin(115200);
    M5.begin();
    M5.IMU.Init();
    Wire.begin(32,33);

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
    
    setupWifi();
    client.setServer(mqtt_server,1883);  // Sets the server details.
    client.setCallback(callback);  // Sets the message callback function.
}

void loop() {
    int analog_value = analogRead(36);
    int voltage = floatMap(analog_value, 0, 4095, 0, 360);

    M5.IMU.getAccelData(&accX, &accY, &accZ);
    M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);

    //UI-voltage
    M5.Lcd.setCursor(65, 80);
    M5.Lcd.println(voltage);

    //UI-acc
    M5.Lcd.setCursor(40, 40);
    M5.Lcd.print(accX);

    M5.Lcd.setCursor(90, 40);
    M5.Lcd.print(accY);

    M5.Lcd.setCursor(140, 40);
    M5.Lcd.print(accZ);

    //UI-gyro
    M5.Lcd.setCursor(40, 65);
    M5.Lcd.print(gyroX);

    M5.Lcd.setCursor(90, 65);
    M5.Lcd.print(gyroY);

    M5.Lcd.setCursor(140, 65);
    M5.Lcd.print(gyroZ);

    if (M5.BtnA.wasReleasefor(100)) {
        restart();
    }

    if (!client.connected()) {
        reConnect();
    }

unsigned long now = millis();  

        snprintf(msg_voltage, MSG_BUFFER_SIZE," %ld",voltage);
        snprintf(msg_accX, MSG_BUFFER_SIZE," %7.2f",accX);
        snprintf(msg_accY, MSG_BUFFER_SIZE," %7.2f",accY);
        snprintf(msg_accZ, MSG_BUFFER_SIZE," %7.2f",accZ);
        snprintf(msg_gyroX, MSG_BUFFER_SIZE," %7.2f",gyroX);
        snprintf(msg_gyroY, MSG_BUFFER_SIZE," %7.2f",gyroY);
        snprintf(msg_gyroZ, MSG_BUFFER_SIZE," %7.2f",gyroZ);
<<<<<<< HEAD

client.publish(outtopic_voltage, msg_voltage); 
client.publish(outtopic_accX, msg_accX);
client.publish(outtopic_accY, msg_accY);
client.publish(outtopic_accZ, msg_accZ);
client.publish(outtopic_gyroX, msg_gyroX);
client.publish(outtopic_gyroY, msg_gyroY);

client.publish(outtopic_gyroZ, msg_gyroZ);
delay(500);

client.loop();  

     if (M5.BtnA.wasReleasefor(100)) {
        esp_restart();
     }
      M5.update();  // Detect whether the keystroke state has changed.
      delay(100);  
}

void setupWifi() { 
    delay(10);
    M5.Lcd.setCursor(95, 95);
    M5.Lcd.print(ssid);
    WiFi.mode(WIFI_STA);  // Set the mode to WiFi station mode.

    WiFi.begin(ssid, password);  // Start Wifi connection. 

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.print(".");
    }
    M5.Lcd.setCursor(95, 95);
    M5.Lcd.printf("Success");
}
void callback(char* intopic, byte* payload, unsigned int length) {

    const int max_array = 50;
    const char* angle_sub[max_array];
    int count = 0;
    char array[50];
    
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      array[i] = ((char)payload[i]);
    }
    Serial.println();

    int angle_1, angle_2;
    sscanf(array, "%d%d", &angle_1, &angle_2);
<<<<<<< HEAD
    Serial.print("Angle1: ");
    Serial.println(angle_1);
    Serial.print("Angle2: ");
    Serial.println(angle_2);

    int angle_mock = 180;

    int curr_time = millis();

    Serial.println("Motor Testing");
    delay(1000);
}

void reConnect() {
    while (!client.connected()) {
        M5.Lcd.setCursor(95, 110);
        M5.Lcd.println("Attempting");

        String clientId = "MM5St-";
        clientId += String(random(0xffff), HEX);

        if (client.connect(clientId.c_str())) {
            M5.Lcd.setCursor(95, 110);
            M5.Lcd.printf("Success");
            client.subscribe(intopic);

            if (String("TEST/TESA") == "esp32/output") {
                Serial.print("Changing output to ");}
        } else {
            M5.Lcd.setCursor(95, 110);
            M5.Lcd.print("Failed");

            delay(5000);
        }
    }
}

void restart() {
    esp_restart();
    M5.update(); 
    delay(100); 
}
