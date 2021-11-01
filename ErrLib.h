#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>


#ifndef _ERROR_H_
#define _ERROR_H_

// Error codes
#define SUCCESS                  0  // Successfully
#define LEX_ERROR                1  // Lexical error
#define SYNTAX_ERROR             2  // Syntax error
#define SEM_ERROR_UNDEFINED      3  // Semantic error - undefined variable or function
#define SEM_ERROR_ASSIGN_COMMAND 4  // Semantic error - assignment command
#define SEM_ERROR_TYPE_INCORRECT 5  // Semantic error - incorrect number/parameter types
#define SEM_ERROR_TYPE_COMPAT    6  // Semantic error - types uncompatible
#define SEM_ERROR_OTHER		       7  // Semantic error - other
#define UNEXP_ZERO_VALUE_ERROR   8  // Startup error when working with an unexpected zero value
#define INT_DIV_ERROR            9  // Error of integer division by zero constant
#define INTERNAL_ERROR           99 // Internal error, eg. malloc error etc


#endif // _ERROR_H_
