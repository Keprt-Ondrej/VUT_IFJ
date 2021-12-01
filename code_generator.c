
#include "code_generator.h"
#include <stdio.h> 
#include <stdlib.h>

const char* nil_string = "nil@nil";
const char* bool_string_true = "bool@true";
const char* bool_string_false = "bool@false";

void free_instruction(instruction_t *ins){
    free(ins->op1);
    free(ins->op2);
    free(ins->op3);
    free(ins);
}

char *int_to_string(int value){
    int length = snprintf(NULL,0,"%d",value) + 1 ;
    char * str = calloc(length + 4,sizeof(char));
    if(str == NULL){
        //TODO ret val ERROR handel
    }
    snprintf(str,length, "int@%d",value);
    return str;
}

char *double_to_string(double value){
    int length = snprintf(NULL,0,"%a",value) + 1 ;
    char * str = calloc(length + 6,sizeof(char));
    if(str == NULL){
        //TODO ret val ERROR handel
    }
    snprintf(str,length, "float@%a",value);
    return str;
}

char *string_to_string(char *string){
    //TODO replace white spaces with escape seq etc..

    int lenght = strlen(string) + 8; //  1 + 7 is string@    1 is \0
    char * str = calloc(lenght,sizeof(char));
    if(str == NULL){
        //TODO ret val error handle
    }
    snprintf(str,lenght,"string@%s",string);
    return str;
}

instruction_t *create_instruction(OP_code_t opcode,char *op1,char *op2,char *op3){
    instruction_t *inst = calloc(1,sizeof(instruction_t));
    if(inst == NULL){
        //TODO error handle
    }
    inst->opcode = opcode;
    inst->op1 = op1;
    inst->op2 = op2;
    inst->op3 = op3;
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
        free_instruction(delete);
    }
}

void print_opcode(OP_code_t opcode){
    switch (opcode){
        case MOVE: printf("move\t"); break;
        case CREATEFRAME: print("createframe\t"); break;
        case PUSHFRAME: printf("pushframe\t"); break;
        case POPFRAME: printf("popframe\t"); break;
        case DEFVAR: printf("defvar\t"); break;
        case CALL: printf("call\t"); break;
        case RETURN: printf("return\t"); break;
        case PUSHS: printf("pushs\t"); break;
        case POPS: printf("pops\t"); break;
        case CLEARS: printf("clears\t"); break;
        case ADD: printf("add\t"); break;
        case SUB: printf("sub\t"); break;
        case MUL: printf("mul\t"); break;
        case DIV: printf("div\t"); break;
        case IDIV: printf("idiv\t"); break;
        case ADDS: printf("adds\t"); break;
        case SUBS: printf("subs\t"); break;
        case MULS: printf("muls\t"); break;
        case DIVS: printf("divs\t"); break;
        case IDIVS: printf("idivs\t"); break;
        case LT: printf("lt\t"); break;
        case GT: printf("gt\t"); break;
        case EQ: printf("eq\t"); break;
        case LTS: printf("lts\t"); break;
        case GTS: printf("gts\t"); break;
        case EQS: printf("eqs\t"); break;
        case AND: printf("and\t"); break;
        case OR: printf("or\t"); break;
        case NOT: printf("not\t"); break;
        case ANDS: printf("ands\t"); break;
        case ORS: printf("ors\t"); break;
        case NOTS: printf("nots\t"); break;
        case INT2FLOAT: printf("int2float\t"); break;
        case FLOAT2INT: printf("float2int"); break;
        case INT2CHAR: printf("int2char\t"); break;
        case STRI2INT: printf("stri2int\t"); break;
        case INT2FLOATS: printf("int2floats\t"); break;
        case FLOAT2INTS: printf("float2ints\t"); break;
        case INT2CHARS: printf("int2chars\t"); break;
        case STRI2INTS: printf("stri2ints\t"); break;        
        case READ: printf("read\t"); break;
        case WRITE: printf("write\t"); break;
        case CONCAT: printf("concat\t"); break;
        case STRLEN: printf("strlen\t"); break;
        case GETCHAR: printf("getchar\t"); break;
        case SETCHAR: printf("setchar\t"); break;
        case TYPE: printf("type\t"); break;
        case LABEL: printf("label\t"); break;
        case JUMP: printf("jump\t"); break;
        case JUMPIFEQ: printf("jumpifeq\t"); break;
        case JUMPIFNEQ: printf("jumpifneq\t"); break;    
        case JUMPIFEQS: printf("jumpifeqs\t"); break;
        case JUMPIFNEQS: printf("jumpifneq\t"); break;
        case EXIT: printf("exit\t"); break;
        case BREAK: printf("break\t"); break;
        case DPRINT: printf("dprint\t"); break;
    }
}