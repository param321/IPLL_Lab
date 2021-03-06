/*
IPLL Assignment 1
Author: Pooja Bhagat

Environment used to run the program :  
`gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)` 

Command to compile:  
`g++ 180101057_Assign01.cpp`
   
Command to run the code:  
`./a.out program.txt`
*/

#include "assembler.h"
#include "assembler.cpp"
#include "pass_one.cpp"
#include "pass_two.cpp"

int main(int argc, char *argv[])
{
    char progFile[MAXS];
    stpcpy(progFile, argv[1]);
    const char *outFile = "objectCode.txt";
    const char *listingFile = "codeListing.txt";

    FILE *prog, *intm, *out, *list;

    // load opcode table
    if (load_optab() < 0)
    {
        printf("Error loading opcode table. \n");
    }

    // open program, intermediate and output file
    prog = fopen(progFile, "r");
    intm = fopen(INTM, "w");
    out = fopen(outFile, "w");
    list = fopen(listingFile, "w");

    // if error opening files
    if (!prog || !intm || !out || !list)
    {
        printf("Error opening files. \n");
        return 0;
    }

    // execute pass 1 and generate intermediate file
    if (pass_one(prog, intm) < 0)
    {
        printf("Error in Pass one. \n");
        return 0;
    }

    // open intermediate file in read mode
    fclose(intm);
    intm = fopen(INTM, "r");

    // execute pass 2 to generate output
    if (pass_two(intm, out, list) < 0)
    {
        printf("Error in Pass two. \n");
        return 0;
    }

    // close files
    fclose(prog);
    fclose(intm);
    fclose(out);

    // for(auto itr: symtab_list)
    // {
    //     cout<<itr.first<<endl;
    //     itr.second->print_symtab();
    // }

    printf("Assembly Successful. \n");
    printf("ERROR_FLAG = %d\n", ERROR_FLAG);
    return 0;
}