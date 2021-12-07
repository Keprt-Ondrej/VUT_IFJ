

#ifndef __PARSER_PRECEDENCE_H__
#define __PARSER_PRECEDENCE_H__
#include "scanner.h"
#include "symtable.h"
#include "parser.h"
#include "code_generator.h"


typedef enum{
    answer_number,
    answer_str,
    answer_int,
    answer_nil
}Answer_type;

typedef struct{
    precedence_token_t **token;
    int index;
    int active_index;
    int current_length;
    int left_bracket;
    int right_bracket;    
}Buffer_for_token;

typedef enum{
    P,  //push
    R,  //reduse
    E,  //equal
    N,  //no precedence
}compare_result;


#endif