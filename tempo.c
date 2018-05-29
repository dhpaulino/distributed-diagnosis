#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "smpl.h"
#include "list.h"
#include "vcube.c"

/***================================LIST======================================*/
#include <stdlib.h>
#include <stdbool.h>

/**/
/* colors*/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/*evento*/

#define test 1
#define fault 2
#define repair 3
#define round_end 4

/* states */
#define faulty 1
#define fault_free 0
#define unknown -1


/*descritor nodo*/

typedef struct {
	int id;
	int *states;
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

int make_tests(int id, int n_nodes){
	int i;
    int test_counter=0;
	for(i=next_node(id, n_nodes); i!=id; i=next_node(i, n_nodes)){
			test_counter++;
			if(nodo[i].states[i] == fault_free){
				//printf("ENTROU id:%d i:%d\n", id, i);
					copy_states(id, i, n_nodes);
                    return test_counter;
			}else if(nodo[i].states[i] == faulty){
					nodo[id].states[i] = faulty;
			}else{
				fprintf(stderr, "Erro o nodo %d está com o state %d\n", i, nodo[i].states[i]);
				exit(1);
			}
	}
	return test_counter;
}

// void print_states(int id, int n_nodes){
// 	int i;
// 	printf("STATES [ ");
// 	for(i=0; i < n_nodes;++i){
// 		if(nodo[id].states[i] == faulty){
// 			printf(ANSI_COLOR_RED"x ");
// 		}else if(nodo[id].states[i] == fault_free){
// 			printf(ANSI_COLOR_GREEN"® ");
// 		}else{
// 			printf("* " );
// 		}
// 		printf(ANSI_COLOR_RESET);

// 	}
// 	printf("]\n");
// }
void main(int argc, char *argv[]){
	static int n, token, event, r, i;
	static char fa_name[5];
	int round_counter = 1;
	int node_event, event_number, round_event, last_node_reached, test_event_counter = 0;
    int max_time;
	/*faulty*/
	if(argc!=3){
		puts("Uso correto: tempo <num-nodo> <max-time>");
		exit(1);
	}

	n = atoi(argv[1]);
	max_time = atoi(argv[2]);
    smpl(0, "programa tempo");
	reset();
	stream(1);

	nodo = (tnodo*)malloc(sizeof(tnodo)*n);

    Network* network = init_network(n);

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

		//VCUBE
		calculate_test_list(network, i);
		printf("Node: %d\n",i);
		// print_list(network->nodes[i].tests);
	}
	for(i=0;i<network->qty_nodes;++i){
		run_tests(network, i);
	}

	for(i=0;i<n;++i){
		schedule(test,10.0,i);
	}
	schedule(fault, 11.0, 2);
	schedule(fault, 11.0, 3);
	schedule(fault, 11.0, 4);
	schedule(fault, 11.0, 6);
	//schedule(repair, n*10.0,1);
	schedule(round_end, 10.0, 0);



	while( time() < max_time){
		cause(&event,&token);
		switch(event){
			case test:
				if(status(nodo[token].id) != 0){
					break;//falho
				}
				run_tests(network, token);
				 printf("[%5.1f] node "ANSI_COLOR_YELLOW"%d "ANSI_COLOR_BLUE "TEST "ANSI_COLOR_RESET " => ", time(), token);

    //             if(token != node_event){
    //                 test_event_counter  += make_tests(token,n);
    //             }
    //             // if i'm the one who should test the node that the event happend
				// if(last_node_reached ==  -1 && next_node(token, n) == node_event){
				// 	last_node_reached = token;
				// }
				// if(next_node(token, n) == last_node_reached){
				// 	last_node_reached = token;
				// }
				print_states(network->nodes[token], network->qty_nodes);
				schedule(test, 10.0, token);
				break;
			case fault:
				r = request(nodo[token].id,token,0);
				if(r!=0){
					puts("Impossivel falhar nodo");
					exit(1);
				}
				network->nodes[token].states[token]++;
				printf("[%5.1f] ***" ANSI_COLOR_RED "FAULT"ANSI_COLOR_RESET " on node "ANSI_COLOR_YELLOW"%d" ANSI_COLOR_RESET"***\n", time(), token);
				// nodo[token].states[token] = faulty;
				// node_event = token;
				// event_number = fault;
				// round_event = round_counter;
				// last_node_reached = -1;
			 //    test_event_counter = 0;
				break;

			case repair:
				release(nodo[token].id, token);
				printf("[%5.1f] ***" ANSI_COLOR_GREEN "REPAIR"ANSI_COLOR_RESET " on node "ANSI_COLOR_YELLOW"%d" ANSI_COLOR_RESET"***\n", time(), token);
				// nodo[token].states[token] = fault_free;
				network->nodes[token].states[token]++;
                schedule(test, 10.0, token);
       //          node_event = token;
       //          event_number = repair;
       //          round_event = round_counter;
       //          last_node_reached = -1;
			    // test_event_counter = 0;
			    // make_tests(token,n);
                break;

			case round_end:
				printf("*** END OF ROUND %d***\n", round_counter);
				// if(event_number != -1 && last_node_reached == next_node(node_event, n)){
				// 	printf("Event diagnosed in %d rounds\n", round_counter-round_event+1);
				// 	printf("Number of tests: %d\n", test_event_counter);
    //                 event_number = -1;
				// 	last_node_reached = -1;
    //             }

				schedule(round_end, 10.0, token);
				round_counter++;
		}
	}

}
