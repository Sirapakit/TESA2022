%initiation%
px =0;
py= 0;
pz =0;
tm = [0,0,0];

% connect to mqtt %
Address = "tcp://broker.hivemq.com";
%Address = "tcp://10.13.8.163";
Topic1 = "TGR/Pp/data";
mqClient = mqttclient(Address, Port=1883);
subscribe(mqClient,Topic1);

while(1)

    data = read(mqClient);
    
    pause(1)

    if isempty(data)
         warning = "no data";
        disp(warning)
        continue

    else
        disp("data received");
        r = jsondecode (data.Data) ;
        break
    end
end

% set position matrix
tm = [r.px,r.py,r.pz];
%set environement%
load('KINOVAGen3GripperColl.mat'); 
home = homeConfiguration(robot);
EE ='EndEffector_Link';
iviz = interactiveRigidBodyTree(robot);
parts = {[0.4 -0.1 0.26], [0.5 -0.15 0.26], [0.52 0 0.26]};
colors = ["r", "g", "b"];
env = tgrCreateEnv(parts, colors);
robotCell = tgrRobotEnvInteraction(robot, home, "gripper", env);


%inverse kinematics for moving to mqtt position %TODO: add numeric field
%edit to  tm if present
params.MaxIterations = 100;
ik = inverseKinematics('RigidBodyTree', robotCell.Robot, 'SolverParameters', params);
%ik = inverseKinematics('RigidBodyTree', robotCell.Robot, 'SolverAlgorithm','LevenbergMarquardt', 'SolverParameters', params);
weights = ones(1, robotCell.NumJoints-1);
total_t = 20;
sample_t = 0.5;
t_steps = zeros(sample_t,total_t);
angleM = [0 1 0 pi];
for i = 1:total_t
    tic;
    init_pose = robotCell.CurrentJointConfig;
    new_task_states = trvec2tform(tm)*axang2tform(angleM);
    [joint_sols, sols_info] = ik(robotCell.RobotEndEffector, new_task_states, weights, init_pose);
    robotCell.moveJoints(joint_sols);
    t_steps(i) = toc;
end



%move to next position %ok
for i = length(parts)
init_pose = robotCell.CurrentJointConfig;
next_part = i;
grasp_pose = trvec2tform(robotCell.Environment.Parts{next_part}.centerPoint + [0 0 0.06])*axang2tform(angleM);
[joint_sols, sols_info] = ik(robotCell.RobotEndEffector, grasp_pose, weights, init_pose);
robotCell.moveJoints(joint_sols);
end

%grab

robotCell.PartOnRobot = next_part;
robotCell.moveJoints(robotCell.CurrentJointConfig);


%move to obj position
releaseM = [0.23 0.62 0.33]; %TODO: set this pose
init_pose = robotCell.CurrentJointConfig;
release_pose = trvec2tform(releaseM)*axang2tform(angleM);
%release_pose = trvec2tform([0.23 -0.62 0.33])*axang2tform([0 1 0 pi]);
[joint_sols, sols_info] = ik(robotCell.RobotEndEffector, release_pose, weights, init_pose);
robotCell.moveJoints(joint_sols);

%release
if robotCell.PartOnRobot ~= 0
    robotCell.Environment.Parts{robotCell.PartOnRobot}.mesh.Pose = robotCell.CurrentTaskConfig * trvec2tform([0 0 0.06]);
    robotCell.Environment.Parts{robotCell.PartOnRobot}.plotHandle.Matrix = robotCell.Environment.Parts{robotCell.PartOnRobot}.mesh.Pose;
    robotCell.PartOnRobot = 0;
    robotCell.moveJoints(robotCell.CurrentJointConfig);
end












