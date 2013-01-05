#!/system/bin/sh

new_mode=$1
current_mode=`cat /sys/bus/platform/drivers/usb20_otg/force_usb_mode`

if [ "$current_mode" != "$new_mode" ]; then
	echo "Switching USB mode: $current_mode -> $new_mode"
	echo $new_mode > /sys/bus/platform/drivers/usb20_otg/force_usb_mode
fi
