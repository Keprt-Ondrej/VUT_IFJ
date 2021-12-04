
#include "code_generator.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

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
    int length = snprintf(NULL,0,"int@%d",value) + 1 ;
    char * str = calloc(length,sizeof(char));
    if(str == NULL){
        exit(INTERNAL_ERROR);
    }
    snprintf(str,length, "int@%d",value);
    return str;
}

char *double_to_string(double value){
    int length = snprintf(NULL,0,"float@%a",value) + 1 ;
    char * str = calloc(length,sizeof(char));
    if(str == NULL){
        exit(INTERNAL_ERROR);
    }
    snprintf(str,length, "float@%a",value);
    return str;
}

char *string_to_string(char *string){
    //TODO replace white spaces with escape seq etc..

    int lenght = strlen(string) + 8; //  1 + 7 is string@    1 is \0
    char * str = calloc(lenght,sizeof(char));
    if(str == NULL){
        exit(INTERNAL_ERROR);
    }
    snprintf(str,lenght,"string@%s",string);
    return str;
}

instruction_t *create_instruction(OP_code_t opcode,char *op1,char *op2,char *op3){
    instruction_t *inst = calloc(1,sizeof(instruction_t));
    if(inst == NULL){
        exit(INTERNAL_ERROR);
    }
    inst->opcode = opcode;
    inst->op1 = op1;
    inst->op2 = op2;
    inst->op3 = op3;
    inst->next = NULL;
    return inst;
}

void generate_code(instruction_t *inst){

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
            printf("%s",inst->op3);
        }
        printf("\n");
        delete = inst;
        inst = inst->next;
        free_instruction(delete);
    }
}

char *allocate_var_name_3AC(const char* frame,htab_item *item){
    size_t lenght = snprintf(NULL,0,"%s%s_$%zu",frame,item->key,item->frame_ID) + 1;
    char *var = calloc(lenght,sizeof(char));
    if(var == NULL){
        exit(INTERNAL_ERROR);
    }
    snprintf(var,lenght,"%s%s_$%zu",frame,item->key,item->frame_ID);
    return var;
}   

void print_opcode(OP_code_t opcode){
    switch (opcode){
        case MOVE: printf("move\t"); break;
        case CREATEFRAME: printf("createframe\t"); break;
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


void genetate_build_in_functions(){
    printf("label\t$_substr__0\ndefvar LF@result\neq LF@result LF@%%1 nil@nil\njumpifeq $_nil_exception_0 LF@result bool@true\neq LF@result LF@%%2 nil@nil\njumpifeq $_nil_exception_0 LF@result bool@true\neq LF@result LF@%%3 nil@nil\njumpifeq $_nil_exception_0 LF@result bool@true\n\nlt LF@result LF@%%2 float@0x1p+0\njumpifeq $$_substr_return_empty_0 LF@result bool@true\ndefvar LF@lenght\nstrlen LF@lenght LF@%%1\nint2float LF@lenght LF@lenght\ngt LF@result LF@%%3 LF@lenght\njumpifeq $$_substr_return_empty_0 LF@result bool@true\ngt LF@result LF@%%2 LF@lenght\njumpifeq $$_substr_return_empty_0 LF@result bool@true\ngt LF@result LF@%%2 LF@%%3\njumpifeq $$_substr_return_empty_0 LF@result bool@true\nfloat2int LF@%%2 LF@%%2\nfloat2int LF@%%3 LF@%%3\ndefvar LF@_char\nSUB LF@%%2 LF@%%2 int@1\nmove LF@$1 string@\nlabel $$_substr_loop_0\nlt LF@result LF@%%2 LF@%%3\njumpifneq $$_substr_return_0 LF@result bool@true\ngetchar LF@_char LF@%%1 LF@%%2\nconcat LF@$1 LF@$1 LF@_char\nadd LF@%%2 LF@%%2 int@1\njump $$_substr_loop_0\nlabel $$_substr_return_0\npopframe\nreturn\nlabel $$_substr_return_empty_0\nmove LF@$1 string@\npopframe\nreturn\n");
}