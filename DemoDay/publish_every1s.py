from random import randrange
import paho.mqtt.client as mqtt
import random
import json
import time

host = "broker.hivemq.com"
topic = "tgr2022/bmeranger12/getting_position"
port = 1883

while True:
    x = random.uniform(0, 0.05)
    y = random.uniform(0, 0.05)
    z = random.uniform(0, 0.05)

    msg = json.dumps({
        "x": round(x,3),
        "y": round(y,3),
        "z": round(z,3)
        })

    client = mqtt.Client()
    client.connect(host)
    client.publish(topic, msg)
    time.sleep(1)  #Send json every 1 second
    