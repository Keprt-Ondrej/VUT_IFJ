/**
 * @file symtable.h
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief Declaration of interface for table of symbols
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


#define TABLE_SIZE 101  //<size of table


/**
 * @brief information, what information carry item
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
typedef enum{
    function_declared,  ///< only in global hash table for functions, this item should be de defined later
    function_defined,   ///< only in global hash table for functions
    integer,            ///< type for local frames and precedence analysis
    number,             ///< type for local frames and precedence analysis
    string,             ///< type for local frames and precedence analysis
    nil,                ///< type for local frames and precedence analysis
    type_bool,          ///< type for local frames and precedence analysis
    identifier,         ///< type for generatin function call
} data_type_t;

/**
 * @brief structure for list of parameters and return list
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
typedef struct data_token{
    data_type_t data_type;      ///< data type of item
    char *key;                  ///< name of item
    struct data_token* next;    ///< pointer on the next item in list
}data_token_t;

/**
 * @brief sturcture for hash table
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
typedef struct htab{
    size_t size;                ///< counter of items in table
    size_t arr_size;            ///< size of array, where lists of items are stored
    struct htab *next;          ///< pointer on next table, "stack" representation of local frames
    struct htab_item *array[];  ///< array of pointers on lists of items   
}htab_t;

typedef struct htab_item{
    char *key;                     ///< name of symbol stored in item
    data_type_t type;              ///< type of data, which represents this item
    data_token_t *param_list;      ///< list of parameters, if item is symbol of function, else NULL
    data_token_t *return_list;     ///< list of returning values, if item is symbol of function, else NULL
    struct htab_item *next;        ///< pointer on the parent table ("frame")
    size_t frame_ID;               ///< ID of frame, where symbol was defined
}htab_item;

/**
 * @brief allocates memory for item and sets default values
 * 
 * allocated additional memory for key, which is stored in item
 * 
 * @param key name of symbol
 * @return allocated memory of item
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
struct htab_item *create_htab_item(char *key);

/**
 * @brief free memory of item
 *  
 * @param item pointer to memory, which should be cleared
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void free_htab_item(struct htab_item * item);

/**
 * @brief hash function
 * 
 * @param str name of symbol
 * @return hashed symbol
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
size_t htab_hash_function(char * str);

/**
 * @brief allocation and initialization of table
 * 
 * @param n size of array for lists
 * @return pointer on table, NULL if allocation failed
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
htab_t *htab_init(size_t n);

/**
 * @brief move all items to newly allocated hash table
 * 
 * @param n size of new table
 * @param from 
 * @return htab_t* 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
htab_t *htab_move(size_t n, htab_t *from);

/**
 * @brief 
 * 
 * @param t pointer on table 
 * @return number of items in the table
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
size_t htab_size(const htab_t * t);

/**
 * @brief returns size of array, where are stored pointers on lists of items
 * 
 * @param t pointer on table
 * @return size of array
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
size_t htab_bucket_count(const htab_t * t);

/**
 * @brief recursivly finds item in structure of hash tables
 * 
 * find search in first table, than continue with table t->next
 * 
 * @param t pointer on table
 * @param key key for searched item
 * @return finded item or NULL, when item was not found
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
htab_item * htab_find(htab_t * t, char * key);  // hledání

/**
 * @brief add item to hash table
 * 
 * this function is not recursive!
 * 
 * @param t pointer on table
 * @param key name of symbol
 * @return created item in hash table or NULL if item was defined before
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
htab_item * htab_lookup_add(htab_t * t, char * key);


/**
 * @brief Erase item defined by key from hash table
 * 
 * @param t pointer on table
 * @param key key of item, which should be deleted
 * @return true item was deleted
 * @return false item was not found
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
bool htab_erase(htab_t * t, char * key);

/**
 * @brief apply function f on each item of the table
 * 
 * @param t pointer on table
 * @param f pointer on function, whish should be applied on all items in table
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void htab_for_each(const htab_t * t, void (*f)(htab_item *data));

/**
 * @brief delete all items of table, but not the table
 * 
 * @param t pointer on table, where all items should be erased and memory freed
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void htab_clear(htab_t * t);

/**
 * @brief recursivly free memory of table and all items of the table
 * 
 * @param t pointer on table, which memory should be freed
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void htab_free(htab_t * t);     // destruktor tabulky

void print_htab_item_values(htab_item *data);  //tiskne data, ktera dostane v argumentu
void push_back_data_token(data_token_t *token, data_token_t **place);
void free_data_token(data_token_t *token);
data_token_t *create_data_token();
void free_data_token_list(data_token_t **place);
void htab_definition_control(htab_item *data);


#endif