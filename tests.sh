#!/bin/sh
RED='\033[0;31m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'
BLUE='\033[01;34m'
DIR=test_files
test(){
    ./$4 <"test_files/"$1  >tmp.tmp
    ret_val=$?
    if [ $ret_val -eq $3 ]
        then {
            diff tmp.tmp "test_files/"$2 >/dev/null
            ret_val_diff=$?
            if [ $ret_val_diff -eq 0 ]
                then {
                    printf "";
                    echo "$3   $ret_val\t$1\t$2\t"${GREEN}"Done!${NC}"; echo "${BLUE}----------------------------------------------------------------------${NC}"
                } 
                else {
                    echo "$3   $ret_val\t$1\t$2\t" "${GREEN}Return value OK\t${NC}" ${RED}"Diff error!!!"${NC}; 
                    echo "${BLUE}----------------------------------------------------------------------${NC}"                   
                }
            fi
        }
        else {
            echo "$3   $ret_val\t$1\t$2\t"${RED}"error, wrong return value!!!"${NC};  
            echo "${BLUE}----------------------------------------------------------------------${NC}"          
        }
    fi
    
}

echo "${RED}--------------------------tests---------------------------------------${NC}"
#parameters: input file for program, refference file, expected return val of program
#writes: expected return value of program, real return value, program input file, reference file for diff
test input.txt main.c_refference.txt 0 lex_test
test nothin.syntax_test_in no_output.txt 0 IFJ-Projekt-2021
echo "${RED}--------------------------tests---------------------------------------${NC}"
rm -f tmp.tmp