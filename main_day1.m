i = 0;
while (i < 10)
    address = "tcp://broker.hivemq.com";
    topic = "tgr2022/bmeranger12/testing";
    mqClient = mqttclient(address, Port=1883);
    subscribe(mqClient,topic);
    data = read(mqClient);
    dis = jsondecode(data.Data)
    dx = dis.x;
    dy = dis.y;
    dz = dis.z;

    % Load model of kinova Gen 3 robot model
    load('KINOVAGen3GripperColl.mat'); 
    joint_states = homeConfiguration(robot); 
    parts = {[0.4 -0.1 0.26], [0.5 -0.15 0.26], [0.52 0 0.26]};
    colors = ["r", "g", "b"];
    env = tgrCreateEnv(parts, colors);
    robotCell = tgrRobotEnvInteraction(robot, joint_states, "gripper", env);
    robotCell.visualizeStation();
    robotCell.visualizeParts();
    
    % IK
    robotCell.HomeRobotTaskConfig = trvec2tform([0.4, 0, 0.5])*axang2tform([0 1 0 pi]);
    ik = inverseKinematics('RigidBodyTree', robotCell.Robot);
    weights = ones(1, 6);
    init_pose = robotCell.CurrentJointConfig;
    [joint_sols, sols_info] = ik(robotCell.RobotEndEffector, robotCell.HomeRobotTaskConfig, weights, init_pose);
    robotCell.moveJoints(joint_sols);
    
    % move to the desired position
    init_pose = robotCell.CurrentJointConfig;
    next_part = 2;
    grasp_pose = trvec2tform(robotCell.Environment.Parts{next_part}.centerPoint + [dx dy dz])*axang2tform([0 1 0 pi]);
    [joint_sols, sols_info] = ik(robotCell.RobotEndEffector, grasp_pose, weights, init_pose);
    robotCell.moveJoints(joint_sols);
    
    % catch the obj
    robotCell.PartOnRobot = next_part;
    robotCell.moveJoints(robotCell.CurrentJointConfig);
    
    % new position
    init_pose = robotCell.CurrentJointConfig;
    release_pose = trvec2tform([0.23 0.62 0.33])*axang2tform([0 1 0 pi]);
    [joint_sols, sols_info] = ik(robotCell.RobotEndEffector, release_pose, weights, init_pose);
    robotCell.moveJoints(joint_sols);
    
    % letting the obj
    if robotCell.PartOnRobot ~= 0
        robotCell.Environment.Parts{robotCell.PartOnRobot}.mesh.Pose = robotCell.CurrentTaskConfig * trvec2tform([dx dy dz]);
        robotCell.Environment.Parts{robotCell.PartOnRobot}.plotHandle.Matrix = robotCell.Environment.Parts{robotCell.PartOnRobot}.mesh.Pose;
        robotCell.PartOnRobot = 0;
        robotCell.moveJoints(robotCell.CurrentJointConfig);
    end
    
    % return home
    init_pose = robotCell.CurrentJointConfig;
    [joint_sols, sols_info] = ik(robotCell.RobotEndEffector, robotCell.HomeRobotTaskConfig, weights, init_pose);
    robotCell.moveJoints(joint_sols);

    i = i+1;
    close all
    clear
end