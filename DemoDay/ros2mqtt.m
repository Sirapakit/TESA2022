classdef ros2mqtt
    properties
        mqttClient
        mqttTopic;
        ros2Node
        ros2Pub
        rosTopic
        str_x
        str_y
        str_z
    end

    methods
        function obj = ros2mqtt(rosTopic,mqttTopic)
            obj.mqttClient = mqttclient('tcp://broker.hivemq.com');
            obj.mqttTopic = mqttTopic;
            obj.ros2Node = ros2node('ros2mqttNode');
            obj.ros2Pub = ros2subscriber(obj.ros2Node, rosTopic, "std_msgs/String", @obj.rosMsgHandler);
            obj.rosTopic = rosTopic;
        end

        function rosMsgHandler(obj, rosmsg)
     
           
            write(obj.mqttClient, obj.mqttTopic, rosmsg.data)
            
        end
    end
end