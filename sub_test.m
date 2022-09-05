<<<<<<< HEAD
topic = "TEST/TESA";
=======
topic = "TOPIC_TEST1";
>>>>>>> main
host = "tcp://broker.mqttdashboard.com";
port = 1883;
mqttClient = mqttclient(host,Port=port);
mySub = subscribe(mqttClient,topic);
while true
    msg = read(mqttClient)
end
