import paho.mqtt.client as mqtt
host = "broker.mqttdashboard.com"
port = 1883
a = 0
while True:
    a += 1
    client = mqtt.Client()
    client.connect(host)
    client.publish("TOPIC_TEST1", a)