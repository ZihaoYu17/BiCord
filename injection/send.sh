#!/bin/sh
sleep 2s
sh ./setup_inject.sh 13 HT20
sleep 3s
echo 0x4101 | sudo tee `find /sys -name monitor_tx_rate`
sudo ./wifi_send 1000 100 1 1000
echo "finish!"
