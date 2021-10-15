#!/bin/sh
RED='\033[0;31m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'
BLUE='\033[01;34m'
test(){
    
    ./IFJ-Projekt-2021 <$1 >tmp.tmp
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
                    echo "$3   $ret_val\t$1\t$2\t"${RED}"Diff error!!!"${NC}; 
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
#parameters: input file for IFJ, refference file, expected return val of IFJ
#writes: expected return value of IFJ, real return value, IFJ input file, reference file for diff
test main.c main.c_refference.txt 0
test main.h main.c_refference.txt 0
test main.c main.c_refference.txt 42
echo "${RED}--------------------------tests---------------------------------------${NC}"
rm -f tmp.tmp