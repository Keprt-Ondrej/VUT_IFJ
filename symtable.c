/**
 * @file symtable.c
 * @brief Definition of hash table
 * 
 * IFJ-2021 Compiler
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/

#include "symtable.h"
#include "parser.h"
#include "ErrLib.h"

data_token_t *create_data_token(){
    data_token_t *token = calloc(1,sizeof(data_token_t));
    if(token == NULL){
        exit(INTERNAL_ERROR);
    }
    return token;
}

void push_back_data_token(data_token_t *token, data_token_t **place){
    data_token_t *walking_token = *place;
    if(walking_token == NULL){
        *place = token;
        return;
    }
    while(walking_token->next != NULL){ //walk through list
        walking_token = walking_token->next;
    }
    walking_token->next = token;    //set last 
}

void free_data_token(data_token_t *token){
    free(token->key);
    free(token);
}

void free_data_token_list(data_token_t **place){
    data_token_t *token = *place;
    data_token_t *delete;
    while(token != NULL){   //until end
        delete = token;
        token = token->next;
        free_data_token(delete);
    }
    *place = NULL;
}

size_t htab_bucket_count(const htab_t * t){
    return t->arr_size;
}

void htab_clear(htab_t * t){
    size_t arr_size = htab_bucket_count(t);
    struct htab_item *deleting_item = NULL;
	for (size_t index = 0; index < arr_size; index++){  //for all lists starts
		while(t->array[index] != NULL){         //until end of list
            deleting_item = t->array[index];
            t->array[index] = t->array[index]->next;
            free_htab_item(deleting_item);
        }
	}
    t->size = 0;
}

bool htab_erase(htab_t * t, char * key){
    size_t index = (htab_hash_function(key) % t->arr_size);  
    struct htab_item *deleting_item = NULL;
    if (t->array[index] != NULL){
        if (strcmp(key,t->array[index]->key) == 0){ //item is first in list
            deleting_item = t->array[index];
            t->array[index] = t->array[index]->next;
            free_htab_item(deleting_item);
            t->size--;
            return true;
        }
    }
    else return false;

    struct htab_item *walking_item = t->array[index];   
    while(walking_item->next != NULL){          //walk through list
        deleting_item = walking_item->next;
        if (strcmp(deleting_item->key,key) == 0){
            walking_item->next = deleting_item->next;
            free_htab_item(deleting_item);
            t->size--;
            return true;
        }
        else walking_item = walking_item->next ;
    }
    return false; 
}

htab_item * htab_find(htab_t * t, char * key){
    size_t index = (htab_hash_function(key) % t->arr_size);
    struct htab_item *walking_item = t->array[index];
    while (walking_item != NULL){                   //find in actual lists
        if (strcmp(walking_item->key,key) == 0){
            return walking_item;        //found, return
        }
        else walking_item = walking_item->next;
    }
    //end of list, search in next level of table stack
    if(t->next != NULL){
        return htab_find(t->next,key);
    }
    return NULL;
}

void htab_for_each(const htab_t * t, void (*f)(htab_item *data)){
    size_t arr_size = htab_bucket_count(t);
    struct htab_item * walking_item = NULL;
    for (size_t i = 0; i < arr_size;i++){
        walking_item = t->array[i];        
        while(walking_item != NULL){
            f(walking_item);
            walking_item = walking_item->next;
        }        
    }
}

void htab_free(htab_t * t){
    if(t->next != NULL){        //recursive free
        htab_free(t->next);
    }   
    htab_clear(t);
    free(t);
}



//http://www.cse.yorku.ca/~oz/hash.html SDBM implementation
size_t htab_hash_function(char *str) {
    size_t hash = 0;
    int c = *str++;
    while(c != '\0'){
        hash = c + (hash << 6) + (hash << 16) - hash;
        c = *str++;
    }
    return hash;
}


htab_t *htab_init(size_t n){
    htab_t *tab = calloc(1,sizeof(htab_t)+ sizeof(struct htab_item *[n]));
    if (tab == NULL){
        fprintf(stderr,"Can not allocate hash table\n");
        exit(INTERNAL_ERROR);
    }
    tab->arr_size = n;
    tab->size = 0;  
    tab->next = NULL;
    return tab;
}

htab_item * htab_lookup_add(htab_t * t, char * key){
    size_t index = (htab_hash_function(key) % t->arr_size);  
    if (t->array[index] == NULL){       //list is empty
        t->array[index] = create_htab_item(key);
        if (t->array[index] == NULL){
            exit(INTERNAL_ERROR);
        }
        t->size++;       
        return t->array[index];
    }

    if (strcmp(t->array[index]->key,key) == 0){ //find item with same key
        return NULL;;
    }

    struct htab_item *walking_item = t->array[index];
    while (walking_item->next != NULL){                 //find ind list
        if (strcmp(walking_item->next->key,key) == 0){            
            return NULL;
        }
        walking_item = walking_item->next;
    }

    walking_item->next = create_htab_item(key); //end, can insert item
    if (walking_item->next == NULL){      
        exit(INTERNAL_ERROR);
    }
    else{
        t->size++; 
        return walking_item->next;
    }   
    return NULL; // death code 
}

htab_t *htab_move(size_t n, htab_t *from){
    htab_t *tab = htab_init(n);
    if (tab == NULL){
        return NULL; 
    }  
    size_t old_arr_size = htab_bucket_count(from);
    struct htab_item *moving_item = NULL;
    for (size_t i = 0; i < old_arr_size;i++){
        moving_item = from->array[i];
        while(moving_item != NULL){ 
            htab_item *new_pair = htab_lookup_add(tab, moving_item->key);
            if (new_pair == NULL){
                fprintf(stderr,"Can not move table\n");
                htab_free(tab);
                return NULL;
            }
            else{ 
                moving_item = moving_item->next;
            }           
        }        
    }
    tab->size = htab_size(from);    
    htab_clear(from);
    return tab;
}

size_t htab_size(const htab_t * t){
    return t->size;
}

struct htab_item *create_htab_item( char * key){    
    struct htab_item *item = malloc(sizeof(struct htab_item));
    if (item == NULL){
        return NULL;
    }
    item->next = NULL;
    item->param_list = NULL;
    item->return_list = NULL;

    char *new_key = malloc(strlen(key)+1); 
    if (new_key == NULL){
        free(item);
        return NULL;
    }
    strcpy(new_key,key);  
    item->key = new_key;
    return item;
}

void free_htab_item(struct htab_item * item){    
    if(item->param_list != NULL){
        free_data_token_list(&(item->param_list));        
    }
    if(item->return_list != NULL){
        free_data_token_list(&(item->return_list));
    }
    free((void *)item->key);  
    free(item);                       
}

void print_htab_item_values(htab_item *data){    
    fprintf(stderr,"variable in table: %s %zu %d\n",data->key,data->frame_ID,data->type);    
}

void htab_definition_control(htab_item *data){
    if(data->type == function_declared){
        fprintf(stderr,"Undefined function %s\n",data->key);
        exit(SEM_ERROR_REDEFINE_UNDEFINE_VAR);
    }
}
 