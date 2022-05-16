// Curtiss Williams
// Demo for a MOGL (McCarthy's Original Lisp) interpreter.

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "reserved_hash.h"
#include "mexp.h"
#include "scope.h"

#define MAX_INPUT 3000

// global variables
char input[MAX_INPUT];
int pos = 0;
scope *global;
scope *cur;
mexp *nil; // only want one reference to true/false
mexp *true;

// circular dependencies
mexp *parse_mexp();
mexp *parse_list();
void print_list(mexp *m);
void print(mexp *m);

//////////////////////
char *read_program(char *source){ // reads file into an array
    FILE *pf = fopen(source, "r");

    if(!pf){
        fprintf(stderr, "%s not found\n", source);
        exit(1);
    }

    char *_input = malloc(sizeof(char) * (MAX_INPUT/2)); // half for standard library, half for user input
    int c;
    int _pos = 0;

    while((c=getc(pf)) != EOF){ 
        if(c == '~') while((c=getc(pf)) != EOF && c != '\n');
        if(c == EOF) break;
        if(c == '\n' || c == '\t') c = ' ';
        if(c == '\r') continue;
        _input[_pos++] = c;
    }

    _input[_pos] = '\0';
    fclose(pf);
    return _input;
}

void init_reserved(){
    insert_rsvd(FUN, "fun");
    insert_rsvd(CAR, "car");
    insert_rsvd(CDR, "cdr");
    insert_rsvd(CONS, "cons");
    insert_rsvd(ADD, "add");
    insert_rsvd(SUB, "sub");
    insert_rsvd(LT, "lt");
    insert_rsvd(GT, "gt");
    insert_rsvd(REM, "rem");
    insert_rsvd(MULT, "mult");
    insert_rsvd(DIV, "div");
    insert_rsvd(ATOM, "atom");
    insert_rsvd(EQ, "eq");
    insert_rsvd(TRUEE, "true");
    insert_rsvd(NILL, "nil");
    insert_rsvd(PUT, "put");
    insert_rsvd(SET, "set");
    insert_rsvd(COND, "cond");
}

void init_interpreter(){
    nil = make_nilexp();
    true = make_trueexp();
    global = make_scope();
    cur = global;
    init_reserved();
}

void interpret(){
    while(input[pos] != '\0'){
        parse_mexp();
    }
}

void repl(){
    char repl_buffer[100];
    while(1){
        printf("Mogl> ");
        if(!fgets(repl_buffer, 100, stdin)){
            puts("bye bye!"); break;
        }
        repl_buffer[strcspn(repl_buffer, "\r\n")] = 0; // https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
        strcat(input, repl_buffer);
        print(parse_mexp()); putchar('\n');
    }
}

// helper functions
void skip_blanks(){
    while(input[pos] == ' ') pos++;
}

void skip_brackets(int open){
    do {
        if(input[pos] == '\0'){
            fprintf(stderr, "Expected closing bracket.\n");
            exit(1);
        }
        if(input[pos] == '[')      open++;
        else if(input[pos] == ']') open--;
        pos++;
    } while(open > 0);
    skip_blanks();
}

char *substr(int start, int stop){
    char *ret = malloc((stop-start)+1);
    char *s = ret;
    for(int i = start; i < stop; i++)
        *s++ = input[i];
        
    *s = '\0';
    return ret;
}

int is_alnum(int c){
    return (isalnum(c) || c == '_' ); // underscores are valid in names
}

void consume(int c, char *error_msg){ // idea from crafting interpreters
    if(input[pos] != c){
        fprintf(stderr, "%s\n", error_msg);
        exit(1);
    }
    pos++;
    skip_blanks();
}

char *extract_name(){
    int start = pos;
    while(is_alnum(input[pos])) pos++;
    char *str = substr(start, pos);
    skip_blanks();

    return str;
}

///////////////////////////////////////
void fun(){
    consume('[', "Expected '[' before function declaration\n");
    char *name = extract_name(); // get fun name
    if(cur != global){
        fprintf(stderr, "Cannot define functions in local scope.\n");
        exit(1);
    }

    if(fetch(global, name) != NULL){
        fprintf(stderr, "Redefinition of %s", name);
        exit(1);
    }

    consume(';', "Expected semicolon after fun name\n");
    nlist *nl = make_nlist();
    while(input[pos] != ';'){
        add_name(nl, extract_name());
    }

    consume(';', "Expected ';' after functions arguments");
    int body = pos; // index of the beginning of the function's body
    skip_brackets(1); // implicit consumption of the closing bracket
    install(cur, name, make_funexp(nl, body));
}

