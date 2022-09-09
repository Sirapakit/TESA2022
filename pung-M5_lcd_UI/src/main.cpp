#include <PubSubClient.h>
#include <Arduino.h>
#include <M5StickCPlus.h>
#include <WiFi.h>
#include <analogWrite.h>
#include <Adafruit_PWMServoDriver.h>
#include <SimpleKalmanFilter.h>
#include <Tone32.h>

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
SimpleKalmanFilter simpleKalmanFilter(0.001, 0.03, 0.03);

#define BUZZER_PIN 2
#define BUZZER_CHANNEL 0

int notes[9] = {NOTE_C6, NOTE_D6, NOTE_E6, NOTE_F6, NOTE_G6, NOTE_A6, NOTE_B6, NOTE_C7, NOTE_D7};

float ONE_STEP_Z_AXIS = 0.1;

const char *intopic = "PWM";

const char *topic_json_co = "TonyA";
const char *topic_json_gyro = "TonyB";

char payload_co[200];
char payload_gyro[200];

float accX, accY, accZ;
float gyroX, gyroY, gyroZ;
float disX, disY, disZ;
int Voltage;

int MIDDLE_FINGER_POTEN = 36;
int THRES = 4095;


bool wifi_status = false;
bool sub_status = false;
int gripper_state = 0;

char array_from_payload[50];
int sub_angle_servo_1, sub_angle_servo_2;

int PWM_FREQ = 60;
int SERVO_1, SERVO_2;

#define GPIO_ANALOG_READ_MIDDLE_FINGER  2
#define GPIO_ANALOG_READ_RING_FINGER  3

int delay_lcd_task = 1000;
int delay_mqtt_reconnect = 5000;
int delay_mqtt_pub = 1000;
int delay_adc_read_task = 100;
int delay_reset_buttoon_task = 3000;
int delay_mqtt_json_pub = 3000;

static float coX = 0;
static float coY = 0;
static float coZ = 0;

void setupWifi();
void callback(char *intopic, byte *payload, unsigned int length);
void reConnect();

