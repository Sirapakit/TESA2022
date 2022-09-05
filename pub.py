import paho.mqtt.client as mqtt
host = "broker.hivemq.com"
port = 1883
a = 0
b = 0.5
c = 0
d = 0.5
while True:
    a += 0.1
    client = mqtt.Client()
    client.connect(host)
    client.publish("TGR/data", str({"rx":a, "ry":b, "rz":c, "p":d}))