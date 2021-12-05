/** 
 * @file scanner.h
 *
 * @brief Scanner 
 * 
 * Functions needed for Lexical analysis
 *
 * @author Ondřej Keprt <xkeprt03@stud.fit.vutbr.cz>
 */

#ifndef SCANNER_H
#define SCANNER_H
#include "cLibs.h"

/**
 * @brief Initialization size of reading buffer
 * @warning Always should be greater than 10, for correct deallocation
 */
#define reading_buffer_init_size 50 

/**
 * @brief Pointer on reading buffer
 * @warning Must be initialized before using by init_reading_buffer()
 */ 
char *reading_buffer;               
int size_of_reading_buffer;   /**< Actual size of reading buffer */

/**
 * @brief Allocate memory for reading buffer
 * @return False if request fails, true when memory was allocated 
 */
bool init_reading_buffer();

/**
 * @brief Store character to reading buffer
 * 
 * If reading buffer is full, it try to extend it by small amount
 * @param index in reading buffer, where should be character stored
 * @param character
 * @return false if request fails, true when character was added
 */ 
bool add_char_to_reading_buffer(int char_cnt,char c);

/**
 * @brief Free memory, where was reading buffer
 */
void delete_reading_buffer();

/**
 * @enum Token types
 * @warning Don't change order!!!! print_token() would be destroyed
 */
typedef enum {          
    key_word,   //0
    int64_value,    //1
    double_value,   //2
    identif,    //3
    EoF,    //4
    EoL,    //5
    division,   //6
    string,     //7
    curly_bracket_close,    //8
    curly_bracket_open,     //9
    bracket_open,   //10
    bracket_close,  //11
    semicolon,  //12
    comma,  //13
    times,  //14
    plus,   //15
    minus,  //16
    init,   //17
    less,   //18
    less_eq,    //19
    greater,    //20
    greater_eq, //21
    not_eq,     //22
    assignment, //23
    eq,     //24
} token_type;

/**
 * @enum Keywords
 * @warning don't change order of ENUM!!!! print_token() would be destroyed
 * @see print_token()
 * 
 * If token includes key word, this defines which one
 */ 
typedef enum{           
    kw_else,    //0
    kw_float64, //1
    kw_for,     //2
    kw_func,     //3
    kw_if,      //4
    kw_int,     //5
    kw_package, //6
    kw_return,  //7
    kw_string,   //8
} key_word_value;

/**
 * @union Data, which token contains
 * 
 * Data stored in token depends on his type
 */
typedef union{
    int64_t int_value;      
    double double_value;     
    char *str;                  /**<Pointer to memory, where is stored readed string or identificator name */
    key_word_value key_word;
} token_data;

/**
 * @struct Token
 */
typedef struct{
    token_type type;
    token_data data;
} LA_token;

/**
 * @enum States of Scanner finite state machine
 */
typedef enum{
    start,
    end_of_file,
    int64,
    decimal_point,
    zero,
    double_point_value,
    double_exponent_begin,
    double_exponent_sign,
    double_exponent_value,
    end_of_line,
    slash,
    line_comment,
    block_comment_loop,
    block_comment_end,
    block_comment_end_with_EOL,
    block_comment_loop_with_EOL,
    identif_and_kw,
    curly_bracket_close_st,
    curly_bracket_open_st,
    bracket_open_st,
    bracket_close_st,
    semicolon_st,
    comma_st,
    times_st,
    plus_st,
    minus_st,
    init_begin,
    init_end,
    less_st,
    less_eq_st,
    greater_st,
    greater_eq_st,
    not_eq_begin,
    not_eq_end,
    eq_st,
    assignment_st,
    string_loop,
    string_end,
    string_wrong_input,
    escape_seq_begin,
    escape_seq_x,
    escape_seq_x_number,    
} FSM_state;

/**
 * @brief Allocate memory for token 
 * @return return pointer on token, NULL if request fails
 */
LA_token* create_token();

/**
 * @brief Free memory of token
 * 
 * If token was identificator or string it also free this memory
 * @param pointer on token which should be freed 
 */
void delete_token(LA_token* token_ptr);

/**
 * @brief Store loaded string or identicator to token memory
 * @param pointer on token, where should be data stored
 * @return true if string was stored, false when request fails
 */
bool store_identif_or_string_to_token(LA_token* token_ptr);

/**
 * @brief convert hexadecimal number to decimal
 * @param sixteens of converted number
 * @param units of converted number
 * @return decimal number converted from params
 */
int from_hex_to_decimal(char sixteen,char zero);

/**
 * @brief Send token with readed data
 * 
 * Function calls create_token
 * @return pointer on token with data, NULL if request fails (and set exit_code on LEX_ERROR)
 */
LA_token* get_token();
#endif

/*** End of file scanner.h ***/
