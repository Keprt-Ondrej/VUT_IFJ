
#include "scanner.h"

int main(){
    //printf("Hello world\n");
    //scanner_hello();
    //symtable_hello();
    Token *token = read_token();
    Token_data data = token->data;
    Token_type type = token->type;

    while(token != NULL && type != 15) {
        printf("%3d: ", type);
        if(type == 16 || type == 19)
            printf("%s\n", data.str);
        else if(type == 17)
            printf("%d\n", data.type_integer);
        else if(type == 18)
            printf("%f\n", data.type_double);  
        else
            printf("%d\n", type);  
        token = read_token();
        if (token == NULL){
            return LEX_ERROR;
        }
        type = token->type;
        data = token->data;
    }
    if(type == 15)
        printf("%3d: %d\n", type, type);
    //wordcount();
    return 0;
}