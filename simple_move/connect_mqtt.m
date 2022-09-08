function connect_mqtt
global data
    Address = "tcp://172.20.18.163";
    Topic1 = "demo";
    username = 'tesa';
    password ='1234';
    port = 9883;
    mqClient = mqttclient(Address, Port=port, Username = username,Password=password);
    subscribe(mqClient,Topic1);
    disp("MQTT connected");
    data = read(mqClient);
    pause(.5)
end