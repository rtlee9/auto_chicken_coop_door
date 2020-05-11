all:
	arduino-cli cache clean
	arduino-cli compile -p /dev/ttyACM0 -b arduino:megaavr:nona4809
	arduino-cli upload -p /dev/ttyACM0 -b arduino:megaavr:nona4809
	stty -F /dev/ttyACM0* 9600 raw -clocal -echo
	cat /dev/ttyACM0*
