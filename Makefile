
all: getversion getversion2 simpledrive joydrive simpledrive
	echo "all done..."

getversion: getversion.cpp Makefile
	g++ getversion.cpp -o getversion -lboost_system -lpthread -g

getversion2: getversion2.cpp RoboClaw.o Makefile
	g++ getversion2.cpp RoboClaw.o -o getversion2 -lboost_system -lpthread -g

InputTimeoutHandler.o: InputTimeoutHandler.cpp InputTimeoutHandler.h
	g++ -c InputTimeoutHandler.cpp -o InputTimeoutHandler.o

MantisMovement.o: MantisMovement.cpp MantisMovement.h
	g++ -c MantisMovement.cpp -o MantisMovement.o

RoboClaw.o: RoboClaw.cpp RoboClaw.h Makefile
	g++ -c RoboClaw.cpp -o RoboClaw.o

simpledrive: simpledrive.cpp RoboClaw.o InputTimeoutHandler.o MantisMovement.o Makefile
	g++ simpledrive.cpp \
		RoboClaw.o \
		InputTimeoutHandler.o MantisMovement.o \
		-o simpledrive -lboost_system -lpthread -g -lcurses

joydrive: joydrive.cpp RoboClaw.o InputTimeoutHandler.o MantisMovement.o Makefile
	g++ joydrive.cpp \
		RoboClaw.o \
		InputTimeoutHandler.o MantisMovement.o \
		-o joydrive -lboost_system -lpthread -g -lcurses
