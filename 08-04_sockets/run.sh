# scp ./* alex@192.168.5.106:/home/alex/LinuxLearning/08-04_sockets
# pkill server; gcc -o server server.c
# ./server &
# ssh alex@192.168.5.106 "export DISPLAY=:0.0; cd  ~/LinuxLearning/csocket; gcc -o client client.c; ./client 192.168.5.108"
# sleep 5
	# gcc -o server server.c
	# ./server &
	# gcc -o cslient client.c
	# ./cslient 127.0.0.1 &
	# sleep 3
	# sudo pkill cslient
	
echo "Remember, not killing server"