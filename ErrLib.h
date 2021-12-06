/**
 * @file ErrLib.h
 * @brief Definition of exit codes
 * 
 * IFJ-2021 Compiler
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @author Trofimov Oleg (xtrofi00@stud.fit.vutbr.cz)
 * @author Gerasimov Maxim (xgeras00@stud.fit.vutbr.cz)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>


#ifndef __ERRLIB_H__
#define __ERRLIB_H__

// Error codes
#define SUCCESS                         0  // Successfully
#define LEX_ERROR                       1  // Lexical error
#define SYNTAX_ERROR                    2  // Syntax error
#define SEM_ERROR_REDEFINE_UNDEFINE_VAR 3  // Semantic error - undefined variable or function
#define SEM_ERROR_ASSIGN_COMMAND        4  // Semantic error - assignment command
#define SEM_ERROR_TYPE_NUMBER_PARAM_RET_INCORRECT        5  // Semantic error - incorrect number/parameter types
#define SEM_ERROR_TYPE_COMPAT           6  // Semantic error - types uncompatible
#define SEM_ERROR_OTHER		            7  // Semantic error - other
#define UNEXP_ZERO_VALUE_ERROR          8  // Startup error when working with an unexpected zero value
#define INT_DIV_ERROR                   9  // Error of integer division by zero constant
#define INTERNAL_ERROR                  99 // Internal error, eg. malloc error etc


#endif // __ERRLIB_H__
