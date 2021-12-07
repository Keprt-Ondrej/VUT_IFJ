
#include "parser_precedence.h"
#include "parser.h"
#include "ErrLib.h"

#define buffer_length 25
#define buffer_expend_length 2

bool buffer_is_empty(Buffer_for_token *buffer){
    if(buffer->index ==  0 || buffer->token[buffer->index]->type == token_type_$) return true;
    return false;
}




void print_new_token(precedence_token_t *token, char string[100]){
    printf("%s \n", string);
    Token_type type = token->type;
    printf("printing token type %3d: ", type);
    if(type == token_type_identifier || type == token_type_string)
        printf("token_type_identifier\n");
    else if(type == token_type_integer)
        printf("token_type_integer\n");
    else if(type == token_type_number)
        printf("token_type_number\n");
    else {
        if(token_type_length == type)                   printf("#\n");
        if(token_type_mul == type)                      printf("*\n");
        if(token_type_div == type)                      printf("/\n");
        if(token_type_floor_div == type)                printf("//\n");
        if(token_type_plus == type)                     printf("+\n");
        if(token_type_minus == type)                    printf("-\n");
        if(token_type_concat == type)                   printf("..\n");
        if(token_type_lth == type)                      printf("<\n");
        if(token_type_leq == type)                      printf("<=\n");
        if(token_type_gth == type)                      printf(">\n");
        if(token_type_geq == type)                      printf(">=\n");
        if(token_type_equal == type)                    printf("==\n");
        if(token_type_ineq == type)                     printf("~=\n");
        if(token_type_assign == type)                   printf("=\n");
        if(token_type_left_bracket == type)             printf("(\n");
        if(token_type_right_bracket == type)            printf(")\n");
        if(token_type_square_left_bracket == type)      printf("[\n");
        if(token_type_square_right_bracket == type)     printf("]\n");
        if(token_type_colon == type)                    printf(":\n");
        if(token_type_comma == type)                    printf(",\n");
        if(token_type_$ == type)                        printf("empty\n");
        if(token_type_E == type)                        printf("E\n");
        if(token_type_shift == type)                    printf("<\n");
    }

    printf("shift %d\n", token->shift);
    printf("redused %d\n\n", token->redused);
}

void buffer_init(Buffer_for_token *buffer){
    buffer->token = (precedence_token_t **) malloc(sizeof(precedence_token_t) + buffer_length);
    buffer->index= 0;
    buffer->current_length = buffer_length;

    precedence_token_t * temp = malloc(sizeof(precedence_token_t));
    temp->type = token_type_$;
    buffer->token[buffer->index] = temp;
    buffer->active_index = 0;
}

precedence_token_t * remake_token(parser_data_t *data){
    precedence_token_t * new_token = malloc(sizeof(precedence_token_t));
    if(new_token == NULL){
        free_parser_data(data);
        printf("allocation failed");
        exit(1);
    }
        
    new_token->type = data->token->type;
    new_token->shift = false;
    new_token->redused = false;
    //////////////////experiment
    if(new_token->type == token_type_integer) new_token->data.type_integer = data->token->data.type_integer;
    if(new_token->type == token_type_number) new_token->data.type_double = data->token->data.type_double;
    if(new_token->data.str != NULL) {
        new_token->data.str = malloc(50);
        new_token->data.str = data->token->data.str;
    }
    /////////////////experiment
    
    if(data->token->type == token_type_identifier){
        htab_item * temp = htab_find_variable(data->local_symtable, data->token->data.str);
        if(NULL == temp){
            printf("here it is");
        }
        new_token->data_type = temp->type;
        new_token->identifier = temp->key;
        //printf("data_type = %d\n", new_token->data_type);
    }

    return new_token;
}

void expend_buffer(Buffer_for_token *buffer){
    buffer->token = (precedence_token_t **) realloc(buffer->token, buffer->current_length * buffer_expend_length);
    if(buffer->token == NULL){
        exit(1);
    }
    buffer->current_length = buffer->current_length * buffer_expend_length;
}