void mqtt_wifi_init()
{
    const char *mqtt_server = "10.13.8.163";
    // const char *mqtt_server = "169.254.132.183";
    // const char *mqtt_server = "10.13.8.180";
    // const char *mqtt_server = "broker.hivemq.com";
    // const char *mqtt_server = "tcp://0.tcp.ap.ngrok.io:17656";
    setupWifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void setupWifi()
{
    const char *ssid = "catsvn";
    const char *password = "catsvn2000";

    // IPAddress staticIP(192, 168, 1, 150);
    // IPAddress gateway(10, 13, 8, 69);
    // IPAddress subnet(255, 255, 255, 0);
    // IPAddress dns(192, 168, 1, 254);

    // const char *ssid = "TESA_TOP_GUN";

    // const char *ssid = "Wifi ของ korrawiz";
    // const char *password = "korrawiz";

    // const char *ssid = "BCILAB 2.4";
    // const char *password = "bcimemberonly";

    delay(50);
    M5.Lcd.setCursor(95, 95);
    M5.Lcd.print(ssid);
    WiFi.mode(WIFI_STA); // Set the mode to WiFi station mode.

    WiFi.begin(ssid, password); // Start Wifi connection.
    // WiFi.begin(ssid); // Start Wifi connection.

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
    coX = 0;
    coY = 0;
    static long now;
    M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);

    if (millis() - now >= 100)
    {
        M5.IMU.getAccelData(&accX, &accY, &accZ);
        float estimated_valueX = simpleKalmanFilter.updateEstimate(accX);
        float estimated_valueY = simpleKalmanFilter.updateEstimate(accY);

        disX = ((estimated_valueX / 2) * (((millis() - now) / 1000.0) * ((millis() - now) / 1000.0)));
        disY = ((estimated_valueY / 2) * (((millis() - now) / 1000.0) * ((millis() - now) / 1000.0)));

        now = millis();
        coX = coX + disX;
        coY = coY + disY;

    }
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
    static long now;
    if (millis() - now >= delay_lcd_task)
    {
        M5.Lcd.setCursor(65, 80);
        M5.Lcd.println(Voltage);

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

void mqtt_reconnect()
{

    const char *mqttUser = "tesa";
    const char *mqttPassword = "1234";
    while (!client.connected())
    {
        M5.Lcd.setCursor(95, 110);
        M5.Lcd.println("Attempting");

        // String clientId = "MM5St-";
        // clientId += String(random(0xffff), HEX);

        if (client.connect("BMErangerClient", mqttUser, mqttPassword))
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
            static long now;
            // if (millis() - now >= delay_mqtt_reconnect)
            // {
            M5.Lcd.setCursor(95, 110);
            M5.Lcd.print("Failed");
            delay(5000);
            // now = millis();
            // }
        }
    }
}

void pz_logic()
{
    static long now;
    if (M5.BtnA.pressedFor(1000))
    {
        coZ = coZ + ONE_STEP_Z_AXIS;
        // digitalWrite(2, HIGH);
        tone(BUZZER_PIN, notes[1], 500, HIGH); // NUM2 IS BUZZER
        delay(1000);
        Serial.println("Can Pressed again");
    }
    M5.update();
}

void json_task_send_coord()
{

    static long now;

    if (millis() - now >= delay_mqtt_json_pub)
    {
        // TonyA
        // sprintf(payload_co, "{\"px\":%7.2f,\"py\":%7.2f,\"pz\":%7.2f,\"gp\":%s,\"TIME_STAMP\":%d}", 4, 4, 4, gripper_logic, millis());
        sprintf(payload_co, "{\"px\":%7.2f,\"py\":%7.2f,\"pz\":%7.2f,\"gp\":%d,\"TIME_STAMP\":%d}", coX, coY, coZ, gripper_state, millis());
        // need pz logic and gp logic

        // TonyB
        // sprintf(payload_gyro, "{\"gyroX\":%7.2f,\"gyroY\":%7.2f,\"gyroZ\":%7.2f,\"accX\":%7.2f,\"accY\":%7.2f,\"accZ\":%7.2f,\"TIME_STAMP\":%d}", gyroX, gyroY, gyroZ, accX, accY, accZ, millis());
        sprintf(payload_gyro, "{\"gyroX\":%.2f,\"gyroY\":%.2f,\"gyroZ\":%.2f,\"accX\":%.2f,\"accY\":%.2f,\"accZ\":%.2f,\"TIME_STAMP\":%d}", gyroX, gyroY, gyroZ, accX, accY, accZ, millis());

        client.publish(topic_json_co, payload_co);
        client.publish(topic_json_gyro, payload_gyro);

        client.loop();
        now = millis();
    }
}

void gripper_logic()
{
    static long now;
    if (analogRead(MIDDLE_FINGER_POTEN) >= THRES)
    {
        gripper_state = 1;
    }
    else
    {
        gripper_state = 0;
    }
}

void callback(char *intopic, byte *payload, unsigned int length)
{
    for (int i = 0; i < length; i++)
    {
        array_from_payload[i] = ((char)payload[i]);
    }
    sub_status = true;
}

void mqtt_sub()
{
    if (sub_status == false)
    {
        array_from_payload[0] = {'\0'};
    }
    else
    {
        // Serial.println("Hello World Debuggin");
        Serial.print(array_from_payload);
        sscanf(array_from_payload, "%d%d", &sub_angle_servo_1, &sub_angle_servo_2);
        sub_status = false;
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
    SERVO_1 = sub_angle_servo_1;
    SERVO_2 = sub_angle_servo_2;
    // for (int i = 0; i <= SERVO_1; i = i + 20)
    // {
    //     for (int j = 0; j <= SERVO_2; j = j + 20)
    //     {
    //         pwm.setPWM(0, 0, i);
    //         pwm.setPWM(1, 0, j);
    pwm.setPWM(6, 0, SERVO_1);
    // pwm.setPWM(7, 0, SERVO_2);
    // }
    // }
    // delay(5000);
    // then reset to the initial position
    // or use the for loop to gradually get to the final and initial
}

void adc_read_task()
{
    pot_value_middle_finger = analogRead(GPIO_ANALOG_READ_MIDDLE_FINGER);
    pot_value_ring_finger = analogRead(GPIO_ANALOG_READ_RING_FINGER);
    static long now;
    if (millis() - now >= delay_adc_read_task)
    {
        if (pot_value_middle_finger > threshold_middle_finger && pot_value_ring_finger > threshold_ring_finger){
            gripper_logic = true;
        }
        else{
            gripper_logic = false;
        }
        now = millis();
    }
}

void reset_button_task()
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
    mqtt_wifi_init();
    imu_init();
    servo_motor_init();
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
        gripper_logic();
        break;
    }
    case 2:
    {
        lcd_task();
        break;
    }
    case 3:
    {
        servo_motor();
        break;
    }
    case 4:
    {
        imu_task();
        break;
    }
    case 5:
    {
        // adc_read_task();
        break;
    }
    case 6:
    {
        json_task_send_coord();
        break;
    }
    default:
    {
        state = 0;
        break;
    }
    }
    state++;
    // Serial.println("HelloWorld Debugging");
    reset_button_task();
    pz_logic();
}
// 1.Check Coord
// 2. Circuit
// 3. pz logic
// 4. gripper logic