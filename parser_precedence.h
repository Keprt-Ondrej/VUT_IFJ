/**
 * @file parser_precedence.h
 * @brief precedence interface.
 * 
 * IFJ-2021 Compiler
 * 
 * @author Maxim Gerasimov (xgeras00@stud.fit.vutbr.cz)
*/

#ifndef __PARSER_PRECEDENCE_H__
#define __PARSER_PRECEDENCE_H__
#include "scanner.h"
#include "symtable.h"
#include "parser.h"
#include "code_generator.h"

/**
 * @brief Buffer/stack 
 * 
*/

typedef struct{
    precedence_token_t **token;
    int index;
    int current_length;
    int left_bracket;
    int right_bracket;    
}Buffer_for_token;

/**
 * @brief precedence table 
 * 
*/

typedef enum{
    P,  //push
    R,  //reduse
    N,  //no precedence
}compare_result;


#endif