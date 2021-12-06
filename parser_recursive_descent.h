/**
 * @file parser_recursive_descent.c
 * @brief Interface for recursive descent 
 * 
 * IFJ-2021 Compiler
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
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

/**
 * @brief create declaration representation in global symtable
 * 
 * function takes parametrs from data->param_list and store them
 * 
 * @param key identifier of new function
 * @param data parser data
 * @return htab_item* pointer on new created declaration or NULL if function was declared/defined before
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
htab_item *htab_declare_function(char * key,parser_data_t *data);

/**
 * @brief create definition representation in global symtable
 * 
 * function takes parametrs from data->param_list and store them
 * 
 * @param key identifier of new function
 * @param data parser data
 * @return htab_item* pointer on new created declaration or NULL if function was declared/defined before
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
htab_item *htab_define_function(char * key,parser_data_t *data);

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

/**
 * @brief Initialization of parser data and start of parser 
 * 
 * call function for generating code
 * 
 * @return int 0, if succes, another when error occured
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
int parser();

/**
 * @brief Set name for last item in param list,
 * 
 * @param key ALLOCATED key name
 * @param list first item of parameters
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void set_last_data_token_key(char *key,data_token_t *list);

/**
 * @brief compare 2 lits, if data types are same
 * 
 * @param list1 pointer on beginnig of first list
 * @param list2 pointer on beginnig of second list
 * @return true if lists have same data types
 * @return false if lists do not have same data types
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
bool full_data_token_compare(data_token_t *list1,data_token_t *list2);

/**
 * @brief create proper function representation of build in functions in global symtable
 * 
 * @param data parser data
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void prepare_build_in_functions(parser_data_t *data);

/**
 * @brief reverse list of data tokens
 * 
 * @param place pointer at beginning of the list
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void reverse_list(data_token_t **place);

/************************************************ IFJcode21_generating ***************************************/

/**
 * @brief ALLOCATE string, containing proper label in IFJcode21
 * 
 * @param function actual function, which is processed
 * @param what string descriptor, for more diversification, can be ""
 * @param frame_counter identificator of construction(if/while...)
 * @return char* ALLOCATED label in IFJcode21
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
char *label_generator(char *function,char *what, size_t frame_counter);

/**
 * @brief prepare function call in IFJcode21
 * 
 * control parametrs with function parameters, prepare frame, generate proper call, prepare parameters to frame...
 * 
 * @param data parser data
 * @param function pointer to global symtable on called function
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void generate_function_call(parser_data_t *data,htab_item *function);
#endif