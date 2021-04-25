/*
** Lists.c
**
** Functions to manipulate lists.
**
** 19 Aug 1992 - Created    - J. Terrell
**
*/
#include "headers/tracer.h"

struct Min_Node *remtail(struct Min_List *list)
{

  struct Min_Node *node,*newtail;

  node = list->lh_TailPred;

  newtail = node->ln_Prev;

  list->lh_TailPred = newtail;
  newtail->ln_Succ = (struct Min_Node *)&list->lh_Tail;

  return(node);

}

void addtail(struct Min_List *list,struct Min_Node *nd)
{

  struct Min_Node *otail;

  otail = list->lh_TailPred;

  otail->ln_Succ = nd;
  nd->ln_Prev = otail;

  nd->ln_Succ = &list->lh_Tail;
  list->lh_TailPred = nd;

}

void addhead(struct Min_List *list,struct Min_Node *nd)
{

  struct Min_Node *ohead;

  ohead = list->lh_Head;

  ohead->ln_Prev = nd;
  nd->ln_Succ = ohead;

  nd->ln_Prev = (struct Min_Node *)list;
  list->lh_Head = nd;

}

BOOL IsListEmpty(struct Min_List *l)
{

  return( (BOOL)(l->lh_Head == (struct Min_Node *)&l->lh_Tail ? TRUE : FALSE) );

}

void newlist(struct Min_List *l)
{

  l->lh_Head = &l->lh_Tail;
  l->lh_Tail = 0;
  l->lh_TailPred = &l->lh_Head;

}

void removenode(struct Min_Node *node)
{

  struct Min_Node *prev,*succ;

  succ = node->ln_Succ;
  prev = node->ln_Prev;

  succ->ln_Prev = prev;

  prev->ln_Succ = succ;

}
