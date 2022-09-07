classdef mqtt2ros
    %UNTITLED3 Summary of this class goes here
    %   Detailed explanation goes here

    properties
        mqttClient
        
        %node name
        ros2node
        ros2Pub
        rosTopic
    end

    methods
        function obj = mqtt2ros(mqttTopic,rosTopic)
            obj.mqttClient = mqttClient('tcp://broker.hivemq.com');
            read(obj.mqttClient, mqttTopic, 'Callback', @obj.mqttMsgHandler)
            obj.ros2node = ros2node('myNode');
            obj.ros2Pub = ros2publisher(obj.ros2node,rosTopic,'std_msgs/String');
            obj.rosTopic = rosTopic;
        end

        function mqttMsgHandler(obj,topic,msg)
            if topic == obj.rosTopic
                rosmsg = ros2messasge(obj.ros2Pub);
                rosmsg.data =msg;
                send(obj.ros2Pub,msg)

            else
                disp('wrong ros topic')
            end
        end
    end
end