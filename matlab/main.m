%clear
%clc


file_name = 'log.dat';
%file_name = '../yzh/4_23/send_0423_3.dat';

%%%%%%%%%%%%%%%%%%%%%% the following two lines should be commented if you
%%%%%%%%%%%%%%%%%%%%%% want to operate parameter_adaption.m
 time_threshold = 8000;
 var_threshold = 0.6;

%get_var_all(file,packet_num_all,rad_num,carrier_num,time_threshold)
[csi_var,packet_num] = get_var_all(file_name,777,2,2,time_threshold);

% x=linspace(0,3,20);
% csi_var = csi_var(csi_var<3)
% hist(csi_var,x)

%var_threshold = 0.3;
alarm_rate = sum(csi_var > var_threshold)/length(csi_var);

%plot(csi_timestamp,csi_rad(1,:));
%plot(packet_num_move);
%plot(csi_var_all);