#ifndef MEXP_H
#define MEXP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum exp_type {FUNCTION, LIST, NUM, NIL, TRUE};
typedef struct _mexp mexp;
typedef struct _list_exp list_exp;
typedef struct _fun function;
typedef struct _nn nnode;
typedef struct _nl nlist;

typedef struct _mexp {
    enum exp_type type;
    union {
        int val;
        function *fun;
        list_exp *list;
    };
} mexp;

typedef struct _list_exp {
    mexp *car;
    mexp *cdr;
} list_exp;

typedef struct _fun {
    nlist *args;
    int body;
    mexp *retval;
} function;

typedef struct _nn { // name node
    char *name;
    nnode *next;
} nnode;

typedef struct _nl {
    nnode *head;
    nnode *tail;
} nlist;

mexp  *make_numexp(int val);
mexp  *make_listexp(mexp *car, mexp *cdr);
mexp *make_funexp(nlist *args, int body);
mexp  *make_nilexp();
mexp  *make_trueexp();
// TODO: nnode stuff should have its own file
nnode *make_nnode(char *name);
nlist *make_nlist();
void add_name(nlist *nl, char *name);

#endif