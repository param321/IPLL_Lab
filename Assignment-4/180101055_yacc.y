%{

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int error = 1;
int count = 0;
int store[500];

int setTableType(int x);
int getTableType(char* s);
int install_num(int x, char* s);
int install_id(int x, char* s);

// stores tokens and their codes 
struct node{
    int tokenCode;
    char* token;
    int diffId;
    struct node *next;
}* table[7];

extern int yylineno;
int yylex(void);
void yyerror(char const *s);

%}

%start prog

%union{
	char* str;
	int num;
	char id;
	char arr[25];
} 

%token tTO		
%token tDO		
%token tSEMICOLON	
%token tCOLON		
%token <str> tID		
%token <integer> tINT
%token <float> tReal
%token tCOMMA		
%token tASSIGN	
%token tADD		
%token tINTEGER		
%token tWRITE
%token tSUBTRACT	
%token tMULT		
%token tDIV		
%token tBRACKET_OPEN	
%token tBRACKET_CLOSE	
%token tPROGRAM
%token tVAR
%token tBEGIN		
%token tEND		
%token tEND_
%token tREAL	
%token tFOR
%token tREAD	

%left '*' '/'
%left '+' '-'
%left UNEG UPOS
%nonassoc '='
		


%%

read 			: tREAD tBRACKET_OPEN id_list tBRACKET_CLOSE ;

write 			: tWRITE tBRACKET_OPEN id_list tBRACKET_CLOSE ;	

for 			: tFOR index_exp tDO body {}

index_exp 		: tID tASSIGN exp tTO exp ;
	            
type 			: tINTEGER {setTableType(tINTEGER);}
				| tREAL {setTableType(tREAL);}

dec_id_list 	: tID {install_id(tID, $1);} 
				| dec_id_list tCOMMA tID {
					install_id(tID, $3);
				}

id_list 		: tID ; 
				| id_list tCOMMA tID ;

stmt_list 		: stmt ;
		  		| stmt_list tSEMICOLON stmt ;
		  	
stmt 			: assign ;
				| error stmt {	
					yyerror("");
					printf("Semicolon is Missing in Line %d\n", yylineno);
				}
				| write ;
				| for ;
				| read ;
				

prog 			:  tPROGRAM prog_name tVAR dec_list tBEGIN    
				   stmt_list tEND_ ;

prog_name 		: tID ;	

dec_list 		: dec
 				| dec_list tSEMICOLON dec ;

dec 			:  error dec {
					yyerror("");
					printf("Semicolon Is Missing Line %d\n", yylineno-1);
				}
				| dec_id_list type {	
					yyerror("");
					printf("Add Colon in \n Line %d", yylineno);
				}	
				| dec_id_list tCOLON type ;
				| dec_id_list tCOLON {
					yyerror("");
					printf("Type Name is Missing in Line %d\n", yylineno - 1);
				}

term 			: factor ;
				| term tMULT factor ;
				| term tDIV factor ;

factor 			:  tINT {
					int typeId = tINTEGER;
					if(!store[count]){
						store[count] = typeId ;
					}else if(store[count] != typeId){
						store[count] = -1;			
					}		
				}
				| tBRACKET_OPEN exp tBRACKET_CLOSE ;
				| tID {
					int typeId = getTableType($1);

					if(!store[count]){
						store[count] = typeId ;
					}else if(store[count] != typeId){
						store[count] = -1;
					}
				}	
			    | tReal {
			    	int typeId = tREAL;
					if(!store[count]){
							store[count] = typeId ;
					}else if(store[count] != typeId){
						store[count] = -1;
					}
				
			    }      

body 			: stmt ;
	 			| tBEGIN stmt_list tEND ;	

assign 			:  tID tASSIGN exp {	
						if(!findToken($1)){
							yyerror("");
							printf("Variable Undeclared in Line %d\n", yylineno);
						}else{
							if(store[count] == -1 || getTableType($1) != store[count]){
								yyerror("");
								printf("Assignment Type Mismatch Line %d\n", yylineno);
							}
						}
				}

exp 			: term ;
				| exp tADD term ;
				| exp tSUBTRACT term ;

%%

//calculates hash value 
int hashFunction(char* string){
    int hash = 0;
    int i = 0;
    // loop to process each character in string and calculate hash
    for(;string[i] != '\0';){
        hash *= 71; 
		if(string[i] >= 'A' && string[i] <= 'Z'){
			hash += (string[i] - 'A');
		}else{
			hash += (string[i] - '0');
		}
        hash %= 7;
        i++;
    }
    return hash;
}

int setTableType(int type){
	int i=0;
	while(i<7){
		struct node *temp = table[i];
		for(;temp;){
			if(!(temp->diffId)){
				temp->diffId = type;
			}
			temp = temp->next;
		}
		i++;
	}
	return 0;
}

// inserts a given token in the hashtable
int tableInsert(int code, char* token){
    struct node* last = table[hashFunction(token)];

    // if there is atleast one token in the given index's linked list
    if(last){
        //traverse to the last entry in the list 
        for(;last->next;){
            last = last->next;
		}
		struct node* temp;
		if(last!=NULL){
			temp = (struct node*) malloc(sizeof(struct node));
			last->next = temp;
			temp->token = token;
			temp->diffId = 0;
		}
		temp->next = NULL;
		temp->tokenCode = code;

        return 0;
    }

    // if there are no tokens in the given index's linked list
    struct node* temp ;
	temp = (struct node*) malloc(sizeof(struct node));

    temp->tokenCode = code;
    temp->token = token;
    temp->next = NULL;
    temp->diffId = 0;

    //assign this token as the first token in this index's linked list
    table[hashFunction(token)] = temp;

    return 0;
}

//finds a token in the hashtable
int findToken(char* token){

	int hash = hashFunction(token);
	struct node *temp = table[hash];

	// traverse the appropriate linked list 
	while(temp){
		char* x = temp->token;
		if(!strcmp(token, x)){
			return 1;
		}

		temp = temp->next;
	}
	// return 0 if the token is not found
	return 0;
}


//find type of identifier
int getTableType(char* token){
	int hash = hashFunction(token);
	struct node* temp = table[hash];

	for(;temp;){
		char* x = temp->token;
		if(!strcmp(token, x)){
			return temp->diffId;
		}
		temp = temp->next;
	}
	//not found
	return -1;
}


// auxiliary function to insert a new id in the hashtable
int install_id(int code, char* token){
    (findToken(token))? :tableInsert(code, token); // insert only if the token does not exist
	return 0;
}

// auxiliary function to insert a new num in the hashtable
int install_num(int code, char* token){
    (findToken(token))? :tableInsert(code, token); // insert only if the token does not exist
	return 0;
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