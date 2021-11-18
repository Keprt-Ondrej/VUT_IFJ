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
    //grammar rule 41
    if(!is_token(data,token_type_left_bracket)){
        //TODO ERRNO
        return false;
    }
    get_token(data);
    if(!value_list(data)){
        //TODO ERRNO
        return false;
    }
    if(!is_token(data,token_type_right_bracket)){
        //TODO ERRNO
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
    if(!is_token(data,kw_return)){
        //TODO ERRNO
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
    else if(is_token(data,token_type_identifier)){
        //grammar rule 27
        get_token(data);
        return after_id(data);
    }
    else if(is_token(data,kw_if)){
        //grammar rule 38
        get_token(data);
        if(!is_expression_start(data->token)){
            //TODO ERRNO
            return false;
        }
        if(!expression(data)){
            //TODO ERRNO
            return false;
        }
        if(!is_token(data,kw_then)){
            //TODO ERRNO
            return false;
        }
        get_token(data);
        if(!st_list(data)){
            //TODO ERRNO
            return false;
        }
        if(!is_token(data,kw_else)){
            //TODO ERRNO
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
        //TODO ERRNO
        return false;
    }
    else if(is_token(data,kw_while)){
        //grammar rule 39
        get_token(data);
                if(!is_expression_start(data->token)){
            //TODO ERRNO
            return false;
        }
        if(!expression(data)){
            //TODO ERRNO
            return false;
        }
        if(!is_token(data,kw_do)){
            //TODO ERRNO
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
        //TODO ERRNO
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

    //decide if it is identifier of function or variable TODO

    //grammar rule 25
    return call_fce(data);

    //grammar rule 26
    return expression(data);
}

bool expression(parser_data_t *data){
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
            //TODO ERRNO
            return false;
        }

        return assignment(data);
    }
    //TODO ERRNO
    return false;
}

bool assignment(parser_data_t *data){
    //TODO
    //grammar rule 32
    //grammar rule 33
}

bool identif_list(parser_data_t *data){
    //grammar rule 31
    if(is_token(data,token_type_assign)){
        return true;
    }

    //grammar rule 30
    if(!is_token(data,token_type_comma)){
        //TODO ERRNO
        return false;
    }
    get_token(data);
    if(!is_token(data,token_type_identifier)){
        //TODO ERRNO
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
    
    //TODO ERRNO
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
        //TODO ERRNO
        return false;
    }
    get_token(data); 
    if(is_expression_start(data->token)){
        return expression(data) && expression_list2(data);
    }

    //TODO ERRNO
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


    //grammar rule 43
    return true;
}