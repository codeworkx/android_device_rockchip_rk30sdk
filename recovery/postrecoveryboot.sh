#!/sbin/sh
while [ ! -e /dev/block/mtdblock0 ]; do
        sleep 1
done
dd if=/dev/zero of=/dev/block/mtdblock0 bs=1 seek=6144 count=75
