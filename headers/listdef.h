#ifndef INCLUDE_LISTDEF_H
#define INCLUDE_LISTDEF_H

struct Min_Node {

  void *ln_Succ;
  void *ln_Prev;

};

struct Full_Node {

  void *ln_Succ;
  void *ln_Prev;
  char *ln_Name;

};

struct Min_List {

  void *lh_Head;
  void *lh_Tail;
  void *lh_TailPred;

};

#endif /* INCLUDE_LISTDEF_H */
