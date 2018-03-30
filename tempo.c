#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

/*evento*/

#define test 1
#define fault 2
#define repair 3


/*descritor nodo*/


typedef struct { 
	int id;
} tnodo;

tnodo *nodo;


void main(int argc, char *argv[]){
	static int n, token, event, r, i;
	static char fa_name[5];
	
	/*faulty*/
	if(argc!=2){
		puts("Uso correto: tempo <num-nodo>");
		exit(1);
	}

	n = atoi(argv[1]);
	smpl(0, "programa tempo");
	reset();
	stream(1);

	nodo = (tnodo*)malloc(sizeof(tnodo)*n);


	for(i=0; i<n; ++i){
		memset(fa_name,'\0',5);
		sprintf(fa_name,"%d",i);
		nodo[i].id = facility(fa_name,1);

	}

	for(i=0;i<n;++i){
		schedule(test,30.0,i);
	}
	schedule(fault, 31.0, 1);
	schedule(repair, 69.0,1);



	while( time() < 130.0){
		cause(&event,&token);
		switch(event){
			case test:
				if(status(nodo[token].id) != 0){
					break;//falho
				}
				printf("o nodo %d vai testar no tempo %5.1f\n", token, time());
				schedule(test, 30.0, token);
				break;
			case fault:
				r = request(nodo[token].id,token,0);
				if(r!=0){
					puts("Impossivel falhar nodo");
					exit(1);
				}
				printf("O nodo %d falhou no tempo %5.1f\n",token, time());
				break;

			case repair:
				release(nodo[token].id, token);
				printf("O nodo %d recuperou no tempo %5.1f\n", token, time()); 
				schedule(test, 30.0, token);
				break;	
		}
	}

}

