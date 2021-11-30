
#include "code_generator.h"
#include <stdio.h> 
#include <stdlib.h>

void free_instruction(instruction_t *ins){
    free(ins->op1);
    free(ins->op2);
    free(ins->op3);
    free(ins);
}


//add int@
char *int_to_string(int value){
    int length = snprintf(NULL,0,"%d",value) + 1 ;
    char * str = calloc(length,sizeof(char));
    if(str == NULL){
        //TODO ret val ERROR handel
    }
    snprintf(str,length, "%d",value);
    return str;
}


//TODO add float@ literal
char *double_to_string(double value){
    int length = snprintf(NULL,0,"%a",value) + 1 ;
    char * str = calloc(length,sizeof(char));
    if(str == NULL){
        //TODO ret val ERROR handel
    }
    snprintf(str,length, "%a",value);
    return str;
} 

//TODO create nil@nil

instruction_t *alloc_instruction(){
    instruction_t *inst = calloc(1,sizeof(instruction_t));
    if(inst == NULL){
        //TODO error handle
    }
    return inst;
}

void generate_code(instruction_t *inst){
    printf(".IFJcode21\n");
    //TODO vestavene funkce

    instruction_t *delete;
    while(inst != NULL){
        print_opcode(inst->opcode);
        if(inst->op1 != NULL){
            printf("%s\t",inst->op1);
        }
        if(inst->op2 != NULL){
            printf("%s\t",inst->op2);
        }
        if(inst->op3 != NULL){
            printf("%s\n",inst->op3);
        }
        delete = inst;
        inst = inst->next;
        free(delete);
    }
}

void print_opcode(OP_code_t opcode){
    //switch atd... nezpomenout na \t
}