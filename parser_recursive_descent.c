/**
 * @file parser_recursive_descent.c
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#include "parser_recursive_descent.h"

int parser(){
    parser_data_t data;    
    data.token = read_token();
    if(data.token == NULL){
        return LEX_ERROR;
    }
    //print_token(data.token);    
    data.token_list_first = data.token;
    data.global_symtable = htab_init(TABLE_SIZE);
    data.errno = SUCCESS;
    data.expression_list = NULL;
    data.return_list = NULL;
    data.param_list = NULL;
    data.identif_list = NULL;
    data.while_counter = 0;

    const char *label = "__IFJ21__$KEPY$___&START&__";
    char *first_label = calloc(strlen(label)+1,sizeof(char));
    if(first_label == NULL){
        free_parser_data(&data);
        exit(INTERNAL_ERROR);
    }
    strcpy(first_label,label);
    data.program = create_instruction(LABEL,first_label,NULL,NULL);
    data.last_instruction = data.program;

    //syntax analysis
    //get_token(&data);
    if (!intro(&data)){
        if(data.errno == SYNTAX_ERROR){
            fprintf(stderr,"Syntax error\n");   
        }
        free_parser_data(&data);
        return data.errno;
    }
    htab_for_each(data.global_symtable,htab_definition_control);
    free_parser_data(&data);
    return SUCCESS;
}

htab_item * htab_define_function(char * key,parser_data_t *data){
    htab_item * item = htab_lookup_add(data->global_symtable,key);   
    if(item == NULL){
        set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
        return NULL;
    }
    item->type = function_defined;
    item->param_list = data->param_list;
    return item;
}

htab_item *htab_declare_function(char * key,parser_data_t *data){ 
    htab_item * item = htab_lookup_add(data->global_symtable,key);   
    if(item == NULL){
        set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
        return NULL;
    }
    item->type = function_declared;
    item->param_list = data->param_list;
    return item;
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
        fprintf(stderr,"chyba: %s\n",__func__);
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
    fprintf(stderr,"chyba: %s\n",__func__);
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
    fprintf(stderr,"chyba: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool fce_decl(parser_data_t *data){
    //grammar rule 7
    if(!is_token(data,kw_global)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR); 
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_identifier)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR); 
        return false;
    }
    data->param_list = NULL; 
    //control of function existence
    char *identifier = data->token->data.str;
    htab_item *item = htab_find(data->global_symtable,identifier);
    if(item != NULL){   //funkce je jiz deklarovana/definovana
        fprintf(stderr,"Multiple declaration of function %s\n",identifier);
        set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
        return false;
    }
    char *store_identifier = malloc(strlen(identifier)+1);
    if(store_identifier == NULL){
        set_errno(data,INTERNAL_ERROR);
        return false;
    }
    strcpy(store_identifier,identifier);
    
    //syntax chceck continue:
    get_token(data);
    if(!is_token(data,token_type_colon)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!is_token(data,kw_function)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_left_bracket)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!type_list(data)){
        return false;
    }

    if(!is_token(data,token_type_right_bracket)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    htab_item *func_item = htab_declare_function(store_identifier,data);
    if(func_item == NULL){
        return false;
    }
    data->param_list = NULL;  

    get_token(data);
    if(!ret_list(data)){
        return false;
    }
    //fprintf(stderr,"declaration %d\n",data->param_list->data_type);
    func_item->return_list = data->param_list; //it is param list, because <type> store data tokens to param list    
    free(store_identifier);
    data->param_list = NULL; 
    return true;
}

bool fce_def(parser_data_t *data){
    //grammar rule 14    
    if(!is_token(data,kw_function)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_identifier)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    data->param_list = NULL;
    data->return_list = NULL;
    data->while_counter = 0;
    data->frame_counter = 0;
    //semantic check if function is defined
    char *identifier = data->token->data.str;

    htab_item *func_item = htab_find(data->global_symtable,identifier);
    bool was_declared;
    if(func_item != NULL){        
        if(func_item->type != function_declared){
            set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR );
            fprintf(stderr,"Trying to redefine function %s\n",identifier);
            return false;
        }
        was_declared = true;
    }
    else{
        was_declared = false;   
    }
    data->actual_function = strcpy_alloc(data,identifier);

    //create label for function
    instruction_t *label = create_instruction(LABEL,label_generator(data->actual_function,"",0),NULL,NULL); 
    push_instruction(data,label);

    get_token(data);
    if(!is_token(data,token_type_left_bracket)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!param_def_list(data)){
        return false;
    }

    if(!is_token(data,token_type_right_bracket)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);

    if(was_declared){
        if(!full_data_token_compare(data->param_list,func_item->param_list)){
            fprintf(stderr,"Parameters dont match with declaration, function: %s\n",data->actual_function);
            set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
            return false;
        }
        data_token_t *change = func_item->param_list;
        func_item->param_list = data->param_list;
        free_data_token_list(&change);               
    }
    else{
        func_item = htab_define_function(data->actual_function,data);
        if(func_item == NULL){
            return false;
        }
    }

    //param setup
    data->local_symtable = htab_init(TABLE_SIZE);
    size_t param_number = 1;
    data_token_t *walking_item = data->param_list;   
    while(walking_item != NULL){        
        htab_item * item = htab_lookup_add(data->local_symtable,walking_item->key);
        if(item == NULL){
            set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
            return false;
        }
        item->frame_ID = data->frame_counter;
        item->type = walking_item->data_type;
        defvar_3AC(data,allocate_var_name_3AC("LF@",item));
        size_t lenght = snprintf(NULL,0,"LF@%%%zu",param_number) + 1;
        char *param = calloc(lenght,sizeof(char));
        if(param == NULL){
            set_errno(data,INTERNAL_ERROR);
            return false;
        }
        snprintf(param,lenght,"LF@%%%zu",param_number);
        instruction_t *instruction = create_instruction(MOVE,allocate_var_name_3AC("LF@",item),param,NULL);
        push_instruction(data,instruction);
        walking_item = walking_item->next;
        param_number++;
    }

    data->param_list = NULL;

    if(!ret_list(data)){
        return false;
    }

    if(was_declared){
        //fprintf(stderr,"param_list: %d return_list: %d\n",data->param_list->data_type,func_item->return_list->data_type);        
        if(!full_data_token_compare(data->param_list,func_item->return_list)){  //param list is ok, beacuse function <type> store data tokens to param list
            fprintf(stderr,"Return values dont match with declaration, function: %s\n",data->actual_function);
            set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
            return false;
        }
        free_data_token_list(&(data->param_list));
    }
    else{
        func_item->return_list = data->param_list;
        data->param_list = NULL;
    } 

    if(!st_list(data)){
        return false;
    }

    if(!is_token(data,kw_end)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);


    func_item->type = function_defined;
    push_instruction(data,create_instruction(POPFRAME,NULL,NULL,NULL));
    data->param_list = NULL;
    data->return_list = NULL;
    free(data->actual_function);
    data->actual_function = NULL;
    return true;
}

bool call_fce(parser_data_t *data){
    //grammar rule 41
    if(!is_token(data,token_type_left_bracket)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!value_list(data)){
        return false;
    }
    if(!is_token(data,token_type_right_bracket)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return true;
}

bool type_list(parser_data_t *data){
    //grammar rule 8 
    if(is_token(data,token_type_right_bracket)){        
        return true;
    }

    //grammar rule 9
    if(is_token(data,kw_integer) || is_token(data,kw_number) || is_token(data,kw_string)){
        return type(data) && type_list2(data);
    }

    fprintf(stderr,"chyba: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;    
}

bool ret_list(parser_data_t *data){    
    switch(data->token->type){
        //grammar rule 13
        case kw_end:
        case kw_function:
        case kw_global:
        case kw_if:
        case kw_local:
        case kw_return:
        case kw_while:
        case token_type_EOF:
        case token_type_identifier:            
            return true;
        break;
        case token_type_colon: 
            //grammar rule 12
            get_token(data);
            return type(data) && type_list2(data);  
        break;
        default:
            fprintf(stderr,"chyba: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;            
        break;        
    }
    return false; //deathcode
}

bool type(parser_data_t *data){
    //grammar rule 51
    //grammar rule 52
    //grammar rule 53
    data_token_t *token = create_data_token(data);
    switch (data->token->type){
        case kw_integer:
            token->data_type = integer;
        break;
        case kw_number:
            token->data_type = number;
        break;
        case kw_string:
            token->data_type = string;
        break;              
        default:
            fprintf(stderr,"chyba: %s\n",__func__);
            free_data_token(token);
            set_errno(data,SYNTAX_ERROR);
            return false;
        break;
    }
    push_back_data_token(token,&(data->param_list));
    get_token(data);
    return true;
}

bool type_list2(parser_data_t *data){
    //grammar rule 11
    switch(data->token->type){
        //grammar rule 11
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
            return true;
        break;
        case token_type_comma:
            //grammar rule 10
            get_token(data);
            return type(data) && type_list2(data);
        default:
            fprintf(stderr,"chyba: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        break;        
    }
    return false; //deathcode
}

bool param_def_list(parser_data_t *data){
    //grammar rule 15
    if(is_token(data,token_type_right_bracket)){       
        return true;
    }

    //grammar rule 16
    if(is_token(data,token_type_identifier)){
        return param(data) && param_def_list2(data);
    }

    fprintf(stderr,"chyba: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;    
}

bool param_def_list2(parser_data_t *data){
    //grammar rule 18
    if(is_token(data,token_type_right_bracket)){
        return true;
    }

    //grammar rule 17
    if(!is_token(data,token_type_comma)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return param(data) && param_def_list2(data);
}

bool param(parser_data_t *data){
    //grammar rule 19
    if(!is_token(data,token_type_identifier)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    char *param_name = strcpy_alloc(data,data->token->data.str);
    //TODO semnatis identifier add to local hash table
    get_token(data);
    if(!is_token(data,token_type_colon)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    
    get_token(data);

    bool type_retval = type(data);   
    if(type_retval == false){
        return false;
    }
    set_last_data_token_key(param_name,data->param_list);
    return true;
}

bool st_list(parser_data_t *data){
    switch (data->token->type){
        //grammar rule 21
        case kw_else:
        case kw_end:
            return true;
        break;
        //grammar rule 20
        case kw_if:
        case kw_local:
        case kw_while:
        case token_type_identifier:            
            return statement(data) && st_list(data);
        break;
        //grammar rule 40
        case kw_return:
            get_token(data);
            return expression_list(data);
        default:
            fprintf(stderr,"chyba: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        break;
    }
    return false; //deathcode
}

bool statement(parser_data_t *data){
    if(is_token(data,kw_local)){
        //grammar rule 22
        get_token(data);
        if(!is_token(data,token_type_identifier)){
            fprintf(stderr,"chyba: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        }


        //TODO semantics
        get_token(data);
        if(!is_token(data,token_type_colon)){
            fprintf(stderr,"chyba: %s\n",__func__);
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
            fprintf(stderr,"chyba: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        if(!expression(data)){
            return false;
        }
        if(!is_token(data,kw_then)){
            fprintf(stderr,"chyba: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        get_token(data);
        if(!st_list(data)){
            return false;
        }
        if(!is_token(data,kw_else)){
            fprintf(stderr,"chyba: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        get_token(data);
        if(!st_list(data)){
            return false;
        }
        if(is_token(data,kw_end)){
            get_token(data);
            return true;
        }
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    else if(is_token(data,kw_while)){
        //grammar rule 39
        if(data->while_counter == 0){
            data->before_while = data->last_instruction;
        }
        data->while_counter++;
        get_token(data);
        if(!is_expression_start(data->token)){
            fprintf(stderr,"chyba: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        if(!expression(data)){
            return false;
        }
        if(!is_token(data,kw_do)){
            fprintf(stderr,"chyba: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        get_token(data);
        if(!st_list(data)){
            return false;
        }
        if(is_token(data,kw_end)){
            get_token(data);
            data->while_counter--;
            return true;
        }
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    fprintf(stderr,"chyba: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
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
        case token_type_assign:
            //grammar rule 24
            get_token(data);
            return init2(data);
        break;
        default:            
            fprintf(stderr,"chyba: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        break;
    }
    return false; //deathcode
}

bool init2(parser_data_t *data){ 
    if(is_token(data,token_type_identifier)){
        htab_item *item = htab_find(data->global_symtable,data->token->data.str);
        if(item != NULL){
            //grammar rule 25
            get_token(data);
            return call_fce(data);
        }
    }

    if(!is_expression_start(data->token)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    //grammar rule 26
    return expression(data);
}

bool after_id(parser_data_t *data){
    //grammar rule 28
    if(is_token(data,token_type_left_bracket)){
        return call_fce(data);
    }

    //grammar rule 29
    if(is_token(data,token_type_comma) || is_token(data,token_type_assign)){
        if(!identif_list(data)){
            return false;
        }
        if(!is_token(data,token_type_assign)){
            fprintf(stderr,"chyba: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;       
        }
        get_token(data);
        return assignment(data);
    }
    fprintf(stderr,"chyba: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool assignment(parser_data_t *data){
    if(is_token(data,token_type_identifier)){
        htab_item *item = htab_find(data->global_symtable,data->token->data.str);
        if(item != NULL){
            //grammar rule 33
            get_token(data);
            return call_fce(data);
        }
    }

    if(is_expression_start(data->token)){
        //grammar rule 32
        return expression(data);
    }
    fprintf(stderr,"chyba: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;  
}

bool identif_list(parser_data_t *data){
    //grammar rule 31
    if(is_token(data,token_type_assign)){
        return true;
    }

    //grammar rule 30
    if(!is_token(data,token_type_comma)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_identifier)){
        fprintf(stderr,"chyba: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    //TODO semantics
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
    
    fprintf(stderr,"chyba: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool expression_list2(parser_data_t *data){    
    switch (data->token->type){
        //grammar rule 37
        case kw_else:
        case kw_end:
        case kw_if:
        case kw_local:
        case kw_return:
        case kw_while:
        case token_type_identifier:
            return true;
        break;  
        case token_type_comma:
            //grammar rule 36
            get_token(data); 
            if(is_expression_start(data->token)){
                return expression(data) && expression_list2(data);
            }  
        default:
            ;
        break;
    }
    fprintf(stderr,"chyba: %s\n",__func__);
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
            ;
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
    if(is_token(data,token_type_string) || is_token(data,token_type_integer) || is_token(data,token_type_number) || is_token(data,token_type_identifier) || is_token(data,kw_nil)){
        return value(data) && value_list2(data);
    }
    fprintf(stderr,"chyba: %s\n",__func__);
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
        fprintf(stderr,"chyba: %s\n",__func__);
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
    fprintf(stderr,"chyba: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;
}

char *label_generator(char *function,char *what, size_t frame_counter){
    char *label = NULL;
    int lenght = snprintf(NULL,0,"$_%s_%s_%zu",function,what,frame_counter)+1;
    label = calloc(lenght,sizeof(char));
    if (label == NULL){
        exit(INTERNAL_ERROR);
    }
    snprintf(label,lenght,"$_%s_%s_%zu",function,what,frame_counter);
    return label;
}

bool fake_expression(parser_data_t *data){
    if(!is_expression_start(data->token)){
        //TODO ERRNO
        return false;
    }
    get_token(data);
    return true;
}

void set_last_data_token_key(char *key,data_token_t *list){
    if(list == NULL){
        exit(INTERNAL_ERROR);
    }
    data_token_t *walking_item = list;
    while(walking_item->next != NULL){
        walking_item = walking_item->next;
    }
    walking_item->key = key;
}

bool full_data_token_compare(data_token_t *list1,data_token_t *list2){
    data_token_t *walking_item1 = list1;
    data_token_t *walking_item2 = list2;
    while(walking_item1 != NULL && walking_item2 != NULL){
        if(walking_item1->data_type != walking_item2->data_type){
            return false;
        }
        walking_item1 = walking_item1->next;
        walking_item2 = walking_item2->next;
    }
    if((walking_item1 == NULL && walking_item2 != NULL) || (walking_item1 != NULL && walking_item2 == NULL)){
        return false;
    }
    return true;
}

/*

        if a + b then 

     mul a b DEST1 | add e C dest2 | JMP 

A = 5
B = 2

A*B
<E + C >

5   

statement statement1 statement2 statement3

jmp label

{
    statements_t *next;
    action MUL ADD
    symb op1
    symb op2
    symb dest1 
} statements_t

print("mul A 5 DEST1")
print("add DEST1 C DEST2")



A*B + C
*/