mexp *car(){
    consume('[', "Expected '[' before car arguments\n");
    mexp *m = parse_mexp();
    if(m->type != LIST){
        fprintf(stderr, "car argument was not a list\n");
        exit(1);
    }
    consume(']', "Expected ']' after car arguments\n");
    return m->list->car;

}

mexp *cdr(){
    consume('[', "Expected '[' before cdr arguments\n");
    mexp *m = parse_mexp();
    if(m->type != LIST){
        fprintf(stderr, "cdr argument was not a list\n");
        exit(1);
    }
    consume(']', "Expected ']' after cdr arguments\n");
    return m->list->cdr;
}

mexp *cons(){
    consume('[', "Expected '[' before cons arguments\n");
    mexp *left  = parse_mexp();
    consume(';', "Expected ';' between cons arguments\n");
    mexp *right = parse_mexp();
    consume(']', "Expected ']' after cons arguments\n");

    switch(right->type){
        case LIST:
        case NIL:
            return make_listexp(left, right);
        default:
            fprintf(stderr, "Second cons argument was not a list\n");
            exit(1);
    }

    return nil;
}

mexp *atom(){
    consume('[', "Expected '[' before atom arguments\n");
    mexp *m = parse_mexp();
    consume(']', "Expected ']' after atom arguments\n");
    if(m->type == LIST) return nil; // nil is not considered a list in McCarthy lisp
    return true;
}

mexp *eq(){
    consume('[', "Expected '[' before eq arguments\n");
    mexp *left = parse_mexp();
    consume(';', "Expected ';' between eq arguments\n");
    mexp *right = parse_mexp();
    consume(']', "Expected ']' after eq arguments\n");

    if(left == right) return true; // bool/same element check
    if(left->type == NUM && right->type == NUM){
        if(left->val == right->val) return true;
        else return nil;
    }

    return nil;
}

void print_list(mexp *m){
    if(m->type == NIL){
        printf(") "); return;
    }
    print(m->list->car);
    print_list(m->list->cdr);
}

void print(mexp *m){
    switch(m->type){
        case NUM:
            printf("%d ", m->val); break;
        case LIST:
            printf("( ");
            print_list(m); break;
        case NIL:
            printf("nil "); break;
        case TRUE:
            printf("true "); break;
        default:
            fprintf(stderr, "not printable\n");
            exit(1);
    }
}

void put(){
    consume('[', "Expected '[' before put argument\n");
    mexp *m = parse_mexp();
    consume(']', "Expected ']' after put argument\n");
    print(m);
}

void set(){
    consume('[', "Expected '[' before set arguments\n");
    char *name = extract_name();
    consume(';', "Expected ';' between set arguments\n");
    mexp *value = parse_mexp();
    consume(']', "Expected ']' after set arguments");
    if(get_rsvd(name) != NOT_FOUND){
        fprintf(stderr, "%s is a reserved keyword\n", name);
        exit(1);
    }
    install(cur, name, value);
}

mexp *math(enum rsvd_type type){
    consume('[', "Expect '[' before math operators\n");
    mexp *left = parse_mexp();
    consume(';', "Expect ';' between math operands\n");
    mexp *right = parse_mexp();
    consume(']', "Expect ']' after math operands\n");

    if(left->type != NUM || right->type != NUM){
        fprintf(stderr, "Math is only valid for numbers\n");
        exit(1);
    }

    switch(type){
        case REM:  return make_numexp(left->val % right->val);
        case ADD:  return make_numexp(left->val + right->val);
        case SUB:  return make_numexp(left->val - right->val);
        case MULT: return make_numexp(left->val * right->val);
        case DIV:  return make_numexp(left->val / right->val);
        case LT:   if(left->val < right->val) return true;
                   else return nil;
        case GT:   if(left->val > right->val) return true;
                   else return nil;
        default: break; //unreachable
    }
    
    return NULL; //unreachable
}

