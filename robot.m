

mqClient = mqttclient("tcp://broker.hivemq.com")
subscribe(mqClient,"TGR/data")

 gen3 = loadrobot("kinovaGen3", "DataFormat", "column", "Gravity", [0 0 -9.81]);
 
       % Set up 
       tspan = 0:0.02:1; %1 sec w/ 0.02 sec period
       begin = zeros(7,1);
       Home =   homeConfiguration(gen3);
       Start = [Home; begin];
        Rx = zeros(0,4);
        Ry = zeros(0,4);
        Rz = zeros(0,4);
           
        

%interp

while(true)

    data = read(mqClient)
    vt = [1 1 0];
     r = jsondecode (data.Data);
        rx = double(r.rx);
        ry = double(r.ry);
        rz = double(r.rz);
        p = double(r.p);
        
         pause(5)
  
     
        t0 = [0 0 0 0 0 0];
   
    
    %if isempty(data)==false
        % r = raw, rx= rotationX, ry = rotationY rz =rotationZ
        %form = {"rx":0, "ry":0, "rz":0, "p":0}
       
       
     
        
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
       


