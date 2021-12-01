function [csi_timestamp,csi_rad] = prepocess(file_name, packet_num_all, carrier, rad_num)

csi_trace = read_bf_file(file_name);

csi_rad = zeros(3,packet_num_all);
csi_timestamp = zeros(1,packet_num_all);

for i=1:packet_num_all
    csi_temp = get_scaled_csi(csi_trace{i});
    csi_temp2 = db(abs(squeeze(csi_temp)));
    csi_rad(:,i) = csi_temp2(rad_num,carrier)';
    csi_timestamp(1,i) = csi_trace{i}.timestamp_low;
end

end