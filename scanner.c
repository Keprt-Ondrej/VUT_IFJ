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
    token->next = NULL;
    return token;    
}

Token* read_token() {
    Token *token = create_token();
    String buffer;  
    str_init(&buffer);

    FSM_state state = start;
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
                    buffer.string[buffer.length++] = c;
                    state = div_st;                        
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
        }
    }
    token = NULL;    
    str_free(&buffer);
    return NULL;
}
