#Compilador
CC=gcc

#Flags
CFLAGS= #-DDEBUG

#dependencias
DEPS=*.h

#objetos
OBJ=ppos-core-aux.o  pingpong-disco2.o ppos_disk.o  

#biblioteca
LIB=libppos_static.a

ppos-core-aux.o: ppos-core-aux.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)	

pingpong-disco1.o: pingpong-disco2.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)	

ppos_disk.o: ppos_disk.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)	
	
#pingpong-preempcao.o: pingpong-preempcao.c $(DEPS)
#	$(CC) -g -c -o $@ $< $(CFLAGS)	

ppos-test: $(OBJ)
	$(CC) -g -o $@ $^ $(CFLAGS) $(LIB)
	rm *.o

run: ppos-test
	./ppos-test

clean:
	rm -f $(OBJ)