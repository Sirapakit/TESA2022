
mqClient = mqttclient("tcp://broker.hivemq.com");
subscribe(mqClient,"TGR/poten");
i =1;
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
        i = i+1;
        
 
    end
    ang1 = double(r.pt1);
    ang2 = double(r.pt2);
    if ang1>0 && ang2 >0 && ang1<90 && ang2<90
    L =6;
    
    x = 0
    y = 0
    xi2=x+(L*cosd(ang1));
    yi2=y+(L*sind(ang1));

    xt2=x+(L*sind(ang2));
    yt2=y+(L*cosd(ang2));
    line([x, xi2], [y,yi2], 'Color', 'r', 'LineWidth', 2);
    hold on
    line([x xt2], [y,yt2], 'Color', 'b', 'LineWidth', 2);
    else
    disp("invalid input")
    end

end
