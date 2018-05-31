#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "smpl.h"
#include "list.h"
#include "vcube.c"

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


typedef struct{
    unsigned int node_number;
    unsigned int tests_counter;
    unsigned int rounds_counter;
    unsigned int node_state;
    bool diagnosed;
} Event_info;

void init_event_info(Event_info* event_info, unsigned node_number, unsigned int node_state){
    event_info->tests_counter = 0;
    event_info->rounds_counter = 0;
    event_info->diagnosed = false;
    event_info->node_state = node_state;
    event_info->node_number = node_number;

}
void update_event_situation(Event_info* event_info, Network* network){
    //TODO: verify only for the corrected nodes
    bool reach_all_nodes = true;
    for(int i=0; i < network->qty_nodes && reach_all_nodes; ++i){
        if(network->nodes[i].states[event_info->node_number] < event_info->node_state){
            reach_all_nodes = false;
        }
    }
    event_info->diagnosed = reach_all_nodes;
    event_info->rounds_counter++;
}
void main(int argc, char *argv[]){
    static int n, token, event, r, i;
    static char fa_name[5];
    unsigned int round_counter = 1;
    Event_info event_info;
    event_info.diagnosed = true;

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

    Network* network = init_network(n);

    for(i=0; i<n; ++i){
        memset(fa_name,'\0',5);
        sprintf(fa_name,"%d",i);
        network->nodes[i].id = facility(fa_name,1);
    }

    for(i=0;i<n;++i){
        schedule(test,10.0,i);
    }
    schedule(fault, 11.0, 3);
    schedule(repair, 40.0,3);
    schedule(round_end, 10.0, 0);



    while( time() < max_time){
        cause(&event,&token);
        switch(event){

            case test:
                if(status(network->nodes[token].id) != 0){
                        break;//failed
                    }
                printf("[%4.1f] Node "ANSI_COLOR_YELLOW"%d " ANSI_COLOR_BLUE"TESTS: "ANSI_COLOR_RESET, time(), token);
                print_list(network->nodes[token].tests);  
                unsigned int qty_tests = run_tests(network, token);   
                if(!event_info.diagnosed){
                    event_info.tests_counter += qty_tests;
                }
                printf("    => ");
                print_states(network->nodes[token], network->qty_nodes);
                schedule(test, 10.0, token);
                break;

            case fault:
                r = request(network->nodes[token].id,token,0);
                if(r!=0){
                    puts("Impossible to fail node");
                    exit(1);
                }
                network->nodes[token].states[token]++;
                init_event_info(&event_info, token, network->nodes[token].states[token]);
                printf("[%5.1f] ***" ANSI_COLOR_RED "FAULT"ANSI_COLOR_RESET " on node "ANSI_COLOR_YELLOW"%d" ANSI_COLOR_RESET"***\n", time(), token);
                break;

            case repair:
                release(network->nodes[token].id, token);
                printf("[%5.1f] ***" ANSI_COLOR_GREEN "REPAIR"ANSI_COLOR_RESET " on node "ANSI_COLOR_YELLOW"%d" ANSI_COLOR_RESET"***\n", time(), token);
                network->nodes[token].states[token]++;
                init_event_info(&event_info, token, network->nodes[token].states[token]);
                schedule(test, 10.0, token);
                break;

            case round_end:
                if(!event_info.diagnosed){
                    update_event_situation(&event_info, network);
                    if(event_info.diagnosed){
                        printf("Event diagnosed in %u rounds and %u tests.\n", event_info.rounds_counter, event_info.tests_counter);
                    }
                }
                printf("--------END OF ROUND %d--------\n\n", round_counter);
                schedule(round_end, 10.0, token);
                round_counter++;
            }
        }
        destroy_network(network);
    }
