/**
 * @file symtable.c
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#include "symtable.h"

// htab_bucket_count.c
// Řešení IJC-DU2, příklad 2), 17.4.2021
// Autor: Ondřej Keprt, FIT
// Přeloženo: gcc 7.5
// 



size_t htab_bucket_count(const htab_t * t){
    return t->arr_size;
}

// htab_clear.c
// Řešení IJC-DU2, příklad 2), 17.4.2021
// Autor: Ondřej Keprt, FIT
// Přeloženo: gcc 7.5
// 



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
}// htab_erase.c
// Řešení IJC-DU2, příklad 2), 17.4.2021
// Autor: Ondřej Keprt, FIT
// Přeloženo: gcc 7.5
// 





bool htab_erase(htab_t * t, htab_key_t key){
    size_t index = (htab_hash_function(key) % t->arr_size); //rychlejsi, nez volat funkci size_t htab_bucket_count(const htab_t * t), pokud se ale ukladani velikosti pole zmeni, bude se muset zmenit i zde, coz pri pouziti funkce nehrozi, ale zmena v tomto ukolu jiz nenastane...  
    struct htab_item *deleting_item = NULL;
    if (t->array[index] != NULL){
        if (strcmp((char *)key,(char *)t->array[index]->key) == 0){
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
        if (strcmp((char *)deleting_item->key,(char *)key) == 0){
            walking_item->next = deleting_item->next;
            free_htab_item(deleting_item);
            t->size--;
            return true;
        }
        else walking_item = walking_item->next ;
    }
    return false; //prosel jsem cely list a nenasel jsem
}// htab_find.c
// Řešení IJC-DU2, příklad 2), 17.4.2021
// Autor: Ondřej Keprt, FIT
// Přeloženo: gcc 7.5
// 



htab_item * htab_find(htab_t * t, htab_key_t key){
    size_t index = (htab_hash_function(key) % t->arr_size);     //rychlejsi, nez volat funkci size_t htab_bucket_count(const htab_t * t), pokud se ale ukladani velikosti pole zmeni, bude se muset zmenit i zde, coz pri pouziti funkce nehrozi, ale zmena v tomto ukolu jiz nenastane... 
    struct htab_item *walking_item = t->array[index];
    while (walking_item != NULL){
        if (strcmp((char *)walking_item->key,(char *)key) == 0){
            return walking_item;
        }
        else walking_item = walking_item->next;
    }
    return NULL; //prosel jsem cely list a nenasel jsem
}// htab_for_each.c
// Řešení IJC-DU2, příklad 2), 17.4.2021
// Autor: Ondřej Keprt, FIT
// Přeloženo: gcc 7.5
// 



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
}// htab_free.c
// Řešení IJC-DU2, příklad 2), 17.4.2021
// Autor: Ondřej Keprt, FIT
// Přeloženo: gcc 7.5
// 



void htab_free(htab_t * t){
    htab_clear(t);
    free(t);
}// htab.h -- rozhraní knihovny htab (řešení IJC-DU2)
// Licence: žádná (Public domain)



#include <string.h>     // size_t
#include <stdbool.h>    // bool


#ifdef HASHTEST
    size_t htab_hash_function(const char *str) {
        //fprintf(stderr,"myhash\n");
        uint32_t hash = 43;
        uint32_t lenght = strlen(str);
        for(uint32_t i = 0; i < lenght;i++){
            hash = (str[i] * hash >> 4) + 33;
        }  
        return hash;
    }
#else 
    //funkce ze zadani
    size_t htab_hash_function(const char *str) {
        uint32_t h=0;     // musí mít 32 bitů
        const unsigned char *p;
        for(p=(const unsigned char*)str; *p!='\0'; p++){
            h = 65599*h + *p;
        }        
        return h;
    }
#endif

htab_t *htab_init(size_t n){
    htab_t *tab = calloc(1,sizeof(htab_t)+ sizeof(struct htab_item *[n]));
    if (tab == NULL){
        fprintf(stderr,"Chyba: nepodarilo se alokovat pamet pro novou hash table\n");
        return NULL;
    }
    tab->arr_size = n;
    tab->size = 0;  
    /*              
    for(size_t i = 0; i < n;i++){       //pokud by se misto calloc pouzil malloc
        tab->array[i] = NULL;           // ale NULL je definovan jako ((void*)0), proto to jiz pri alokaci pomoci calloc "nastavim na NULL"
    }                                   // pri vetsi tabulce by toto prochazeni mohlo spomalit program
    */
    return tab;
}

