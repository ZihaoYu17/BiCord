function [csi_var_all,packet_num_move] = get_var_all(file_name,packet_num_all,rad_num,carrier_num,...
    time_threshold)
% 


%file_name = '../yzh/4_23/air_0423_1.dat';
%file_name = '../yzh/4_23/send_0423_1.dat';

%packet_num_all = 20000;
carrier = [19,20,21];
%rad_num = 1;

[csi_timestamp,csi_rad] = prepocess(file_name, packet_num_all, carrier, rad_num);

%time_threshold = 10000; %5ms  

csi_var_all = zeros(1,packet_num_all-50);
packet_num_move = zeros(1,packet_num_all-50);
for ii = 1:packet_num_all-50
    index_start = ii;
    index_end = ii+50;
    csi_trace = csi_rad(carrier_num,index_start:index_end);
    timestamp = csi_timestamp(1,index_start:index_end);
    [packet_num, csi_var] = get_var(csi_trace,timestamp,time_threshold);
    csi_var_all(1,ii) = csi_var;
    if packet_num<=3
        csi_var_all(1,ii) = 0;
    end
    packet_num_move(1,ii) = packet_num;
end


end

