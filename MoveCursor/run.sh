scp /home/alex/pi_fs/home/pi/LinuxLearning/MoveCursor/movecursor.c alex@192.168.5.106:/home/alex/LinuxLearning/MoveCursor; ssh alex@192.168.5.106 "export DISPLAY=:0.0; cd  ~/LinuxLearning/MoveCursor; gcc movecursor.c -lX11; ./a.out"