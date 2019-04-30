#DIR := ${GET_WORD}
DIR = /remotehomes/tam.t.nguyen/Downloads/getWord
VPATH = src:./include
Main : getWord.o crc64.o sTools.o hashingTools.o Main.c libget.a
	gcc -g -o wordpairs Main.c getWord.o crc64.o sTools.o hashingTools.o -I$(DIR)/lib/libget.a
getWord.o : getWord.c getWord.h
	cc -c -I$(DIR)/include/getWord.c $(DIR)/include/getWord.h	
#	cc -c include/getWord.c include/getWord.h 
crc64.o : crc64.c crc64.h
	cc -c  crc64.c crc64.h
sTools.o : sTools.c sTools.h
	cc -c  sTools.c sTools.h
hashingTools.o : hashingTools.c hashingTools.h
	cc -c  hashingTools.c hashingTools.h
clean :
	rm $(DIR)/include/*.h.gch *.h.gch
