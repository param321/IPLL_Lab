#include "assembler.h"

// Pass 1
// Takes program file as input
// and generates intermediate file
// and symbol table
int pass_one(FILE *inputFile, FILE *outputFile)
{

    fseek(inputFile, 0, SEEK_SET);

    //clear symbol table
    FILE *st = fopen(SYMTAB, "w");
    fclose(st);

    char *line = NULL, temp[MAXS];
    size_t len = 0;
    char *args[MAXW];
    int words;
    char *OPCODE, *LABEL, *OPERAND;

    // store addresses
    int LOCCTR;

    // read first line
    getline(&line, &len, inputFile);
    strcpy(temp, line);
    if (line[0] == ' ')
    {
        words = break_line(temp, 1, args);
        LABEL = NULL;
        OPCODE = args[1];
    }
    else
    {
        words = break_line(temp, 0, args);
        LABEL = args[0];
        OPCODE = args[1];
    }

    if (strcmp(args[1], "START") == 0)
    {
        OPERAND = args[2];
        STARTADDR = strtol(OPERAND, NULL, 16);
        strcpy(PROGNAME, LABEL);
        LOCCTR = STARTADDR;

        // write line to intermediate file
        fprintf(outputFile, "%04X\t%s", LOCCTR, line);

        // read next input line
        getline(&line, &len, inputFile);
        strcpy(temp, line);
        if (line[0] == ' ')
        {
            words = break_line(temp, 1, args);
            LABEL = NULL;
            OPCODE = args[1];
        }
        else
        {
            words = break_line(temp, 0, args);
            LABEL = args[0];
            OPCODE = args[1];
        }
    }
    else
    {
        LOCCTR = 0;
    }

    symtab *base = new symtab;
    symtab_list[(string)PROGNAME] = base;
    while (strcmp(OPCODE, "END") != 0)
    {
        if (line[0] != '.') // check if not a comment
        {
            if (strcmp(OPCODE, "CSECT") == 0)
            {
                // DO THIS
                LOCCTR = print_literals(LOCCTR, outputFile, base);
                base->set_length(LOCCTR);
                base = new symtab;
                symtab_list[(string)LABEL] = base;
                LOCCTR = 0;
            }
            else if (LABEL) // check if symbol in label
            {
                if (base->search_symtab(LABEL))
                {
                    printf("Error: Duplicate Symbol. %s\n", LABEL);
                    ERROR_FLAG = 1;
                    return -1;
                }
                else
                {
                    base->insert_symtab(LABEL, LOCCTR);
                }
            }

            bool extended = false;
            if (OPCODE[0] == '+')
            {
                extended = true;
                OPCODE++;
            }
            if (words > 2)
            {
                OPERAND = args[2];
                if (OPERAND[0] == '=')
                {
                    literal_pool.insert((string)OPERAND);
                }
            }

            // write line to intermediate file
            if (strcmp(OPCODE, "EXTDEF") == 0 || strcmp(OPCODE, "EXTREF") == 0 || strcmp(OPCODE, "LTORG") == 0)
                fprintf(outputFile, "    \t%s", line);
            // else if (strcmp(OPCODE, "EQU") == 0)
            // {
            //     if (strcmp(OPERAND, "*") == 0)
            //     {
            //         fprintf(outputFile, "%04X\t%s", LOCCTR, line);
            //     }
            //     else
            //     {
            //         int size = break_line(OPERAND, 0, args, "+");
            //         int val = 0;
            //         if(size == 2)
            //         {
            //             val += strtol(base->args[0]
            //         }
            //         else {
            //             size = break_line(OPERAND, 0, args, "-");
            //             if(size == 2)
            //             {

            //             }
            //             else {

            //             }
            //         }
            //     }
            // }
            else
                fprintf(outputFile, "%04X\t%s", LOCCTR, line);

            if (opcode_info *info = search_optab(OPCODE))
            {
                if (extended)
                {
                    LOCCTR = LOCCTR + 4 * ILEN;
                }
                else
                    LOCCTR = LOCCTR + info->format * ILEN;
            }
            else if (strcmp(OPCODE, "WORD") == 0)
            {
                LOCCTR = LOCCTR + 3;
            }
            else if (strcmp(OPCODE, "RESW") == 0)
            {
                LOCCTR = LOCCTR + 3 * strtol(OPERAND, NULL, 10);
            }
            else if (strcmp(OPCODE, "RESB") == 0)
            {
                LOCCTR = LOCCTR + strtol(OPERAND, NULL, 10);
            }
            else if (strcmp(OPCODE, "BYTE") == 0)
            {
                if (OPERAND[0] == 'C')
                    LOCCTR = LOCCTR + strlen(OPERAND) - 3;
                else if (OPERAND[0] == 'X')
                    LOCCTR = LOCCTR + (strlen(OPERAND) - 3 + 1) / 2;
            }
            else if (strcmp(OPCODE, "EXTREF") == 0)
            {
                // DO THIS
                int size = break_line(OPERAND, 0, args, ",");
                for (int i = 0; i < size; i++)
                {
                    base->insert_extref(args[i]);
                }
            }
            else if (strcmp(OPCODE, "EXTDEF") == 0)
            {
                // DO THIS
                OPERAND = args[2];
                int size = break_line(OPERAND, 0, args, ",");
                for (int i = 0; i < size; i++)
                {
                    base->insert_extdef(args[i]);
                }
            }
            else if (strcmp(OPCODE, "LTORG") == 0)
            {
                // DO THIS
                LOCCTR = print_literals(LOCCTR, outputFile, base);
            }
            else if (strcmp(OPCODE, "EQU") == 0)
            {
                // DO THIS
            }
            else if (strcmp(OPCODE, "CSECT") == 0)
            {
                // DO THIS
            }
            else
            {
                printf("Error: Invalid operation code. \n");
                ERROR_FLAG = 1;
                return -1;
            }
        }
        else
        {
            fprintf(outputFile, "    \t%s", line);
        }

        // read next input line
        getline(&line, &len, inputFile);
        strcpy(temp, line);
        if (line[0] == ' ')
        {
            words = break_line(temp, 1, args);
            LABEL = NULL;
            OPCODE = args[1];
        }
        else
        {
            words = break_line(temp, 0, args);
            LABEL = args[0];
            OPCODE = args[1];
        }
    }

    fprintf(outputFile, "    \t%s\n", line);

    LOCCTR = print_literals(LOCCTR, outputFile, base);
    base->set_length(LOCCTR);

    // store program length
    PROGLEN = LOCCTR - STARTADDR;

    printf("PASS ONE COMPLETED.\n");

    return 1;
}
