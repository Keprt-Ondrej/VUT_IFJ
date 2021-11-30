

#ifndef __PARSER_PRECEDENCE_H__
#define __PARSER_PRECEDENCE_H__
#include "scanner.h"
#include "symtable.h"
#include "parser.h"
#include "code_generator.h"


typedef struct prec_token{
    Token_type type;
    char *identifier;
    htab_record_type_t data_type;
}
precedence_token_t;

int hello_world();


#endif