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
	char *str;      // string ened by '\0' byte
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

typedef enum{           
    kw_and,         //0 
    kw_break,       //1 
    kw_do,          //2 
    kw_else,        //3 
    kw_elseif,      //4 
    kw_end,         //5
    kw_false,       //6
    kw_for,         //7
    kw_function,    //8
    kw_goto,        //9
    kw_if,          //10
    kw_in,          //11
    kw_local,       //12
    kw_nil,         //13
    kw_not,         //14
    kw_or,          //15
    kw_repeat,      //16
    kw_return,      //17
    kw_then,        //18
    kw_true,        //19
    kw_until,       //20
    kw_while,       //21
} Keyword;

/**
 * @enum Type of token.
 */

typedef enum
{
	token_type_EOF,                   // End of file
	token_type_EOL,                   // End of line
	token_type_empty,                 // Empty
	token_type_identifier,            // Identifier
	token_type_keyword,               // Keyword

	token_type_int_num,               // Integer number
	token_type_double_num,            // Double number
	token_type_string,                // String

	// operators
	token_type_plus,                  // (+)   addition 
    token_type_minus,                 // (-)   subtraction 
    token_type_mul,                   // (*)   multiplication 
    token_type_div,                   // (/)   division 
    token_type_mod,                   // (%)   modulo 
    token_type_exp,                   // (^)   exponentiation 
    token_type_length,                // (#)   length 
    token_type_bit_and,               // (&)   bitwise AND 
    token_type_bit_not,               // (~)   bitwise NOT 
    token_type_bit_or,                // (|)   bitwise OR 
    token_type_bit_left_shift,        // (<<)  bitwise left shift 
    token_type_bit_right_shift,       // (>>)  bitwise right shift 
    token_type_floor_div,             // (//)  floor division 

    token_type_equal,                 // (==)  equal 
    token_type_ineq,                  // (~=)  inequality 
    token_type_leq,                   // (<=)  less equal 
    token_type_meq,                   // (>=)  more equal 
    token_type_ltn,                   // (<)   less than 
    token_type_mth,                   // (>)   more than 
    token_type_assign,                // (=)   assignment 

    token_type_left_bracket,          // (     left bracket
    token_type_right_bracket,         //  )    right bracket
    token_type_curly_left_bracket,    // ({)   curly left bracket
    token_type_curly_right_bracket,   // (})   curly right bracket
    token_type_square_left_bracket,   // ([)   square left bracket
    token_type_square_right_bracket,  // (])   square right bracket
    token_type_double_colon,          // (::)  double colon
    token_type_semicolon,             // (;)   semicolon
    token_type_colon,                 // (:)   colon
    token_type_comma,                 // (,)   comma
    token_type_dot,                   // (.)   dot
    token_type_concat,                // (..)  concatenation 
    token_type_empty_var              // (...) empty variable at the end of the parameter list
} Token_type;


#endif
