bin/server:obj/active.o obj/mainact.o
	gcc obj/mainact.o obj/active.o -o bin/server -Wall -g -pthread
obj/active.o:src/active.c
	gcc -o obj/active.o -c src/active.c -Wall -Iinclude 
obj/mainact.o:src/mainact.c
	gcc -o obj/mainact.o -c src/mainact.c -Wall -Iinclude 
	
