%{

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

extern int yylineno;
int yylex(void);
void yyerror(char const *s);

int error = 1;
int count = 0;
int store[500];

// stores tokens and their codes in a table
struct node
{
    int code;
    char* token;
    int diff_id;
    struct node *link;
}* table[7];

void set_type(int x);
int get_type(char* s);
void install_num(int x, char* s);
void install_id(int x, char* s);

%}

%start prog

%union
{
	char* str;
	int num;
	char id;
	char arr[25];
} 

%left '*' '/'
%left '+' '-'
%left UNEG UPOS
%nonassoc '='

%token tPROGRAM
%token tVAR
%token tBEGIN		
%token tEND		
%token tEND_		
%token tINTEGER
%token tREAL	
%token tFOR
%token tREAD		
%token tWRITE		
%token tTO		
%token tDO		
%token tSEMICOLON	
%token tCOLON		
%token tCOMMA		
%token tASSIGN	
%token tADD		
%token tSUBTRACT	
%token tMULT		
%token tDIV		
%token tBRACKET_OPEN	
%token tBRACKET_CLOSE	
%token <str> tID		
%token <integer> tINT
%token <float> tReal

%%

prog 			:  tPROGRAM prog_name tVAR dec_list tBEGIN    
				   stmt_list tEND_ ;

prog_name 		: tID ;	

dec_list 		: dec
 				| dec_list tSEMICOLON dec ;

dec 			:  error dec {
					yyerror("");
					printf("Line %d: Semicolon is missing\n", yylineno-1);
			}

			| dec_id_list tCOLON type ;

			| dec_id_list tCOLON {
				yyerror("");
				printf("Line %d: Type name is missing\n", yylineno - 1);
			}

		        | dec_id_list type {	
				yyerror("");
				printf("Line %d: Add colon\n", yylineno);
			}	
	            
type 			: tINTEGER {set_type(tINTEGER);}
				| tREAL {set_type(tREAL);}

dec_id_list 	: tID {install_id(tID, $1);} 
				| dec_id_list tCOMMA tID {
					install_id(tID, $3);
				}

id_list 		: tID ; 
				| id_list tCOMMA tID ;

stmt_list 		: stmt ;
		  		| stmt_list tSEMICOLON stmt ;
		  	
stmt 			: assign ;
				| read ;
				| write ;
				| for ;
				| error stmt 
				{	
					yyerror("");
					printf("Line %d: Semicolon is missing \n", yylineno);
				}


assign 			:  tID tASSIGN exp {	
						
						if(!findToken($1))
						{
							yyerror("");
							printf("Line %d: Undeclared variable \n", yylineno);
						}

						else 
						{
							if(store[count] == -1 || get_type($1) != store[count])
							{
								yyerror("");
								printf("Line %d: Assignment type mismatch \n", yylineno);
							}
						}
				}

exp 			: term ;
				| exp tSUBTRACT term ;
				| exp tADD term ;
				

term 			: factor ;
				| term tDIV factor ;
				| term tMULT factor ;
			

factor 			:  tINT {
				int typeId = tINTEGER;

				if(!store[count])
					store[count] = typeId ;
					
				else if(store[count] != typeId)
					store[count] = -1;					
				}


			    | tReal {
			    	int typeId = tREAL;
				
				if(!store[count])
				    	store[count] = typeId ;
						
				else if(store[count] != typeId)
					store[count] = -1;
				
			    }

			    | tBRACKET_OPEN exp tBRACKET_CLOSE ;

			    | tID {
					int typeId = get_type($1);

					if(!store[count])
						store[count] = typeId ;

					else if(store[count] != typeId)
						store[count] = -1;

				}	  

read 			: tREAD tBRACKET_OPEN id_list tBRACKET_CLOSE ;

write 			: tWRITE tBRACKET_OPEN id_list tBRACKET_CLOSE ;	

for 			: tFOR index_exp tDO body {}

index_exp 		: tID tASSIGN exp tTO exp ;

body 			: stmt ;
	 			| tBEGIN stmt_list tEND ;	 

%%
/*C code*/

// *** Function To Hash Values *** //

// calculates hash value of a string
int hashFunction(char* string){
    // sum: hashvalue, index of str in the hashtable
    int hash = 0;
    int i = 0;
    // loop to process each character in str and calculate hashvalue
    for(;string[i] != '\0';){
        hash *= 69; 
        hash += (string[i] >= 'A' && string[i] <= 'Z')? (string[i] - 'A') : (string[i] - '0');
        hash %= 7;
        i++;
    }
    return hash;
}

// inserts a given token (along with its code) in the hashtable
void table_insert(int code, char* token)
{
    // value: index in the hashtable where the new object will be stored
    int value = hashFunction(token);
    struct node* last = table[value];

    // if there is atleast one token in the given index's linked list
    if(last)
    {
        // traverse to the last entry in the linked list 
        while(last->link)
            last = last->link;

        // allocate space and store the token
        struct node* temp = (struct node*) malloc(sizeof(struct node));
        temp->code = code;
        temp->token = token;
        temp->link = NULL;
	temp->diff_id = 0;

        last->link = temp;
        return;
    }

    // if there are no tokens in the given index's linked list
    struct node* temp = (struct node*) malloc(sizeof(struct node));
    temp->code = code;
    temp->token = token;
    temp->link = NULL;
    temp->diff_id = 0;

    // assign this token as the first token in this index's linked list
    table[value] = temp;
    return;
}

// finds a token in the hashtable
int findToken(char* token){

	int hash = hashFunction(token);
	struct node *temp = table[hash];

	// traverse the appropriate linked list 
	// and return 1 if the token is found
	while(temp){
		if(!strcmp(token, temp->token))
			return 1;

		temp = temp->link;
	}
	// return 0 if the token is not found
	return 0;
}

void set_type(int type)
{
	for(int i=0; i<7; i++)
	{
		struct node *temp = table[i];
		
		while(temp) 
		{
			if(!(temp->diff_id)) 
				temp->diff_id = type;
			temp = temp->link;
		}
	}
}


//find type of identifier
int get_type(char* token)
{
	int value = hashFunction(token);
	struct node* temp = table[value];

	while(temp)
	{
		if(!strcmp(token, temp->token))
			return temp->diff_id;

		temp = temp->link;
	}

	return -1;
}


// auxiliary function to insert a new id in the hashtable
void install_id(int code, char* token)
{
    (findToken(token))? :table_insert(code, token); // insert only if the token does not exist
}

// auxiliary function to insert a new num in the hashtable
void install_num(int code, char* token)
{
    (findToken(token))? :table_insert(code, token); // insert only if the token does not exist
}


int main(void) {
	yyparse();
	if(error){
		printf("Parsing Successfully Done!!!!!!\n");
	}else{
		printf("Parsing Failed !!!!!!!\n");
	}
	return 0;
}

void yyerror(char const *s)
{	
	error = 0;
	return;
}