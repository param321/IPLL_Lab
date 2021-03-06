#include "assembler.h"

// Break line into words
int break_line(char *str, int start, char *words[], const char *delimiter)
{
	str = strtok(str, "\n");

	int size = start; // Store number of words
	char *ptr = strtok(str, delimiter);

	while (ptr != NULL)
	{
		words[size++] = ptr;
		ptr = strtok(NULL, delimiter);
	}

	return size;
}

// Loads the optable from OpcodeTable.txt
// Return 1 is successful or -1 if unsuccessful
int load_optab()
{
	FILE *ot = fopen(OPTAB, "r");
	if (!ot)
	{
		cout << "Error here" << endl;
		return -1;
	}
	char row1[MAXS], row2[MAXS];
	int format;
	string opcode, code;
	while (fscanf(ot, "%s %s %d", row1, row2, &format) != EOF)
	{
		opcode = row1;
		code = row2;
		if (optab.find(opcode) != optab.end())
			return -1;
		optab[opcode] = {code, format};
	}
	fclose(ot);
	return 1;
}

// Search opcode table for specified
// operation and return corresponding opcode
opcode_info *search_optab(char *OPCODE)
{
	string opcode = OPCODE;
	if (optab.find(opcode) == optab.end())
	{
		return NULL;
	}
	else
	{
		return &optab[opcode];
		// const char *code = (optab[opcode]).addr.c_str();
		// return code;
	}
	return NULL;
}

// Search symbol table for specified
// symbol and return corresponding adress
symbol_info *symtab::search_symtab(char *LABEL)
{
	string label = LABEL;
	table.find("RLOOP");
	if (table.find(label) == table.end())
	{
		return NULL;
	}
	else
	{
		return &table[label];
		// const char *code = (symtab[label]).addr.c_str();
		// return code;
	}
	return NULL;
}

// Insert symbol and adress in the symbol table
int symtab::insert_symtab(const char *LABEL, int LOCCTR)
{
	FILE *filesymtab = fopen(SYMTAB, "a");
	string label = LABEL;
	char addr[MAXW];
	sprintf(addr, "%0X", LOCCTR);
	table[label] = {addr};
	fprintf(filesymtab, "%s\t%s\n", addr, LABEL);
	fclose(filesymtab);
	return 1;
}

int symtab::print_symtab()
{
	for (auto i = table.begin(); i != table.end(); i++)
	{
		cout << i->first << ' ' << i->second.addr << endl;
	}
	return 1;
}

int symtab::insert_extdef(char *LABEL)
{
	extdef.insert((string)LABEL);
	return 1;
}

int symtab::insert_extref(char *LABEL)
{
	extref.insert((string)LABEL);
	return 1;
}

int print_literals(int LOCCTR, FILE *outputFile, symtab *base)
{
	while (literal_pool.size())
	{
		string lit = *literal_pool.begin();
		fprintf(outputFile, "%04X\t*\t%s\n", LOCCTR, lit.c_str());
		base->insert_symtab(lit.c_str(), LOCCTR);
		if (lit[1] == 'C')
		{
			LOCCTR += lit.length() - 4;
		}
		else
		{
			LOCCTR += (lit.length() - 4 + 1) / 2;
		}
		literal_pool.erase(literal_pool.begin());
	}
	return LOCCTR;
}

symtab::symtab()
{
	
}

void symtab::set_length(int length)
{
	this->length = length;
}

int symtab::get_length()
{
	return length;
}