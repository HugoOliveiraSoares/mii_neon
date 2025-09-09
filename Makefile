# CREATED BY VIM-PIO
all:
	platformio -f -c vim run

upload:
	platformio -f -c vim run --target upload

clean:
	platformio -f -c vim run --target clean

program:
	platformio -f -c vim run --target program

uploadfs:
	platformio -f -c vim run --target uploadfs

monitor:
	platformio -f -c vim device monitor --baud 115200

create_tar:
	tar -cf filesystem.tar -C data js css index.html settings.html 

liveserver:
	echo "Starting live-server"
	live-server data
