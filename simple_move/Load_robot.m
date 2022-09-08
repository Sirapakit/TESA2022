% Load model of kinova Gen 3 robot model
function Load_robot
    global tgrCreateEnv
    load('KINOVAGen3GripperColl.mat'); 
    joint_states = homeConfiguration(robot);
    parts = {[0.4 -0.1 0.26], [0.5 -0.15 0.26], [0.52 0 0.26]};
    colors = ["r", "g", "b"];
    env = tgrCreateEnv(parts, colors);
    robotCell = tgrRobotEnvInteraction(robot, joint_states, "gripper", env);
    robotCell.visualizeStation();
    robotCell.visualizeParts();
end