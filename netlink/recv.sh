sleep 2s;
sh ../injection/setup_monitor_csi.sh 13 HT20;
sleep 3s;
sudo ./wifi_recv ../data/log2.dat;
