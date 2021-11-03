/**
 * @file parser.h
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#ifndef __PARSER_H__
#define __PARSER_H__

#include "scanner.h"
#include "symtable.h"


typedef struct 
{   
    Token *token;
    Token *token_list_first;
    htab_t *global_symtable;
} parser_data_t;

void get_token(parser_data_t *data);
bool intro(parser_data_t *data);
void parser();


#endif