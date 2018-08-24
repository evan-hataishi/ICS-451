#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_
#include "list_node.h"

void add_node(struct list_node ** head, unsigned char * data);
void delete_front(struct list_node ** head);
void print_length(struct list_node ** head);

#endif
