#include "parser_precedence.h"
#include "parser.h"
#include "ErrLib.h"

#define buffer_length 10
#define buffer_expend_length 2
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






bool buffer_is_empty(Buffer_for_token *buffer){
    if(buffer->index ==  0 || buffer->token[buffer->index]->type == token_type_$) return true;
    return false;
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
        fprintf(stderr, "allocation failed");
        exit(INTERNAL_ERROR);
    }
    new_token->type = data->token->type;
    new_token->shift = false;
    new_token->redused = false;

    if(new_token->type == token_type_integer){
        new_token->data.type_integer = data->token->data.type_integer;
        new_token->identifier = int_to_string(data->token->data.type_integer);
        new_token->data_type = integer;
    } 
    if(new_token->type == token_type_number){
        new_token->identifier = int_to_string(data->token->data.type_double);
        new_token->data.type_double = data->token->data.type_double;
        new_token->data_type = number;
    }
    if(new_token->type == token_type_string) {
        new_token->identifier = string_to_string(data->token->data.str);
        new_token->data_type = string;
    }
    
   if(data->token->type == token_type_identifier){
        htab_item * temp = htab_find_variable(data->local_symtable, data->token->data.str);
        if(NULL == temp){
            temp = htab_find_variable(data->global_symtable, data->token->data.str);
            if(NULL == temp){
                exit(SEM_ERROR_REDEFINE_UNDEFINE_VAR);
            }            
            return new_token;
        }else{
            new_token->identifier = allocate_var_name_3AC("LF@", temp);
            new_token->data_type = temp->type;
            return new_token;
        }
    }

    return new_token;
}

void expand_buffer(Buffer_for_token *buffer){
    fprintf(stderr,"resizing\n");
    buffer->token = (precedence_token_t **) realloc(buffer->token, buffer->current_length * buffer_expend_length * sizeof(precedence_token_t *));
    if(buffer->token == NULL){
        exit(INTERNAL_ERROR);
    }
    buffer->current_length = buffer->current_length * buffer_expend_length;
}

void buffer_push(Buffer_for_token *buffer, precedence_token_t *token){
    buffer->index++;
    fprintf(stderr," index: %d, delka %d\n",buffer->index,buffer->current_length);
    if(buffer->index >= buffer->current_length){
        expand_buffer(buffer);
    } 
    buffer->token[buffer->index] = token;
}

