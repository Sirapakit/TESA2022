function EnvSetup()
       global part1lo ;
       global part2lo;
       global part3lo;
       global robotCell;
       part1lo = [0.4 -0.1 0.26] ;
       part2lo = [0.5 -0.15 0.26] ;
       part3lo =  [0.52 0 0.26];

        load('KINOVAGen3GripperColl.mat'); 
        joint_states = homeConfiguration(robot);
        parts = {part1lo,part2lo ,part3lo};
        colors = ["r", "g", "b"];
        env = tgrCreateEnv(parts, colors);
        robotCell = tgrRobotEnvInteraction(robot, joint_states, "gripper", env);
        robotCell.visualizeStation();
        robotCell.visualizeParts();
end