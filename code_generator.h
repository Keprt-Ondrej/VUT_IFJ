/**
 * @file code_generator.h
 * @brief Interface of functions for generating IFJcode21
 * 
 * IFJ-2021 Compiler
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/

#ifndef __CODE_GENERATOR_H__
#define __CODE_GENERATOR_H__

#include "ErrLib.h"
#include "symtable.h"

extern const char* nil_string;          ///< String in IFJcode21 for nil value
extern const char* bool_string_true;    ///< String in IFJcode21 for true value
extern const char* bool_string_false;   ///< String in IFJcode21 for false value

/**
 * @brief enum for every IFJcode21 instruction
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
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
    ADDS,SUBS,MULS,DIVS,IDIVS,
    LT,
    GT,
    EQ,
    LTS,GTS,EQS,
    AND,
    OR,
    NOT,   
    ANDS, ORS, NOTS,
    INT2FLOAT,
    FLOAT2INT,
    INT2CHAR,
    STRI2INT,
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
    JUMPIFEQS,JUMPIFNEQS,
    EXIT,
    BREAK,
    DPRINT,
} OP_code_t;

/**
 * @brief Data structure for storing generated instruction
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
typedef struct instr{
    struct instr *next;     ///< pointer on next instruction
    OP_code_t opcode;       ///< instruction opcode
    char *op1;              ///< first ALLOCATED operand of instruction
    char *op2;              ///< second ALLOCATED operand of instruction
    char *op3;              ///< third ALLOCATED operand of instruction
}instruction_t;

/**
 * @brief Free all string operands and whole structure
 * 
 * @param ins pointer on deleting instruction
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void free_instruction(instruction_t *ins);

/**
 * @brief ALLOCATE string, representing integer in IFJcode21
 * 
 * @param value integer in IFJ21
 * @return char* ALLOCATED string representing integer
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
char *int_to_string(int value);

/**
 * @brief ALLOCATE string, representing number in IFJcode21
 * 
 * @param value double in IFJ21
 * @return char* ALLOCATED string representing number
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
char *double_to_string(double value);

/**
 * @brief ALLOCATE string, representing string in IFJcode21
 * 
 * @param string string in IFJ21
 * @return char* ALLOCATED string representing string
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
char *string_to_string(char *string);

/**
 * @brief Create a instruction object, REQUIRE ALLOCATED operands
 * 
 * @param opcode OPCODE of instruction
 * @param op1 ALLOCATED operand or NULL
 * @param op2 ALLOCATED operand or NULL
 * @param op3 ALLOCATED operand or NULL
 * @return instruction_t* ALLOCATED instruction
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
instruction_t *create_instruction(OP_code_t opcode,char *op1,char *op2,char *op3);

/**
 * @brief Walk through instruction list and print instructions, free instructions too
 * 
 * @note function does not set instruction list on NULL!!!
 * 
 * @param inst start of instruction list 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void generate_code(instruction_t *inst);

/**
 * @brief ALLOCATE memory for variable name and properly generate it
 * 
 * @param frame string, specifing frame, pass LF@,TF@,GF@ 
 * @param item variable representation in table of symbols
 * @return char* final name of variable in 3AC
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
char *allocate_var_name_3AC(const char* frame,htab_item *item);

/**
 * @brief print instruction name 
 * 
 * @param opcode instruction opcode
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void print_opcode(OP_code_t opcode);

/**
 * @brief print code in IFJcode21 for build in functions
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void genetate_build_in_functions();


#endif