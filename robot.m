
% connect to mqtt %
Address = "tcp://broker.hivemq.com";
Topic = "TGR/data";
mqClient = mqttclient(Address)
subscribe(mqClient,Topic)
% end %

%get data%
%form = {"rx":0, "ry":0, "rz":0, "px":0, "py":0, "pz":0}
while(true)

    data = read(mqClient)
    pause(5)
    r = jsondecode (data.Data);
        rx = double(r.rx);
        ry = double(r.ry);
        rz = double(r.rz);
        px = double(r.px);
        px = double(r.py);
        pz = double(r.pz);
        
    pause(5)
end
% end %
%load robot%
gen3 = loadrobot("kinovaGen3", "DataFormat", "column", "Gravity", [0 0 -9.81]); %collect data in row and add gravity -9.81m/s2
Home =   homeConfiguration(gen3); % first pose
endEffector = "EE"; % set end effector
   
% Set up param %    
Rx = [1 0 0 rx]
Ry = [1 0 0 ry]
Rz = [1 0 0 rz]
P = [px py pz];

no_Joint = numel(Home); %number of joints
timeStep = 0.1; % calculation time
speed = 0.1;

% SYNTAX transform = getTransform(robot,configuration,bodyname)
T0 = getTransform(gen3,Home,EE);
Tf = trvec2tform([0.4,0,0.6])*axang2tform([0 1 0 pi]);
% use task space trajectory        
           
        

%interp


        
       
     
        
        %if isempty (rx)== false || isempty (ry)== false||isempty (rz) == false ||isempty (p) == false
            Rx = [1 0 0 rx]; 
            Ry = [0 1 0 ry];
            Rz = [0 0 1 rz];
            vt = [1 1 p];
        %else
            %print("no orientation/ position");
        %end

    %end
     
        tf = axang2tform(Rx)*trvec2tform(vt);
       %tf = axang2tform(Rx)*axang2tform(Ry)*axang2tform(Rz)*trvec2tform(vt);
        Pose = trvec2tform(vt);      
        Vel = zeros(6,1);
      
       motionModel = taskSpaceMotionModel("RigidBodyTree", gen3, "EndEffectorName", "EndEffector_Link");
       
       [t,robotState] = ode15s(@(t,state)derivative(motionModel, state, Pose, Vel), tspan, Start);
       NewConfig = [homeConfiguration(gen3);zeros(7,1)];
%        pPose =Pose;
%        [t,robotState] = ode15s(@(t,state)derivative(motionModel, state, pPose, Vel), tspan, NewConfig);
       
       %[q, qd, qdd] = cubictraj[Pose,0.5,tspan]

       [tfInterp, v1, a1] = transformtraj(t0,tf,tInterval,tvec);

        rotations = tform2quat(tfInterp);
        translations = tform2trvec(tfInterp);
        plotTransforms(gen3, translations,rotations)
 
%    figure
%        show(gen3, Start(1:7));
%        hold on
%        plot3(Pose(1,4), Pose(2,4), Pose(3,4) );
%          rate = rateControl(5);    %
%        
%        for i = 1:size(robotState,1)
%             show(gen3, robotState(i, 1:7)', "PreservePlot", false);
%             waitfor(rate);
%        end
%      
   
end
       


