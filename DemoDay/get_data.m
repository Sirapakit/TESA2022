function get_data()
    
    global data r 
    init_data()
    r = jsondecode(data.Data);
    

end