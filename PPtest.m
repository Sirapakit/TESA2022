
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

vector = [0 0 0];

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
    end

     % extract data from struct
        rx = double(r.rx);
        ry = double(r.ry);
        rz = double(r.rz);
        px = double(r.px);
        py = double(r.py);
        pz = double(r.pz);

        sx = accdeg(px);
        sy = accdeg(py);
        sz = accdeg(pz);
        vector = [sx;sy ;sz];

        n = 50;
        %plot%
        for i = 1:n
           
            plot3(vector(1,1), vector(2,1),vector(3,1),'+','MarkerSize',5); 
            box on 
            grid on
        end
end
%   

% %%
%     % Set up param %    
% Rx = [1 0 0 rx];
% Ry = [0 1 0 ry];
% Rz = [0 0 1 rz];
% P = [px py pz];
% 
% X = [1 ;0 ;0];
% Y = [0; 1; 0];
% Z = [0 ;0; 1];
% wp = [X; Y;Z]
% 
% sx=0;
% 
% sx = accdeg(px);
% sy = accdeg(py);
% sz = accdeg(pz);
% 
% distance =0;
% t0 = 0;
% v = 1;
% tf= distance/v;
% 
% vector = [sx;sy ;sz]
% Init = zeros(3,3);
% % translation0 = trvec2tform(Init);
% % translationF = trvec2tform(vector);
% % 
% % distance = norm(tform2trvec(Translation0)-tform2trvec(TranslationF))
% 
% v = 1;
% Interval = t0:tf
% 
% n = 50;
% %plot%
% for i = 1:n
% grid on
% plot3(vector(1,1), vector(2,1),vector(3,1),'+','MarkerSize',5,'MarkerFaceColor','r'); 
% %plot3(translationF(1,3), translationF(2,3), translationF(3,3));
% box on
% end
% 
% 
% 
% 
% 