void buffer_push(Buffer_for_token *buffer, precedence_token_t *token){
    buffer->index++;
    if(buffer->index >= buffer->current_length) expend_buffer(buffer);
    buffer->token[buffer->index] = token;
}

void buffer_pop(Buffer_for_token *buffer){
    buffer->token[buffer->index] = NULL;
    buffer->index--;
}

int precedence_compare(Buffer_for_token * buffer, precedence_token_t *token){

    Token_type left_type;
    Token_type right_type;

    int temp = buffer->index;
    if(buffer->token[buffer->index]->redused == true) temp = buffer->index-1;
    left_type = buffer->token[temp]->type;

    right_type = token->type;
    int precedence_table[22][22] =
    {
//  l     # | .. | * | / | // | + | - | < | <= | > | >= | == | ~= | ( | ) | id | int | num | str | nil | $       r
        { E ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , P , R , R  ,  R  ,  R  ,  R  ,  N  , P }, // #
        { R ,  P , R , R , R  , R , R , P , P  , P , P  , P  , P  , N , N , P  ,  N  ,  N  ,  R  ,  N  , P }, // ..
        { R ,  P , R , R , R  , P , P , P , P  , P , P  , P  , P  , N , N , R  ,  R  ,  R  ,  R  ,  N  , P }, // *
        { R ,  P , R , R , R  , P , P , P , P  , P , P  , P  , P  , N , N , R  ,  R  ,  R  ,  R  ,  N  , P }, // /
        { R ,  P , R , R , R  , P , P , P , P  , P , P  , P  , P  , N , N , R  ,  R  ,  R  ,  R  ,  N  , P }, // //
        { R ,  P , R , R , R  , R , R , P , P  , P , P  , P  , P  , N , N , R  ,  R  ,  R  ,  R  ,  N  , P }, // +
        { R ,  P , R , R , R  , R , R , P , P  , P , P  , P  , P  , N , N , R  ,  R  ,  R  ,  R  ,  N  , P }, // -
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , N , R  ,  R  ,  R  ,  R  ,  R  , P }, // <
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , N , R  ,  R  ,  R  ,  R  ,  R  , P }, // <=
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , N , R  ,  R  ,  R  ,  R  ,  R  , P }, // >
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , N , R  ,  R  ,  R  ,  R  ,  R  , P }, // >=
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , N , R  ,  R  ,  R  ,  R  ,  R  , P }, // ==
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , N , R  ,  R  ,  R  ,  R  ,  R  , P }, // ~=
        { P ,  P , P , N , P  , P , P , P , P  , P , P  , P  , P  , P , N , N  ,  N  ,  N  ,  N  ,  N  , P }, // (
        { N ,  N , N , N , N  , N , N , N , N  , N , N  , N  , N  , N , P , R  ,  R  ,  R  ,  R  ,  R  , P }, // )
        { P ,  P , P , P , P  , P , P , P , P  , P , P  , P  , P  , P , N , N  ,  N  ,  N  ,  N  ,  N  , P }, // id
        { P ,  N , P , P , P  , P , P , P , P  , P , P  , P  , P  , P , N , N  ,  N  ,  N  ,  N  ,  N  , P }, // int
        { P ,  N , P , P , P  , P , P , P , P  , P , P  , P  , P  , P , N , N  ,  N  ,  N  ,  N  ,  N  , P }, // num
        { P ,  P , N , N , N  , N , N , P , P  , P , P  , P  , P  , P , N , N  ,  N  ,  N  ,  N  ,  N  , P }, // str
        { N ,  N , N , N , N  , N , N , P , P  , P , P  , P  , P  , P , R , N  ,  N  ,  N  ,  N  ,  N  , P }, // nil
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , R , R  ,  R  ,  R  ,  R  ,  R  , P }  /// $
    };
    //printf("%d = left_type\n %d = right_type", left_type, right_type);
    return precedence_table[right_type][left_type];
}

