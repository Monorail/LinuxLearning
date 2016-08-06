scp ./* alex@192.168.5.106:/home/alex/LinuxLearning/csocket
pkill server; gcc -o server server.c
./server &
ssh alex@192.168.5.106 "export DISPLAY=:0.0; cd  ~/LinuxLearning/csocket; gcc -o client client.c; ./client 192.168.5.108"
sleep 5
pkill server; 