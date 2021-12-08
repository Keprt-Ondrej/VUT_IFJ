/**
 * @file scanner.c
 * @author Oleg Trofimov (xtrofi00@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#include "scanner.h"

#define size_of_length 100

bool str_init(String *s) {
    s->string = (char *) malloc(size_of_length);
    if(NULL == s->string){
        exit(INTERNAL_ERROR);
    }
    s->current_index = 0;
    s->string[s->current_index] = '\0';
    s->alloc_size = size_of_length;
    return 0;
}

void str_free(String *s) {
   free(s->string);
}

String* extend_buffer(String *s) {
    int alloc_new_size = s->alloc_size + size_of_length;
    s->string = (char *) realloc(s->string, alloc_new_size);
        if(NULL == s->string) {
            exit(INTERNAL_ERROR);
        }    
    s->alloc_size = alloc_new_size;    
    return 0;
}

bool str_add_char(String *s, char c) {
    if(s->current_index + 1 >= s->alloc_size) {
        if(NULL == extend_buffer(s)) {
            exit(INTERNAL_ERROR);
        }
    }
	s->string[s->current_index++] = c;
	s->string[s->current_index] = '\0';
    return true;
}    

Token* create_token() {
    Token *token = malloc(sizeof(Token));
    if (NULL == token) {
        exit(INTERNAL_ERROR);
    }    
    return token;    
}

bool add_str_to_token(String buffer, Token *token) {
    token->data.str = (char *) malloc(buffer.current_index);
    if(NULL == token->data.str) {
        exit(INTERNAL_ERROR);
    }
    strcpy(token->data.str, buffer.string);
    return true;
}

void delete_token(Token* token) {
    if ((token->type == token_type_identifier) || (token->type == token_type_string)) { 
        free(token->data.str);
    }
    free(token);
}

Token* read_token() {
    Token *token = create_token();
    String buffer;  
    str_init(&buffer);

    FSM_state state = start;
    char *endptr;
    char c;
    char a;
    char array[3];
    bool succes;
    while(true) {
        switch(state) {
            case start: 
                c = getc(stdin);
                if(c == EOF) {
                    state = end_of_file;
                }
                else if(c == '\n') {
                    state = start;
                }
                else if(isspace(c)) {
                    state = start; 
                }
                else if(c >= '1' && c <= '9') {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }                    
                    state = int_number;
                }    
                else if(c == '0') {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }
                    state = zero;
                }
                else if ((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (c == '_')) {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }                    
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
                else if(c == '-') {
                    state = minus_st;
                }
                else {
                    return NULL;
                }
            break;

            case end_of_file:
                token->type = token_type_EOF;
                return token;
            break;

            case int_number:
                c = getc(stdin);
                if(c >= '0' && c <= '9') {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }  
                    state = int_number;
                }   
                else if(c == '.') {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }  
                    state = decimal_point;
                }
                else if((c == 'e') || (c == 'E')){
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }  
                    state = double_exponent_begin;
                }
                else {
                    ungetc(c,stdin);
                    succes = str_add_char(&buffer, '\0');
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }  
                    token->type = token_type_integer;
                    token->data.type_integer = atoi(buffer.string);
                    return token;                    
                }
            break;

            case zero:
                c = getc(stdin);
                if(c == '.') {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }
                    state = decimal_point;                             
                }
                else {
                    ungetc(c,stdin);
                    token->type = token_type_integer;
                    token->data.type_integer = 0;
                    return token;
                }
            break;
            
            case identif_and_kw:
                c = getc(stdin);
                if (isdigit(c) || (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (c == '_')) {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }  
                    state = identif_and_kw;
                }
                else {
                    ungetc(c, stdin);
                    if (strcmp(buffer.string, "do") == 0) {
                        token->type = kw_do;
                        return token;
                    }
                    else if (strcmp(buffer.string, "else") == 0) {
                        token->type = kw_else;
                        return token;
                    }
                    else if (strcmp(buffer.string, "end") == 0) {
                        token->type = kw_end;
                        return token;
                    }
                    else if (strcmp(buffer.string, "function") == 0) {
                        token->type = kw_function;
                        return token;
                    }
                    else if (strcmp(buffer.string, "global") == 0) {
                        token->type = kw_global;
                        return token;
                    }
                    else if (strcmp(buffer.string, "if") == 0) {
                        token->type = kw_if;
                        return token;
                    }
                    else if (strcmp(buffer.string, "integer") == 0) {
                        token->type = kw_integer;
                        return token;
                    }
                    else if (strcmp(buffer.string, "local") == 0) {
                        token->type = kw_local;
                        return token;
                    }
                    else if (strcmp(buffer.string, "nil") == 0) {
                        token->type = kw_nil;
                        return token;
                    }
                    else if (strcmp(buffer.string, "number") == 0) {
                        token->type = kw_number;
                        return token;
                    }
                    else if (strcmp(buffer.string, "require") == 0) {
                        token->type = kw_require;
                        return token;
                    }
                    else if (strcmp(buffer.string, "return") == 0) {
                        token->type = kw_return;
                        return token;
                    }
                    else if (strcmp(buffer.string, "string") == 0) {
                        token->type = kw_string;
                        return token;
                    }
                    else if (strcmp(buffer.string, "then") == 0) {
                        token->type = kw_then;
                        return token;
                    }
                    else if (strcmp(buffer.string, "while") == 0) {
                        token->type = kw_while;
                        return token;
                    }
                    else {
                        token->type = token_type_identifier;
                        succes = add_str_to_token(buffer, token);    
                        if (!succes){
                            delete_token(token);
                            return NULL;
                        }  
                        return token;
                    }
                }
            break;

            case decimal_point:
                c = getc(stdin);
                if(c >= '0' && c <= '9') {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }
                    state = double_point_value;
                } 
                else {
                    delete_token(token);
                    return NULL;
                }
            break;    

            case double_point_value:
                c = getc(stdin);
                if(c >= '0' && c <= '9') {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }
                    state = double_point_value;
                }
                else if((c == 'e') || (c == 'E')) {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }
                    state = double_exponent_begin;
                }
                else {
                    ungetc(c, stdin);
                    succes = str_add_char(&buffer, '\0');
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }
                    token->type = token_type_number;
                    token->data.type_double = strtod(buffer.string, &endptr);      
                    return token;
                }
            break;

            case double_exponent_begin:
                c = getc(stdin);
                if((c == '+') || (c == '-')) {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }
                    state = double_exponent_sign;
                }
                else if(c >= '0' && c <= '9') {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }
                    state = double_exponent_value;
                }
                else {
                    delete_token(token);
                    return NULL;
                }
            break;

            case double_exponent_sign:
                c = getc(stdin);
                if(c >= '0' && c <= '9') {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }
                    state = double_exponent_value;
                }
                else {
                    delete_token(token);
                    return NULL;
                }
            break;

            case double_exponent_value:
                c = getc(stdin);
                if(c >= '0' && c <= '9') {
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }
                    state = double_exponent_value;
                }
                else {
                    ungetc(c, stdin);
                    succes = str_add_char(&buffer, '\0');
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }
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
                    delete_token(token);
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
                    delete_token(token);
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
                else if(c == 32) {
                    a = 32;
                    state = escape_seq_end;
                }                
                else if(c == 35) {
                    a = 35;
                    state = escape_seq_end;
                }
                else if(c == EOF || c == '\n') {
                    delete_token(token);     
                    return NULL;
                }
                else if(c > 31) {                            // specifikace zadani
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }
                    state = string_loop;    
                }        
            break;

            case string_end:
                succes = add_str_to_token(buffer, token);
                if (!succes) {
                    delete_token(token);
                    return NULL;
                }
                token->type = token_type_string;
                return token;
            break;

            case escape_seq:
                c = getc(stdin);               
                if(c == '"') {
                    succes = str_add_char(&buffer, '"');
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }
                    state = string_loop;
                }
                else if (c == '0') {
                    array[0] = c;
                    state = escape_seq_zero;
                }
                else if (c == '1') {
                    array[0] = c;
                    state = escape_seq_one;
                }
                else if (c == '2') {
                    array[0] = c;
                    state = escape_seq_two;
                }
                else if (c == '\\') {
                    a = '\\';
                    state = escape_seq_end;
                }
                else if (c == 't') {
                    a = '\t';
                    state = escape_seq_end;
                } 
                else if (c == 'n') {
                    a = '\n';
                    state = escape_seq_end;    
                }
                else {
                    delete_token(token);
                    return NULL;   
                }
            break;

            case escape_seq_end:   
                if(a == 9) { // a = '\t'
                    succes = str_add_char(&buffer, '\\'); 
                    succes = str_add_char(&buffer, '0'); 
                    succes = str_add_char(&buffer, '0'); 
                    succes = str_add_char(&buffer, '9'); 
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }  
                    state = string_loop;                                                                             
                }
                else if(a == 10) { // a = '\n'
                    succes = str_add_char(&buffer, '\\'); 
                    succes = str_add_char(&buffer, '0'); 
                    succes = str_add_char(&buffer, '1'); 
                    succes = str_add_char(&buffer, '0'); 
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }            
                    state = string_loop;                                                                   
                }
                else if(a == 32) { // a = ' '
                    succes = str_add_char(&buffer, '\\'); 
                    succes = str_add_char(&buffer, '0'); 
                    succes = str_add_char(&buffer, '3'); 
                    succes = str_add_char(&buffer, '2');     
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }               
                    state = string_loop;                      
                }          
                else if(a == 35) { // a = '#'
                    succes = str_add_char(&buffer, '\\'); 
                    succes = str_add_char(&buffer, '0'); 
                    succes = str_add_char(&buffer, '3'); 
                    succes = str_add_char(&buffer, '5');     
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }               
                    state = string_loop;                      
                }                      
                else if(a == 92) { // a = '\\'
                    succes = str_add_char(&buffer, '\\'); 
                    succes = str_add_char(&buffer, '0'); 
                    succes = str_add_char(&buffer, '9'); 
                    succes = str_add_char(&buffer, '2');     
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }               
                    state = string_loop;                                                         
                }
                else {
                    delete_token(token);
                    return NULL;
                }
            break;

            case escape_seq_zero:
                c = getc(stdin);
                if(c == '0') {
                    array[1] = c;
                    state = escape_seq_zero_zero;
                }

                else if(isdigit(c) && c != '0') {
                    array[1] = c;
                    state = escape_seq_num;
                }
                else {
                    delete_token(token);
                    return NULL;
                }
            break;

            case escape_seq_zero_zero:
                c = getc(stdin);
                if(isdigit(c) && c != '0') {
                    array[2] = c;
                    int tmp = (int)strtol(array, &endptr, 10);
                    c = (char) tmp;
                    succes = str_add_char(&buffer, c);
                    if (!succes) {
                        delete_token(token);
                        return NULL;
                    }      
                    state = string_loop;
                }
                else {
                    delete_token(token);
                    return NULL;
                }
            break;

            case escape_seq_one:
                c = getc(stdin);
                if(isdigit(c)) {
                    array[1] = c;
                    state = escape_seq_num;
                }
                else {
                    delete_token(token);
                    return NULL;
                }                
            break;

            case escape_seq_two:
                if(c == '0' || c == '1' || c == '2' || c == '3' || c == '4') {
                    array[1] = c;
                    state = escape_seq_num;
                }
                else if(c == '5') {
                    array[1] = c;
                    state = escape_seq_two_five;
                }
                else {
                    delete_token(token);
                    return NULL;
                }
            break;

            case escape_seq_two_five:
                if(c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5') {
                    array[2] = c;
                    state = escape_seq_num;
                }    
                else {
                    delete_token(token);
                    return NULL;
                }
            break;

            case escape_seq_num:
                c = getc(stdin);
                if(isdigit(c)) {
                    array[2] = c;
                    int tmp = (int)strtol(array, &endptr, 10);
                    c = (char) tmp;
                    if(c == 9) { // c = '\t'
                        succes = str_add_char(&buffer, '\\'); 
                        succes = str_add_char(&buffer, '0'); 
                        succes = str_add_char(&buffer, '0'); 
                        succes = str_add_char(&buffer, '9'); 
                        if (!succes) {
                            delete_token(token);
                            return NULL;
                        }  
                        state = string_loop;                                                                             
                    }
                    else if(c == 10) { // a = '\n'
                        succes = str_add_char(&buffer, '\\'); 
                        succes = str_add_char(&buffer, '0'); 
                        succes = str_add_char(&buffer, '1'); 
                        succes = str_add_char(&buffer, '0'); 
                        if (!succes) {
                            delete_token(token);
                            return NULL;
                        }            
                        state = string_loop;                                                                   
                    }
                    else if(c == 32) { // c = ' '
                        succes = str_add_char(&buffer, '\\'); 
                        succes = str_add_char(&buffer, '0'); 
                        succes = str_add_char(&buffer, '3'); 
                        succes = str_add_char(&buffer, '2');     
                        if (!succes) {
                            delete_token(token);
                            return NULL;
                        }               
                        state = string_loop;                      
                    }          
                    else if(c == 35) { // a = '#'
                        succes = str_add_char(&buffer, '\\'); 
                        succes = str_add_char(&buffer, '0'); 
                        succes = str_add_char(&buffer, '3'); 
                        succes = str_add_char(&buffer, '5');     
                        if (!succes) {
                            delete_token(token);
                            return NULL;
                        }               
                        state = string_loop;                      
                    }                      
                    else if(c == 92) { // a = '\\'
                        succes = str_add_char(&buffer, '\\'); 
                        succes = str_add_char(&buffer, '0'); 
                        succes = str_add_char(&buffer, '9'); 
                        succes = str_add_char(&buffer, '2');     
                        if (!succes) {
                            delete_token(token);
                            return NULL;
                        }           
                        state = string_loop;   
                    }           
                    else {
                        succes = str_add_char(&buffer, c);  
                        if (!succes) {
                            delete_token(token);
                            return NULL; 
                        }
                        state = string_loop;
                    }
                }    
                else {
                    delete_token(token);
                    return NULL;
                }
            break;

            case minus_st:  
                c = getc(stdin);
                if(c == '-') {
                    state = line_comment;
                }
                else {
                    ungetc(c,stdin);
                    token->type = token_type_minus;
                    return token;
                }    
            break;

            case line_comment:
                c = getc(stdin);
                if(c == '[') {
                    state = block_comment_begin;
                }
                else if (c == '\n'){
                    state = start;
                }
                else if (c == EOF) {
                    state = end_of_file;
                }
                else {
                    state = line_comment;
                }
            break;

            case block_comment_begin:
                c = getc(stdin);
                if(c == '[') {
                    state = block_comment_loop;
                }
                else {
                    state = line_comment;
                }
            break;

            case block_comment_loop:
                c = getc(stdin);
                if(c == ']') {
                    state = block_comment_end;
                }
                else if (c == EOF && c != ']') {
                    state = end_of_file;        
                }
                else {
                    state = block_comment_loop;
                }    
            break;

            case block_comment_end:
                c = getc(stdin);
                if(c == ']') {
                    state = start;
                }
                else if (c == EOF && c != ']') {
                    state = end_of_file;        
                }                
                else {
                    state = end_of_file;       
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
