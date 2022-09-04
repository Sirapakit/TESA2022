function call_robot()
global gen3 
    gen3 = loadrobot("kinovaGen3", "DataFormat", "row", "Gravity", [0 0 -9.81]); %collect data in row and add gravity -9.81m/s2
    home = homeConfiguration(gen3); % first pose
    %EEL = "EndEffector_Link"; % set end effector
    disp("robot is alrealdy created")
end