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
	int current_index;     // lenght of string
	int alloc_size; // number of chars alocated for string
} String;

int str_init(String *s);

void str_free(String *s);

String* extend_buffer(String *s);

void str_clear(String *s);

int str_add_char(String *s, char c);

char str_del_char(String *s);

/**
 * @enum Type of token.
 */

typedef enum {

    // Operators
    token_type_length,                // (#)   length               21
    token_type_concat,                // (..)  concatenation        27

    token_type_mul,                   // (*)   multiplication       22
    token_type_div,                   // (/)   division             23
    token_type_floor_div,             // (//)  floor division       24
	token_type_plus,                  // (+)   addition             25
    token_type_minus,                 // (-)   subtraction          26   
    token_type_lth,                   // (<)   less than            28
    token_type_leq,                   // (<=)  less equal           29
    token_type_gth,                   // (>)   greater than         30
    token_type_geq,                   // (>=)  greater equal        31
    token_type_equal,                 // (==)  equal                32
    token_type_ineq,                  // (~=)  inequality           33    

    token_type_left_bracket,          // (     left bracket         35
    token_type_right_bracket,         //  )    right bracket        36


    token_type_identifier,            // Identifier     16    
	token_type_integer,               // Integer        17
	token_type_number,                // Number         18
	token_type_string,                // String         19
    token_type_nil,                   // Nil            20

    token_type_$,
    token_type_E,
    token_type_shift,                  //(<) shift in buffer



    token_type_assign,                // (=)   assignment           34
    token_type_square_left_bracket,   // ([)   square left bracket  37
    token_type_square_right_bracket,  // (])   square right bracket 38
    token_type_colon,                 // (:)   colon                39
    token_type_comma,                 // (,)   comma                40
    
    // Keywords
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

	token_type_EOF,                   // End of file    15
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
} Token_data;

/**
 * @struct Token
 */
typedef struct token Token;
struct token {
    Token_type type;
    Token_data data;
    Token *next;
};

/**
 * @enum States of Scanner finite state machine
 */
typedef enum {
    start,                          //0
    end_of_file,                    //1
    int_number,                     //2
    decimal_point,                  //3
    zero,                           //4
    double_point_value,             //5
    double_exponent_begin,          //6
    double_exponent_sign,           //7
    double_exponent_value,          //8                 
    minus_st,                       //9
    line_comment,                   //10
    block_comment_begin,            //11
    block_comment_loop,             //12
    block_comment_end,              //13
    identif_and_kw,                 //14
    div_st,                         //15
    floor_div_st,                   //16
    plus_st,                        //17
    mul_st,                         //18
    colon_st,                       //19
    bracket_right_st,               //20
    bracket_left_st,                //21
    comma_st,                       //22
    length_st,                      //23
    ineq_begin_st,                  //24  
    ineq_end_st,                    //25
    assignment_st,                  //26  
    equal_st,                       //27
    less_st,                        //28
    less_eq_st,                     //29
    greater_st,                     //30
    greater_eq_st,                  //31
    dot_st,                         //32
    concat_st,                      //33
    string_loop,                    //34
    string_end,                     //35
    escape_seq,                     //36
} FSM_state;

Token* create_token(); // Alocate memory for token

void add_str_to_token(String buffer, Token *token);

Token* read_token();

#endif // __SCANNER_H__
