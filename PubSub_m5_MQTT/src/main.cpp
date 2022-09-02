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
    // M5.Lcd.setCursor(10, 70);
    M5.Lcd.println("Connected");

    M5.Lcd.setCursor(10, 110);
    M5.Lcd.print("MQTT-Status :  ");
    // M5.Lcd.setCursor(10, 70);
    M5.Lcd.println("Connected");

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

    
    if (!client.connected()) 
    {
        reConnect();
    }
    client.loop();  // This function is called periodically to allow clients to
                    // process incoming messages and maintain connections to the
                    // server.
unsigned long now = millis();  // Obtain the host startup duration. 
     
    //  if (now - lastMsg > 2000) {
        //  lastMsg = now;
        //  ++value;
        snprintf(msg_voltage, MSG_BUFFER_SIZE," %ld",voltage);
        snprintf(msg_accX, MSG_BUFFER_SIZE," %7.2f",accX);
        snprintf(msg_accY, MSG_BUFFER_SIZE," %7.2f",accY);
        snprintf(msg_accZ, MSG_BUFFER_SIZE," %7.2f",accZ);
        snprintf(msg_gyroX, MSG_BUFFER_SIZE," %7.2f",gyroX);
        snprintf(msg_gyroY, MSG_BUFFER_SIZE," %7.2f",gyroY);
        snprintf(msg_gyroZ, MSG_BUFFER_SIZE," %7.2f",gyroZ);

//M5.Lcd.print("Publish message: ");
//M5.Lcd.println(msg_voltage);
client.publish(outtopic_voltage, msg_voltage);  // Publishes a message to the specified  topic.

//M5.Lcd.print("Publish message: ");
//M5.Lcd.println(msg_accX);
client.publish(outtopic_accX, msg_accX);
client.publish(outtopic_accX, ",");

//M5.Lcd.print("Publish message: ");
//M5.Lcd.println(msg_accY);
client.publish(outtopic_accY, msg_accY);
client.publish(outtopic_accY, ",");

//M5.Lcd.print("Publish message: ");
//M5.Lcd.println(msg_accZ);
client.publish(outtopic_accZ, msg_accZ);
client.publish(outtopic_accZ, ",");

//M5.Lcd.print("Publish message: ");
//M5.Lcd.println(msg_gyroX);
client.publish(outtopic_gyroX, msg_gyroX);
client.publish(outtopic_gyroX, ",");

//M5.Lcd.print("Publish message: ");
//M5.Lcd.println(msg_gyroY);
client.publish(outtopic_gyroY, msg_gyroY);
client.publish(outtopic_gyroY, ",");

//M5.Lcd.print("Publish message: ");
//M5.Lcd.println(msg_gyroZ);
client.publish(outtopic_gyroZ, msg_gyroZ);
client.publish(outtopic_gyroZ, ",");

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


        //  if (value %7 == 0) {
        //    M5.Lcd.fillScreen(BLACK);
        //     M5.Lcd.setCursor(0, 0);
        // }
    //  }


        if (u < 1023)
        {
            u = 1023;
        }


void setupWifi() { 
    delay(10);
    M5.Lcd.setCursor(95, 95);
    M5.Lcd.println("%s", ssid);
    //M5.Lcd.print("Wifi-Status : %s", ssid);
    // M5.Lcd.setCursor(10, 70);
    //M5.Lcd.println("Connected");
    //M5.Lcd.printf("Connecting to %s", ssid);
    WiFi.mode(
        WIFI_STA);  // Set the mode to WiFi station mode.
    WiFi.begin(ssid, password);  // Start Wifi connection. 

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.print(".");
    }
    M5.Lcd.setCursor(95, 95);
    M5.Lcd.printf("Success");
}
void callback(char* intopic, byte* payload, unsigned int length) {
    //M5.Lcd.print("Message arrived [");
    //M5.Lcd.print(intopic);
    //M5.Lcd.print("] ");
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
    Serial.print("Angle: ");
    Serial.println(angle_1);
    Serial.println(angle_2);

    //int x = atoi(array);



    float kp = 7;
    //float ki = 0.03;
    //float kd = 0.5;
    int curr_angle = analogRead(36);
    
    // for(int i = 0; i < length; i++) {
    //     // int test;
    //     // test = atoi(array);
    //     // Serial.printf("%ld", test);
    //     Serial.print((int(array[i])+0)-48);
    // }
    //     Serial.println();
    float degree = (curr_angle*360)/4096;
    error = angle_1 - degree;
    curr_time = millis();
    //float ed = (error - prev_error)/(curr_time - prev_time)*0.001;
    //float eint = eint + error*(curr_time - prev_time)*0.001;
    float u = kp*error; //+ ki*eint+ kd*ed;
    if (u > 1023){
         u = 1023;
      }

    if (u < 1023){
         u = 1023;
      }
     
    if (error > 0) {
      pwm = abs (u);
      ledcWrite (ledChannel,pwm);
    }

  prev_time = curr_time;
  prev_error = error;
    Serial.println("Motor Testing");
    delay(1000);
}

void reConnect() {
    while (!client.connected()) {
        // Create a random client ID. 
        M5.Lcd.setCursor(95, 110);
        M5.Lcd.println("Attempting")
        // M5.Lcd.setCursor(10, 70);
        //M5.Lcd.println("Connected");
        String clientId = "MM5St-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect.
        if (client.connect(clientId.c_str())) {
            M5.Lcd.setCursor(95, 110);
            M5.Lcd.printf("Success");
            // Once connected, publish an announcement to the topic.
            //client.publish("M5Stack", "hello world");
            // ... and resubscribe. 
            client.subscribe(intopic);

            if (String("TEST/TESA") == "esp32/output") {
                Serial.print("Changing output to ");}
        } else {
            M5.Lcd.setCursor(95, 110);
            M5.Lcd.print("Failed");
            M5.Lcd.print(client.state());
            //M5.Lcd.println("try again in 5 seconds");
            delay(5000);

        }
    }
