/**
 * @file scanner.c
 * @author Oleg Trofimov (xtrofi00@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#include "scanner.h"
#include "ErrLib.h"

#define size_of_length 10

int str_init(String *s) {
    s->string = (char *) malloc(size_of_length);
    if(NULL == s->string){
        return INTERNAL_ERROR;
    }
    s->length = 0;
    s->string[s->length] = '\0';
    s->alloc_size = size_of_length;
    return 0;
}

void str_free(String *s) {
   free(s->string);
}

void str_clear(String *s) {
    s->length = 0;
    s->string[s->length] = '\0';
}

int str_add_char(String *s, char c) {
	if (s->length + 1 >= s->alloc_size) {
		int alloc_new_size = s->length + size_of_length;
		s->string = (char *) realloc(s->string, alloc_new_size);
        if(NULL == s->string)
           return INTERNAL_ERROR; 
		s->alloc_size = alloc_new_size;
	}
	s->string[s->length] = c;
	s->string[s->length++] = '\0';
	return 0;
}    

char str_del_char(String *s) {
	if(s->length) {
        return INTERNAL_ERROR;
    }
    else {
        return 0;
    }
        
	return s->string[s->length--]; // return deleted char
}

Token* create_token() {
    Token *token = malloc(sizeof(Token));
    if (NULL == token)
        return NULL;
    return token;    
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
                    buffer.string[buffer.length++] = c; 
                    state = int_number;
                }    
                else if(c == '0') {
                    buffer.string[buffer.length++] = c;
                    state = zero;
                }
                else if((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'))) {
                    buffer.string[buffer.length++] = c;
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
            break;

            case end_of_line:
                token->type = token_type_EOF;
                return token;
            break;

            case int_number:
                c = getc(stdin);
                if(c >= '1' && c <= '9') {
                    buffer.string[buffer.length++] = c;
                    state = int_number;
                }   
                else if(c == '.') {
                    buffer.string[buffer.length++] = c;
                    state = decimal_point;
                }
                else if ((c == 'e') || (c == 'E')){
                    buffer.string[buffer.length++] = c;
                    state = double_exponent_begin;
                }
                else {
                    ungetc(c,stdin);
                    buffer.string[buffer.length] = '\0';
                    token->type = token_type_integer;
                    token->data.type_integer = atoi(buffer.string);
                    return token;                    
                }
            break;

            case zero:
                c = getc(stdin);
                if(c == '.') {
                    buffer.string[buffer.length++] = c;
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
                    buffer.string[buffer.length++] = c;
                    state = double_point_value;
                } 
                else {
                    return NULL;
                }
            break;    

            case double_point_value:
                c = getc(stdin);
                if(c >= '0' && c <= '9') {
                    buffer.string[buffer.length++] = c;
                    state = double_point_value;
                }
                else if((c == 'e') || (c == 'E')) {
                    buffer.string[buffer.length++] = c;
                    state = double_exponent_begin;
                }
                else {
                    ungetc(c, stdin);
                    buffer.string[buffer.length++] = c;
                    token->type = token_type_number;
                    token->data.type_double = strtod(buffer.string, &endptr);      
                    return token;              
                }
            break;

            case double_exponent_begin:
                c = getc(stdin);
                if((c == '+') || (c == '-')) {
                    buffer.string[buffer.length++] = c;
                    state = double_exponent_sign;
                }
                else if(c >= '0' && c <= '9') {
                    buffer.string[buffer.length++] = c;
                    state = double_exponent_value;
                }
                else {
                    return NULL;
                }
            break;

            case double_exponent_sign:
                c = getc(stdin);
                if(c >= '0' && c <= '9') {
                    buffer.string[buffer.length++] = c;
                    state = double_exponent_value;
                }
                else {
                    return NULL;
                }
            break;

            case double_exponent_value:
                c = getc(stdin);
                if(c >= '0' && c <= '9') {
                    buffer.string[buffer.length++] = c;
                    state = double_exponent_value;
                }
                else {
                    ungetc(c, stdin);
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
            default:
                printf("NICE!");
            break;
        }
    }
    token = NULL;    
    str_free(&buffer);
    return NULL;
}
