/**
 * @file parser.c
 * @brief 
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/

#include "parser.h"
#include "code_generator.h"

void set_errno(parser_data_t *data,int errno){
    if(data->errno == 0){
        data->errno = errno;
    }
}

void get_token(parser_data_t *data){
    Token *token = read_token();
    if(token == NULL){
        fprintf(stderr,"Lexical error\n");
        exit(LEX_ERROR);
    }    
    //print_token(token);
    data->token->next = token;
    data->token = token;
}

bool is_token(parser_data_t *data,Token_type type){
    if (data->token->type == type){
        return true;
    }
   return false;
}

void free_parser_data(parser_data_t *data){
    //TODO free
    htab_free(data->global_symtable);
    return;
}


htab_item *htab_find_variable(htab_t *table,char *key){
    return htab_find(table,key);
}

void push_precedence_token(parser_data_t *data, precedence_token_t *token){
    if (data->expression_list == NULL){
        data->expression_list = token;
    }
    precedence_token_t *walking_item = data->expression_list;
    while(walking_item->next != NULL){
        walking_item = walking_item->next;
    }
    walking_item->next = token;
}

void push_instruction(parser_data_t *data, instruction_t *instruction){
    if(data->actual_function == NULL){
        data->last_call->next = instruction;
        data->last_call = instruction;
    }
    else{
        data->last_instruction->next = instruction;
        data->last_instruction = instruction;
    }
}

//TODO switcher
void push_data_token(parser_data_t *data, data_token_t *token){
    if (data->param_list == NULL){
        data->param_list = token;
    }
    data_token_t *walking_item = data->param_list;
    while(walking_item->next != NULL){
        walking_item = walking_item->next;
    }
    walking_item->next = token;
}

void defvar_3AC(parser_data_t *data,char *key){
    instruction_t *instruction = create_instruction(DEFVAR,key,NULL,NULL);
    if(data->while_counter == 0){
        push_instruction(data,instruction);
    }
    else{
        instruction->next = data->before_while->next;
        data->before_while->next = instruction;
    }
}

char *strcpy_alloc(parser_data_t *data, const char *str){
    char *string = calloc(strlen(str) + 1,sizeof(char));
    if(string == NULL){
        free_parser_data(data);
        exit(INTERNAL_ERROR);
    }
    strcpy(string,str);
    return string;
}

char *allocate_new_tmp_name(parser_data_t *data,const char *frame){
    size_t lenght = snprintf(NULL,0,"%s$_tmp_var_$%zu",frame,data->tmp_counter);
    char *name = calloc(lenght,sizeof(char));
    if(name == NULL){
        free_parser_data(data);
        exit(INTERNAL_ERROR);
    }
    snprintf(name,lenght,"%s$_tmp_var_$%zu",frame,data->tmp_counter);
    data->tmp_counter++;
    return name;
}

int print_token(Token *token){
    Token_data data_token = token->data;
    Token_type type = token->type;
    printf("%3d: ", type);
    if(type == token_type_identifier || type == token_type_string)
        printf("%s\n", data_token.str);
    else if(type == token_type_integer)
        printf("%d\n", data_token.type_integer);
    else if(type == token_type_number)
        printf("%f\n", data_token.type_double);
    else if(type == token_type_EOF)//if EOF
        return 0;
    else {
        if(token_type_length == type)                   printf("#\n");
        if(token_type_mul == type)                      printf("*\n");
        if(token_type_div == type)                      printf("/\n");
        if(token_type_floor_div == type)                printf("//\n");
        if(token_type_plus == type)                     printf("+\n");
        if(token_type_minus == type)                    printf("-\n");
        if(token_type_concat == type)                   printf("..\n");
        if(token_type_lth == type)                      printf("<\n");
        if(token_type_leq == type)                      printf("<=\n");
        if(token_type_gth == type)                      printf(">\n");
        if(token_type_geq == type)                      printf(">=\n");
        if(token_type_equal == type)                    printf("==\n");
        if(token_type_ineq == type)                     printf("~=\n");
        if(token_type_assign == type)                   printf("=\n");
        if(token_type_left_bracket == type)             printf("(\n");
        if(token_type_right_bracket == type)            printf(")\n");
        if(token_type_square_left_bracket == type)      printf("[\n");
        if(token_type_square_right_bracket == type)     printf("]\n");
        if(token_type_colon == type)                    printf(":\n");
        if(token_type_comma == type)                    printf(",\n");
        if(token_type_$ == type)                        printf("empty\n");
        if(token_type_E == type)                        printf("E\n");
        if(token_type_shift == type)                    printf("<\n");
    }
    return 1;
}

bool expression(parser_data_t *data){
    //return fake_expression(data);

    return precedence(data);
}