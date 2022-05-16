#include "scope.h"

unsigned long djjb2(char *str){
    unsigned long hash = 5381;
    for(unsigned int i = 0; i < strlen(str); i++)
        hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
    return hash;
}

snode *make_snode(char *name, mexp *m){
    snode *s = malloc(sizeof(snode));
    s->name = malloc(strlen(name)+1);
    strcpy(s->name, name);
    s->m = m;

    return s;
}

scope *make_scope(int global){
    scope *s = malloc(sizeof(scope));
    s->table = malloc(sizeof(snode)*SCOPE_SIZE);
    s->size = 0;
    for(int i = 0; i < SCOPE_SIZE; i++) s->table[i] = NULL; 
    
    return s;
}

void free_scope(scope *s){
    for(int i = 0; i < SCOPE_SIZE; i++){
        if(s->table[i]) free(s->table[i]);
    }
    free(s->table);
    free(s);
}

void install(scope *s, char *name, mexp *m){
    snode *sn = make_snode(name, m);
    unsigned long x = djjb2(name);
    s->size++;
    if(s->size > SCOPE_SIZE){
        fprintf(stderr, "Too many names.\n");
        exit(1);
    }
    int key = x % SCOPE_SIZE;
    while(s->table[key] != NULL && strcmp(s->table[key]->name, name) != 0){
        key++;
        key %= SCOPE_SIZE;
    }
    s->table[key] = sn;
}

mexp *fetch(scope *s, char *name){
    unsigned long x = djjb2(name);
    int key = x % SCOPE_SIZE;
    while(s->table[key] != NULL){
        if(strcmp(s->table[key]->name, name) == 0){
            return s->table[key]->m;
        }
        key++;
        key %= SCOPE_SIZE;
    }
    return NULL;
}