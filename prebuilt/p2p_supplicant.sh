#!/system/bin/sh

/system/bin/usb_mode.sh 1
sleep 1
/system/bin/wpa_supplicant -Dwext -iwlan0 -c/data/misc/wifi/wpa_supplicant.conf -N -Dwext -ip2p0 -c/data/misc/wifi/p2p_supplicant.conf -e/data/misc/wifi/entropy.bin -puse_p2p_group_interface=1
