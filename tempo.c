#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "smpl.h"

/*evento*/

#define test 1
#define fault 2
#define repair 3

#define faulty 1
#define fault_free 0
#define unknown -1

/*descritor nodo*/


typedef struct {
	int id;
	int* states;
} tnodo;

tnodo *nodo;

int next_node(int i, int n_nodes){
	return (i+1) % n_nodes;
}
void copy_states(int tester_id, int tested_id, int n_nodes){
	int i;
	for(i=tested_id; i!= tester_id; i=next_node(i,n_nodes)){
			// TODO: improve using memcpy
			//printf("COPYING to %d[%d] from %d[%d]: %d",tester_id,i,tested_id,i, nodo[tested_id].states[i]);
			nodo[tester_id].states[i] = nodo[tested_id].states[i];
	}
}

void make_tests(int id, int n_nodes){
	int i;
	bool found_fault_free = false;
	for(i=next_node(id, n_nodes); i!=id && !found_fault_free;
	 																										i=next_node(i, n_nodes)){
			if(nodo[i].states[i] == fault_free){
				//printf("ENTROU id:%d i:%d\n", id, i);
					copy_states(id, i, n_nodes);
					found_fault_free = true;
			}else if(nodo[i].states[i] == faulty){
					nodo[id].states[i] = faulty;
			}else{
				fprintf(stderr, "Erro o nodo %d está com o state %d\n", i, nodo[i].states[i]);
				exit(1);
			}
	}
}

void print_states(int id, int n_nodes){
	int i;
	printf("STATES [ ");
	for(i=0; i < n_nodes;++i){
		printf("%d ", nodo[id].states[i]);
	}
	printf("]\n");
}
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
		nodo[i].states = (int*)malloc(sizeof(int)*n);
		int j;
		for(j=0;j<n;++j){
			nodo[i].states[j] = unknown;
		}
		nodo[i].states[i] = fault_free;
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
				make_tests(token,n);
				print_states(token,n);
				schedule(test, 30.0, token);
				break;
			case fault:
				r = request(nodo[token].id,token,0);
				if(r!=0){
					puts("Impossivel falhar nodo");
					exit(1);
				}
				printf("O nodo %d falhou no tempo %5.1f\n",token, time());
				nodo[token].states[token] = faulty;
				break;

			case repair:
				release(nodo[token].id, token);
				printf("O nodo %d recuperou no tempo %5.1f\n", token, time());
				nodo[token].states[token] = fault_free;
				schedule(test, 30.0, token);
				break;
		}
	}

}
