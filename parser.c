/**
 * @file parser.c
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#include "parser.h"

void set_errno(parser_data_t *data,int errno){
    if(data->errno == 0){
        data->errno = errno;
    }
}

void print_token(Token *token){
    Token_data data_token = token->data;
    Token_type type = token->type;
    printf("%3d: ", type);
    if(type == 16 || type == 19)
        printf("%s\n", data_token.str);
    else if(type == 17)
        printf("%d\n", data_token.type_integer);
    else if(type == 18)
        printf("%f\n", data_token.type_double);  
    else
        printf("%d\n", type);
}

void get_token(parser_data_t *data){
    Token *token = read_token();
    if(token == NULL){
        free_parser_data(data);
        fprintf(stderr,"Lexical error\n");
        exit(LEX_ERROR);
    }    
    print_token(token);
    data->token->next = token;
    data->token = token;
}

bool is_token(parser_data_t *data,Token_type type){
    if (data->token->type == type){
        return true;
    }
   return false;
}

void free_parser_data(parser_data_t *data){
    //TODO free
    htab_free(data->global_symtable);
    return;
}

int parser(){
    parser_data_t data;    
    data.token = read_token();
    if(data.token == NULL){
        return LEX_ERROR;
    }
    print_token(data.token);    
    data.token_list_first = data.token;
    data.global_symtable = htab_init(TABLE_SIZE);
    data.errno = 0;

    //syntax analysis
    //get_token(&data);
    if (!intro(&data)){
        fprintf(stderr,"Syntax error\n");
        free_parser_data(&data);
        return data.errno;
    }
    free_parser_data(&data);
    return 0;
}

void set_function_lists(parser_data_t *data){

}

bool htab_define_function(htab_key_t key,parser_data_t *data){
    htab_item * item = htab_lookup_add(data->global_symtable,key);   
    if(item == NULL){
        //TODO ERRNO
        return false;
    }
    item->type = function_defined;
    //TODO SET PARAMS
    return true;
}

bool htab_declare_function(htab_key_t key,parser_data_t *data){ 
    htab_item * item = htab_lookup_add(data->global_symtable,key);   
    if(item == NULL){
         //TODO ERRNO
        return false;
    }
    item->type = function_declared;
    //TODO SET PARAMS
    return true;
}

bool intro(parser_data_t *data){    
    if (is_token(data,kw_require)){
        //grammar rule 1
        return prolog(data) && prog(data);
    }
    else if(is_token(data,token_type_EOF)){
        return true;
    }
    else{
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
}

bool prolog(parser_data_t* data){
    //grammar rule 6
    if(is_token(data,kw_require)){        
        get_token(data);
        if(is_token(data,token_type_string)){
            if(!strcmp("ifj21",data->token->data.str)){
                get_token(data);
                return true;
            }            
        }
    }
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool prog(parser_data_t* data){
    if(is_token(data,kw_global)){    
        //grammar rule 2
        return fce_decl(data) && prog(data);
    }
    else if(is_token(data,kw_function)){
        //grammar rule 3
        return fce_def(data) && prog(data);
    }
    else if(is_token(data,token_type_identifier)){
        //grammar rule 4
        //TODO SEMANTIC
        get_token(data);    
        return call_fce(data) && prog(data);
    }
    else if(is_token(data,token_type_EOF)){
        //grammar rule 5
        return true;
    }
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool fce_decl(parser_data_t *data){
    //grammar rule 7
    if(!is_token(data,kw_global)){
        set_errno(data,SYNTAX_ERROR); 
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_identifier)){
        set_errno(data,SYNTAX_ERROR); 
        return false;
    }
    char *identifier = data->token->data.str;
    htab_item *item = htab_find(data->global_symtable,identifier);
    if(item != NULL){   //funkce je jiz deklarovana/definovana
        fprintf(stderr,"Multiple declaration of function %s\n",identifier);
        set_errno(data,REDEFINE_UNDEFINE_VAR);
        return false;
    }
    char *store_identifier = malloc(strlen(identifier)+1);
    if(store_identifier == NULL){
        set_errno(data,INTERNAL_ERROR);
        return false;
    }
    strcpy(store_identifier,identifier);
    
    get_token(data);
    if(!is_token(data,token_type_colon)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!is_token(data,kw_function)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_left_bracket)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!type_list(data)){
        //TODO ERRNO
        return false;
    }

    if(!is_token(data,token_type_left_bracket)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!ret_list(data)){
        //TODO ERRNO
        return false;
    }
    
    htab_declare_function(store_identifier,data);
    free(store_identifier);
    return true;
}

bool fce_def(parser_data_t *data){
    //grammar rule 14
    if(!is_token(data,kw_function)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_identifier)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    char *identifier = data->token->data.str;
    htab_item *item = htab_find(data->global_symtable,identifier);
    char *store_identifier;
    if(item != NULL){
        if(item->type != function_declared){
            set_errno(data,REDEFINE_UNDEFINE_VAR);
            fprintf(stderr,"Trying to redefine function %s\n",identifier);
            return false;
        }
    }
    else{
        store_identifier = malloc(strlen(identifier)+1);
        if(store_identifier == NULL){
            set_errno(data,INTERNAL_ERROR);
            return false;
        }
        strcpy(store_identifier,identifier);
    }

    get_token(data);
    if(!is_token(data,token_type_left_bracket)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!param_def_list(data)){
        //TODO ERRNO
        return false;
    }

    if(!is_token(data,token_type_right_bracket)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);

    if(!ret_list(data)){
        //TODO ERRNO
        return false;
    }

    //define function
    if(item == NULL){
        htab_define_function(store_identifier,data);
    }
    else{
        //TODO pridej listy
        item->type = function_defined;
    }    
    free(store_identifier);

    if(!st_list(data)){
        //TODO ERRNO
        return false;
    }

    if(!is_token(data,kw_end)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return true;
}

bool call_fce(parser_data_t *data){
    //grammar rule 41
    if(!is_token(data,token_type_left_bracket)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!value_list(data)){
        //TODO ERRNO
        return false;
    }
    if(!is_token(data,token_type_right_bracket)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return true;
}

bool type_list(parser_data_t *data){
    //grammar rule 8 
    if(is_token(data,token_type_right_bracket)){
        get_token(data);
        return true;
    }

    //grammar rule 9
    return type(data) && type_list2(data);
}

bool ret_list(parser_data_t *data){
    //grammar rule 13
    switch(data->token->type){
        case kw_end:
        case kw_function:
        case kw_global:
        case kw_if:
        case kw_local:
        case kw_return:
        case kw_while:
        case token_type_EOF:
        case token_type_identifier:
            get_token(data);
            return true;
        break;
        default:
            ;
        break;        
    }

    //grammar rule 12
    if(!is_token(data,token_type_colon)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return type(data) && type_list2(data);
}

bool type(parser_data_t *data){
    //grammar rule 51
    //grammar rule 52
    //grammar rule 53
    switch (data->token->type){
        case kw_integer:
        case kw_number:
        case kw_string:
            get_token(data);
            return true;
        break;              
        default:
        break;
    }
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool type_list2(parser_data_t *data){
    //grammar rule 11
    switch(data->token->type){
        case kw_end:
        case kw_function:
        case kw_global:
        case kw_if:
        case kw_local:
        case kw_return:
        case kw_while:
        case token_type_right_bracket:
        case token_type_EOF:
        case token_type_identifier:
            get_token(data);
            return true;
        break;
        default:
            ;
        break;        
    }

    //grammar rule 10
    if(!is_token(data,token_type_comma)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return type(data) && type_list2(data);
}

bool param_def_list(parser_data_t *data){
    //grammar rule 15
    if(is_token(data,token_type_right_bracket)){       
        return true;
    }

    //grammar rule 16
    return param(data) && param_def_list2(data);
}

bool param_def_list2(parser_data_t *data){
    //grammar rule 18
    if(is_token(data,token_type_right_bracket)){
        return true;
    }

    //grammar rule 17
    if(!is_token(data,token_type_comma)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return param(data) && param_def_list2(data);
}

bool param(parser_data_t *data){
    //grammar rule 19
    if(!is_token(data,token_type_identifier)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    //TODO semnatis identifier
    get_token(data);
    if(!is_token(data,token_type_colon)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return type(data);   
}

bool st_list(parser_data_t *data){
    //grammar rule 21
    if(is_token(data,kw_else) || is_token(data,kw_end)){
        return true;
    } 

    //grammar rule 20
    switch (data->token->type){
        case kw_if:
        case kw_local:
        case kw_while:
        case token_type_identifier:
            return statement(data) && st_list(data);
        break;
        default:
        break;
    }
    
    //grammar rule 40
    if(!is_token(data,kw_return)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return expression_list(data);
}

bool statement(parser_data_t *data){
    if(is_token(data,kw_local)){
        //grammar rule 22
        get_token(data);
        if(!is_token(data,token_type_identifier)){
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        get_token(data);
        if(!is_token(data,token_type_colon)){
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        get_token(data);
        return type(data) && init(data);    
    }
    else if(is_token(data,token_type_identifier)){
        //grammar rule 27
        get_token(data);
        return after_id(data);
    }
    else if(is_token(data,kw_if)){
        //grammar rule 38
        get_token(data);
        if(!is_expression_start(data->token)){
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        if(!expression(data)){
            //TODO ERRNO
            return false;
        }
        if(!is_token(data,kw_then)){
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        get_token(data);
        if(!st_list(data)){
            //TODO ERRNO
            return false;
        }
        if(!is_token(data,kw_else)){
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        get_token(data);
        if(!st_list(data)){
            //TODO ERRNO
            return false;
        }
        if(is_token(data,kw_end)){
            get_token(data);
            return true;
        }
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    else if(is_token(data,kw_while)){
        //grammar rule 39
        get_token(data);
        if(!is_expression_start(data->token)){
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        if(!expression(data)){
            //TODO ERRNO
            return false;
        }
        if(!is_token(data,kw_do)){
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        get_token(data);
        if(!st_list(data)){
            //TODO ERRNO
            return false;
        }
        if(is_token(data,kw_end)){
            get_token(data);
            return true;
        }
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    return false;
}

bool init(parser_data_t *data){
    //grammar rule 23
    switch (data->token->type){
        case kw_end:
        case kw_if:
        case kw_local:
        case kw_return:
        case kw_while:
        case token_type_identifier:
            return true;
        break;    
        default:
        break;
    }

    //grammar rule 24
    if(!is_token(data,token_type_assign)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return init2(data);
}

bool init2(parser_data_t *data){
    if(!is_token(data,token_type_identifier)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }

    htab_item *item = htab_find(data->global_symtable,data->token->data.str);
    if(item != NULL){
        //grammar rule 25
        get_token(data);
        return call_fce(data);
    }

    //grammar rule 26
    return expression(data);
}

bool expression(parser_data_t *data){
    return fake_expression(data);

    //drop your code here:
    return true;
}

bool after_id(parser_data_t *data){
    //grammar rule 28
    if(is_token(data,token_type_left_bracket)){
        return call_fce(data);
    }

    //grammar rule 29
    if(is_token(data,token_type_identifier) || is_token(data,token_type_assign)){
        if(!identif_list(data)){
            //TODO ERRNO
            return false;
        }

        if(!is_token(data,token_type_assign)){
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        get_token(data);
        return assignment(data);
    }
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool assignment(parser_data_t *data){
    if(!is_token(data,token_type_identifier)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    
    htab_item *item = htab_find(data->global_symtable,data->token->data.str);
    if(item != NULL){
        //grammar rule 33
        get_token(data);
        return call_fce(data);
    }

    //grammar rule 32
    return expression(data);
}

bool identif_list(parser_data_t *data){
    //grammar rule 31
    if(is_token(data,token_type_assign)){
        return true;
    }

    //grammar rule 30
    if(!is_token(data,token_type_comma)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_identifier)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return identif_list(data);
}

bool expression_list(parser_data_t *data){
    //grammar rule 35
    if(is_token(data,kw_end) || is_token(data,kw_else)){
        return true;
    }

    //grammar rule 34
    if(is_expression_start(data->token)){
        return expression(data) && expression_list2(data); 
    }
    
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool expression_list2(parser_data_t *data){
    //grammar rule 37
    switch (data->token->type){
        case kw_else:
        case kw_end:
        case kw_if:
        case kw_local:
        case kw_return:
        case kw_while:
        case token_type_identifier:
            return true;
        break;        
        default:
        break;
    }

    //grammar rule 36
    if(!is_token(data,token_type_comma)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data); 
    if(is_expression_start(data->token)){
        return expression(data) && expression_list2(data);
    }

    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool is_expression_start(Token *token){
    switch (token->type){
        case kw_nil:
        case token_type_left_bracket:
        case token_type_integer:
        case token_type_number:
        case token_type_string:
        case token_type_identifier:
        case token_type_length: // #
            return true;           
        break;    
        default:
            return false;
        break;
    }
    return false;
}

bool value_list(parser_data_t *data){
    //grammar rule 42
    if(is_token(data,token_type_right_bracket)){
        return true;
    }

    //grammar rule 43
    if(is_token(data,token_type_string) || is_token(data,token_type_integer) || is_token(data,token_type_number)){
        return value(data) && value_list2(data);
    }
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool value_list2(parser_data_t *data){
    //grammar rule 45
    if(is_token(data,token_type_right_bracket)){
        return true;
    }

    //grammar rule 44
    if(!is_token(data,token_type_comma)){
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return value(data) && value_list2(data);
}

bool value(parser_data_t *data){
    //grammar rule 46 47 48 49 50
    switch (data->token->type){
        case token_type_integer:
        case token_type_number:
        case token_type_string:
        case kw_nil:
            get_token(data);
            return true;
        break;
        case token_type_identifier:
            //TODO control of existence
            get_token(data);
            return true;        
        default:
        break;
    }
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool fake_expression(parser_data_t *data){
    if(!is_expression_start(data->token)){
        //TODO ERRNO
        return false;
    }
    get_token(data);
    return true;
}

