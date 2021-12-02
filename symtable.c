/**
 * @file symtable.c
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief Definition of functions for work with table of symbols    
 *  
*/

#include "symtable.h"
#include "parser.h"
#include "ErrLib.h"

size_t htab_bucket_count(const htab_t * t){
    return t->arr_size;
}

void htab_clear(htab_t * t){
    size_t arr_size = htab_bucket_count(t);
    struct htab_item *deleting_item = NULL;
	for (size_t index = 0; index < arr_size; index++){
		while(t->array[index] != NULL){
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
        if (strcmp(key,t->array[index]->key) == 0){
            deleting_item = t->array[index];
            t->array[index] = t->array[index]->next;
            free_htab_item(deleting_item);
            t->size--;
            return true;
        }
    }
    else return false; //pokud na indexu nic nebylo

    struct htab_item *walking_item = t->array[index];
    while(walking_item->next != NULL){
        deleting_item = walking_item->next;
        if (strcmp(deleting_item->key,key) == 0){
            walking_item->next = deleting_item->next;
            free_htab_item(deleting_item);
            t->size--;
            return true;
        }
        else walking_item = walking_item->next ;
    }
    return false; //prosel jsem cely list a nenasel jsem
}

htab_item * htab_find(htab_t * t, char * key){
    size_t index = (htab_hash_function(key) % t->arr_size);     //rychlejsi, nez volat funkci size_t htab_bucket_count(const htab_t * t), pokud se ale ukladani velikosti pole zmeni, bude se muset zmenit i zde, coz pri pouziti funkce nehrozi, ale zmena v tomto ukolu jiz nenastane... 
    struct htab_item *walking_item = t->array[index];
    while (walking_item != NULL){
        if (strcmp(walking_item->key,key) == 0){
            return walking_item;
        }
        else walking_item = walking_item->next;
    }

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
    htab_clear(t);
    free(t);
}

size_t htab_hash_function(char *str) {
    uint32_t h=0;     // musí mít 32 bitů
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++){
        h = 65599*h + *p;
    }        
    return h;
}


htab_t *htab_init(size_t n){
    htab_t *tab = calloc(1,sizeof(htab_t)+ sizeof(struct htab_item *[n]));
    if (tab == NULL){
        fprintf(stderr,"Chyba: nepodarilo se alokovat pamet pro novou hash table\n");
        return NULL; //EXIT CODES TODO
    }
    tab->arr_size = n;
    tab->size = 0;  
    tab->next = NULL;
    return tab;
}

//TODO
htab_item * htab_lookup_add(htab_t * t, char * key){
    size_t index = (htab_hash_function(key) % t->arr_size); //rychlejsi, nez volat funkci size_t htab_bucket_count(const htab_t * t), pokud se ale t->arr_size nejak zmeni, bude se muset zmenit i zde, coz pri pouziti funkce nehrozi, ale zmena v tomto ukolu jiz nenastane... 
    if (t->array[index] == NULL){
        t->array[index] = create_htab_item(key);
        if (t->array[index] == NULL){
            exit(INTERNAL_ERROR);
        }
        t->size++;       
        return t->array[index];
    }

    if (strcmp(t->array[index]->key,key) == 0){
        return NULL;;
    }

    struct htab_item *walking_item = t->array[index];
    while (walking_item->next != NULL){        
        if (strcmp(walking_item->next->key,key) == 0){            
            return NULL;
        }
        walking_item = walking_item->next;
    }

    walking_item->next = create_htab_item(key);
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
                fprintf(stderr,"Chyba: nepodarilo se presunout tabulku\n");
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
    //TODO INIT values

    char *new_key = malloc(strlen(key)+1); 
    if (new_key == NULL){
        free(item);
        return NULL;
    }
    strcpy(new_key,key);  //nastavim pole na hodnotu klice
    item->key = new_key; //ukazatel nastavim na pole, kde je klic
    return item;
}

void free_htab_item(struct htab_item * item){    
    //TODO uvolnit seznamy pro params and return vals
    free((void *)item->key);   // z const char delam void *,protoze prekladac haze warning,
    free(item);                     // ale k polozce uz by se nemelo pristupovat, mela by byt tedy smazana   
}

void print_htab_item_values(htab_item *data){    
    printf("%s\n",data->key);    
}
 
/*
void test(){   
    int a;  //0
    int b;  //0
    if (a < 5){
        int b = 5;  //b1
        a = b;      //a0 b1
        int a = 3;  //a1

        if(a < b){
            int a = 10; //a2

            b = a;  //b1 a2

        }
        else{
            a = 4;  //a1

            int a = 87; //a2
        }

        a = 42; //a1 

        if (a < b){
            int a;  //a3

            b = a;  //b1 a3
            if(b > a){  
                a = b;  //a3 b1
                b = 10; //b1
                int a; //a4

            }
            else{ 
                a = 4; //a3
                int a = b; //a4 b1 
                int b;  //b4    
            }
        }
        else{
            a  = 78; //a1
            int b = 97; //b3
        }
    }
    else{
        int a = 42; //a1
        int b;  //b1
        b = a;  //b1 a1
    }

    a = 478;    //a0
    b ;     //b0
}
*/