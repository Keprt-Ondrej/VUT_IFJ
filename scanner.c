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

String* extend_buffer(String *s) {
    int alloc_new_size = s->alloc_size + size_of_length;
    s->string = (char *) realloc(s->string, alloc_new_size);
        if(NULL == s->string)
            return NULL;
    s->alloc_size = alloc_new_size;    
    return 0;
}

int str_add_char(String *s, char c) {
    if(s->current_index + 1 >= s->alloc_size) {
        extend_buffer(s);
    }
	s->string[s->current_index++] = c;
	s->string[s->current_index] = '\0';
	return 0;
}    

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
                if(c == EOF) {
                    state = end_of_file;
                }
                else if(c == '\n') {
                    state = start;
                }
                else if(c >= '1' && c <= '9') {
                    str_add_char(&buffer, c);
                    state = int_number;
                }    
                else if(c == '0') {
                    str_add_char(&buffer, c); 
                    state = zero;
                }
                else if ((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (c == '_')) {
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
                    state = start; 
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
            break;

            case end_of_file:
                token->type = token_type_EOF;
                return token;
            break;

            case int_number:
                c = getc(stdin);
                if(c >= '0' && c <= '9') {
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
            
            case identif_and_kw:
                c = getc(stdin);
                if (isdigit(c) || (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (c == '_')) {
                    str_add_char(&buffer, c);
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
                        add_str_to_token(buffer, token);    
                        token->type = token_type_identifier;
                        return token;
                    }
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
            
            case minus_st:                          //dopisu pro komentare
                token->type = token_type_minus;
                return token;
            break;

            default:
                return NULL;
            break;
        }
    }
    str_free(&buffer);
    return token;	
}
