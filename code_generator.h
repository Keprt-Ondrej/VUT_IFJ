

#ifndef __CODE_GENERATOR_H__
#define __CODE_GENERATOR_H__

#include "ErrLib.h"
#include "symtable.h"

extern const char* nil_string;
extern const char* bool_string_true;
extern const char* bool_string_false;

typedef enum {
    MOVE,
    CREATEFRAME,
    PUSHFRAME,
    POPFRAME,
    DEFVAR,
    CALL,
    RETURN,
    PUSHS,
    POPS,
    CLEARS,
    ADD,
    SUB,
    MUL,
    DIV,
    IDIV,
    //zasobnikove verze
    ADDS,SUBS,MULS,DIVS,IDIVS,

    LT,
    GT,
    EQ,
    //zasobnikove verze
    LTS,GTS,EQS,
    
    AND,
    OR,
    NOT,
    //zasobnikove verze
    ANDS, ORS, NOTS,

    INT2FLOAT,
    FLOAT2INT,
    INT2CHAR,
    STRI2INT,
    //zasobnikove verze
    INT2FLOATS,FLOAT2INTS,INT2CHARS,STRI2INTS,

    READ,
    WRITE,
    CONCAT,
    STRLEN,
    GETCHAR,
    SETCHAR,
    TYPE,
    LABEL,
    JUMP,
    JUMPIFEQ,
    JUMPIFNEQ,
    //zasobnikove verze
    JUMPIFEQS,JUMPIFNEQS,
    EXIT,

    //ladici instr
    BREAK,
    DPRINT,
} OP_code_t;

typedef struct instr{
    struct instr *next;
    OP_code_t opcode;
    char *op1;
    char *op2;
    char *op3;
}instruction_t;

void free_instruction(instruction_t *ins);
char *int_to_string(int value);
char *double_to_string(double value);
char *string_to_string(char *string);

/**
 * @brief Create a instruction object, REQUIRE ALLOCATED operands
 * 
 * @param opcode OPCODE of instruction
 * @param op1 ALLOCATED operand or NULL
 * @param op2 ALLOCATED operand or NULL
 * @param op3 ALLOCATED operand or NULL
 * @return instruction_t* 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
instruction_t *create_instruction(OP_code_t opcode,char *op1,char *op2,char *op3);
void generate_code(instruction_t *inst);
void print_opcode(OP_code_t opcode);

/**
 * @brief ALLOCATE memory for variable name and properly generate it
 * 
 * @param frame string, specifing frame, pass LF@,TF@,GF@ 
 * @param item variable representation in table of symbols
 * @return char* final name of variable in 3AC
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
char *allocate_var_name_3AC(const char* frame,htab_item *item);
void genetate_build_in_functions();


#endif