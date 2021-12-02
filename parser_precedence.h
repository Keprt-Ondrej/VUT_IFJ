

#ifndef __PARSER_PRECEDENCE_H__
#define __PARSER_PRECEDENCE_H__
#include "scanner.h"
#include "symtable.h"
#include "parser.h"
#include "code_generator.h"

typedef struct{
    precedence_token_t **new_token;
    int index;
    int length;

}Buffer_for_token;

int precedence(parser_data_t *data);

#endif