void buffer_pop(Buffer_for_token *buffer){
    free(buffer->token[buffer->index]->identifier);
    
    free(buffer->token[buffer->index]);
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

    if(right_type == kw_local || right_type == kw_if || right_type == kw_while || right_type == kw_return || right_type == kw_end || right_type == kw_end || right_type == kw_else || right_type == kw_then || right_type == kw_do || right_type == token_type_comma){
        return N;
    }
    
    int precedence_table[22][22] =
    {
//  l     # | .. | * | / | // | + | - | < | <= | > | >= | == | ~= | ( | ) | id | int | num | str | nil | $       r
        { P ,  P , P , P , P  , P , P , P , P  , P , P  , P  , P  , P , R , R  ,  R  ,  R  ,  R  ,  N  , P }, // #
        { R ,  P , R , R , R  , R , R , P , P  , P , P  , P  , P  , N , R , P  ,  N  ,  N  ,  R  ,  N  , P }, // ..
        { R ,  P , R , R , R  , P , P , P , P  , P , P  , P  , P  , N , R , R  ,  R  ,  R  ,  R  ,  N  , P }, // /
        { R ,  P , R , R , R  , P , P , P , P  , P , P  , P  , P  , N , R , R  ,  R  ,  R  ,  R  ,  N  , P }, // //
        { R ,  P , R , R , R  , P , P , P , P  , P , P  , P  , P  , N , R , R  ,  R  ,  R  ,  R  ,  N  , P }, // *
        { R ,  P , R , R , R  , R , R , P , P  , P , P  , P  , P  , N , R , R  ,  R  ,  R  ,  R  ,  N  , P }, // +
        { R ,  P , R , R , R  , R , R , P , P  , P , P  , P  , P  , N , R , R  ,  R  ,  R  ,  R  ,  N  , P }, // -
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , R , R  ,  R  ,  R  ,  R  ,  N  , P }, // <
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , R , R  ,  R  ,  R  ,  R  ,  N  , P }, // <=
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , R , R  ,  R  ,  R  ,  R  ,  N  , P }, // >
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , R , R  ,  R  ,  R  ,  R  ,  N  , P }, // >=
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , R , R  ,  R  ,  R  ,  R  ,  R  , P }, // ==
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , R , R  ,  R  ,  R  ,  R  ,  R  , P }, // ~=
        { P ,  P , P , N , P  , P , P , P , P  , P , P  , P  , P  , P , N , N  ,  N  ,  N  ,  N  ,  N  , P }, // (
        { N ,  N , N , N , N  , N , N , N , N  , N , N  , N  , N  , N , R , R  ,  R  ,  R  ,  R  ,  R  , P }, // )
        { P ,  P , P , P , P  , P , P , P , P  , P , P  , P  , P  , P , N , N  ,  N  ,  N  ,  N  ,  N  , P }, // id
        { P ,  N , P , P , P  , P , P , P , P  , P , P  , P  , P  , P , N , N  ,  N  ,  N  ,  N  ,  N  , P }, // int
        { P ,  N , P , P , P  , P , P , P , P  , P , P  , P  , P  , P , N , N  ,  N  ,  N  ,  N  ,  N  , P }, // num
        { P ,  P , N , N , N  , N , N , P , P  , P , P  , P  , P  , P , N , N  ,  N  ,  N  ,  N  ,  N  , P }, // str
        { N ,  N , N , N , N  , N , N , N , N  , N , N  , P  , P  , P , N , N  ,  N  ,  N  ,  N  ,  N  , P }, // nil
        { R ,  R , R , R , R  , R , R , R , R  , R , R  , R  , R  , N , R , R  ,  R  ,  R  ,  R  ,  R  , P }  /// $
    };
    return precedence_table[right_type][left_type];
}

