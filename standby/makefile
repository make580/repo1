bin/client:obj/stan_state.o obj/mainsta.o obj/standby.o
	gcc obj/mainsta.o obj/stan_state.o obj/standby.o -o bin/client -Wall -g
obj/stan_state.o:src/stan_state.c
	gcc -o obj/stan_state.o -c src/stan_state.c -Wall -Iinclude 
obj/mainsta.o:src/mainsta.c
	gcc -o obj/mainsta.o -c src/mainsta.c -Wall -Iinclude 
obj/standby.o:src/standby.c
	gcc -o obj/standby.o -c src/standby.c -Wall -Iinclude 
