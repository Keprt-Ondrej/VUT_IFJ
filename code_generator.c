/**
 * @file code_generator.c
 * @brief Implementation of functions for generating IFJcode21
 * 
 * IFJ-2021 Compiler
 * 
 * @see code_generator.h for documentation
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
#include "code_generator.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

const char* nil_string = "nil@nil";             ///< String in IFJcode21 for nil value
const char* bool_string_true = "bool@true";     ///< String in IFJcode21 for true value
const char* bool_string_false = "bool@false";   ///< String in IFJcode21 for false value

void free_instruction(instruction_t *ins){
    free(ins->op1);
    free(ins->op2);
    free(ins->op3);
    free(ins);
}

char *int_to_string(int value){
    size_t length = snprintf(NULL,0,"int@%d",value) + 1 ;
    char * str = calloc(length,sizeof(char));
    if(str == NULL){
        exit(INTERNAL_ERROR);
    }
    snprintf(str,length, "int@%d",value);
    return str;
}

char *double_to_string(double value){
    size_t length = snprintf(NULL,0,"float@%a",value) + 1 ;
    char * str = calloc(length,sizeof(char));
    if(str == NULL){
        exit(INTERNAL_ERROR);
    }
    snprintf(str,length, "float@%a",value);
    return str;
}

char *string_to_string(char *string){
    size_t lenght = snprintf(NULL,0,"string@%s",string)+1; 
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
    size_t lenght = snprintf(NULL,0,"%s%s_$%zu",frame,item->key,item->frame_ID) + 1; //frame id for differentiate frames
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
    //substr
    printf("\nlabel\t$_substr__0\ndefvar LF@result\neq LF@result LF@%%1 nil@nil\njumpifeq $_nil_exception__0 LF@result bool@true\neq LF@result LF@%%2 nil@nil\njumpifeq $_nil_exception__0 LF@result bool@true\neq LF@result LF@%%3 nil@nil\njumpifeq $_nil_exception__0 LF@result bool@true\n\nlt LF@result LF@%%2 float@0x1p+0\njumpifeq $$_substr_return_empty_0 LF@result bool@true\ndefvar LF@lenght\nstrlen LF@lenght LF@%%1\nint2float LF@lenght LF@lenght\ngt LF@result LF@%%3 LF@lenght\njumpifeq $$_substr_return_empty_0 LF@result bool@true\ngt LF@result LF@%%2 LF@lenght\njumpifeq $$_substr_return_empty_0 LF@result bool@true\ngt LF@result LF@%%2 LF@%%3\njumpifeq $$_substr_return_empty_0 LF@result bool@true\nfloat2int LF@%%2 LF@%%2\nfloat2int LF@%%3 LF@%%3\ndefvar LF@_char\nSUB LF@%%2 LF@%%2 int@1\nmove LF@$1 string@\nlabel $$_substr_loop_0\nlt LF@result LF@%%2 LF@%%3\njumpifneq $$_substr_return_0 LF@result bool@true\ngetchar LF@_char LF@%%1 LF@%%2\nconcat LF@$1 LF@$1 LF@_char\nadd LF@%%2 LF@%%2 int@1\njump $$_substr_loop_0\nlabel $$_substr_return_0\npopframe\nreturn\nlabel $$_substr_return_empty_0\nmove LF@$1 string@\npopframe\nreturn\n");
    //ord
    printf("\nlabel $_ord__0\ndefvar LF@result\neq LF@result LF@%%1 nil@nil\njumpifeq $_nil_exception__0 LF@result bool@true\neq LF@result LF@%%2 nil@nil\njumpifeq $_nil_exception__0 LF@result bool@true\nlt LF@result LF@%%2 int@1\njumpifeq $$_ord_ret_nil_0 LF@result bool@true\ndefvar LF@lenght\nstrlen LF@lenght LF@%%1\ngt LF@result LF@%%2 LF@lenght\njumpifeq $$_ord_ret_nil_0 LF@result bool@true\nsub LF@%%2 LF@%%2 int@1\nstri2int LF@$1 LF@%%1 LF@%%2\npopframe\nreturn\nlabel $$_ord_ret_nil_0\nmove LF@$1 nil@nil\npopframe\nreturn\n");
    //chr
    printf("\nlabel $_chr__0\ndefvar LF@result\neq LF@result LF@%%1 nil@nil\njumpifeq $_nil_exception__0 LF@result bool@true\nlt LF@result LF@%%1 int@0\njumpifeq $$_chr_ret_nil_0 LF@result bool@true\ngt LF@result LF@%%1 int@255\njumpifeq $$_chr_ret_nil_0 LF@result bool@true\nint2char LF@$1 LF@%%1\npopframe\nreturn\nlabel $$_chr_ret_nil_0\nmove LF@$1 nil@nil\npopframe\nreturn\n");
    //tointeger
    printf("\nlabel $_tointeger__0\ndefvar LF@result\neq LF@result LF@%%1 nil@nil\njumpifeq $$_tointeger_ret_nil_0 LF@result bool@true\nfloat2int LF@$1 LF@%%1\npopframe\nreturn\nlabel $$_tointeger_ret_nil_0\nmove LF@$1 nil@nil\npopframe\nreturn\n");
    //reads
    printf("\nlabel $_reads__0\nread LF@$1 string\npopframe\nreturn\n");
    //readi
    printf("\nlabel $_readi__0\nread LF@$1 int\npopframe\nreturn\n");
    //readn
    printf("\nlabel $_readn__0\nread LF@$1 float\npopframe\nreturn\n");
    //write
    printf("\nlabel $_write__0\ndefvar LF@tmp\ndefvar LF@result\nlabel $_write_loop_start_0\nlt LF@result LF@%%1 int@1\njumpifeq $_write_end_printing_0 LF@result bool@true\nsub LF@%%1 LF@%%1 int@1\npops LF@tmp\neq LF@result LF@tmp nil@nil\njumpifeq $_write_write_nil_0 LF@result bool@true\nwrite LF@tmp\njump $_write_loop_start_0\nlabel $_write_write_nil_0\nwrite string@nil\njump $_write_loop_start_0\nlabel $_write_end_printing_0\npopframe\nreturn\n");
    //nil exception
    printf("\nlabel $_nil_exception__0\nexit int@8\n");
}