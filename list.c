#include <stdlib.h>
#include <stdbool.h>
#include "list.h"
#include <stdio.h>

List init_list(int max_qtd_elements){
  List list = malloc(sizeof(struct List));
  list->size = 0;
  list->max_qtd_elements = max_qtd_elements;
  list->data = malloc(sizeof(int)*max_qtd_elements);

  return list;
}

bool add_list(List list, int element){
  if(list->size==list->max_qtd_elements){
    return false;
  }
  list->data[list->size++] = element;

  return true;

}
int get_list(List list, unsigned int index){
  return list->data[index];
}

unsigned int get_size_list(List list){
  return list->size;
}
void set_size_list(List list, int size){
  list->size = size;
}
void clean_list(List list){
  free(list->data);
  free(list);
}
void print_list(List list){
    for(int i=0;i<list->size;++i){
        printf("[%d]:%d\n", i, list->data[i]);
    }
}
