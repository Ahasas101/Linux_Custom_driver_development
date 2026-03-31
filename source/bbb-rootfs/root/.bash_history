/debootstrap/debootstrap --second-stage
echo "bbb" > /etc/hostname
passwd
apt update
apt install sudo ssh net-tools ifupdown
nano /etc/inittab
exit
