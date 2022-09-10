classdef mqtt2ros
    properties
        mqttClient
        ros2Node
        ros2Pub
        rosTopic
    end

    methods
        function obj = mqtt2ros(mqttTopic,rosTopic)
            obj.ros2Node = ros2node('mqtt2rosNode');
            obj.ros2Pub = ros2publisher(obj.ros2Node, rosTopic, "std_msgs/String");
            obj.rosTopic = rosTopic;            
            obj.mqttClient = mqttclient('tcp://10.13.8.163', Port = 8883,Username="tesa",Password = "1234");
            subscribe(obj.mqttClient, mqttTopic, 'Callback', @(topic,msg) obj.mqttMsgHandler(topic,msg))            
        end

        function mqttMsgHandler(obj, topic, msg)
            if strcmp(topic, obj.rosTopic)
                rosmsg = ros2message(obj.ros2Pub);
                rosmsg.data = convertStringsToChars(msg);
                send(obj.ros2Pub, rosmsg)
            else
                disp('Wrong ROS topic')
            end
        end
    end
end