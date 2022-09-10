% Load model of kinova Gen 3 robot model
load('KINOVAGen3GripperColl.mat'); 
joint_states = homeConfiguration(robot);
parts = {[0.4 -0.1 0.26], [0.5 -0.15 0.26], [0.52 0 0.26]};
colors = ["r", "g", "b"];
env = tgrCreateEnv(parts, colors);
robotCell = tgrRobotEnvInteraction(robot, joint_states, "gripper", env);
robotCell.visualizeStation();
robotCell.visualizeParts();
%initiation%
px =0;
py= 0;
pz =0;
tm = [0,0,0];

% Trajectory sample time
ts = 0:0.1:0.6;
trajTimes = 0:0.1;


waypointVels = 0.1 *[ 0  1  0;
                     -1  0  0;
                      0 -1  0;
                      1  0  0;
                      0  1  0
                       0  1  0;
                     -1  0  0]';


% connect to mqtt %
%Address = "tcp://broker.hivemq.com";
% Address = "tcp://10.13.8.163";
% Topic1 = "TonyA";
% username = 'tesa';
% password ='1234';
% port = 9883;
% mqClient = mqttclient(Address,Username = username,Password=password, Port=port);
% subscribe(mqClient,Topic1);
% 
% while(1)
% 
%     data = read(mqClient);
%     
%     pause(1)
% 
%     if isempty(data)
%          warning = "no data";
%         disp(warning)
%         continue
% 
%     else
%         disp("data received");
%         r = jsondecode (data.Data) ;
%         disp(r)
%         
%     end
% end

init_pose = robotCell.CurrentJointConfig;
ik = inverseKinematics('RigidBodyTree', robotCell.Robot);    
weights = ones(1,6);
new_task_states = trvec2tform([0.4, 0, 0.5])*axang2tform([0 1 0 pi]);
    [joint_sols, sols_info] = ik(robotCell.RobotEndEffector, new_task_states, weights, init_pose);
jointWaypoints= [init_pose;joint_sols];

[q,qd,qdd] = cubicpolytraj(jointWaypoints,ts,trajTimes, ... 
            'VelocityBoundaryCondition',zeros(7,2));

 robotCell.moveJoints(joint_sols);

% 
% for idx = 1:numel(trajTimes) 
%     % Solve IK
%     tgtPose = trvec2tform(q(:,idx)');
%     [joint_sols, sols_info] = ik(robotCell.RobotEndEffector, robotCell.HomeRobotTaskConfig, weights, init_pose);
% robotCell.moveJoints(joint_sols);
% 
% 
% end


 %%
 robot = loadrobot("kinovaGen3","DataFormat","column","Gravity",[0 0 -9.81]);
numJoints = numel(homeConfiguration(robot));

%% this one%%

tpts = 0:1;
sampleRate = 30;
tvec = tpts(1):1/sampleRate:tpts(end);
numSamples = length(tvec);
robot = loadrobot('kinovaGen3',DataFormat='column');
rng default
joint_sols = [0.7392;0.6753; -1.8275;1.1540;0.7497;2.0500;-0.7505];
wps = [robot.homeConfiguration joint_sols];
frankaTimepoints = linspace(tvec(1),tvec(end),3);
[q,qd] = trapveltraj(wps,numSamples);
figure
set(gcf,'Visible','on');
rc = rateControl(sampleRate);
for i = 1:numSamples
    show(robot,q(:,i),FastUpdate=true,PreservePlot=false);
    waitfor(rc);
end

%%
load('KINOVAGen3GripperColl.mat'); 
joint_states = homeConfiguration(robot);
parts = {[0.4 -0.1 0.26], [0.5 -0.15 0.26], [0.52 0 0.26]};
colors = ["r", "g", "b"];
env = tgrCreateEnv(parts, colors);
robotCell = tgrRobotEnvInteraction(robot, joint_states, "gripper", env);
robotCell.visualizeStation();
robotCell.visualizeParts();

%%
rng default
wps = [0,0,0;0.4, 0, 0.5];
robot = loadrobot("kinovaGen3","DataFormat","column","Gravity",[0 0 -9.81]);
frankaTimepoints = linspace(tvec(1),tvec(end),3);
[q,qd] = trapveltraj(wps,numSamples);
figure
set(gcf,'Visible','on');
rc = rateControl(sampleRate);
for i = 1:numSamples
    show(robot,q(:,i),FastUpdate=true,PreservePlot=false);
    waitfor(rc);
end