/**
 * @file scanner.h
 * @author Oleg Trofimov (xtrofi00@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#ifndef __SCANNER_H__
#define __SCANNER_H__
#include "ErrLib.h"


/**
 * @struct Representation of dynamic string.
 */
typedef struct {
	char *string;   // string ended by '\0' byte
	int length;     // lenght of string
	int alloc_size; // number of chars alocated for string
} String;

int str_init(String *s);

void str_ree(String *s);

void str_clear(String *s);

int str_add_char(String *s, char c);

char str_del_char(String *s);


/**
 * @enum Reserved keywords.
 */

typedef enum {           
    kw_do,          //0
    kw_else,        //1     
    kw_end,         //2
    kw_function,    //3
    kw_global,      //4
    kw_if,          //5
    kw_integer,     //6
    kw_local,       //7
    kw_nil,         //8
    kw_number,      //9
    kw_require,     //10
    kw_return,      //11
    kw_string,      //12
    kw_then,        //13
    kw_while,       //14
} Keyword;

/**
 * @enum Type of token.
 */

typedef enum {
	token_type_EOF,                   // End of file
	token_type_EOL,                   // End of line
	token_type_identifier,            // Identifier
	token_type_keyword,               // Keyword
    
    // type of data
	token_type_integer,               // Integer
	token_type_number,                // Number
	token_type_string,                // String
    token_type_nil,                   // Nil

	// operators
    token_type_length,                // (#)   length 
    token_type_mul,                   // (*)   multiplication 
    token_type_div,                   // (/)   division 
    token_type_floor_div,             // (//)  floor division
	token_type_plus,                  // (+)   addition 
    token_type_minus,                 // (-)   subtraction      
    token_type_concat,                // (..)  concatenation   
    token_type_ltn,                   // (<)   less than 
    token_type_leq,                   // (<=)  less equal     
    token_type_mth,                   // (>)   more than       
    token_type_meq,                   // (>=)  more equal 
    token_type_equal,                 // (==)  equal 
    token_type_ineq,                  // (~=)  inequality 
    token_type_assign,                // (=)   assignment 
    token_type_left_bracket,          // (     left bracket
    token_type_right_bracket,         //  )    right bracket
    token_type_square_left_bracket,   // ([)   square left bracket
    token_type_square_right_bracket,  // (])   square right bracket
    token_type_colon,                 // (:)   colon
    token_type_comma,                 // (,)   comma
} Token_type;


/**
 * @union Data, which token contains
 * 
 * Data stored in token depends on his type
 */
typedef union {
    int type_integer;      
    double type_double;     
    char *str;
    Keyword key_word;
} Token_data;


typedef struct {
    Token_type type;
    Token_data data;
} Token;

/**
 * @enum States of Scanner finite state machine
 */
typedef enum {
    start,                          //0
    end_of_file,                    //1
    end_of_line,                    //2
    int_number,                     //3
    decimal_point,                  //4
    zero,                           //5
    double_point_value,             //6
    double_exponent_begin,          //7
    double_exponent_sign,           //8
    double_exponent_value,          //9                 
    minus_st,                       //10
    line_comment,                   //11
    block_comment_begin,            //12
    block_comment_loop,             //13
    block_comment_end,              //14
    block_comment_loop_with_EOL,    //15
    block_comment_end_with_EOL,     //16
    identif_and_kw,                 //17
    div_st,                         //18
    floor_div_st,                   //19
    plus_st,                        //20
    mul_st,                         //21
    colon_st,                       //22
    bracket_right_st,               //23
    bracket_left_st,                //24
    comma_st,                       //25
    length_st,                      //26
    ineq_begin_st,                  //27  
    ineq_end_st,                    //28
    assignment_st,                  //29  
    equal_st,                       //30
    less_st,                        //31
    less_eq_st,                     //32
    greater_st,                     //33
    greater_eq_st,                  //34
    dot_st,                         //35
    concat_st,                      //36
    string_loop,                    //37
    string_end,                     //38
    string_wrong_input,             //39
    escape_seq,                     //40
} FSM_state;

Token* create_token(); // Alocate memory for token

Token* read_token();

#endif // __SCANNER_H__
