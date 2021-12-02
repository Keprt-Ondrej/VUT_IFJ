

#ifndef __PARSER_PRECEDENCE_H__
#define __PARSER_PRECEDENCE_H__
#include "scanner.h"
#include "symtable.h"
#include "parser.h"
#include "code_generator.h"


typedef struct prec_token{
    Token_type token_type;
    char *identifier;
    data_type_t data_type;
    struct prec_token *next;
}
precedence_token_t;


typedef struct{
    precedence_token_t **new_token;
    int index;
    int length;

}Buffer_for_token;

int precedence(parser_data_t *data);

#endif