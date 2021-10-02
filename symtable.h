/**
 * @file symtable.h
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <stdio.h>
#include <string.h>     // size_t
#include <stdbool.h>    // bool
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

void symtable_hello();

// Typy:
typedef const char * htab_key_t;        // typ klíče
typedef int htab_value_t;               // typ hodnoty

// Tabulka:
typedef struct htab{
    size_t size;
    size_t arr_size;
    struct htab_item *array[];    
}htab_t;

// Dvojice dat v tabulce:
typedef struct htab_pair {
    htab_key_t    key;          // klíč
    htab_value_t  value;        // asociovaná hodnota
} htab_pair_t;                  // typedef podle zadání

typedef struct htab_item{
    htab_pair_t data;
    struct htab_item *next;
}htab_item;

struct htab_item *create_htab_item(const htab_key_t key);
void free_htab_item(struct htab_item * item);




// Rozptylovací (hash) funkce (stejná pro všechny tabulky v programu)
// Pokud si v programu definujete stejnou funkci, použije se ta vaše.
size_t htab_hash_function(htab_key_t str);

// Funkce pro práci s tabulkou:
htab_t *htab_init(size_t n);                    // konstruktor tabulky
htab_t *htab_move(size_t n, htab_t *from);      // přesun dat do nové tabulky
size_t htab_size(const htab_t * t);             // počet záznamů v tabulce
size_t htab_bucket_count(const htab_t * t);     // velikost pole

htab_pair_t * htab_find(htab_t * t, htab_key_t key);  // hledání
htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key);

bool htab_erase(htab_t * t, htab_key_t key);    // ruší zadaný záznam

// for_each: projde všechny záznamy a zavolá na ně funkci f
void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data));

void htab_clear(htab_t * t);    // ruší všechny záznamy
void htab_free(htab_t * t);     // destruktor tabulky

void print_htab_item_values(htab_pair_t *data);  //tiskne data, ktera dostane v argumentu
int read_word(char *s, int max, FILE *f);
void wordcount();


#endif