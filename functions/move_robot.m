function move_robot()
    global data r gen3

    while(1)
        pause(1)
  
        % extract data from struct
        px = double(r.px);
        myMatrix = [0 0 0 0 0 px 0];
    
        show(gen3,myMatrix)
        pause(.5)
    end
end