void reduse_fnc(Buffer_for_token *buffer, parser_data_t *data){


    if(buffer->token[buffer->index]->shift == true ){// E
        buffer->token[buffer->index]->redused = true;
        buffer->token[buffer->index]->shift = false;
        return;
    }

    
    if(buffer->token[buffer->index]->shift == false && !buffer_is_empty(buffer) ){// <E + E

    precedence_token_t * new_token = malloc(sizeof(precedence_token_t));
    new_token->type = token_type_E;
    new_token->shift =false;

    int opcode = 0;
        switch (buffer->token[buffer->index - 1]->type)
        {
        case token_type_length:
            opcode = STRLEN;
        break;
        case token_type_concat:
            opcode = CONCAT;
            break;        
        case token_type_mul:
            opcode = MUL;
            break;
        case token_type_div:
            opcode = DIV;
            break;
        case token_type_plus:
            opcode = ADD;
            break;
        case token_type_minus:
            opcode = SUB; 
            break;       
        case token_type_equal:
            opcode = EQ;
            break;

        case token_type_lth:
            opcode = LT;
        break;
        case token_type_gth:
            opcode = GT;
        break;
        default:
        break;
        }

        char * tmp_name = allocate_new_tmp_name(data, "LF@");

        defvar_3AC(data,strcpy_alloc(data,tmp_name));
        if(opcode == STRLEN){
            push_instruction(data, create_instruction(opcode, strcpy_alloc(data,tmp_name),strcpy_alloc(data,buffer->token[buffer->index]->identifier),NULL));
            new_token->identifier = tmp_name;
            new_token->data_type = integer;
            buffer_pop(buffer);
            buffer_pop(buffer);
        }
        else{
            push_instruction(data, create_instruction(opcode, strcpy_alloc(data,tmp_name),strcpy_alloc(data,buffer->token[buffer->index - 2]->identifier),strcpy_alloc(data,buffer->token[buffer->index]->identifier)));
            new_token->identifier = tmp_name;

            if((buffer->token[buffer->index - 2]->data_type == number || buffer->token[buffer->index]->data_type == number) && (buffer->token[buffer->index - 2]->data_type == string || buffer->token[buffer->index]->data_type == string)){
                new_token->data_type = number;
            }
            if(buffer->token[buffer->index - 2]->data_type == integer && buffer->token[buffer->index]->data_type == integer){
                new_token->data_type = integer;
            }else if(buffer->token[buffer->index - 2]->data_type == string && buffer->token[buffer->index]->data_type == string){
                new_token->data_type = string;
            }else{

            }
            buffer_pop(buffer);
            buffer_pop(buffer);
            buffer_pop(buffer);
        }

            


        buffer_push(buffer, new_token);
        return;
    }
 //
   

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
    
    if(right_type == kw_local || right_type == kw_if || right_type == kw_while || right_type == kw_return || right_type == kw_end || right_type == kw_end || right_type == kw_else || right_type == kw_then || right_type == kw_do || right_type == token_type_comma){
        return true;
    }

    int precedence_table[22][22] =
    {
//  l     # | .. | * | / | // | + | - | < | <= | > | >= | == | ~= | ( | ) | id | int | num | str | nil | $       r
        { F ,  F , T , T , T  , T , T , T , T  , T , T  , T  , T  , T , F , T  ,  F  ,  F  ,  F  ,  F  , T }, // #
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  F  ,  F  ,  T  ,  F  , T }, // ..
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  F  , T }, // *
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  F  , T }, // /
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  F  , T }, // //
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  F  , T }, // +
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  F  ,  F  , T }, // -
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  T  ,  F  , T }, // <
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  T  ,  F  , T }, // <=
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  T  ,  F  , T }, // >
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  T  ,  F  , T }, // >=
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  T  ,  T  , T }, // ==
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  T  ,  T  , T }, // ~=
        { T ,  T , T , T , T  , T , T , T , T  , T , T  , T  , T  , T , F , F  ,  F  ,  F  ,  F  ,  F  , T }, // (
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , T , T  ,  T  ,  T  ,  T  ,  T  , T }, // )
        { T ,  T , T , T , T  , T , T , T , T  , T , T  , T  , T  , T , F , T  ,  T  ,  T  ,  T  ,  T  , T }, // id
        { F ,  F , T , T , T  , T , T , T , T  , T , T  , T  , T  , T , F , T  ,  T  ,  T  ,  T  ,  T  , T }, // int
        { F ,  F , T , T , T  , T , T , T , T  , T , T  , T  , T  , T , F , T  ,  T  ,  T  ,  T  ,  T  , T }, // num
        { T ,  T , F , F , F  , F , F , T , T  , T , T  , T  , T  , T , F , T  ,  T  ,  T  ,  T  ,  T  , T }, // str
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , T  , T  , T , F , T  ,  T  ,  T  ,  T  ,  T  , T }, // nil
        { F ,  F , F , F , F  , F , F , F , F  , F , F  , F  , F  , F , F , F  ,  F  ,  F  ,  F  ,  F  , T }  /// $
    };
    
    
    if(precedence_table[right_type][left_type] == N && (left_type == token_type_nil || right_type == token_type_nil)) exit(UNEXP_ZERO_VALUE_ERROR);
    
    return precedence_table[right_type][left_type];
}

bool precedence(parser_data_t *data){
    precedence_token_t * new_token = remake_token(data);

    Buffer_for_token buffer;
    buffer_init(&buffer);
   
     while(1){
        if(!check_rule(buffer.token[buffer.index]->type, new_token->type)) exit(SYNTAX_ERROR);

        switch (precedence_compare(&buffer, new_token))
        {
            case P:
            //is oper
                new_token->shift = true;

                buffer_push(&buffer, new_token);

                break;
            case R:
          
                reduse_fnc(&buffer, data);// E -> id
                while(precedence_compare(&buffer, new_token) == R) {
                    reduse_fnc(&buffer, data);
                    if(buffer.index == 3) {
                        reduse_fnc(&buffer, data);
                    }
                }
                
                buffer_push(&buffer, new_token);
                break;
            case N:
                while(buffer.index !=1){

                    reduse_fnc(&buffer, data);

                }
                buffer.token[buffer.index]->next = NULL;
                push_precedence_token(data, buffer.token[buffer.index]);


                return true;
                break;    
        }//switch
            print_buffer(&buffer);
            get_token(data);
            new_token = remake_token(data);

    }
    //while

    return true;
}