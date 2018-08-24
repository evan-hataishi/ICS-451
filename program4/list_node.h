#ifndef LIST_NODE_H_
#define LIST_NODE_H_

struct list_node
{
  unsigned char data[1500];
  struct list_node * next;
};

#endif
