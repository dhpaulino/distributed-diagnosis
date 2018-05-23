#ifndef LIST_H
#define LIST_H
#include <stdlib.h>
#include <stdbool.h>

struct List{
  int size;
  int max_qtd_elements;
  int* data;
};

typedef struct List* List;
List init_list(int max_qtd_elements);
bool add_list(List list, int element);
void set_size_list(List list, int size);
void clean_list(List list);
#endif
