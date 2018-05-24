#include "list.h"
#include <stdbool.h>
#include "math.h"
#include "list.h"
#include "cisj.c"

/* colors*/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct {
    int id;
    int *states;
    List tests;
} Node;

typedef struct{
    Node* nodes;
    unsigned int qty_nodes;
} Network;



bool is_node_fault_free(unsigned int qty_events_node){
    if( qty_events_node % 2 == 0){
        return true;
    }
    return false;
}
double log2(double x) {
    return log(x) / log(2);
}
void calculate_test_list(Network* network, unsigned int index_node){
    unsigned int qty_cluster = (unsigned int) log2(network->qty_nodes);
    Node node = network->nodes[index_node];
    set_size_list(node.tests, 0);
    // printf("clusters:%u nodes: %u\n", qty_cluster, qty_nodes);
    //printf("Node:%d\n", index_node);
    for(int i=1; i <= qty_cluster;++i){
        node_set* ns = cis(index_node, i);
        for(int j=0;j<ns->size;j++){
            //TODO: test if node is failed not just check if someone knows that
            if(is_node_fault_free(node.states[ns->nodes[j]])){
                add_list(node.tests, ns->nodes[j]);
                break;
            }
        }
    }

    //print_list(node.tests);
}
bool init_node(Node* node, int qty_nodes){
    node->states = malloc(sizeof(int)*qty_nodes);
    if(!node->states)
        return false;
    node->tests = init_list(qty_nodes);
    if(!node->tests){
        free(node->states);
        return false;
    }

    for(int i=0;i<qty_nodes;++i){
        node->states[i] = 1;
    }
    return true;
}

Network* init_network(unsigned int qty_nodes){
    Network* network = malloc(sizeof(Network));
    if(!network)
        return NULL;
    network->nodes = malloc(sizeof(Node)*qty_nodes);
    if(!network->nodes){
        free(network);
        return NULL;
    }
    for(int i=0;i<qty_nodes;++i){
        //TODO: free previous node when init_node fails
        if(!init_node(&network->nodes[i], qty_nodes)){
            return NULL;
        }
    }
    network->qty_nodes = qty_nodes;
    return network;
}


void run_tests(Network* network, unsigned int index_node){
    bool novelty = false;
    //TODO: iterate over test list
    List tests = network->nodes[index_node].tests;
    unsigned int qty_tests = get_size_list(tests);
    for(int i=0;i<qty_tests;++i){
        int index_testing = get_list(tests, i);
        if(!is_node_fault_free(network->nodes[index_testing].states[index_testing])){
            network->nodes[index_node].states[index_testing]++;
        }else{
            for(int j=0;j<network->qty_nodes;++j){
                if(network->nodes[index_node].states[j] < network->nodes[index_testing].states[j]){
                    network->nodes[index_node].states[j] =  network->nodes[index_testing].states[j];
                    novelty = true;
                }
            }
        }
    }
    if(novelty){
        calculate_test_list(network, index_node);  
    }
}
void print_states(Node node, unsigned int qty_nodes){
    int i;
    printf("STATES [ ");
    for(i=0; i < qty_nodes;++i){
        if(!is_node_fault_free(node.states[qty_nodes])){
            printf(ANSI_COLOR_RED"x ");
        }else{
            printf(ANSI_COLOR_GREEN"® ");
        }
        printf(ANSI_COLOR_RESET);

    }
    printf("]\n");
}
