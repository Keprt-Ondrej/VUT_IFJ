/**
 * @file scanner.c
 * @author Oleg Trofimov (xtrofi00@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#include "scanner.h"
#include "ErrLib.h"

#define size_of_length 10

// 1. Инициаизация баффера
// 2. Чтение символа из стдин
// 2.1 Добавление его в баффер
// 3. Вывести весь баффер

// 1. Написать конечный автомат

int str_init(String *s) {
    s->str = (char *) malloc(size_of_length);
    if(NULL == s->str) {
        return INTERNAL_ERROR;
    }
    s->length = 0;
    s->str[s->length] = '\0';
    s->alloc_size = size_of_length;
    return 0;
}

void str_free(String *s) {
   free(s->str);
}

void str_clear(String *s) {
    s->length = 0;
    s->str[s->length] = '\0';
}

int str_add_char(String *s, char c) {
	if (s->length + 1 >= s->alloc_size) {
		int alloc_new_size = s->length + size_of_length;
		s->str = (char *) realloc(s->str, alloc_new_size);
        if(NULL == s->str) {
           return INTERNAL_ERROR; 
        }
		s->alloc_size = alloc_new_size;
	}
	s->str[s->length] = c;
	s->str[s->length++] = '\0';
	return 0;
}    

char str_del_char(String *s) {
	if (s->length) {
        return INTERNAL_ERROR;
    }
    else {
        return 0;
    } 
	return s->str[s->length--]; // return deleted char
}
