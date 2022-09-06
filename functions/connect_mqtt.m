function connect_mqtt()
    global Address mqClient data 

    % connect to mqtt %
    Address = "tcp://broker.hivemq.com";  % tcp://10.13.8.163
    Topic1 = "TGR/Pp/data";
    port = 1883;
    mqClient = mqttclient(Address, Port = port);
    sub_ch1 = subscribe(mqClient,Topic1,Callback = @showMessage);
    disp("MQTT connected");
    data = read(mqClient)

end