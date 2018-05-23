all: tempo

tempo: tempo.o smpl.o rand.o list.o
	$(LINK.c) -o $@ -Bstatic tempo.o smpl.o rand.o list.o -lm

smpl.o: smpl.c smpl.h
	$(COMPILE.c)  -g smpl.c

tempo.o: tempo.c smpl.h
	$(COMPILE.c) -g  tempo.c

rand.o: rand.c
	$(COMPILE.c) -g rand.c

list.o: list.h list.c 
	$(COMPILE.c) -g list.c
clean:
	$(RM) *.o tempo relat saida
