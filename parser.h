/**
 * @file parser.h
 * @brief 
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/

#ifndef __PARSER_H__
#define __PARSER_H__

#include "scanner.h"
#include "symtable.h"
#include "parser_precedence.h"
#include "code_generator.h"

/**
 * @brief Data for parser
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
typedef struct parser_data
{   
    Token *token;               ///< actual token on input
    Token *token_list_first;    ///< first token in token list
    htab_t *global_symtable;    ///< table of symbols for functions  
    int errno;                  ///< exit code
} parser_data_t;

void set_errno(parser_data_t *data,int errno);

/**
 * @brief Read token from scanner and store it to data structure
 * 
 * @param data pointer to data 
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

void free_parser_data(parser_data_t *data);
int print_token(Token *token);

bool expression(parser_data_t *data);
int precedence(parser_data_t *data);

#endif

