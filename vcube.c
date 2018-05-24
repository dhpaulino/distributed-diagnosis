#include "list.h"
#include <stdbool.h>
#include "math.h"
#include "list.h"
#include "cisj.c"

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
// int **calculate_default_tests(int qty_nodes){
//
//     //sum i from 0..qty_nodes-1 of qty_nodes*2^i = n*[(2^n) -1]
//     const unsigned int data_size_node = pow(2,qty_nodes)-1;
//     const unsigned int data_size = qty_nodes*data_size_node;
//
//     int **default_tests = malloc(sizeof(int*)*qty_nodes + sizeof(int)*data_size);
//     const int* start_data = default_tests+qty_nodes;
//
//     for(int i=0;i<qty_nodes;++i){
//         default_tests[i] = start_data+i;
//         //iterate over each cluster
//         for(int j=0;j<sqrt(qty_nodes);j++){
//             //iterate over each element of cluster with a start offset of i
//             for(int k=0;k<pow(2, j);++k){
//                 const int index = (pow(2,j)-1) + ((i+k) % pow(2,j));
//                 default_tests[i][index] = pow(2,j)-1
//             }
//         }
//     }
//
// }
void calculate_test_list(Network* network, unsigned int index_node, int qty_nodes){
    unsigned int qty_cluster = (unsigned int) log2(qty_nodes);
    Node node = network->nodes[index_node];
    set_size_list(node.tests, 0);
    // printf("clusters:%u nodes: %u\n", qty_cluster, qty_nodes);
    // printf("Node:%d\n", index_node);
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

    print_list(node.tests);
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
    return network;
}


void run_tests(Network* network, unsigned int index_node){
    // bool novelty = false;
    // //TODO: iterate over test list
    // for(int i=0;i<network->qty_nodes;++i){
    //     if(network->nodes[i] > network->nodes[index_node]){
    //         network_nodes[i]  = network->nodes[index_node];
    //         novelty = true;
    //     }
    // }
    // if(novelty){
    //     //TODO: recalculate test list
    //     node_set* ns = cis(index_node, 3);
    // }
}
