# Note: You MUST have sdl2 installed!
# This makefile supports only Ubuntu linux distros

ifndef OS
install:
	sudo apt install libsdl2-dev
	rm -r -f /usr/include/jgame
	mkdir /usr/include/jgame
	cp -r ./* /usr/include/jgame
else
install:
	echo WINDOWS IS NOT SUPPORTED BY THIS MAKEFILE
endif