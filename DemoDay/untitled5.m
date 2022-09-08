clear proxyIn
%receive data from mqttfrom this topic(mqtt) to this topic (ros2)
%{"dX": 1, "dY": 2, "dZ":5}
proxyIn = mqtt2ros("tgr2022/bmeranger12/in","tgr2022/bmeranger12/in");
cliNode = ros2node("Clinode"); %create node
ros2 node list %check node
rosSub = ros2subscriber(cliNode,"tgr2022/bmeranger12/in"); %ros subscribe mqtt,"geometry_msgs/Twist"
receive(rosSub);% receive data


%%
cliNode = ros2node("Clinode"); %create node
proxyOut = ros2mqtt("tgr2022/bmeranger12/out","tgr2022/bmeranger12/out");
rosPub = ros2publisher(cliNode, "tgr2022/bmeranger12/out");
rosmsg = ros2message(rosPub);
rosmsg.linear.x = 1;
rosmsg.linear.y = 1;
rosmsg.linear.z = 1;
rosmsg.data = rosmsg.linear;
%%rosmsg.data  = num2str(rosmsg.linear.x) + num2str(rosmsg.linear.y) + num2str(rosmsg.linear.z) + "...";
send(rosPub,rosmsg)

