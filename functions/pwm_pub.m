function pwm_pub()
   
    Address = "tcp://broker.hivemq.com";  % tcp://10.13.8.163
    Topic = "PWM/TEST";
    mqClient = mqttclient(Address);
    msg = "70"; %   output value
    write(mqClient, Topic, msg)

end