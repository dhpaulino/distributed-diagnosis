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



unsigned int* my_cis(unsigned int i, unsigned int s){
    unsigned int cluster_size = pow(2, s-1);
    unsigned int offset = i%cluster_size;
    unsigned int initial_node_cluster  = (i/cluster_size)*cluster_size;
    unsigned int inital_node_pair_cluster = (i/(cluster_size*2))*(cluster_size*2);


    //asume that i is in the first cluster of the pair
    unsigned int initial_node_other_cluster = initial_node_cluster+cluster_size;
    //if i is in the second cluster of the pair
    if(initial_node_cluster != inital_node_pair_cluster){
        initial_node_other_cluster = inital_node_pair_cluster;
    }

    unsigned int* tests = malloc(sizeof(cluster_size));
    for(int i=0; i < cluster_size;i++){
        tests[i] = initial_node_other_cluster + offset;
        offset = (offset + 1) % cluster_size;
    }

    return tests;


}
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
    for(int i=0; i < network->qty_nodes;++i){
        //iterate over all clusters
        for(int j=1;j<=qty_cluster;j++){
            int size_default_tests = pow(2,j-1);
             unsigned int* default_tests = my_cis(i, j);
             // printf("N:%d C:%d:", i, j);
             //iterate over all nodes from cluster j, ordered with offset i%2^(j-1)
             for(int k=0;k<size_default_tests;k++){
                // printf("%d", ns->nodes[k]);
                //TODO: test if node is failed not just check if someone knows that
                if(is_node_fault_free(node.states[default_tests[k]])){
                    // printf("ADICIONADO:TESTA N:%d C:%d = %d => STATUS %d\n", i, j, ns->nodes[k], node.states[ns->nodes[k]]);
                    if(default_tests[k] == index_node){
                        // printf("ADICIONADO:TESTA N:%d C:%d = %d => STATUS %d\n", i, j, ns->nodes[k], node.states[ns->nodes[k]]);
                        // printf("adicionado\n");
                        add_list(node.tests, i);
                    }
                    break;
                }else{
                    // printf("TESTA N:%d C:%d = %d => STATUS %d\n", i, j, ns->nodes[k], node.states[ns->nodes[k]]);
                }
            }
            // printf("\n");
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
        node->states[i] = 0;
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

    for(int i=0; i < qty_nodes; ++i){
      calculate_test_list(network, i);
    }
    return network;
}


unsigned int run_tests(Network* network, unsigned int index_node){
    bool novelty = false;
    List tests = network->nodes[index_node].tests;
    unsigned int qty_tests = get_size_list(tests);
    for(int i=0;i<qty_tests;++i){
        int index_testing = get_list(tests, i);

        for(int j=0;j<network->qty_nodes;++j){
            if(network->nodes[index_node].states[j] < network->nodes[index_testing].states[j]){
                printf("    !!!NEWS DETECTED FROM NODE %u: change at node %u!!!\n", index_testing, j);
                network->nodes[index_node].states[j] =  network->nodes[index_testing].states[j];
                novelty = true;
            }
        }
        
    }
    if(novelty){
        calculate_test_list(network, index_node);
        printf("    Recalculated test list: ");
        print_list(network->nodes[index_node].tests);
    }

    return qty_tests;
}
void print_states(Node node, unsigned int qty_nodes){
    int i;
    printf("STATES [ ");
    for(i=0; i < qty_nodes;++i){
        if(!is_node_fault_free(node.states[i])){
            printf(ANSI_COLOR_RED"x ");
        }else{
            printf(ANSI_COLOR_GREEN"® ");
        }
        printf(ANSI_COLOR_RESET);

    }
    printf("]\n");
}
