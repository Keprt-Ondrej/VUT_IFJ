/**
 * @file parser.c
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#include "parser.h"

void get_token(parser_data_t *data){
    Token *token = read_token();
    if(token == NULL){
        //TODO clear memory
        exit(LEX_ERROR);
    }    
    data->token->next = token;
    data->token = token;
}

bool is_token(parser_data_t *data,Token_type type){
    if (data->token->type == type){
        return true;
    }
   return false;
}

void parser(){
    parser_data_t data;    
    data.token = read_token();
    data.token_list_first = data.token;
    data.global_symtable = htab_init(TABLE_SIZE);
    data.errno = 0;

    //syntax analysis
    if (!intro(&data)){
        fprintf(stderr,"Lexical error");
        return;
    }
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
        return false; //TODO ERRNO
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
    return false; //TODO ERRNO
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
        get_token(data);    
        return call_fce(data) && prog(data);
    }
    else if(is_token(data,token_type_EOF)){
        //grammar rule 5
        return true;
    }
    //lex error 
    data->errno = 42; //TODO ERRNO
    return false;
}

bool fce_decl(parser_data_t *data){
    //grammar rule 7
    if(!is_token(data,kw_global)){
        //TODO ERRNO 
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_identifier)){
        //TODO ERRNO 
        return false;
    }
    
    get_token(data);
    if(!is_token(data,token_type_colon)){
        //TODO ERRNO
        return false;
    }
    get_token(data);
    if(!is_token(data,kw_function)){
        //TODO ERRNO
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_left_bracket)){
        //TODO ERRNO
        return false;
    }
    get_token(data);
    if(!type_list(data)){
        //TODO ERRNO
        return false;
    }

    if(!is_token(data,token_type_left_bracket)){
        //TODO ERRNO
        return false;
    }
    get_token(data);
    if(!ret_list(data)){
        //TODO ERRNO
        return false;
    }
    return true;
}

bool fce_def(parser_data_t *data){
    //grammar rule 14
    if(!is_token(data,kw_function)){
        //TODO ERRNO
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_identifier)){
        //TODO ERRNO
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_left_bracket)){
        //TODO ERRNO
        return false;
    }
    get_token(data);
    if(!param_def_list(data)){
        //TODO ERRNO
        return false;
    }

    if(!is_token(data,token_type_right_bracket)){
        //TODO ERRNO
        return false;
    }
    get_token(data);

    if(!ret_list(data)){
        //TODO ERRNO
        return false;
    }

    if(!st_list(data)){
        //TODO ERRNO
        return false;
    }

    if(!is_token(data,kw_end)){
        //TODO ERRNO
        return false;
    }
    get_token(data);
    return true;
}

bool call_fce(parser_data_t *data){
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
        //TODO ERRNO
        return false;
    }
    get_token(data);
    return type(data) && type_list2(data);
}

bool type(parser_data_t *data){
    return true;
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
        //TODO ERRNO
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
        //TODO ERRNO
        return false;
    }
    get_token(data);
    return param(data) && param_def_list2(data);
}

bool param(parser_data_t *data){
    //grammar rule 19
    if(!is_token(data,token_type_identifier)){
        //TODO ERRNO
        return false;
    }
    //TODO semnatis identifier
    get_token(data);
    if(!is_token(data,token_type_colon)){
        //TODO ERRNO
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
    return true;
}

bool statement(parser_data_t *data){
    if(is_token(data,kw_local)){
        //grammar rule 22
        get_token(data);
        if(!is_token(data,token_type_identifier)){
            //TODO ERRNO
            return false;
        }
        get_token(data);
        if(!is_token(data,token_type_colon)){
            //TODO ERRNO
            return false;
        }
        get_token(data);
        return type(data) && init(data);    
    }
    return true;
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
        //TODO ERRNO
        return false;
    }
    get_token(data);
    return init2(data);
}

bool init2(parser_data_t *data){
    if(!is_token(data,token_type_identifier)){
        return false;
    }

    //decide if it is identifier of function or variable

    //grammar rule 25
    return call_fce(data);

    //grammar rule 26
    return expression(data);
}

bool expression(parser_data_t *data){
    return true;
}