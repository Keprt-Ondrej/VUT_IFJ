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

#define SYNTAX_ERROR 2
#define REDEFINE_UNDEFINE_VAR 3
#define INTERNAL_ERROR 99

/**
 * @brief Data for parser
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
typedef struct parser_data
{   
    Token *token;       //< actual token on input
    Token *token_list_first;    //< first token in token list
    htab_t *global_symtable;    //< table of symbols for functions  
    int errno;                  //< exit code
} parser_data_t;

void set_errno(parser_data_t *data,int errno);

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

/**
 * @brief Seek if on input token is start of expression
 * 
 * @param token pointer on token on input
 * @return true if it is start of expression
 * @return false it is not expression start
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
bool is_expression_start(Token *token);

bool htab_declare_function(htab_key_t key,parser_data_t *data);
bool htab_define_function(htab_key_t key,parser_data_t *data);
void free_parser_data(parser_data_t *data);

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
bool after_id(parser_data_t *data);
bool assignment(parser_data_t *data);
bool identif_list(parser_data_t *data);
bool expression_list(parser_data_t *data);
bool expression_list2(parser_data_t *data);
bool value_list(parser_data_t *data);
bool value_list2(parser_data_t *data);
bool value(parser_data_t *data);

int parser();

bool fake_expression(parser_data_t *data);
#endif