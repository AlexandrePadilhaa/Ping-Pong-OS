#Compilador
CC=gcc

#Flags
CFLAGS= #-DDEBUG

#dependencias
DEPS=*.h

#objetos
OBJ=ppos-core-aux.o  pingpong-scheduler-srtf.o  #pingpong-preempcao.o #pingpong-scheduler-srtf.o 

#biblioteca
LIB=libppos_static.a

ppos-core-aux.o: ppos-core-aux.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)	

pingpong-scheduler-srtf.o: pingpong-scheduler-srtf.c $(DEPS)
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