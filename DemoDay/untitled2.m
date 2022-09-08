% Load model of kinova Gen 3 robot model
load('KINOVAGen3GripperColl.mat'); 
joint_states = homeConfiguration(robot);
parts = {[0.4 -0.1 0.26], [0.5 -0.15 0.26], [0.52 0 0.26]};
colors = ["r", "g", "b"];
env = tgrCreateEnv(parts, colors);
robotCell = tgrRobotEnvInteraction(robot, joint_states, "gripper", env);

%initiation%
px =0;
py= 0;
pz =0;
tm = [0,0,0];

% connect to mqtt %
%Address = "tcp://broker.hivemq.com";
Address = "tcp://10.13.8.163";
Topic1 = "TonyA";
username = 'tesa';
password ='1234';
port = 9883;
mqClient = mqttclient(Address,Username = username,Password=password, Port=port);
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
        disp(r)
        
    end


    % set position matrix
    tm = [r.px,r.py,r.pz];

    params.MaxIterations = 100;
    ik = inverseKinematics('RigidBodyTree', robotCell.Robot, 'SolverParameters', params);
    %ik = inverseKinematics('RigidBodyTree', robotCell.Robot, 'SolverAlgorithm','LevenbergMarquardt', 'SolverParameters', params);
    weights = ones(1, robotCell.NumJoints-1);
    t_steps = zeros(1,10);
    for i = 1:10
        tic;
        init_pose = robotCell.CurrentJointConfig;
        new_task_states = trvec2tform([r.px+i*0.01,r.py+i*0.01,r.pz+i*0.01])*axang2tform([0 1 0 pi]);
        [joint_sols, sols_info] = ik(robotCell.RobotEndEffector, new_task_states, weights, init_pose);
        robotCell.moveJoints(joint_sols);
        t_steps(i) = toc;
        pause(.1);
    end
    mean(t_steps)
end