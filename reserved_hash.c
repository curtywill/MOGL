#include <stdlib.h>
#include <string.h>

#include "reserved_hash.h"

reserved_node *rsvd_table[TABLE_SIZE];

unsigned long djb2(char *str){
    unsigned long hash = 5381;
    for(unsigned int i = 0; i < strlen(str); i++)
        hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
    return hash;
}

reserved_node *make_rsvdnode(enum rsvd_type type, char *name){
    reserved_node *rv = malloc(sizeof(reserved_node));
    rv->type = type;
    rv->name = malloc(strlen(name)+1);
    strcpy(rv->name, name);
    return rv;
}

void insert_rsvd(enum rsvd_type type, char *name){
    reserved_node *rv = make_rsvdnode(type, name);

    unsigned long x = djb2(name);
    int key = x % TABLE_SIZE;
    while(rsvd_table[key] != NULL && strcmp(rsvd_table[key]->name, name) != 0){
        key++;
        key %= TABLE_SIZE;
    }
    rsvd_table[key] = rv;
}

enum rsvd_type get_rsvd(char *name){
    unsigned long x = djb2(name);
    int key = x % TABLE_SIZE;
    while(rsvd_table[key] != NULL){
        if(strcmp(rsvd_table[key]->name, name) == 0){
            return rsvd_table[key]->type;
        }
        key++;
        key %= TABLE_SIZE;
    }

    return NOT_FOUND;
}