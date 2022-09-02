
% connect to mqtt %
Address = "tcp://broker.hivemq.com";
Topic = "TGR/data";
mqClient = mqttclient(Address, Port=1883);
subscribe(mqClient,Topic);
% end %

%initiation
rx = 0;
ry = 0;
rz = 0;
px = 0;
py = 0;
pz = 0;

Rx = [1 0 0 0];
Ry = [0 1 0 0];
Rz = [0 0 1 0];
P = [0 0 1];

while(1)

    data = read(mqClient)
    
    pause(1)

    if isempty(data)
         warning = "no data";
        disp(warning)
        continue

    else
        r = jsondecode (data.Data) 
    end

     % extract data from struct
        rx = double(r.rx);
        ry = double(r.ry);
        rz = double(r.rz);
        px = double(r.px);
        py = double(r.py);
        pz = double(r.pz);
end
%         pause(5)
%   
%     % Set up param %    
%     Rx = [1 0 0 rx];
%     Ry = [0 1 0 ry];
%     Rz = [0 0 1 rz];
%     P = [px py pz];
% end


