function HomeSetup()
    params.MaxIterations = 100;
    ik = inverseKinematics('RigidBodyTree', robotCell.Robot, 'SolverParameters', params);
    %ik = inverseKinematics('RigidBodyTree', robotCell.Robot, 'SolverAlgorithm','LevenbergMarquardt', 'SolverParameters', params);
    weights = ones(1, robotCell.NumJoints-1);
    t_steps = zeros(1,10);
    for i = 1:10
    tic;
    init_pose = robotCell.CurrentJointConfig;
    new_task_states = trvec2tform([0.4+i*0.01, 0, 0.5])*axang2tform([0 1 0 pi]);
    [joint_sols, sols_info] = ik(robotCell.RobotEndEffector, new_task_states, weights, init_pose);
    robotCell.moveJoints(joint_sols);
    t_steps(i) = toc;
    end
    mean(t_steps)

end