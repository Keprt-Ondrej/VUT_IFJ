/**
 * @file main.c
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#include "main.h"
#include "symtable.h"

int main(){
    //printf("Hello world\n");
    //scanner_hello();
    //symtable_hello();
    Token *token = read_token();
    Token_data data = token->data;
    Token_type type = token->type;

    while(token != NULL) {
        printf("%d :", type);
        if(type == 6 || type == 2)
            printf("%s\n", data.str);
        else if(type == 4)
            printf("%d\n", data.type_integer);
        else if(type == 5)
            printf("%f\n", data.type_double);  
        else
            printf("%d\n", data.key_word);  
        token = read_token();
        type = token->type;
        data = token->data;
    }
    //wordcount();
    return 0;
}
