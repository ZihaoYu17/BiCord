function [packet_num, var_csi] =get_var(csi_trace, timestamp, time_threshold)
% GET_VAL calculates the var_csi of a group of packets with csi trace and timestamp

time_now =  timestamp(1,end);
valid_index = timestamp>(time_now-time_threshold);

%timestamp_valid = timestamp(valid_index);
csi_trace_valid = csi_trace(valid_index);
packet_num = length(csi_trace_valid);
var_csi = var(csi_trace_valid);


