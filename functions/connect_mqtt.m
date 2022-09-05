function connect_mqtt()
    global Address mqClient data 

    % connect to mqtt %
    %Address = "tcp://broker.hivemq.com";  % tcp://10.13.8.163
    Address = "tcp://10.13.8.163";
    Topic = "TEST/TESA";
    mqClient = mqttclient(Address);
    subscribe(mqClient,Topic);
    
    disp("MQTT connected");
    data = read(mqClient)
    pause(.5)

end