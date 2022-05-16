#ifndef RESERVED_HASH_H
#define RESERVED_HASH_H

enum rsvd_type {FUN, CAR, CDR, CONS, REM, ADD, SUB, 
DIV, MULT, EQ, LT, GT, ATOM, TRUEE, NILL, PUT, SET, COND, NOT_FOUND};

#define TABLE_SIZE 25

typedef struct _rn{
    char *name;
    enum rsvd_type type;
} reserved_node;

unsigned long djb2(char *str);
reserved_node *make_rsvdnode(enum rsvd_type type, char *name);
void insert_rsvd(enum rsvd_type type, char *name);
enum rsvd_type get_rsvd(char *name);

#endif