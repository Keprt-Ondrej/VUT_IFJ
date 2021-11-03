/**
 * @file parser.c
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#include "parser.h"

void get_token(parser_data_t *data){
    //Token *token = read_token();
    data->token->next = token;
    data->token = token;

}

bool intro(parser_data_t *data){

    return true;
}

void parser(){
    parser_data_t data = {NULL, NULL};
    data.token = read_token();
    data.token_list_first = data.token;
    data.global_symtable = htab_init(TABLE_SIZE);

    //syntax analysis
    if (!intro(&data)){
        fprintf(stderr,"Lexical error");
        return;
    }
}