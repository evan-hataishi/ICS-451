
#include "linked_list.h"
#include "list_node.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void add_node(struct list_node ** head, unsigned char * data);
void delete_front(struct list_node ** head);
void print_length(struct list_node ** head);

void add_node(struct list_node ** head, unsigned char * data)
{
  if (!*head) {
    *head = (struct list_node *)malloc(sizeof(struct list_node));
    memcpy((*head)->data, data, 1500);
    (*head)->next = NULL;
    return;
  }

  struct list_node * current = *head;

  while (current->next != NULL) {
    current = current->next;
  }

  current->next = (struct list_node *)malloc(sizeof(struct list_node));
  memcpy(current->next->data, data, 1500);
  current->next->next = NULL;
}

void delete_front(struct list_node ** head)
{
  if (!*head) {
    return;
  }

  struct list_node * front = (*head)->next;
  free(*head);
  *head = front;
}

void print_length(struct list_node ** head)
{
  int length = 0;
  struct list_node * current = *head;
  while (current) {
    length++;
    current = current->next;
  }
  printf("List length: %d\n", length);
}
