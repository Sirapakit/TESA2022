#include <Arduino.h>
#include <M5StickCPlus.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

// Configure the name and password of the connected wifi and your MQTT Serve
// host. 
const char* ssid        = "catsvn";
const char* password    = "catsvn2000";
const char* mqtt_server = "10.13.8.163";
int pot_value;
char pot_value_1 =  pot_value;
//const char* pot_value_msg = pot_value_1;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setupWifi();
void callback(char* topic, byte* payload, unsigned int length);
void reConnect();
void setup() {
  Serial.begin(115200);
    M5.begin();
    M5.Lcd.setRotation(3);
    setupWifi();
    client.setServer(mqtt_server,
                     1883);  // Sets the server details.  
    client.setCallback(
        callback);  // Sets the message callback function.  
}

void loop() {
    int pot_value = analogRead(36);
    Serial.print("Pot =");
    Serial.println(pot_value);
    M5.Lcd.print("Pot =");
M5.Lcd.println(pot_value);
    if (!client.connected()) {
        reConnect();
    }
    client.loop();  // This function is called periodically to allow clients to
                    // process incoming messages and maintain connections to the
                    // server.
    

    unsigned long now =
        millis();  // Obtain the host startup duration. 
    if (now - lastMsg > 2000) {
        lastMsg = now;
        ++value;
        snprintf(msg, MSG_BUFFER_SIZE,"pung: %ld",pot_value);
M5.Lcd.print("Publish message: ");
M5.Lcd.println(msg);
        client.publish("M5Stack", msg);  // Publishes a message to the specified
                                         // topic.
        if (value % 7 == 0) {
            M5.Lcd.fillScreen(BLACK);
            M5.Lcd.setCursor(0, 0);
        }
    }
}

void setupWifi() {
    delay(10);
    M5.Lcd.printf("Connecting to %s", ssid);
    WiFi.mode(
        WIFI_STA);  // Set the mode to WiFi station mode.
    WiFi.begin(ssid, password);  // Start Wifi connection. 

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.print(".");
    }
    M5.Lcd.printf("\nSuccess\n");
}

void callback(char* topic, byte* payload, unsigned int length) {
    M5.Lcd.print("Message arrived [");
    M5.Lcd.print(topic);
    M5.Lcd.print("] ");
    for (int i = 0; i < length; i++) {
        M5.Lcd.print((char)payload[i]);
    }
    M5.Lcd.println();
}

void reConnect() {
    while (!client.connected()) {
        M5.Lcd.print("Attempting MQTT connection...");
        // Create a random client ID. 
        String clientId = "M5Stack-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect.
        if (client.connect(clientId.c_str())) {
            M5.Lcd.printf("\nSuccess\n");
            // Once connected, publish an announcement to the topic.
            client.publish("M5Stack", "hello world");
            // ... and resubscribe. 
            client.subscribe("M5Stack");
        } else {
            M5.Lcd.print("failed, rc=");
            M5.Lcd.print(client.state());
            M5.Lcd.println("try again in 5 seconds");
            delay(5000);
        }
    }
}