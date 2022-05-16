mogl: main.o reserved_hash.o mexp.o scope.o
	gcc main.o reserved_hash.o mexp.o scope.o -Wall -std=c99 -o mogl 

main.o: main.c
	gcc -c main.c

reserved_hash.o: reserved_hash.c reserved_hash.h 
	gcc -c reserved_hash.c

mexp.o: mexp.c mexp.h
	gcc -c mexp.c

scope.o: scope.c scope.h 
	gcc -c scope.c

clean:
	rm *.o mogl