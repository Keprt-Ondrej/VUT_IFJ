/**
 * @file parser_recursive_descent.c
 * @brief Definition of recursive descent and semantics action
 * 
 * IFJ-2021 Compiler
 * 
 * @see documentation (dokumentace.pdf) for grammar rules
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/

#include "parser_recursive_descent.h"

int parser(){
    parser_data_t data;    
    data.token = read_token();
    if(data.token == NULL){
        return LEX_ERROR;
    }   
    data.token_list_first = data.token;
    data.global_symtable = htab_init(TABLE_SIZE);
    data.errno = SUCCESS;
    data.expression_list = NULL;
    data.return_list = NULL;
    data.param_list = NULL;
    data.identif_list = NULL;
    data.while_counter = 0;
    data.tmp_counter = 0;

    data.function_calls = create_instruction(LABEL,strcpy_alloc(&data,"__$IFJ_code_21$__$KEPY$__&START&__"),NULL,NULL);
    data.last_call = data.function_calls;
    data.program = create_instruction(EXIT,int_to_string(0),strcpy_alloc(&data,"\n"),NULL);
    data.last_instruction = data.program;

    prepare_build_in_functions(&data);  //setup parametrs and ret vals in global symtable for build in functions
    //start of analysis
    if (!intro(&data)){
        if(data.errno == SYNTAX_ERROR){
            fprintf(stderr,"Syntax error\n");   
        }
        free_parser_data(&data);
        return data.errno;
    }    

    htab_for_each(data.global_symtable,htab_definition_control);    //control if all functions were defined
    printf(".IFJcode21\n");
    generate_code(&(data.function_calls));
    generate_code(&(data.program));
    //genetate_build_in_functions();
    free_parser_data(&data);
    return SUCCESS;
}

htab_item * htab_define_function(char * key,parser_data_t *data){
    htab_item * item = htab_lookup_add(data->global_symtable,key);   
    if(item == NULL){   //function exists
        set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
        return NULL;
    }
    item->type = function_defined;
    item->param_list = data->param_list;
    return item;
}

htab_item *htab_declare_function(char * key,parser_data_t *data){ 
    htab_item * item = htab_lookup_add(data->global_symtable,key);  
    if(item == NULL){   //function exists
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
    else if(is_token(data,token_type_EOF)){ //empty file
        return true;
    }
    else{
        fprintf(stderr,"syntax error in: %s\n",__func__);
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
    fprintf(stderr,"syntax error in: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool prog(parser_data_t* data){
    if(is_token(data,kw_global)){       //declaration of functioon
        //grammar rule 2
        return fce_decl(data) && prog(data);
    }
    else if(is_token(data,kw_function)){    //definition of function
        //grammar rule 3
        return fce_def(data) && prog(data);
    }
    else if(is_token(data,token_type_identifier)){  //function call beetwen declarations
        //grammar rule 4
        char *store_identifier = strcpy_alloc(data,data->token->data.str);
        get_token(data);
        bool ret_val = call_fce(data); //syntax for param list
        if(!ret_val){
            return false;
        }
        htab_item *function = htab_find(data->global_symtable,store_identifier);
        if(function == NULL){   //can not call undefined function
            fprintf(stderr,"Function %s was not declared/defined\n",store_identifier);
            free(store_identifier);
            set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
            return false;
        }
        generate_function_call(data,function);
        return prog(data);
    }
    else if(is_token(data,token_type_EOF)){
        //grammar rule 5
        return true;
    }
    fprintf(stderr,"syntax error in: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool fce_decl(parser_data_t *data){
    //grammar rule 7
    if(!is_token(data,kw_global)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR); 
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_identifier)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
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
    char *store_identifier = strcpy_alloc(data,identifier);
    
    //syntax chceck continue:
    get_token(data);
    if(!is_token(data,token_type_colon)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!is_token(data,kw_function)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_left_bracket)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!type_list(data)){   //set types of parameters on data->param lists 
        return false;
    }

    if(!is_token(data,token_type_right_bracket)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    htab_item *func_item = htab_declare_function(store_identifier,data);
    if(func_item == NULL){
        return false;
    }
    data->param_list = NULL;  

    get_token(data);
    if(!ret_list(data)){    //set types of return values on data->param lists
        return false;
    }

    func_item->return_list = data->param_list; //it is param list, because <type> store data tokens to param list    
    free(store_identifier);
    data->param_list = NULL; 
    return true;
}

bool fce_def(parser_data_t *data){
    //grammar rule 14    
    if(!is_token(data,kw_function)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_identifier)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    data->param_list = NULL;
    data->return_list = NULL;
    data->while_counter = 0;
    data->frame_counter = 0;
    data->tmp_counter = 0;
    //semantic check if function is defined
    char *identifier = data->token->data.str;
    htab_item *func_item = htab_find(data->global_symtable,identifier);
    bool was_declared;
    if(func_item != NULL){  //function found      
        if(func_item->type != function_declared){   //error if function was defined
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
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!param_def_list(data)){ //on data->param_list stores list of parameters with names
        return false;
    }

    if(!is_token(data,token_type_right_bracket)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);

    if(was_declared){
        if(!full_data_token_compare(data->param_list,func_item->param_list)){ //compare data types declaration with definition
            fprintf(stderr,"Parameters dont match with declaration, function: %s\n",data->actual_function);
            set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
            return false;
        }
        data_token_t *change = func_item->param_list;
        func_item->param_list = data->param_list;   //change data types with data types with and name of parameters
        free_data_token_list(&change);               
    }
    else{
        func_item = htab_define_function(data->actual_function,data);   //function wasn t declared so create new definition
        if(func_item == NULL){
            return false;
        }
    }

    //param setup
    data->local_symtable = htab_init(TABLE_SIZE);
    size_t param_number = 1;
    data_token_t *walking_item = data->param_list;   
    while(walking_item != NULL){        //for all parameters
        htab_item * item = htab_lookup_add(data->local_symtable,walking_item->key); //add to local symtable
        if(item == NULL){
            set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
            return false;
        }
        item->frame_ID = data->frame_counter;
        item->type = walking_item->data_type;
        defvar_3AC(data,allocate_var_name_3AC("LF@",item));     //define var in local frame
        size_t lenght = snprintf(NULL,0,"LF@%%%zu",param_number) + 1;   
        char *param = calloc(lenght,sizeof(char));
        if(param == NULL){
            set_errno(data,INTERNAL_ERROR);
            return false;
        }
        snprintf(param,lenght,"LF@%%%zu",param_number);
        instruction_t *instruction = create_instruction(MOVE,allocate_var_name_3AC("LF@",item),param,NULL); //set passed value
        push_instruction(data,instruction);
        walking_item = walking_item->next;
        param_number++;
    }

    data->param_list = NULL;

    if(!ret_list(data)){    //on data->param_list stores list of return values
        return false;
    }

    if(was_declared){
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

    if(!st_list(data)){ //definition of statements
        return false;
    }

    if(!is_token(data,kw_end)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);

    //htab_for_each(data->local_symtable,print_htab_item_values);
    htab_free(data->local_symtable);
    data->local_symtable = NULL;
    data->while_counter = 0;
    func_item->type = function_defined;
    push_instruction(data,create_instruction(POPFRAME,NULL,NULL,NULL));
    push_instruction(data,create_instruction(RETURN,strcpy_alloc(data,"\n"),NULL,NULL));
    data->param_list = NULL;
    data->return_list = NULL;
    free(data->actual_function);
    data->actual_function = NULL;
    return true;
}

bool call_fce(parser_data_t *data){
    //grammar rule 41
    if(!is_token(data,token_type_left_bracket)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!value_list(data)){  //get parameters
        return false;
    }
    if(!is_token(data,token_type_right_bracket)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return true;
}

bool type_list(parser_data_t *data){
    //grammar rule 8 
    if(is_token(data,token_type_right_bracket)){        //empty type list
        return true;
    }

    //grammar rule 9
    if(is_token(data,kw_integer) || is_token(data,kw_number) || is_token(data,kw_string)){
        return type(data) && type_list2(data);
    }

    fprintf(stderr,"syntax error in: %s\n",__func__);
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
            fprintf(stderr,"syntax error in: %s\n",__func__);
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
            fprintf(stderr,"syntax error in: %s\n",__func__);
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
            fprintf(stderr,"syntax error in: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        break;        
    }
    return false; //deathcode
}

bool param_def_list(parser_data_t *data){
    //grammar rule 15
    if(is_token(data,token_type_right_bracket)){       //w empty parametr list
        return true;
    }

    //grammar rule 16
    if(is_token(data,token_type_identifier)){
        return param(data) && param_def_list2(data);
    }

    fprintf(stderr,"syntax error in: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;    
}

bool param_def_list2(parser_data_t *data){
    //grammar rule 18
    if(is_token(data,token_type_right_bracket)){        //end of param list
        return true;
    }

    //grammar rule 17
    if(!is_token(data,token_type_comma)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return param(data) && param_def_list2(data);
}

bool param(parser_data_t *data){
    //grammar rule 19
    if(!is_token(data,token_type_identifier)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    char *param_name = strcpy_alloc(data,data->token->data.str);
    get_token(data);
    if(!is_token(data,token_type_colon)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    
    get_token(data);

    bool type_retval = type(data);   //add data token on tail of data->param_list
    if(type_retval == false){
        return false;
    }
    set_last_data_token_key(param_name,data->param_list);   //set last param name
    return true;
}

bool st_list(parser_data_t *data){
    switch (data->token->type){
        //grammar rule 21
        case kw_else:
        case kw_end:
            return true;    //end of statement list
        break;
        //grammar rule 20
        case kw_if:
        case kw_local:
        case kw_while:
        case token_type_identifier:             //generate statement
            return statement(data) && st_list(data);
        break;
        //grammar rule 40
        case kw_return:     //generate return on oned of statement list
            get_token(data);
            bool ret_val = expression_list(data);      //setup return values on data->expression_list
            if(!ret_val){
                return false;
            }
            htab_item *function = htab_find(data->global_symtable,data->actual_function);   //find actual function for type compability of return values
            if(function == NULL){
                set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
                fprintf(stderr,"Calling undefined function %s\n",data->actual_function);
                return false;
            }
            data_token_t *walking_return = function->return_list;
            precedence_token_t *walking_expression = data->expression_list;
            size_t return_counter = 1;
            size_t lenght;
            char *return_name;
            /*    TODO RUN THIS
            while(walking_return != NULL && walking_expression != NULL){
                if(walking_return->data_type != walking_expression->data_type){
                    if(walking_return->data_type == number && walking_expression->data_type == integer){    //expression can be retyped 
                        push_instruction(data,create_instruction(INT2FLOAT,strcpy_alloc(data,walking_expression->identifier),strcpy_alloc(data,walking_expression->identifier),NULL));
                    }
                    else{
                        fprintf(stderr,"wrong types of expression and return type in function %s\n",data->actual_function);
                        set_errno(data,SEM_ERROR_TYPE_NUMBER_PARAM_RET_INCORRECT);
                        return false;
                    }
                }
                lenght = snprintf(NULL,0,"LF@$%zu",return_counter) + 1;     //setup place for moving value from expression
                return_name = calloc(lenght,sizeof(char));
                if(return_name == NULL){
                    set_errno(data,INTERNAL_ERROR);
                    return false;
                }
                snprintf(return_name,0,"LF@$%zu",return_counter);
                push_instruction(data,create_instruction(MOVE,return_name,walking_expression->identifier,NULL));    //move expression value to return place
                walking_expression->identifier = NULL;
                walking_return = walking_return->next;
                walking_expression = walking_expression->next;
                return_counter++;
            }
            if(walking_expression != NULL && walking_return == NULL){       // too much expressions for return, if number of expressions is lower than return list, nil is returned, which was setup in function calll
                set_errno(data,SEM_ERROR_TYPE_NUMBER_PARAM_RET_INCORRECT);
                fprintf(stderr,"Too much expressions in return statement, function %s\n",data->actual_function);
                return false;
            }     
            */
            //TODO free expression list
            data->expression_list = NULL;
            push_instruction(data,create_instruction(POPFRAME,NULL,NULL,NULL));
            push_instruction(data,create_instruction(RETURN,NULL,NULL,NULL));
            return true;
        break;
        default:
            print_token(data->token);
            fprintf(stderr,"syntax error in: %s\n",__func__);
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
            fprintf(stderr,"syntax error in: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        htab_item *variable = htab_find(data->global_symtable,data->token->data.str);       //variable name can not be same as name of function
        if(variable != NULL){   
            fprintf(stderr,"trying create variable name same as function identifier %s\n",variable->key);
            set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
            return false;
        }
        variable =  htab_lookup_add(data->local_symtable,data->token->data.str);        //add variable to first level of local symtable stack
        if(variable == NULL){
            fprintf(stderr,"trying to redefine variable %s\n",data->token->data.str);
            set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
            return false;
        }
        variable->frame_ID = data->frame_counter;       //set actual frame identifier

        get_token(data);
        if(!is_token(data,token_type_colon)){
            fprintf(stderr,"syntax error in: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        get_token(data);

        bool ret_val = type(data);      //setup on data->param_list type of variable
        if(!ret_val){
            return false;
        }

        variable->type = data->param_list->data_type;
        defvar_3AC(data,allocate_var_name_3AC("LF@",variable));
        free_data_token(data->param_list);
        data->param_list = NULL;
        char *var_name = variable->key;
        variable->key = "$$"; //renaming temporary var, because we cannot call function with it, because we are defininf it now
        ret_val = init(data);       //setup init values 
        if(!ret_val){
            return false;
        } 
        variable->key = var_name;   //changing name back
        if(data->return_list != NULL){
            //init from function call, data are passed directly from temporary frame to var, but type controll needed, return list of called function is stored on data->return_list
            if(variable->type != data->return_list->data_type){     //type control
                if(variable->type == number && data->return_list->data_type == integer){            //can be retyped
                    push_instruction(data,create_instruction(INT2FLOAT,strcpy_alloc(data,"TF@$1"),strcpy_alloc(data,"TF@$1"),NULL));
                }
                else{
                    fprintf(stderr,"wrong types in defining variable %s from function call\n",variable->key);
                    set_errno(data,SEM_ERROR_ASSIGN_COMMAND);
                    return false;
                }
            }
            push_instruction(data,create_instruction(MOVE,allocate_var_name_3AC("LF@",variable),strcpy_alloc(data,"TF@$1"),NULL));      //storing value to variable
            data->return_list = NULL;
        }
        else if(data->expression_list != NULL){             
            //init from expression  
            if(variable->type != data->expression_list->data_type){     //control type
                if(variable->type == number && data->expression_list->data_type == integer){    //can be retepyde
                    push_instruction(data,create_instruction(INT2FLOAT,strcpy_alloc(data,data->expression_list->identifier),strcpy_alloc(data,data->expression_list->identifier),NULL));
                }
                else{
                    fprintf(stderr,"wrong types in defining variable %s from expression init\n",variable->key);
                    set_errno(data,SEM_ERROR_ASSIGN_COMMAND);
                    return false;
                }
            }
            push_instruction(data,create_instruction(MOVE,allocate_var_name_3AC("LF@",variable),data->expression_list->identifier,NULL));   //move value from expression to variable
            //todo free expression token
            data->expression_list = NULL;
        }
        else{
            //default init on nil
            push_instruction(data,create_instruction(MOVE,allocate_var_name_3AC("LF@",variable),strcpy_alloc(data,nil_string),NULL));
        }
        return true;    
    }
    else if(is_token(data,token_type_identifier)){
        //grammar rule 27
        data_token_t *token = create_data_token();
        token->key = strcpy_alloc(data,data->token->data.str);
        data->identif_list = token;   
        get_token(data);
        bool ret_val = after_id(data);
        if(!ret_val){
            return false;
        } 
        if(data->identif_list == NULL){
            //function was called,
            return true;
        }
        //we continued in creating identifier list and we assign values to them
        if(data->return_list != NULL){          //function was called after =
            //assignment from function call
            size_t ret_val_counter = 1;
            data_token_t *walking_identif = data->identif_list;
            data_token_t *walking_return = data->return_list;
            size_t lenght;
            char *return_place;
            while(walking_identif != NULL && walking_return != NULL){       // assign to specific identif
                htab_item *variable = htab_find_variable(data->local_symtable,walking_identif->key);    //control of existence identif
                if(variable == NULL){
                    fprintf(stderr,"undefined variable %s used in assignment\n",walking_identif->key);
                    set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
                    return false;
                }
                lenght = snprintf(NULL,0,"TF@$%zu",ret_val_counter) + 1;    //setup name for place, where is return value on temporary frame
                return_place = calloc(lenght,sizeof(char));
                if(return_place == NULL){
                    set_errno(data,INTERNAL_ERROR);
                    return false;
                }
                snprintf(return_place,lenght,"TF@$%zu",ret_val_counter);
                if(variable->type != walking_return->data_type){    //type control
                    if(variable->type == number && walking_return->data_type == integer){   //return value can be retyped
                        push_instruction(data,create_instruction(INT2FLOAT,strcpy_alloc(data,return_place),strcpy_alloc(data,return_place),NULL));
                    }
                    else{
                        fprintf(stderr,"wrong type of %s used in assignment\n",walking_identif->key);
                        set_errno(data,SEM_ERROR_TYPE_NUMBER_PARAM_RET_INCORRECT);
                        return false;
                    }
                }
                push_instruction(data,create_instruction(MOVE,allocate_var_name_3AC("LF@",variable),return_place,NULL));    //assign value to variable
                ret_val_counter++;
                walking_identif = walking_identif->next;
                walking_return = walking_return->next;
            }
            if(walking_identif != NULL && walking_return == NULL){      //must assign to all identifiers
                set_errno(data,SEM_ERROR_TYPE_NUMBER_PARAM_RET_INCORRECT);  
                fprintf(stderr,"wrong number of return values for assingment\n");
                return false;
            }
            free_data_token_list(&(data->identif_list));
            data->identif_list = NULL;
            data->return_list = NULL; 
        }
        else{   
            //assignment from expression
            data_token_t *walking_identif = data->identif_list;
            precedence_token_t *walking_expression = data->expression_list;
            size_t lenght;
            char *return_place;
            while(walking_identif != NULL && walking_expression != NULL){
                htab_item *variable = htab_find_variable(data->local_symtable,walking_identif->key);    //identifier must be defined
                if(variable == NULL){
                    fprintf(stderr,"undefined variable %s used in assignment\n",walking_identif->key);
                    set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
                    return false;
                }
                if(variable->type != walking_expression->data_type){    //type control
                    if(variable->type == number && walking_expression->data_type == integer){   //can be retyped
                        push_instruction(data,create_instruction(INT2FLOAT,strcpy_alloc(data,walking_expression->identifier),strcpy_alloc(data,walking_expression->identifier),NULL));
                    }
                    else{
                        fprintf(stderr,"wrong type of %s used in assignment\n",walking_identif->key);
                        set_errno(data,SEM_ERROR_TYPE_NUMBER_PARAM_RET_INCORRECT);
                        return false;
                    }
                }
                push_instruction(data,create_instruction(MOVE,allocate_var_name_3AC("LF@",variable),walking_expression->identifier,NULL));  //assign value from expression to variable
                walking_expression->identifier = NULL;
                walking_identif = walking_identif->next;
                walking_expression = walking_expression->next;
            }
            if((walking_identif != NULL && walking_expression == NULL) || (walking_identif == NULL && walking_expression != NULL)){     //wron numbers of identifier/expressions
                set_errno(data,SEM_ERROR_ASSIGN_COMMAND);
                fprintf(stderr,"wrong number of return values for assingment\n");
                return false;
            }
            free_data_token_list(&(data->identif_list));
            data->identif_list = NULL;
            //todo free precedence token            
            data->expression_list = NULL; 
        }
        return true;
    }
    else if(is_token(data,kw_if)){
        //grammar rule 38
        data->frame_counter++;
        size_t actual_if = data->frame_counter;        
        get_token(data);
        if(!is_expression_start(data->token)){
            fprintf(stderr,"syntax error in: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        if(!expression(data)){
            return false;
        }
        //condition_re_type(data,actual_if);
        push_instruction(data,create_instruction(JUMPIFNEQ,label_generator(data->actual_function,"else",actual_if),strcpy_alloc(data,bool_string_true),NULL));
        if(!is_token(data,kw_then)){
            fprintf(stderr,"syntax error in: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        htab_t *table = htab_init(TABLE_SIZE);
        table->next = data->local_symtable;
        data->local_symtable = table;        
        get_token(data);        
        if(!st_list(data)){
            return false;
        }
        htab_clear(data->local_symtable);   //"new" symtable level for else 
        push_instruction(data,create_instruction(JUMP,label_generator(data->actual_function,"endif",actual_if),NULL,NULL));
        data->frame_counter++;
        if(!is_token(data,kw_else)){
            fprintf(stderr,"syntax error in: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        push_instruction(data,create_instruction(LABEL,label_generator(data->actual_function,"else",actual_if),NULL,NULL));
        get_token(data);
        if(!st_list(data)){
            return false;
        }
        if(is_token(data,kw_end)){
            table = data->local_symtable;
            data->local_symtable = data->local_symtable->next;
            table->next =NULL;
            htab_free(table);
            push_instruction(data,create_instruction(LABEL,label_generator(data->actual_function,"endif",actual_if),NULL,NULL)); 
            data->frame_counter++;
            get_token(data);           
            return true;            
        }       
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    else if(is_token(data,kw_while)){
        //grammar rule 39
        if(data->while_counter == 0){       //setup place where should be def war from inside loop
            data->before_while = data->last_instruction;
        }
        data->while_counter++;  // counter > 0 indicates we are in loop, def_var_3AC place defvar instructions on correct place
        data->frame_counter++;
        size_t actual_while = data->frame_counter;
        push_instruction(data,create_instruction(LABEL,label_generator(data->actual_function,"while_before_exp",actual_while),NULL,NULL));
        get_token(data);        
        if(!is_expression_start(data->token)){
            fprintf(stderr,"syntax error in: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        if(!expression(data)){
            return false;
        }
        //condition_re_type(data,actual_while);
        push_instruction(data,create_instruction(JUMPIFNEQ,label_generator(data->actual_function,"while_end",actual_while),strcpy_alloc(data,bool_string_true),NULL/*TODO data->expression_list->identifier*/));
        if(!is_token(data,kw_do)){
            fprintf(stderr,"syntax error in: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        }
        get_token(data);
        htab_t *table = htab_init(TABLE_SIZE);
        table->next = data->local_symtable;
        data->local_symtable = table;        
        if(!st_list(data)){
            return false;
        }
        push_instruction(data,create_instruction(JUMP,label_generator(data->actual_function,"while_before_exp",actual_while),NULL,NULL));
        if(is_token(data,kw_end)){
            push_instruction(data,create_instruction(LABEL,label_generator(data->actual_function,"while_end",actual_while),NULL,NULL));
            table = data->local_symtable;
            data->local_symtable = data->local_symtable->next;
            table->next = NULL;
            htab_free(table);
            get_token(data);
            data->frame_counter++;
            data->while_counter--;
            return true;
        }
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    fprintf(stderr,"syntax error in: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool init(parser_data_t *data){
    //grammar rule 23
    switch (data->token->type){
        case kw_end:
        case kw_else:
        case kw_if:
        case kw_local:
        case kw_return:
        case kw_while:
        case token_type_identifier:
            return true;    //variable without initialization
        break;
        case token_type_assign:
            //grammar rule 24
            get_token(data);
            return init2(data);
        break;
        default:            
            fprintf(stderr,"syntax error in: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        break;
    }
    return false; //deathcode
}

bool init2(parser_data_t *data){ 
    if(is_token(data,token_type_identifier)){
        htab_item *item = htab_find(data->global_symtable,data->token->data.str);
        if(item != NULL){   //initialization wuth function call
            //grammar rule 25            
            get_token(data);
            bool ret_val = call_fce(data);
            if(!ret_val){
                return false;
            }
            if(item->return_list == NULL){
                fprintf(stderr,"can not initialize variable with function %s with no return value\n",item->key);
                set_errno(data,SEM_ERROR_TYPE_NUMBER_PARAM_RET_INCORRECT);
                return false;
            }
            data->return_list = item->return_list;
            generate_function_call(data,item);
            return true;
        }
    }

    if(!is_expression_start(data->token)){      //init by expression
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    //grammar rule 26
    return expression(data);
}

bool after_id(parser_data_t *data){
    //grammar rule 28
    if(is_token(data,token_type_left_bracket)){     //function call
        htab_item *function = htab_find(data->global_symtable,data->identif_list->key);
        if(function == NULL){
            fprintf(stderr,"Function %s was not declared/defined\n",data->identif_list->key);
            set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
            return false;
        }
        bool ret_val = call_fce(data);  //set parameters on data->param list
        if(!ret_val){
            return false;
        }
        generate_function_call(data,function);
        free_data_token(data->identif_list);
        data->identif_list = NULL;
        return true;
    }

    //grammar rule 29
    if(is_token(data,token_type_comma) || is_token(data,token_type_assign)){    // assignment 
        if(!identif_list(data)){    //create list of identifiers on data->identif_list
            return false;
        }
        if(!is_token(data,token_type_assign)){
            fprintf(stderr,"syntax error in: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;       
        }
        get_token(data);
        return assignment(data);
    }
    fprintf(stderr,"syntax error in: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool assignment(parser_data_t *data){
    if(is_token(data,token_type_identifier)){       //assignment with function call
        htab_item *item = htab_find(data->global_symtable,data->token->data.str);
        if(item != NULL){
            //grammar rule 33
            get_token(data);
            bool ret_val = call_fce(data);
            if(!ret_val){
                return false;
            }
            data->return_list = item->return_list;
            generate_function_call(data,item);
            return true;
        }
    }

    if(is_expression_start(data->token)){   //assignment from expression
        //grammar rule 32
        return expression(data);
    }
    fprintf(stderr,"syntax error in: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;  
}

bool identif_list(parser_data_t *data){
    //grammar rule 31
    if(is_token(data,token_type_assign)){   //end of identifier list
        return true;
    }

    //grammar rule 30
    if(!is_token(data,token_type_comma)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_identifier)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    //store idendifier to identifier list
    data_token_t *token = create_data_token();
    token->key = strcpy_alloc(data,data->token->data.str);
    push_back_data_token(token,&(data->identif_list));
    get_token(data);
    return identif_list(data);
}

bool expression_list(parser_data_t *data){
    //grammar rule 35
    if(is_token(data,kw_end) || is_token(data,kw_else)){    //end of expression list in return
        return true;
    }

    //grammar rule 34
    if(is_expression_start(data->token)){                       //evaluate expression ant ask if there is another expression
        return expression(data) && expression_list2(data);      //expresions evaluations are stored at data->expression_list
    }
    
    fprintf(stderr,"syntax error in: %s\n",__func__);
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
            return true;                //end of expression list
        break;  
        case token_type_comma:
            //grammar rule 36
            get_token(data); 
            if(is_expression_start(data->token)){   //another expression expected
                return expression(data) && expression_list2(data);
            }  
        default:
            ;
        break;
    }
    fprintf(stderr,"syntax error in: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool is_expression_start(Token *token){ //control if is token start of expression
    switch (token->type){
        case kw_nil:
        case token_type_left_bracket:
        case token_type_integer:
        case token_type_number:
        case token_type_string:
        case token_type_identifier:
        case token_type_length: // #
        case token_type_minus:
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
    if(is_token(data,token_type_right_bracket)){    //emty value list
        return true;
    }

    //grammar rule 43
    if(is_token(data,token_type_string) || is_token(data,token_type_integer) || is_token(data,token_type_number) || is_token(data,token_type_identifier) || is_token(data,kw_nil)){
        return value(data) && value_list2(data);
    }
    fprintf(stderr,"syntax error in: %s\n",__func__);
    set_errno(data,SYNTAX_ERROR);
    return false;
}

bool value_list2(parser_data_t *data){
    //grammar rule 45
    if(is_token(data,token_type_right_bracket)){    //end of value list
        return true;
    }

    //grammar rule 44
    if(!is_token(data,token_type_comma)){
        fprintf(stderr,"syntax error in: %s\n",__func__);
        set_errno(data,SYNTAX_ERROR);
        return false;
    }
    get_token(data);
    return value(data) && value_list2(data);        //value list continue
}

bool value(parser_data_t *data){
    //grammar rule 46 47 48 49 50
    data_token_t *token = create_data_token();  //store data token on data->param_list
    switch (data->token->type){
        case token_type_integer:
            token->data_type = integer;
            token->key = int_to_string(data->token->data.type_integer);
        break;
        case token_type_number:
            token->data_type = number;
            token->key = double_to_string(data->token->data.type_double);
        break;
        case token_type_string:
            token->data_type = string;
            token->key = string_to_string(data->token->data.str);
        break;
        case kw_nil:
            token->data_type = nil;
            token->key = strcpy_alloc(data,nil_string);
        break;
        case token_type_identifier: ;
            htab_item *variable = htab_find_variable(data->local_symtable,data->token->data.str);
            if(variable == NULL){
                set_errno(data,SEM_ERROR_REDEFINE_UNDEFINE_VAR);
                free_data_token(token);
                return false;
            }
            token->data_type = identifier;     
            token->key = strcpy_alloc(data,data->token->data.str);       
        break;
        default:
            fprintf(stderr,"syntax error in: %s\n",__func__);
            set_errno(data,SYNTAX_ERROR);
            return false;
        break;
    }

    push_back_data_token(token,&(data->param_list));    //push on end of data->param_list
    get_token(data);
    return true;
}

char *label_generator(char *function,char *what, size_t frame_counter){
    char *label = NULL;
    size_t lenght = snprintf(NULL,0,"$_%s_%s_%zu",function,what,frame_counter)+1;
    label = calloc(lenght,sizeof(char));
    if (label == NULL){
        exit(INTERNAL_ERROR);
    }
    snprintf(label,lenght,"$_%s_%s_%zu",function,what,frame_counter);
    return label;
}

bool fake_expression(parser_data_t *data){  //TODO DELETE
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
    while(walking_item->next != NULL){  //walk on end of list
        walking_item = walking_item->next;
    }
    walking_item->key = key;    //set last token key
}

bool full_data_token_compare(data_token_t *list1,data_token_t *list2){  
    data_token_t *walking_item1 = list1;
    data_token_t *walking_item2 = list2;
    while(walking_item1 != NULL && walking_item2 != NULL){  //walk through tokens and compare data types
        if(walking_item1->data_type != walking_item2->data_type){
            return false;
        }
        walking_item1 = walking_item1->next;
        walking_item2 = walking_item2->next;
    }
    if((walking_item1 == NULL && walking_item2 != NULL) || (walking_item1 != NULL && walking_item2 == NULL)){   //must both end at NULL
        return false;
    }
    return true;
}

void prepare_build_in_functions(parser_data_t *data){
    data->param_list = NULL;
    htab_item *fce_item = htab_define_function("reads",data);
    data_token_t *token = create_data_token();
    token->data_type = string;
    fce_item->return_list = token;
    data->param_list = NULL;

    fce_item = htab_define_function("readi",data);
    token = create_data_token();
    token->data_type = integer;
    fce_item->return_list = token;
    data->param_list = NULL;

    fce_item = htab_define_function("readn",data);
    token = create_data_token();
    token->data_type = number;
    fce_item->return_list = token;
    data->param_list = NULL;

    fce_item = htab_define_function("write",data);  //write can have many parametrs, no type control required
    token = create_data_token();
    token->data_type = integer;
    fce_item->return_list = NULL;
    data->param_list = NULL;

    data->param_list = create_data_token();
    data->param_list->data_type = number;
    fce_item = htab_define_function("tointeger",data);
    token = create_data_token();
    token->data_type = integer;
    fce_item->return_list = token;
    data->param_list = NULL;

    data->param_list = create_data_token();
    data->param_list->data_type = string;
    token = create_data_token();
    token->data_type = number;
    push_back_data_token(token,&(data->param_list));
    token = create_data_token();
    token->data_type = number;
    push_back_data_token(token,&(data->param_list));
    fce_item = htab_define_function("substr",data);
    token = create_data_token();
    token->data_type = string;
    fce_item->return_list = token;
    data->param_list = NULL;

    data->param_list = create_data_token();
    data->param_list->data_type = string;
    token = create_data_token();
    token->data_type = integer;
    push_back_data_token(token,&(data->param_list));
    fce_item = htab_define_function("ord",data);
    token = create_data_token();
    token->data_type = integer;
    fce_item->return_list = token;
    data->param_list = NULL;

    data->param_list = create_data_token();
    data->param_list->data_type = integer;
    fce_item = htab_define_function("chr",data);
    token = create_data_token();
    token->data_type = string;
    fce_item->return_list = token;
    data->param_list = NULL;
    data->return_list = NULL;
}

void generate_function_call(parser_data_t *data,htab_item *function){
    push_instruction(data,create_instruction(CREATEFRAME,NULL,NULL,NULL));  //future LF of function
    size_t param_counter = 1;
    data_token_t *walking_item_function = function->param_list;
    data_token_t *walking_item = data->param_list;
    size_t lenght;
    char *param_def;
    char *param;
    if(strcmp(function->key,"write") == 0){     //write function have another call, no type control
        data_token_t *delete = NULL;
        reverse_list(&(data->param_list));      //passing argument by stack and in frame only count of parameters
        param_counter = 0;
        while(data->param_list != NULL){        //while end of param list
            if(walking_item->data_type == identifier){          //if param is identifier
                htab_item *variable = htab_find_variable(data->local_symtable,walking_item->key);       //find, if variable exists
                if(variable == NULL){
                    free_parser_data(data);
                    exit(SEM_ERROR_REDEFINE_UNDEFINE_VAR);
                }
                push_instruction(data,create_instruction(PUSHS,allocate_var_name_3AC("LF@",variable),NULL,NULL));
            }
            else{
                push_instruction(data,create_instruction(PUSHS,strcpy_alloc(data,data->param_list->key),NULL,NULL));     //another term push immedietly on stack      
            }
            delete = data->param_list;
            data->param_list = data->param_list->next;
            free_data_token(delete);
            param_counter++;
        }

        push_instruction(data,create_instruction(DEFVAR,strcpy_alloc(data,"TF@%1"),NULL,NULL));     //store param counter to the frame
        lenght = snprintf(NULL,0,"int@%zu",param_counter) + 1;
        param_def = calloc(lenght,sizeof(char));
        if(param_def == NULL){
            free_parser_data(data);
            exit(INTERNAL_ERROR);
        }
        snprintf(param_def,lenght,"int@%zu",param_counter);
        push_instruction(data,create_instruction(MOVE,strcpy_alloc(data,"TF@%1"),param_def,NULL));
        push_instruction(data,create_instruction(PUSHFRAME,NULL,NULL,NULL));
        push_instruction(data,create_instruction(CALL,label_generator(function->key,"",0),NULL,NULL));
        data->param_list = NULL;
        return;
    }
    while(walking_item_function != NULL && walking_item != NULL){       //param control and setup for all 
        if(walking_item->data_type == identifier){  //TODO otestovat
            htab_item *variable = htab_find_variable(data->local_symtable,walking_item->key);   //find variable
            if(variable == NULL){
                free_parser_data(data);
                exit(SEM_ERROR_REDEFINE_UNDEFINE_VAR);
            }
            if(variable->type != walking_item_function->data_type){     //type control
                if(walking_item_function->data_type == number && variable->type == integer){        //if can be retyped
                    char *tmp = allocate_new_tmp_name(data,"TF@");
                    defvar_3AC(data,strcpy_alloc(data,tmp));
                    push_instruction(data,create_instruction(INT2FLOAT,strcpy_alloc(data,tmp),allocate_var_name_3AC("LF@",variable),NULL));     //prepare one operand
                    param = tmp;
                }
                else{
                    free_parser_data(data);     //type incompability
                    fprintf(stderr,"wrong data type of parametr in function call %s\n",function->key);
                    exit(SEM_ERROR_TYPE_NUMBER_PARAM_RET_INCORRECT);            }
                }                
            else{
                param = allocate_var_name_3AC("LF@",variable);      //prepare one operand
            }           
        }
        else{
            if(walking_item->data_type != walking_item_function->data_type){    //type control
                if(walking_item_function->data_type == number && walking_item->data_type == integer){       //if can be retyped
                    char *tmp = allocate_new_tmp_name(data,"TF@");  
                    defvar_3AC(data,strcpy_alloc(data,tmp));
                    push_instruction(data,create_instruction(INT2FLOAT,strcpy_alloc(data,tmp),walking_item->key,NULL));
                    walking_item->key = NULL;
                    param = tmp;        //one operand setup
                }
                else{
                    free_parser_data(data);
                    fprintf(stderr,"wrong data type of parametr in function call %s\n",function->key);
                    exit(SEM_ERROR_TYPE_NUMBER_PARAM_RET_INCORRECT);
                }                
            }
            else{
                param = walking_item->key;      //one operand setup
                walking_item->key = NULL;
            }            
        }        
        lenght = snprintf(NULL,0,"TF@%%%zu",param_counter) + 1;     
        param_def = calloc(lenght,sizeof(char));
        if(param == NULL){
            free_parser_data(data);
            exit(INTERNAL_ERROR);
        }
        snprintf(param_def,lenght,"TF@%%%zu",param_counter);        //setup place, where is operand stored
        defvar_3AC(data,strcpy_alloc(data,param_def));
        
        push_instruction(data,create_instruction(MOVE,param_def,param,NULL));       //set data for call
        walking_item_function = walking_item_function->next;
        walking_item = walking_item->next;
        param_counter++;
    }
    if(walking_item_function != walking_item){      
        fprintf(stderr,"wrong number of parametrs in function call %s\n",function->key);
        exit(SEM_ERROR_TYPE_NUMBER_PARAM_RET_INCORRECT);
    }
    free_data_token_list(&(data->param_list));
    data->param_list = NULL;
    param_counter = 1;
    walking_item_function = function->return_list;  //setup return values on nil
    while(walking_item_function != NULL){
        lenght = snprintf(NULL,0,"TF@$%zu",param_counter) + 1;
        param_def = calloc(lenght,sizeof(char));
        if(param == NULL){
            free_parser_data(data);
            exit(INTERNAL_ERROR);
        }
        snprintf(param_def,lenght,"TF@$%zu",param_counter);
        defvar_3AC(data,strcpy_alloc(data,param_def));
        push_instruction(data,create_instruction(MOVE,param_def,strcpy_alloc(data,nil_string),NULL));
        walking_item_function = walking_item_function->next;
        param_counter++;
    }
    push_instruction(data,create_instruction(PUSHFRAME,NULL,NULL,NULL));
    push_instruction(data,create_instruction(CALL,label_generator(function->key,"",0),NULL,NULL));
}

void reverse_list(data_token_t **place){    //reverse list of data tokens
    data_token_t *actual = *place;
    data_token_t *next = NULL;
    data_token_t *prev = NULL;
    while(actual != NULL){
        next = actual->next;
        actual->next = prev;
        prev = actual;
        actual = next;
    }
    *place = prev;
}

void condition_re_type(parser_data_t *data,size_t ID){      //condition must convert number, integer and string on bool value
    char * tmp_name = allocate_new_tmp_name(data,"LF@");
    push_instruction(data,create_instruction(DEFVAR,strcpy_alloc(data,tmp_name),NULL,NULL));
    push_instruction(data,create_instruction(TYPE,strcpy_alloc(data,tmp_name),strcpy_alloc(data,data->expression_list->identifier),NULL));
    push_instruction(data,create_instruction(JUMPIFEQ,label_generator(data->actual_function,"before_if",ID),strcpy_alloc(data,tmp_name),string_to_string("nil")));
    push_instruction(data,create_instruction(JUMPIFEQ,label_generator(data->actual_function,"before_if",ID),strcpy_alloc(data,tmp_name),string_to_string("bool")));
    push_instruction(data,create_instruction(MOVE,strcpy_alloc(data,data->expression_list->identifier),strcpy_alloc(data,bool_string_true),NULL));
    push_instruction(data,create_instruction(LABEL,label_generator(data->actual_function,"before_if",ID),NULL,NULL));
    free(tmp_name);    
}
