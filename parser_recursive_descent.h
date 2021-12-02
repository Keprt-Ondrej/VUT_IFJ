/**
 * @file parser_recursive_descent.h
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#ifndef __PARSER_RECURSIVE_DESCENT_H__
#define __PARSER_RECURSIVE_DESCENT_H__

#include "parser.h"
#include "code_generator.h"

/**
 * @brief Seek if on input token is start of expression
 * 
 * @param token pointer on token on input
 * @return true if it is start of expression
 * @return false it is not expression start
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
bool is_expression_start(Token *token);
bool htab_declare_function(char * key,parser_data_t *data);
bool htab_define_function(char * key,parser_data_t *data);

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
bool after_id(parser_data_t *data);
bool assignment(parser_data_t *data);
bool identif_list(parser_data_t *data);
bool expression_list(parser_data_t *data);
bool expression_list2(parser_data_t *data);
bool value_list(parser_data_t *data);
bool value_list2(parser_data_t *data);
bool value(parser_data_t *data);

int parser();

char *label_generator(char *function,char *what, size_t frame_counter);

/**
 * @brief Set name for last item in param list,
 * 
 * @param key ALLOCATED key name
 * @param list first item of parameters
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void set_last_data_token_key(char *key,data_token_t *list);
#endif