mexp *cond(){
    mexp *m = parse_mexp();
    if(m != true && m != nil){
        fprintf(stderr, "Propositional expression must return boolean value.\n");
        exit(1);
    }
    consume('-', "Expect '->' after cond clauses.\n");
    consume('>', "Expect '->' after cond clauses.\n");

    if(m == true){
        mexp *retval = parse_mexp();
        while(input[pos] != ']'){ // skip past the rest of cond
            if(input[pos] == '[') skip_brackets(0);
            else pos++;
        }
        consume(']', "Expected ']' after call to cond.\n");
        return retval;
    }
    else{
        while(input[pos] != ']' && input[pos] != ';'){
            if(input[pos] == '[') skip_brackets(0);
            else pos++;
        }
        if(input[pos] == ']'){
            consume(']', "Expected ']' after call to cond.\n");
            return nil;
        }
        if(input[pos] == ';'){
            consume(';', "Expected ';' between conditional expressions.\n");
            return cond();
        }
    }

    return nil;
}

mexp *call(mexp *f){
    scope *fun_scope = make_scope();
    consume('[', "Expected '[' after fun call\n");

    for(nnode *n = f->fun->args->head; n != NULL; n = n->next){
        install(fun_scope, n->name, parse_mexp()); // put parameters into function scope
        if(n->next != NULL) consume(';', "Expected ';' between arguments");
    }
    consume(']', "Expected ']' after arguments\n");
    
    int temp = pos;
    pos = f->fun->body;
    scope *temp_scope = cur;
    cur = fun_scope;
    mexp *retval = parse_mexp(); // expecting cond or begin
    cur = temp_scope;
    pos = temp;
    free_scope(fun_scope);
    return retval;
}

mexp *identifier(char *name){
    mexp *m = fetch(cur, name);

    if(m == NULL){
        if(cur != global) m = fetch(global, name);
        if(m == NULL){
            fprintf(stderr, "Undefined variable %s\n", name);
            exit(1);
        }
    }

    if(m->type == FUNCTION && input[pos] == '[') m = call(m);
    return m;
}

mexp *keyword(){
    char *str = extract_name();
    enum rsvd_type type = get_rsvd(str);
    switch(type){
        case FUN:       fun(); return nil;
        case CAR:       return car();
        case CDR:       return cdr();
        case CONS:      return cons();
        case ATOM:      return atom();
        case EQ:        return eq();
        case NILL:      return nil;
        case TRUEE:     return true;
        case PUT:       put(); return nil;
        case SET:       set(); return nil;
        case COND:
            consume('[', "Expect '[' before cond clauses.\n");   // cond is recursive so we consume here
            return cond();
        case GT:
        case LT:
        case REM:
        case ADD:
        case SUB:
        case MULT:
        case DIV:       return math(type);
        case NOT_FOUND: return identifier(str);
    }

    return nil;
}

mexp *parse_list(){
    mexp *car;
    mexp *cdr;
    mexp *m;
    if(input[pos] == ')'){
        m = nil;
        pos++; skip_blanks();
    }
    else{
        car = parse_mexp();
        cdr = parse_list();
        m = make_listexp(car, cdr);
    }

    return m;
}

int parse_int(){
    int start = pos;
    while(isdigit(input[pos])) pos++;
    int val = atoi(substr(start, pos));
    skip_blanks();
    return val;
}

mexp *parse_mexp(){
    skip_blanks();
    int c = input[pos];
    if(isdigit(c)) return make_numexp(parse_int());
    else if(c == '-'){pos++; return make_numexp(parse_int() * -1);}
    else if(isalpha(c)) return keyword();
    else if(c == '('){
        pos++; skip_blanks();
        return parse_list();
    }
    else if(c == '\0') return nil;
    else{
        fprintf(stderr, "parse error on %c\n", c);
        exit(1);
    }
    
    return nil;
}

int main(int argc, char **argv){
    init_interpreter();

    strcpy(input, read_program("std.mogl")); // read standard library into memory
    interpret();

    if(argc == 1) repl(); // interactive console
    else if(argc == 2){ // interpret a file
        strcat(input, read_program(argv[1])); 
        interpret();
    }
    else{ // error
        fprintf(stderr, "Usage: ./main [file] to interpret a mogl file\n./main to use the REPL\n");
        exit(1);
    }

    free(nil);
    free(true);
    free_scope(global);
}