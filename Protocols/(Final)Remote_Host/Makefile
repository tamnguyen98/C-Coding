all:
	make client
	make server
client: mftp.c mftp.h
	cc -o client mftp.c mftp.h
server: mftpserve.c mftp.h
	cc -o server mftpserve.c  mftp.h
clean:
	rm client server
