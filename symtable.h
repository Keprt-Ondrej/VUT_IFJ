/**
 * @file symtable.h
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief Deklarace rozhraní pro tanulku symbolů
 * 
 * Základní struktura je převzata z mého projektu z Jazyka C a upravena pro tento projekt
 * Odkaz na zadání, platné asi jen do konce zimního semestru 2021:
 * http://www.fit.vutbr.cz/study/courses/IJC/public/DU2.html.cs
 * Odkaz na vypracovaný projekt Ondřejem Keprtem:
 * https://github.com/KeprtOndrej/ICJ-projekt-2 //obsahuje i původní zadání v HTML
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
typedef enum{
    function,
    integer,
    number,
    string,
} htab_record_type;


// Tabulka:
typedef struct htab{
    size_t size;
    size_t arr_size;
    struct htab_item *array[];    
}htab_t;

typedef struct htab_item{
    htab_key_t    key;          // klíč
    htab_value_t  value;        // asociovaná hodnota
    void * param_list;
    void * return_list;
    bool inicialized;
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

htab_item * htab_find(htab_t * t, htab_key_t key);  // hledání
htab_item * htab_lookup_add(htab_t * t, htab_key_t key);

bool htab_erase(htab_t * t, htab_key_t key);    // ruší zadaný záznam

// for_each: projde všechny záznamy a zavolá na ně funkci f
void htab_for_each(const htab_t * t, void (*f)(htab_item *data));

void htab_clear(htab_t * t);    // ruší všechny záznamy
void htab_free(htab_t * t);     // destruktor tabulky

void print_htab_item_values(htab_item *data);  //tiskne data, ktera dostane v argumentu
int read_word(char *s, int max, FILE *f);
void wordcount();

void htab_define_var(htab_key_t key);
void htab_declare_var(htab_key_t key);

#endif