/**
 * @file parser.c
 * @brief 
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/

#include "parser.h"

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
    print_token(token);
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

void print_token(Token *token){
    Token_data data_token = token->data;
    Token_type type = token->type;
    printf("%3d: ", type);
    if(type == 16 || type == 19)
        printf("%s\n", data_token.str);
    else if(type == 17)
        printf("%d\n", data_token.type_integer);
    else if(type == 18)
        printf("%f\n", data_token.type_double);  
    else
        printf("%d\n", type);
}