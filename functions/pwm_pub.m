
   Address = "tcp://10.13.8.163";  % tcp://10.13.8.163
   Topic = "robot/pwm";
   userName = "tesa";
   password = "1234";
   port = 1883;
   mqClient = mqttclient(Address, Port = port, Username = "tesa", Password = "1234");
   msg = "10 900"; %   output value
  write(mqClient, Topic, msg)