void reduse_fnc(Buffer_for_token *buffer){
    precedence_token_t * new_token = malloc(sizeof(precedence_token_t));
    new_token->type = buffer->token[buffer->index]->type;
    if(buffer->token[buffer->index]->shift == true){
        buffer_pop(buffer);
        new_token->redused = true;
        buffer_push(buffer, new_token);
    }else{
        int temp = buffer->index;
        while(buffer->token[temp]->shift != true && !buffer_is_empty(buffer)){       
            buffer_pop(buffer);
            temp--;
        }
        
        new_token->redused = true;
        buffer_push(buffer, new_token); 
    }

}

bool is_operator(Token_type type){
    if( type == token_type_mul                  ||
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
        type == token_type_ineq                 
    ){return true;}

    return false;
}

bool check_rule(Token_type left_type, Token_type right_type){
    bool F = false;
    bool T = true;

    int precedence_table[22][22] =
    {
//  l     # | .. | * | / | // | + | - | < | <= | > | >= | == | ~= | ( | ) | id | int | num | str | nil | $       r
        { F ,  F , T , T , T  , T , T , T , T  , T , T  , T  , T  , T , F , T  ,  F  ,  F  ,  F  ,  F  , T }, // #
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  F  ,  F  ,  T  ,  F  , T }, // ..
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  T  , T }, // *
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  T  , T }, // /
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  T  , T }, // //
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  T  , T }, // +
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  T  , T }, // -
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  T  , T }, // <
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  T  , T }, // <=
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  T  , T }, // >
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  T  , T }, // >=
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  T  , T }, // ==
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  T  , T }, // ~=
        { T ,  T , T , T , T  , T , T , T , T  , T , T  , T  , T  , T , F , F  ,  F  ,  F  ,  F  ,  F  , T }, // (
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  T  ,  T  , T }, // )
        { T ,  T , T , T , T  , T , T , T , T  , T , T  , T  , T  , T , F , T  ,  T  ,  T  ,  T  ,  T  , T }, // id
        { F ,  F , T , T , T  , T , T , T , T  , T , T  , T  , T  , T , F , T  ,  T  ,  T  ,  T  ,  T  , T }, // int
        { F ,  F , T , T , T  , T , T , T , T  , T , T  , T  , T  , T , F , T  ,  T  ,  T  ,  T  ,  T  , T }, // num
        { T ,  T , F , F , F  , F , F , T , T  , T , T  , T  , T  , T , F , T  ,  T  ,  T  ,  T  ,  T  , T }, // str
        { F ,  F , F , F , F  , F , F , T , T  , T , T  , T  , T  , T , F , T  ,  T  ,  T  ,  T  ,  T  , T }, // nil
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , F , F  ,  F  ,  F  ,  F  ,  F  , T }  /// $
    };
    return precedence_table[right_type][left_type];
}

void print_buffer(Buffer_for_token  * buffer){
    precedence_token_t * token;
    Token_type type;
    printf("This is buffer\n");

  
    printf("|           |\n");
    int i = buffer->index;
    while(i != -1){
        token = buffer->token[i];
        type = token->type;
        i--;
        if(token->redused == true)
            printf("|     E     |\n");
        else if(type == token_type_string)
            printf("|%6s     |\n", token->data.str);
        else if(type == token_type_integer)
            printf("|%6d     |\n", token->data.type_integer);
        else if(type == token_type_number)
            printf("|%6f     |\n", token->data.type_double);
        else {
            if(token_type_length == type)                   printf("|     #     |\n");
            if(token_type_mul == type)                      printf("|     *     |\n");
            if(token_type_div == type)                      printf("|     /     |\n");
            if(token_type_floor_div == type)                printf("|     //    |\n");
            if(token_type_plus == type)                     printf("|     +     |\n");
            if(token_type_minus == type)                    printf("|     -     |\n");
            if(token_type_concat == type)                   printf("|     ..    |\n");
            if(token_type_lth == type)                      printf("|     <     |\n");
            if(token_type_leq == type)                      printf("|     <=    |\n");
            if(token_type_gth == type)                      printf("|     >     |\n");
            if(token_type_geq == type)                      printf("|     >=    |\n");
            if(token_type_equal == type)                    printf("|     ==    |\n");
            if(token_type_ineq == type)                     printf("|     ~=    |\n");
            if(token_type_assign == type)                   printf("|     =     |\n");
            if(token_type_left_bracket == type)             printf("|     (     |\n");
            if(token_type_right_bracket == type)            printf("|     )     |\n");
            if(token_type_square_left_bracket == type)      printf("|     [     |\n");
            if(token_type_square_right_bracket == type)     printf("|     ]     |\n");
            if(token_type_colon == type)                    printf("|     :     |\n");
            if(token_type_comma == type)                    printf("|     ,     |\n");
            if(token_type_$ == type)                        printf("|   empty   |\n");
            if(token_type_E == type)                        printf("|     E     |\n");
            if(token_type_shift == type)                    printf("|     <     |\n");
            if(token_type_identifier == type)               printf("|     id    |\n");
        }

        
    }
    printf("|___________|\n");


}

