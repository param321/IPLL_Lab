#include "assembler.h"

// Pass 2
// Takes intermediate file as input
// and generates object code
int pass_two(FILE *inputFile, FILE *outputFile, FILE *listingFile)
{
    // set read pointer to beginning of file
    fseek(inputFile, 0, SEEK_SET);

    char *line = NULL, temp[MAXS], addr[MAXS];
    size_t len = 0;
    char *args[MAXW];
    int words;
    char *OPCODE, *LABEL, *OPERAND;

    // read first line
    fscanf(inputFile, "%[^\t]s", addr);
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

    bool first_sect = true;
    queue<modrec> modification_records;

    if (strcmp(OPCODE, "START") == 0)
    {
        // write listing line
        fprintf(listingFile, "%s%s", addr, line);

        // read next input line
        fscanf(inputFile, "%[^\t]s", addr);
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

    symtab *base = symtab_list[(string)PROGNAME];

    // write header record
    fprintf(outputFile, "H%-6s%06X%06X\n", PROGNAME, STARTADDR, base->get_length());

    // initialise first text record
    char record[MAXS] = "";
    char firstaddr[MAXS];
    sprintf(firstaddr, "%0X", STARTADDR);

    while (strcmp(OPCODE, "END") != 0 || true)
    {
        char objcode[MAXS];
        strcpy(objcode, "");
        if (line[1] != '.') // check line not a comment
        {
            cout << OPCODE << endl;
            cout << strlen(record) << endl;
            if (strcmp(OPCODE, "CSECT") == 0)
            {
                // start a new control section
                base = symtab_list[(string)LABEL];

                // write text record
                if (strlen(record) > 0)
                {
                    fprintf(outputFile, "T%06X%02X%s\n", (int)strtol(firstaddr, NULL, 16),
                            (int)strlen(record) / 2, record);
                    strcpy(record, "");
                }

                // write modification records
                while (!modification_records.empty())
                {
                    modrec rec = modification_records.front();
                    modification_records.pop();
                    fprintf(outputFile, "M%06X%02X%s%s\n", rec.addr, rec.length, rec.sign ? "+" : "-", rec.symbol.c_str());
                }

                // write end record
                fprintf(outputFile, "E");
                if (first_sect)
                {
                    fprintf(outputFile, "E%06X", 0);
                    first_sect = false;
                }
                fprintf(outputFile, "\n\n\n");

                // write head record
                fprintf(outputFile, "H%-6s%06X%06X\n", LABEL, 0, base->get_length());
            }

            // set operand value if available
            if (words > 2)
                OPERAND = args[2];
            bool extended = false;
            if (OPCODE[0] == '+')
            {
                extended = true;
                OPCODE++;
            }

            // check if opcode found
            if (opcode_info *info = search_optab(OPCODE))
            {
                int n_bit = 0;
                int i_bit = 0;
                int x_bit = 0;
                int b_bit = 0;
                int p_bit = 0;
                int e_bit = 0;
                int operand_value = 0;

                if (info->format == 2)
                {
                    strcat(objcode, info->addr.c_str());
                    int size = break_line(OPERAND, 0, args, ",");
                    strcat(objcode, register_list[args[0]].c_str());
                    if (size == 2)
                        strcat(objcode, register_list[args[1]].c_str());
                    else
                        strcat(objcode, "0");
                }
                else if (info->format == 3 && !extended)
                {
                    int len = strlen(OPERAND);
                    if (len > 1 && OPERAND[len - 1] == 'X' && OPERAND[len - 2] == ',')
                    {
                        x_bit = 1;
                        OPERAND[len - 2] = '\0';
                    }

                    if (words > 2)
                    {
                        if (OPERAND[0] == '#')
                        {
                            n_bit = 0;
                            OPERAND++;
                        }
                        else
                            n_bit = 1;

                        if (OPERAND[0] == '@')
                        {
                            i_bit = 0;
                            OPERAND++;
                        }
                        else
                            i_bit = 1;

                        if (!isdigit(OPERAND[0]))
                        {
                            symbol_info *sym = base->search_symtab(OPERAND);
                            operand_value = (int)strtol(sym->addr.c_str(), NULL, 16) - (int)strtol(addr, NULL, 16) - 3;
                            if (operand_value < 0)
                            {
                                operand_value += 1 << 12;
                            }
                            p_bit = 1;
                        }
                        else
                        {
                            p_bit = 0;
                            operand_value = (int)strtol(OPERAND, NULL, 10);
                        }
                    }
                    else
                    {
                        n_bit = 1;
                        i_bit = 1;
                    }

                    int num_objcode = (int)strtol(info->addr.c_str(), NULL, 16) * pow(16, 4);
                    num_objcode |= ((n_bit << 17) + (i_bit << 16) + (x_bit << 15) + (b_bit << 14) + (p_bit << 13) + (e_bit << 12));
                    num_objcode |= operand_value;
                    sprintf(objcode, "%06X", num_objcode);
                }
                else if (info->format == 3 && extended)
                {
                    if (words > 2)
                    {
                        int len = strlen(OPERAND);
                        if (len > 1 && OPERAND[len - 1] == 'X' && OPERAND[len - 2] == ',')
                        {
                            x_bit = 1;
                            OPERAND[len - 2] = '\0';
                        }

                        if (OPERAND[0] == '#')
                        {
                            n_bit = 0;
                            OPERAND++;
                        }
                        else
                            n_bit = 1;

                        if (OPERAND[0] == '@')
                        {
                            i_bit = 0;
                            OPERAND++;
                        }
                        else
                            i_bit = 1;

                        e_bit = 1;

                        modification_records.push({(int)strtol(addr, NULL, 16) + 1, 5, true, (string)OPERAND});
                    }
                    int num_objcode = (int)strtol(info->addr.c_str(), NULL, 16) * pow(16, 6);
                    num_objcode |= ((n_bit << 17) + (i_bit << 16) + (x_bit << 15) + (b_bit << 14) + (p_bit << 13) + (e_bit << 12)) << 8;
                    sprintf(objcode, "%08X", num_objcode);
                }
            }
            else if (strcmp(OPCODE, "BYTE") == 0)
            {
                if (OPERAND[0] == 'C')
                {
                    int c;
                    for (int i = 2; i < strlen(OPERAND) - 1; i++)
                    {
                        int c = OPERAND[i];
                        char temp[2];
                        sprintf(temp, "%0X", c);
                        strcat(objcode, temp);
                    }
                }
                else if (OPERAND[0] == 'X')
                {
                    strcat(objcode, OPERAND + 2);
                    objcode[strlen(objcode) - 1] = '\0';
                }
            }
            else if (strcmp(OPCODE, "WORD") == 0)
            {
                sprintf(objcode, "%06X", (int)strtol(OPERAND, NULL, 10));
                cout << OPERAND << endl;
                int size = break_line(OPERAND, 0, args, "+");
                if (size == 2)
                {
                    modification_records.push({(int)strtol(addr, NULL, 16), 6, true, (string)args[0]});
                    modification_records.push({(int)strtol(addr, NULL, 16), 6, true, (string)args[1]});
                }
                else
                {
                    size = break_line(OPERAND, 0, args, "-");
                    if (size == 2)
                    {
                        modification_records.push({(int)strtol(addr, NULL, 16), 6, true, (string)args[0]});
                        modification_records.push({(int)strtol(addr, NULL, 16), 6, false, (string)args[1]});
                    }
                    else
                    {
                        modification_records.push({(int)strtol(addr, NULL, 16), 6, true, (string)args[0]});
                    }
                }
            }
            else if (strcmp(OPCODE, "EXTREF") == 0)
            {
                // print refer reocrd
                fprintf(outputFile, "R");
                int size = break_line(OPERAND, 0, args, ",");
                for (int i = 0; i < size; i++)
                {
                    fprintf(outputFile, "%-6s", args[i]);
                }
                fprintf(outputFile, "\n");
            }
            else if (strcmp(OPCODE, "EXTDEF") == 0)
            {
                // print define record
                fprintf(outputFile, "D");
                int size = break_line(OPERAND, 0, args, ",");
                for (int i = 0; i < size; i++)
                {
                    fprintf(outputFile, "%-6s%06X", args[i], (int)strtol(base->search_symtab(args[i])->addr.c_str(), NULL, 16));
                }
                fprintf(outputFile, "\n");
            }
            else if (strcmp(OPCODE, "LTORG") == 0)
            {
                // DO THIS
                // IGNORE
            }
            else if (strcmp(OPCODE, "EQU") == 0)
            {
                // DO THIS
                cout << OPERAND << endl;
                int size = break_line(OPERAND, 0, args, "+");
                if (size == 2)
                {
                    symbol_info *sym = base->search_symtab(args[0]);
                    int val = (int)strtol(sym->addr.c_str(), NULL, 16);
                    sym = base->search_symtab(args[1]);
                    val += (int)strtol(sym->addr.c_str(), NULL, 16);
                    sprintf(addr, "%04X", val);
                }
                else
                {
                    size = break_line(OPERAND, 0, args, "-");
                    if (size == 2)
                    {
                        symbol_info *sym = base->search_symtab(args[0]);
                        int val = (int)strtol(sym->addr.c_str(), NULL, 16);
                        sym = base->search_symtab(args[1]);
                        val -= (int)strtol(sym->addr.c_str(), NULL, 16);
                        sprintf(addr, "%04X", val);
                    }
                    else if (strcmp(OPERAND, "*") != 0)
                    {
                        symbol_info *sym = base->search_symtab(args[0]);
                        int val = (int)strtol(sym->addr.c_str(), NULL, 16);
                        sprintf(addr, "%04X", val);
                    }
                }
            }
            else if (strcmp(OPCODE, "CSECT") == 0)
            {
                // DO THIS
                // IGNORE
            }
            if (LABEL[0] == '*')
            {
                break_line(LABEL, 0, args, "=");
                OPERAND = args[1];
                if (OPERAND[0] == 'C')
                {
                    int c;
                    for (int i = 2; i < strlen(OPERAND) - 1; i++)
                    {
                        int c = OPERAND[i];
                        char temp[2];
                        sprintf(temp, "%0X", c);
                        strcat(objcode, temp);
                    }
                }
                else if (OPERAND[0] == 'X')
                {
                    strcat(objcode, OPERAND + 2);
                    objcode[strlen(objcode) - 1] = '\0';
                }
            }

            // check if new record fits
            // or for discontinuity in adress
            if (strlen(record) + strlen(objcode) > 60 || strcmp(OPCODE, "RESW") == 0 || strcmp(OPCODE, "RESB") == 0)
            {
                if (strlen(record) > 0)
                {
                    fprintf(outputFile, "T%06X%02X%s\n", (int)strtol(firstaddr, NULL, 16),
                            (int)strlen(record) / 2, record);
                }
                strcpy(record, "");
            }

            // set start address of record
            if (strlen(record) == 0)
            {
                strcpy(firstaddr, addr);
            }
            strcat(record, objcode);

            // write listing line
            line[strlen(line) - 1] = '\0';
            fprintf(listingFile, "%s%-26s\t%s\n", addr, line, objcode);
        }
        else
        {
            fprintf(listingFile, "\t%s", line);
        }

        // read next input line
        if (fscanf(inputFile, "%[^\t]s", addr) == -1)
            break;
        getline(&line, &len, inputFile);
        strcpy(temp, line);
        words = break_line(temp, 0, args);
        LABEL = args[0];
        OPCODE = args[1];
        while (LABEL[0] == ' ' || LABEL[0] == '\t')
            LABEL++;
    }

    // write last text record
    if (strlen(record) > 1)
    {
        fprintf(outputFile, "T%06X%02X%s\n", (int)strtol(firstaddr, NULL, 16),
                (int)strlen(record) / 2, record);
    }

    base = symtab_list[(string)PROGNAME];

    while (!modification_records.empty())
    {
        modrec rec = modification_records.front();
        modification_records.pop();
        fprintf(outputFile, "M%06X%02X%s%s\n", rec.addr, rec.length, rec.sign ? "+" : "-", rec.symbol.c_str());
    }

    // write end record
    fprintf(outputFile, "E");
    if (first_sect)
    {
        fprintf(outputFile, "%06X", 0);
        first_sect = false;
    }
    fprintf(outputFile, "\n\n\n");

    // // write last listing line
    // fprintf(listingFile, "%s%s", addr, line);

    printf("PASS TWO COMPLETED.\n");

    return 1;
}