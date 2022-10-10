all: 
	gcc utils.c  console.c builtins.c comm.c shush.c -o main
clean:
	rm main