bool end_of_token_stream(precedence_token_t * token){
    if(token->type == token_type_identifier){
        
        if( token->data_type == string    ||
            token->data_type == integer   ||
            token->data_type == number    ||
            token->data_type == nil       ||
            token->data_type == type_bool
        ){            
                return true;
        }else{
                return false;
        }
    }else if( 
        token_type_length == token->type ||
        token_type_concat == token->type ||
        token_type_mul == token->type ||
        token_type_div == token->type ||
        token_type_floor_div == token->type ||
        token_type_plus == token->type ||
        token_type_minus == token->type ||
        token_type_lth == token->type ||
        token_type_leq == token->type ||
        token_type_gth == token->type ||
        token_type_geq == token->type ||
        token_type_equal == token->type ||
        token_type_ineq == token->type ||
        token_type_left_bracket == token->type ||
        token_type_right_bracket == token->type ||
        token_type_identifier == token->type ||
        token_type_integer == token->type ||
        token_type_number == token->type ||
        token_type_string == token->type ||
        token_type_nil == token->type
    ){
        
        return true;
    }else{
        return false;
    }
}

bool precedence(parser_data_t *data){

    precedence_token_t * new_token = remake_token(data);
    

    // *htab_item temp = htab_find_variable(data->local_symtable, data->token->data.str);


    Buffer_for_token buffer;
    buffer_init(&buffer);

    bool flag = true;
     while(end_of_token_stream(new_token)){
        
            if(!check_rule(buffer.token[buffer.index]->type, new_token->type)) flag = !flag;      

        switch (precedence_compare(&buffer, new_token))
        {
            case P:
                if(is_operator(new_token->type)){
                    buffer.token[buffer.index-1]->shift = true;
                }else if(buffer.token[buffer.index]->type == token_type_left_bracket){
                    buffer.token[buffer.index]->shift = true;
                }else new_token->shift = true;

                if(buffer.token[buffer.index]->type == token_type_left_bracket) buffer.left_bracket++;
                if(buffer.token[buffer.index]->type == token_type_right_bracket) buffer.right_bracket++;
                buffer_push(&buffer, new_token);

                break;
            case R:

                reduse_fnc(&buffer);         
                //print_buffer(&buffer);       
                if(precedence_compare(&buffer, new_token) == R) reduse_fnc(&buffer);
                buffer_push(&buffer, new_token);
                break;
            case E:

                break;
            case N:
            printf("NO PRECEDENCE\n");
                flag = !flag;
                break;    
        }//switch
        
        print_buffer(&buffer);
        printf("\n\n\n\n\n__________________________________\n");
        print_new_token(new_token, "input token");
        print_new_token(buffer.token[buffer.index-1], "buffer token");
        printf("__________________________________\n");
        
        get_token(data);

        new_token = remake_token(data);

    }//while

    if(buffer.left_bracket != buffer.right_bracket) flag = !flag;
    printf("dsaf");

    while(buffer.index != 1){
        reduse_fnc(&buffer);
    }
    return true;
}