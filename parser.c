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
        fprintf(stderr,"Lexical error\n");  //TODO comment 
        free_parser_data(data);
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
    if(data->local_symtable != NULL) htab_free(data->local_symtable);
    if(data->global_symtable != NULL) htab_free(data->global_symtable);
    free(data->actual_function);
    Token *walking_item = data->token_list_first;
    
    while(walking_item != NULL){
        Token *delete = walking_item;
        printf("%p\n",walking_item);
        if(walking_item == NULL){
            break;
        }
        walking_item = walking_item->next;
        free(delete);
    }
    
    //todo precedence free
    free_data_token_list(&(data->param_list));
    free_data_token_list(&(data->return_list));
    free_data_token_list(&(data->identif_list));
    while(data->program != NULL){
        instruction_t *delete = data->program;
        data->program = data->program->next;
        free_instruction(delete);
    }
    while(data->function_calls != NULL){
        instruction_t *delete = data->function_calls;
        data->function_calls = data->function_calls->next;
        free_instruction(delete);
    } 
    return;
}


htab_item *htab_find_variable(htab_t *table,char *key){
    return htab_find(table,key);
}

void push_precedence_token(parser_data_t *data, precedence_token_t *token){
    if (data->expression_list == NULL){
        data->expression_list = token;
        return;
    }
    precedence_token_t *walking_item = data->expression_list;
    while(walking_item->next != NULL){
        walking_item = walking_item->next;
    }
    walking_item->next = token;
}

void push_instruction(parser_data_t *data, instruction_t *instruction){
    if(data->actual_function == NULL){      //decide if we are processing function definition or we are calling function between definitions and declarations
        data->last_call->next = instruction;
        data->last_call = instruction;
    }
    else{
        data->last_instruction->next = instruction;
        data->last_instruction = instruction;
    }
}

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
    if(data->while_counter == 0){       //decide if insert before while or on last item
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
    size_t lenght = snprintf(NULL,0,"%s$_tmp_var_$%zu",frame,data->tmp_counter)+1;
    char *name = calloc(lenght,sizeof(char));
    if(name == NULL){
        free_parser_data(data);
        exit(INTERNAL_ERROR);
    }
    snprintf(name,lenght,"%s$_tmp_var_$%zu",frame,data->tmp_counter);
    data->tmp_counter++;    //new tmp var must have another serial number
    return name;
}

int print_token(Token *token){
    Token_data data_token = token->data;
    Token_type type = token->type;
    fprintf(stderr,"%3d: ", type);
    if(type == token_type_identifier || type == token_type_string)
        fprintf(stderr,"%s\n", data_token.str);
    else if(type == token_type_integer)
        fprintf(stderr,"%d\n", data_token.type_integer);
    else if(type == token_type_number)
        fprintf(stderr,"%f\n", data_token.type_double);
    else if(type == token_type_EOF)//if EOF
        return 0;
    else {
        if(token_type_length == type)                   fprintf(stderr,"#\n");
        if(token_type_mul == type)                      fprintf(stderr,"*\n");
        if(token_type_div == type)                      fprintf(stderr,"/\n");
        if(token_type_floor_div == type)                fprintf(stderr,"//\n");
        if(token_type_plus == type)                     fprintf(stderr,"+\n");
        if(token_type_minus == type)                    fprintf(stderr,"-\n");
        if(token_type_concat == type)                   fprintf(stderr,"..\n");
        if(token_type_lth == type)                      fprintf(stderr,"<\n");
        if(token_type_leq == type)                      fprintf(stderr,"<=\n");
        if(token_type_gth == type)                      fprintf(stderr,">\n");
        if(token_type_geq == type)                      fprintf(stderr,">=\n");
        if(token_type_equal == type)                    fprintf(stderr,"==\n");
        if(token_type_ineq == type)                     fprintf(stderr,"~=\n");
        if(token_type_assign == type)                   fprintf(stderr,"=\n");
        if(token_type_left_bracket == type)             fprintf(stderr,"(\n");
        if(token_type_right_bracket == type)            fprintf(stderr,")\n");
        if(token_type_square_left_bracket == type)      fprintf(stderr,"[\n");
        if(token_type_square_right_bracket == type)     fprintf(stderr,"]\n");
        if(token_type_colon == type)                    fprintf(stderr,":\n");
        if(token_type_comma == type)                    fprintf(stderr,",\n");
        if(token_type_$ == type)                        fprintf(stderr,"empty\n");
        if(token_type_E == type)                        fprintf(stderr,"E\n");
        if(token_type_shift == type)                    fprintf(stderr,"<\n");
    }
    return 1;
}


bool expression(parser_data_t *data){
    //return fake_expression(data);

    return precedence(data);
}