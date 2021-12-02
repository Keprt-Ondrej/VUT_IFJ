
#include "parser_precedence.h"

#define buffer_length 50

int hello_world(){
    return 5;
}

/*
Precedence precedence_compare(Token_type left_type, Token_type right_type){

struct enum{
    P,  //push
    R,  //reduse
    E,  //equal
    N,  //no precedence
}Compare_result;

    char precedence_table[21][21] = {
//  r     # | * | / | // | + | - | .. | < | <= | > | >= | == | ~= | ( | ) | id | int | num | str | nil | $       l
        { N , R , R , R  , R , R , N  , N , N  , N , N  , N  , N  , N , N , P  ,  P  ,  P  ,  P  ,  P  , R }, // *
        { N , R , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // /
        { N , R , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // //
        { N , R , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // +
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // -
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // ..
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // <
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // <=
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // >
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // >=
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // #
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // ==
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // ~=
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // (
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // )
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // id
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // int
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // int
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // num
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // str
        { N , N , N , N  , N , N , N  , N , N  , N , N  , N  , N  , N , N , N  ,  N  ,  N  ,  N  ,  N  , R }, // nil
        { P , P , P , P  , P , P , P  , P , P  , P , P  , P  , P  , P , P , P  ,  P  ,  P  ,  P  ,  P  , R }, // $
    }


    switch (precedence_table[left_type][right_type])//switch        
    {
        case P:
            return push;
        break;

        case R:
            return reduse;
        break;
        
        case E:
            return equal;
        break;

        case N:
            return no_precedence;
        break;
}





void buffer_init(Buffer_for_token *buffer){
    buffer->token = (Token **) malloc(sizeof(Token) * buffer_length);
    buffer->index=0;
    buffer->length = buffer_length;

    Token * token = create_token();
    token->type = token_type_$;
    buffer->token[buffer->index] = token;
}

void buffer_push(Buffer_for_token *buffer, Token *token){
    buffer->index++;
    buffer->token[buffer->index] = token;
}

Token* buffer_copy_last(Buffer_for_token *buffer){
    return buffer->token[buffer->index];
}

Token* buffer_copy_first(Buffer_for_token *buffer){
    if(buffer->token[0]->type == token_type_$)
        return buffer->token[1];
    else 
        return buffer->token[0];
}

void buffer_pop(Buffer_for_token *buffer){
    buffer->token[buffer->index] = NULL;
    buffer->index--;
}

void buffer_shift(Buffer_for_token *buffer){
    buffer->token[1] = NULL;
    Token * token;
    int i = 1;
    while(i != buffer->index){
        token = buffer->token[i+1];
        buffer->token[i+1] = NULL;
        buffer->token[i] = token;
        i++;
    }
    buffer->index--;
    
}

bool buffer_is_empty(Buffer_for_token *buffer){
    if(buffer->index ==  0 || buffer->token[buffer->index]->type == token_type_$) return true;
    return false;
}

bool is_operator(Token_type type){
    if( type == token_type_length               ||
        type == token_type_mul                  ||
        type == token_type_div                  ||
        type == token_type_floor_div            ||
        type == token_type_plus                 ||
        type == token_type_minus                ||
        type == token_type_concat               ||
        type == token_type_lth                  ||
        type == token_type_leq                  ||
        type == token_type_gth                  ||
        type == token_type_geq                  ||
        type == token_type_equal                ||
        type == token_type_ineq                 ||
        type == token_type_assign               ||
        type == token_type_left_bracket         ||
        type == token_type_right_bracket        ||
        type == token_type_square_left_bracket  ||
        type == token_type_square_right_bracket ||
        type == token_type_colon                ||
        type == token_type_comma
    ){return true;}
        

        return false;
}

bool use_rule(Buffer_for_token *buffer){
    
    int i = 0;

    if(buffer->token[buffer->index - i]->type == token_type_identifier ||
    buffer->token[buffer->index - i]->type == token_type_integer ||
    buffer->token[buffer->index - i]->type == token_type_number ||
    buffer->token[buffer->index - i]->type == token_type_string ||
    buffer->token[buffer->index - i]->type == token_type_nil) return true;
   
    if(buffer->token[buffer->index - i]->type == token_type_E){
        i++;
        if(is_operator(buffer->token[buffer->index - i]->type)){
            i++;
            if(buffer->token[buffer->index - i]->type == token_type_E){
                return true;
            }
        }
        return false;
    }
    return false;
}

void rule(Token_type type, Buffer_for_token *evedence_of_E){
    
    switch (type)
    {
    case token_type_identifier:
        evedence_of_E->array[evedence_of_E->index++] = reduse_index;
        //printf("E -> id\n");
        break;

    case token_type_length:
        evedence_of_E->array[evedence_of_E->index++] = reduse_length;
        //printf("E -> E * E\n");
        break;

    case token_type_mul:
        evedence_of_E->array[evedence_of_E->index++] = reduse_mul;
        //printf("E -> E * E\n");
        break;

    case token_type_div:
        evedence_of_E->array[evedence_of_E->index++] = reduse_div;
        //printf("E -> E + E\n");
        break;

    case token_type_floor_div:
        evedence_of_E->array[evedence_of_E->index++] = reduse_floor_div;
        //printf("E -> E + E\n");
        break;

    case token_type_plus:
        evedence_of_E->array[evedence_of_E->index++] = reduse_plus;
        //printf("E -> E + E\n");
        break;

    case token_type_minus:
        evedence_of_E->array[evedence_of_E->index++] = reduse_minus;
        //printf("E -> E + E\n");
        break;

    case token_type_concat:
        evedence_of_E->array[evedence_of_E->index++] = reduse_concat;
        //printf("E -> E + E\n");
        break;

    case token_type_lth:
        evedence_of_E->array[evedence_of_E->index++] = reduse_lth;
        //printf("E -> E + E\n");
        break;

    case token_type_leq:
        evedence_of_E->array[evedence_of_E->index++] = reduse_leq;
        //printf("E -> E + E\n");
        break;

    case token_type_gth:
        evedence_of_E->array[evedence_of_E->index++] = reduse_gth;
        //printf("E -> E + E\n");
        break;

    case token_type_geq:
        evedence_of_E->array[evedence_of_E->index++] = reduse_geq;
        //printf("E -> E + E\n");
        break;

    case token_type_equal:
        evedence_of_E->array[evedence_of_E->index++] = reduse_equal;
        //printf("E -> E + E\n");
        break;

    case token_type_ineq:
        evedence_of_E->array[evedence_of_E->index++] = reduse_ineq;
        //printf("E -> E + E\n");
        break;

    case token_type_assign:
        evedence_of_E->array[evedence_of_E->index++] = reduse_index;
        //printf("E -> E + E\n");
        break;

    default:
        break;
    }
}

void reduse_fnc(Buffer_for_token *buffer, Buffer_for_token *evedence_of_E){

    while(!buffer_is_empty(buffer) && buffer_copy_last(buffer)->type != token_type_shift){
        rule(buffer_copy_last(buffer)->type, evedence_of_E);
        //buffer_push(infix_buffer, buffer_copy_last(buffer));
        buffer_pop(buffer);
    }//while

    if(buffer_copy_last(buffer)->type == token_type_shift) buffer_pop(buffer);
    
    Token *temp_token = create_token();
    temp_token->type = token_type_E;    
    buffer_push(buffer, temp_token);
}
















bool expression(parser_data_t *data){





    
    Buffer_for_token buffer;
    
    Buffer_for_token evedence_of_E;
    buffer_init(&evedence_of_E);

    buffer_init(&buffer);

    print_token(data->token);
    get_token(data);
    get_token(data);
    Token *shift_token;
    Token *temp_token;
    while(!(data->token->type >= kw_do && data->token->type <=token_type_EOF)){
        
        switch(precedence_compare(&buffer, data)){
            case push:
                //adding <
                temp_token = create_token();
                temp_token->type = token_type_shift;
                buffer_push(&buffer, temp_token);
                buffer_push(&buffer, data->token);
            break;
            case reduse:
                reduse_fnc(&buffer, &evedence_of_E);
                //adding <
                if(precedence_compare(&buffer, data) == reduse) reduse_fnc(&buffer, &evedence_of_E);
                

                shift_token = create_token();
                shift_token->type = token_type_shift;

                temp_token = create_token();
                temp_token = buffer_copy_last(&buffer);

                buffer_pop(&buffer);
                buffer_push(&buffer, shift_token);
                buffer_push(&buffer, temp_token);
                buffer_push(&buffer, data->token);
            break;
            case equal:

            break;
            case empty:

            break;
            case no_precedence:

            break;
            }//switch
                        
        get_token(data);

        printf("Printing buffer\n");
        for(int i =buffer.index; i>= 0; i--){
            print_token(buffer.token[i]);
        }  

    }
    
    while(buffer.index != 1){
        reduse_fnc(&buffer, &evedence_of_E);
    }
    printf("Printing buffer\n");
    for(int i =buffer.index; i>= 0; i--){
        print_token(buffer.token[i]);
    } 

    printf("Printing posloupnost E\n");
    for(int i = 0; i<evedence_of_E.index; i++){
        switch (evedence_of_E.array[i])
        {
        case reduse_index:
            printf("E -> id\n");
            break;
        case reduse_length:
            printf("E -> #E\n");
            break;
        case reduse_mul:
            printf("E -> E * E\n");
            break;
        case reduse_div:
            printf("E -> E / E\n");
            break;
        case reduse_floor_div:
            printf("E -> E // E\n");
            break;
        case reduse_plus:
            printf("E -> E + E\n");
            break;
        case reduse_minus:
            printf("E -> E - E\n");
            break;
        case reduse_concat:
            printf("E -> ..E\n");
            break;
        case reduse_lth:
            printf("E -> E < E\n");
            break;
        case reduse_leq:
            printf("E -> E <= E\n");
            break;
        case reduse_gth:
            printf("E -> E > E\n");
            break;
        case reduse_geq:
            printf("E -> E >= E\n");
            break;
        case reduse_equal:
            printf("E -> E == E\n");
            break;
        case reduse_ineq:
            printf("E -> E ~= E\n");
            break;
        case reduse_assign:
            printf("E -> E =\n");
            break;
        default:
            break;
        }
    }
    return true;
}*/