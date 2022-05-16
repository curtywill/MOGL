#ifndef SCOPE_H
#define SCOPE_H

#include <string.h>
#include "mexp.h"

#define SCOPE_SIZE 100

typedef struct _scope scope;
typedef struct _sn snode;

typedef struct _scope {
    snode **table;
    int size;
} scope;

typedef struct _sn {
    char *name;
    mexp *m;
} snode;


unsigned long djjb2(char *str);
snode *make_snode(char *name, mexp *m);
scope *make_scope();
void free_scope(scope *s);
void install(scope *s, char *name, mexp *m);
mexp *fetch(scope *s, char *name);

#endif