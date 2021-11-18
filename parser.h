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
    int errno;
} parser_data_t;

/**
 * @brief Read token from scanner and store it to data structure
 * 
 * @param data 
 */
void get_token(parser_data_t *data);

/**
 * @brief Compare requested token type with type of actual token
 * 
 * @param data parser data for analysis
 * @param type requested type of token
 * @return true token has same type as requested
 * @return false token has another type
 */
bool is_token(parser_data_t *data, Token_type type);


bool intro(parser_data_t *data);
bool prolog(parser_data_t *data);
bool prog(parser_data_t *data);
bool fce_decl(parser_data_t *data);
bool fce_def(parser_data_t *data);
bool call_fce(parser_data_t *data);
bool type_list(parser_data_t *data);
bool ret_list(parser_data_t *data);
bool type(parser_data_t *data);
bool type_list2(parser_data_t *data);
bool param_def_list(parser_data_t *data);
bool st_list(parser_data_t *data);
bool param_def_list2(parser_data_t *data);
bool param(parser_data_t *data);
bool statement(parser_data_t *data);
bool init(parser_data_t *data);
bool init2(parser_data_t *data);
bool expression(parser_data_t *data);



void parser();


#endif