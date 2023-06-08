CC = gcc   #compiler
DIR = $(PWD)
sourceFiles = $(addprefix $(DIR)/src/, keylogger.c keyboard.c eventReader.c network.c)     #all source files
includes = keyboard.h eventReader.h     
IDIR = $(DIR)/src/include#include directory
includeFiles = $(addprefix $(IDIR)/, $(includes))


all: $(sourceFiles) $(includeFiles)
	echo $(DIR)
	gcc -o $(DIR)/bin/keylogger.out -I$(IDIR) $(sourceFiles)




       
