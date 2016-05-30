make
sudo insmod ./hello.ko
sudo rmmod hello
sudo dmesg | grep "ALEX"
