csi_trace = read_bf_file('log.dat');
csi_entry = csi_trace{1}
csi = get_scaled_csi(csi_entry)
csi_var = db(abs(squeeze(csi).'))