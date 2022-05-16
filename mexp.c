#include "mexp.h"

mexp *make_numexp(int val){
    mexp *m = malloc(sizeof(mexp));
    m->type = NUM;
    m->val = val; 
    return m;
}

mexp *make_listexp(mexp *car, mexp *cdr){
    mexp *m = malloc(sizeof(mexp));
    m->type = LIST;
    m->list = malloc(sizeof(list_exp));
    m->list->car = car;
    m->list->cdr = cdr;

    return m;
}

mexp *make_funexp(nlist *args, int body){
    function *f = malloc(sizeof(function));
    f->args = args;
    f->body = body;
    f->retval = NULL;

    mexp *m = malloc(sizeof(mexp));
    m->type = FUNCTION;
    m->fun = f;

    return m;
}

mexp *make_nilexp(){
    mexp *m = malloc(sizeof(mexp));
    m->type = NIL;

    return m;
}

mexp *make_trueexp(){
    mexp *m = malloc(sizeof(mexp));
    m->type = TRUE;

    return m;
}

nnode *make_nnode(char *name){
    nnode *n = malloc(sizeof(nnode));
    n->name = malloc(strlen(name)+1);
    strcpy(n->name, name);
    n->next = NULL;

    return n;
}

nlist *make_nlist(){
    nlist *nl = malloc(sizeof(nlist));
    nl->head = NULL;
    nl->tail = NULL;

    return nl;
}

void add_name(nlist *nl, char *name){
    nnode *n = make_nnode(name);
    if(nl->head == NULL){
        nl->head = n;
        nl->tail = n;
        return;
    }
    nl->tail->next = n;
    nl->tail = n;
}
