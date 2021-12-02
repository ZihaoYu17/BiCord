This repo provides the code for the WiFi sender and receiver in BiCord.
The repo is modified based on the supplementary utilities, firmware, etc., needed for the Linux 802.11n CSI Tool.  See the website(http://dhalperi.github.com/linux-80211n-csitool/) for more info.

** As for ZigBee devices in BiCord
Unfortunately we cannot find the original code of ZigBee devices in BiCord. So here we present the main framework and notes to implement BiCord on ZigBee. As the ZigBee receiver just passively receives the packets from the sender, here we mainly consider the behavior of the ZigBee sender.
Usually, the ZigBee sender transmit data packets with ACK. When two data packets are lost (No ACK), the sender then transmits control packets to conduct cross-technology signaling to WiFi. When the sender receive ACK of one control packet, it begins to send data packets.

Notes:
1. To disable the CCA when the sender transmits control packets
2. To use the primitives provided by the physical layer to set the power of control packets
3. To get the transmission result of each packet (with ACK/ no ACK) and then decide when to switch between the transmission of data or control packets
