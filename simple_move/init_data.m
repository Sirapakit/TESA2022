function init_data()
    global data r
    while(1)
        if isempty(data)
            warning = "no data";
            disp(warning)
            continue
            
         else
             disp("data received");
             r = jsondecode (data.Data);
             break
        end
     end
end