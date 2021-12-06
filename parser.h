/**
 * @file parser.h
 * @brief Interface for parser for precedence and recursive descent analysis
 * 
 * IFJ-2021 Compiler
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/

#ifndef __PARSER_H__
#define __PARSER_H__

#include "scanner.h"
#include "symtable.h"
#include "code_generator.h"

/**
 * @brief Token used in precedence analysis and passing evaluated expression to recursive descent
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
typedef struct prec_token{
    Token_type type;
    char *identifier;
    data_type_t data_type;
    struct prec_token *next;
}
precedence_token_t;

/**
 * @brief Data for parser
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
typedef struct parser_data
{   
    char *actual_function;
    Token *token;                           ///< actual token on input
    Token *token_list_first;                ///< first token in token list
    htab_t *global_symtable;                ///< table of symbols for functions
    htab_t *local_symtable;                 ///< table of symbols for local variables  
    precedence_token_t *expression_list;    ///< list of tokens for assignment/function call
    size_t tmp_counter;                     ///< counter of temporary variable generated
    data_token_t *param_list;               ///< pointer on loaded parameters at recursive descent
    data_token_t *return_list;              ///< pointer on return list of functions
    data_token_t *identif_list;             ///< pointer on list of identifiers loaded in recursive descent
    size_t while_counter;                   ///< counter, in which while block we are, 0 represent no while block
    size_t frame_counter;                   ///< unique identifier of statement block
    instruction_t *function_calls;          ///< list of instructions for function calls beetwen function definitions and declarations
    instruction_t *last_call;               ///< last instruction of function_calls
    instruction_t *before_while;            ///< pointer on instruction before while, for proper definition of vars in IFJcode21
    instruction_t *program;                 ///< first instruction of defined function
    instruction_t *last_instruction;        ///< last instruction of program
    int errno;                              ///< exit code
} parser_data_t;

/**
 * @brief Set the errno if was not before
 * 
 * @param data parser data structure
 * @param errno error number of error
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
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

/**
 * @brief free all memory allocated by parser
 * 
 * @param data pointer to data
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void free_parser_data(parser_data_t *data);

/**
 * @brief 
 * 
 * @param token 
 * @return int 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
int print_token(Token *token);

/**
 * @brief function for non-terminal expression in syntax analysis
 * 
 * @param data 
 * @return true 
 * @return false 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
bool expression(parser_data_t *data);

//TODO
int precedence(parser_data_t *data);


/**
 * @brief find variable in hierarchy of frames
 * 
 * @param table pointer on first table
 * @param key name of variable
 * @return finded item or NULL, when item was not found
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
htab_item *htab_find_variable(htab_t *table,char *key);

/**
 * @brief push back token from precedence analyze to recursive descent, on data->expression_list
 * 
 * @param data parser data
 * @param token pushed precedence token
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void push_precedence_token(parser_data_t *data, precedence_token_t *token);

/**
 * @brief push back data token to data->param_list
 * 
 * @param data parser data
 * @param token pushed data token
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void push_data_token(parser_data_t *data, data_token_t *token);

/**
 * @brief Allocate memory and copy the string
 * 
 * function exit program with error code INTERNAL_ERROR
 * 
 * @param data for clearing data, if instruction fails
 * @param str  copied string
 * @return char* new allocated string
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
char *strcpy_alloc(parser_data_t *data, const char *str);


bool fake_expression(parser_data_t *data); //TODO DELETE after precedence analysis is all done

/************************************************ IFJcode21_generating ***************************************/

/**
 * @brief create instruction for definition local variable in program
 * 
 * 
 * @param data parser data, where instructions are stored
 * @param key ALLOCATED memory with variable name (and frame description), do not free this memory, it will be done at the end of program by code generator
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void defvar_3AC(parser_data_t *data,char *key);

/**
 * @brief ALLOCATE and generate name for tmp variable
 * 
 * @param data parser data for generation
 * @param frame string, specifing frame, pass LF@,TF@,GF@ 
 * @return char* final name of variable in 3AC
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
char *allocate_new_tmp_name(parser_data_t *data,const char *frame);

/**
 * @brief push instruction on right place, depending if we are in definion function or not
 * 
 * @param data parser data
 * @param instruction pushed instruction
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void push_instruction(parser_data_t *data, instruction_t *instruction);

#endif