htab_item * htab_lookup_add(htab_t * t, htab_key_t key){
    size_t index = (htab_hash_function(key) % t->arr_size); //rychlejsi, nez volat funkci size_t htab_bucket_count(const htab_t * t), pokud se ale t->arr_size nejak zmeni, bude se muset zmenit i zde, coz pri pouziti funkce nehrozi, ale zmena v tomto ukolu jiz nenastane... 
    if (t->array[index] == NULL){
        t->array[index] = create_htab_item(key);
        if (t->array[index] == NULL){
            return NULL;
        }
        t->size++;       
        return t->array[index];
    }

    if (strcmp((char *)t->array[index]->key,(char *)key) == 0){
        return t->array[index];
    }

    struct htab_item *walking_item = t->array[index];
    while (walking_item->next != NULL){        
        if (strcmp((char *)walking_item->next->key,(char *)key) == 0){            
            return walking_item->next;
        }
        walking_item = walking_item->next;
    }

    walking_item->next = create_htab_item(key);
    if (walking_item->next == NULL){      
        return NULL;
    }
    else{
        t->size++; 
        return walking_item->next;
    }   
    return NULL; // death code??  
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
                new_pair->value = moving_item->value;  
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

struct htab_item *create_htab_item(const htab_key_t key){    
    struct htab_item *item = malloc(sizeof(struct htab_item));
    if (item == NULL){
        return NULL;
    }
    item->next = NULL;
    item->value = 0;    
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
    free((void *)item->key);   // z const char delam void *,protoze prekladac haze warning,
    free(item);                     // ale k polozce uz by se nemelo pristupovat, mela by byt tedy smazana   
}

void symtable_hello(){
    printf("Hello from symtable\n");
}

void print_htab_item_values(htab_item *data){    
    printf("%s\t%d\n",data->key,(int)data->value);    
}


#define TABLE_SIZE 1009 
#define WORD_MAX_LENGHT 127
int read_word(char *s, int max, FILE *f){  
    static bool was_err_msg = false;  
    int c;
    int char_index = 0;
    max -= 1;    
    while(1){
        c = getc(f);
        if (char_index == max && (!isspace(c) || c == EOF)){     
            s[char_index] = '\0';
            c = getc(f);
            while(!isspace(c)){
                if (c == EOF) break;
                c = getc(f); 
            }  

            if (!was_err_msg){
                fprintf(stderr,"Chyba: prilis dlouhe slovo: %s\n",s);
                was_err_msg = true;
            }         
            return max;
        }
        if (char_index == 0 && c == EOF){ //pokud bych mel sobour kde je konec radku a pak hned EOF
            return EOF;
        }
        if (isspace(c) && char_index == 0 ){ //preskoceni vicero bilych znaku po sobe 
            continue;
        }
        if (isspace(c) || c == EOF){    //soubor muze koncit slovem a pak hned EOF, tak aby se i to posledni slovo zapocitalo
            s[char_index] = '\0'; //pridam za slovo konec
            return strlen(s);                     
        }
        else{
            s[char_index++] = c;  //ukladani po znaku            
        }    
    }
    return  0; //deathcode
}
void wordcount(){    
    htab_t * storage = htab_init(TABLE_SIZE);    
    if (storage == NULL){
        fprintf(stderr,"Chyba: nepodarilo se alokovat pamet pro tabulku\n");
        exit(1);
    } 

    htab_item *result = NULL;
    char s[WORD_MAX_LENGHT] = {'\0'};
    while (read_word(s,WORD_MAX_LENGHT,stdin) != EOF){
        result = htab_lookup_add(storage,s);
        if (result == NULL){    //nepodarilo se pridat slovo
            fprintf(stderr,"Chyba: nepodarilo se alokovat pamet pro prvek: %s",s);
            htab_free(storage);
            exit(1); 
        }
        result->value++;
    }

    #ifdef MOVETEST
        htab_t * movetest_tab = htab_move(TABLE_SIZE/2,storage);  
        if (movetest_tab == NULL){
            fprintf(stderr,"Chyba: nepodarilo se alokovat pamet pro tabulku\n");
            exit(1);
        } 
        htab_for_each(movetest_tab,print_htab_item_values);
        htab_free(movetest_tab);
    #else
        htab_for_each(storage,print_htab_item_values); 
    #endif
       
    htab_free(storage);
    return;
}