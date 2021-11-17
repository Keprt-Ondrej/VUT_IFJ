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

    while(token != NULL && type != 15) {
        printf("%3d: ", type);
        if(type == 17 || type == 20)
            printf("%s\n", data.str);
        else if(type == 18)
            printf("%d\n", data.type_integer);
        else if(type == 19)
            printf("%f\n", data.type_double);  
        else
            printf("%d\n", type);  
        token = read_token();
        type = token->type;
        data = token->data;
    }
    if(type == 15)                        //aby printoval EOF
        printf("%3d: %d\n", type, type);
    //wordcount();

    //print git
    return 0;
}
