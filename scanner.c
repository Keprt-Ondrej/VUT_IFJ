/**
 * @file scanner.c
 * @author Oleg Trofimov (xtrofi00@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#include "scanner.h"
#include "ErrLib.h"

#define size_of_length 11

int str_init(String *s) {
    s->string = (char *) malloc(size_of_length);
    if(NULL == s->string){
        return INTERNAL_ERROR;
    }
    s->current_index = 0;
    s->string[s->current_index] = '\0';
    s->alloc_size = size_of_length;
    return 0;
}

void str_free(String *s) {
   free(s->string);
}

// void str_clear(String *s) {
//     s->current_index = 0;
//     s->string[s->current_index] = '\0';
// }

String* extend_buffer(String *s) {
    int alloc_new_size = s->alloc_size + size_of_length;
    s->string = (char *) realloc(s->string, alloc_new_size);
        if(NULL == s->string)
            return NULL;
    s->alloc_size = alloc_new_size;    
    return 0;
}

int str_add_char(String *s, char c) {
	// if (s->current_index + 1 >= s->alloc_size) {
	// 	int alloc_new_size = s->current_index + size_of_length;
	// 	s->string = (char *) realloc(s->string, alloc_new_size);
    //     if(NULL == s->string)
    //        return INTERNAL_ERROR; 
	// 	s->alloc_size = alloc_new_size;
	// }
    if(s->current_index + 1 >= s->alloc_size) {
        extend_buffer(s);
    }
	s->string[s->current_index++] = c;
	s->string[s->current_index] = '\0';
	return 0;
}    

// char str_del_char(String *s) {
// 	if(s->current_index) {
//         return INTERNAL_ERROR;
//     }
//     else {
//         return 0;
//     }
        
// 	return s->string[s->current_index--]; // return deleted char
// }

Token* create_token() {
    Token *token = malloc(sizeof(Token));
    if (NULL == token)
        return NULL;
    return token;    
}

void add_str_to_token(String buffer, Token *token) {
    token->data.str = (char *) malloc(buffer.current_index);
    strcpy(token->data.str, buffer.string);
}

Token* read_token() {
    Token *token = create_token();
    String buffer;  
    str_init(&buffer);

    FSM_state state = start;
    char *endptr;
    char c;
    while(true) {
        switch(state) {
            case start: 
                c = getc(stdin);
                if(c == '\n') {
                    state = end_of_line;
                }
                else if(c >= '1' && c <= '9') {
                    str_add_char(&buffer, c);
                    state = int_number;
                }    
                else if(c == '0') {
                    str_add_char(&buffer, c); // везде исправить такую строку как в 91
                    state = zero;
                }
                else if((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'))) {
                    str_add_char(&buffer, c);
                    state = identif_and_kw;                    
                }
                else if(c == '/') {
                    state = div_st;                        
                }
                else if(c == '+') {
                    state = plus_st;
                }
                else if(c == '*') {
                    state = mul_st;
                }
                else if(c == ':') {
                    state = colon_st;
                }
                else if(c == ')') {
                    state = bracket_right_st;
                }
                else if(c == '(') {
                    state = bracket_left_st;
                }
                else if(c == ',') {
                    state = comma_st;
                }
                else if(c == '#') {
                    state = length_st;
                }
                else if(c == '~') {
                    state = ineq_begin_st;
                }
                else if(c == '=') {
                    state = assignment_st;
                }
                else if(isspace(c)) {
                    state = start; // Нужно ли что-то добавить в return?
                }
                else if(c == '<') {
                    state = less_st;
                }
                else if(c == '>') {
                    state = greater_st;
                }
                else if(c == '.') {
                    state = dot_st;
                }
                else if(c == '"') {
                    state = string_loop;
                }
            break;

            case end_of_line:
                token->type = token_type_EOF;
                return token;
            break;

            case int_number:
                c = getc(stdin);
                if(c >= '1' && c <= '9') {
                    str_add_char(&buffer, c);
                    state = int_number;
                }   
                else if(c == '.') {
                    str_add_char(&buffer, c);
                    state = decimal_point;
                }
                else if((c == 'e') || (c == 'E')){
                    str_add_char(&buffer, c);
                    state = double_exponent_begin;
                }
                else {
                    ungetc(c,stdin);
                    str_add_char(&buffer, '\0');
                    token->type = token_type_integer;
                    token->data.type_integer = atoi(buffer.string);
                    return token;                    
                }
            break;

            case zero:
                c = getc(stdin);
                if(c == '.') {
                    str_add_char(&buffer, c);
                    state = decimal_point;                             
                }
                else {
                    ungetc(c,stdin);
                    token->type = token_type_integer;
                    token->data.type_integer = 0;
                    return token;
                }
            break;

            case decimal_point:
                c = getc(stdin);
                if(c >= '0' && c <= '9') {
                    str_add_char(&buffer, c);
                    state = double_point_value;
                } 
                else {
                    return NULL;
                }
            break;    

            case double_point_value:
                c = getc(stdin);
                if(c >= '0' && c <= '9') {
                    str_add_char(&buffer, c);
                    state = double_point_value;
                }
                else if((c == 'e') || (c == 'E')) {
                    str_add_char(&buffer, c);
                    state = double_exponent_begin;
                }
                else {
                    ungetc(c, stdin);
                    str_add_char(&buffer, '\0');
                    token->type = token_type_number;
                    token->data.type_double = strtod(buffer.string, &endptr);      
                    return token;
                }
            break;

            case double_exponent_begin:
                c = getc(stdin);
                if((c == '+') || (c == '-')) {
                    str_add_char(&buffer, c);
                    state = double_exponent_sign;
                }
                else if(c >= '0' && c <= '9') {
                    str_add_char(&buffer, c);
                    state = double_exponent_value;
                }
                else {
                    return NULL;
                }
            break;

            case double_exponent_sign:
                c = getc(stdin);
                if(c >= '0' && c <= '9') {
                    str_add_char(&buffer, c);
                    state = double_exponent_value;
                }
                else {
                    return NULL;
                }
            break;

            case double_exponent_value:
                c = getc(stdin);
                if(c >= '0' && c <= '9') {
                    str_add_char(&buffer, c);
                    state = double_exponent_value;
                }
                else {
                    ungetc(c, stdin);
                    str_add_char(&buffer, '\0');
                    token->type = token_type_number;
                    token->data.type_double = strtod(buffer.string, &endptr);
                    return token;
                }
            break;

            case div_st:
                c = getc(stdin);
                if(c == '/') {
                    state = floor_div_st;                    
                }
                else {
                    ungetc(c,stdin);
                    token->type = token_type_div;
                    return token;
                }    
            break;

            case floor_div_st:
                token->type = token_type_floor_div;
                return token;
            break;

            case plus_st:
                token->type = token_type_plus;
                return token;   
            break;

            case mul_st:
                token->type = token_type_mul;
                return token;
            break;

            case colon_st:
                token->type = token_type_colon;
                return token;
            break;

            case bracket_right_st:
                token->type = token_type_right_bracket;
                return token;
            break;

            case bracket_left_st:
                token->type = token_type_left_bracket;
                return token;
            break;        

            case comma_st:
                token->type = token_type_comma;
                return token;
            break;

            case length_st:
                token->type = token_type_length;
                return token;
            break;

            case ineq_begin_st:
                c = getc(stdin);
                if(c == '=') {
                    state = ineq_end_st;
                }
                else {
                    return NULL;
                }
            break;

            case ineq_end_st:
                token->type = token_type_ineq;
                return token;
            break;    

            case assignment_st:
                c = getc(stdin);
                if(c == '=') {
                    state = equal_st;
                }
                else {
                    ungetc(c, stdin);
                    token->type = token_type_assign;
                    return token;
                }
            break;

            case equal_st:
                token->type = token_type_equal;
                return token;
            break;

            case less_st:
                c = getc(stdin);
                if(c == '=') {
                    state = less_eq_st;
                }
                else {
                    ungetc(c, stdin);
                    token->type = token_type_lth;
                    return token;
                }
            break;

            case less_eq_st:
                token->type = token_type_leq;
                return token;
            break;    
            
            case greater_st:
                c = getc(stdin);
                if(c == '=') {
                    state = greater_eq_st;
                }
                else {
                    ungetc(c, stdin);
                    token->type = token_type_gth;
                    return token;
                }
            break;

            case greater_eq_st:
                token->type = token_type_geq;
                return token;
            break;   

            case dot_st:
                c = getc(stdin);
                if(c == '.') {
                    state = concat_st;
                }         
                else {
                    return NULL;
                }
            break;

            case concat_st:
                token->type = token_type_concat;
                return token;
            break;    

            case string_loop:
                c = getc(stdin);
                if(c == '"') {
                    state = string_end;
                }      
                else if(c == '\\') {
                    state = escape_seq;
                }
                else if(c == EOF || c == '\n') {
                    printf("CHYBA STRING EOF!!!!");          // 1 chyba mozna upravovat
                }
                else if(c > 31) {                            // specifikace zadani
                    str_add_char(&buffer, c);
                    state = string_loop;
                }                 
            break;

            case string_end:
                add_str_to_token(buffer, token);
                token->type = token_type_string;
                return token;
            break;

            case escape_seq:
                c = getc(stdin);
                if(c == '"') {
                    str_add_char(&buffer, '"');
                    state = string_loop;
                }
                else if (c == '\\') {
                    str_add_char(&buffer, '\\');
                    state = string_loop;
                }
                else if (c == 't') {
                    str_add_char(&buffer, '\t');
                    state = string_loop;
                } 
                else if (c == 'n') {
                    str_add_char(&buffer, '\n');
                    state = string_loop;
                }
            break;

            default:
                return NULL;
            break;
        }
    }
    str_free(&buffer);
    return token;	
}
