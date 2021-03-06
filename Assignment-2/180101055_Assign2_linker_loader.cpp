/*
ENVIRONMENT: LINUX g++ 
gcc version : 9.3.0
Command To compile the source code : g++ 180101055_Assign2_linker_loader.cpp
Command to execute : ./a.out
*/

#include <bits/stdc++.h>
using namespace std;

//name of the input file containing the object code
string inputFileName;  		

//control_section_address, program_address and execution address 
int controlSecAddr;
int progAddr;
int execAddr;	

ifstream input_file;

ofstream pass1File; //output of pass1 
ofstream pass2File; //output of pass2

class es{
public:
	int address;
	int length;
	es(){
		this->address=0;
		this->length=0;
	}
	es(int addr,int len){
		this->address=addr;
		this->length=len;
	}
};

//external symbol table
vector<pair<string,es>> EStable;	

vector<pair<int,string>> Memory_objcode;	//stores the memory content byte wise : address mapped to Memory_objcode

//searches the vector by name of external symbol name 
int findIndexBySymbolName(vector<pair<string,es>> table,string symbol){
    int i=0;
	for(auto it:table){
		if(it.first==symbol){
			return i;
		}
		i++;
	}
	return -1;
}

//searches the vector by address of external symbol name
int findIndexBySymbolAddr(vector<pair<int,string>> objcode,int addr){
	int i=0;
	for(auto it:objcode){
		if(it.first==addr){
			return i;
		}
		i++;
	}
	return -1;
} 


char intToHexSingleDigit(int n){
    char c;
    if (n <= 9){
        c = '0' + n;
    }else{
        c = 'A' + (n - 10);
    }
    return c;
}

//converts integer to a minimum min_len digit hexadeicmal string
string intToHex(int n,int len){
	string s = "";
    while (n > 0){
        s = intToHexSingleDigit(n % 16) + s;
        n = n / 16;
    }
    while (s.length() < len){
        s = '0' + s;
    }
    return s;
}

int hexToIntSingleChar(char c){
    int n;
    if ((c <= '9') && (c >= '0')){
        n = c - '0';
    }else{
        n = c - 'A' + 10;
    }
    return n;
}

int hexToInt(string s){
    reverse(s.begin(), s.end());
    int j = 1;
    int ans = 0;
    for (int i = 0; i < s.length(); i++){
        ans += (hexToIntSingleChar(s[i]) * j);
        j = j * 16;
    }
    return ans;
}

string hexToStringLen6(int i){
    stringstream ss;
    ss<<hex<<i;
    string res=ss.str();
    if(res.length()<6)res.insert(0,6-res.length(),'0');
    transform(res.begin(), res.end(), res.begin(), ::toupper);
    return res;
}

vector<string> readLineToWords(string str) { 
	vector <string> words;

    string word = ""; 
    for(auto x : str){ 
        if (x == ' '){
			if(word!=""){
				words.push_back(word);
			} 
            word = ""; 
        }else{ 
            word = word + x; 
        } 
    } 
    words.push_back(word);
	return words;
} 


int DTypeRecords(vector<string> records){

	string temp;
	temp = "";
	for(int i=1;i<records[0].length();i++){
		temp+= records[0][i];
	}
	records[0] = temp;

	string es_name;
	int no_of_es;
	es_name = records[0];
	no_of_es = records.size()-1;

	int i=0;
	while(i<no_of_es){
		string es_addr;
		es_addr = "";
		
		for(int j=0;j<=5;j++){
			es_addr += records[i+1][j];
		}

		if(findIndexBySymbolName(EStable,es_name)!=-1){
			printf("ERROR: DUPLICATE SYMBOL FOUND\n");
			exit(0);
			return -1;
		}else{
			int new_addr = hexToInt(es_addr) + controlSecAddr ;

			struct es new_es(new_addr,0);

			EStable.push_back({es_name,new_es});

			pass1File<<"    \t\t"<<es_name<<" \t\t ";

			pass1File << intToHex(new_es.address,4)<<" \t\t \n";

		}

		if(i!=(no_of_es-1)){
			es_name="";
			for(int j=6;j<records[i+1].length();j++){
				es_name+=records[i+1][j];
			}
		}
		i=i+1;
	}

	return 1;
}

int TTypeRecords(string records){
	string addr;

	addr = "";
	
	for(int i=1;i<7;i++){
		addr.push_back(records[i]);
	}
	
	int location = controlSecAddr;
	location += hexToInt(addr);

	int i=9;
	while(i<records.length()){
		string temp;
		temp = "";
		for(int j=i;j<=i+1;j++){
			temp.push_back(records[j]);
		}

		Memory_objcode.push_back({location,temp});

		location=location+1;

		i+=2;
	}

	return 1;
}

string ModificationType(string old_val,string symbol_name,string op){

	int new_val;

	if(old_val[0]=='F'){
		long int overflow_handler = (long int)0xFFFFFFFF000000;
		new_val = overflow_handler; //used as a borrow for doing negative calculations with hex numbers
	}else{
		new_val = 0;
	}

	new_val += hexToInt(old_val); 

	//add or sub according to M record
	if(op.back()=='+'){
		new_val += EStable[findIndexBySymbolName(EStable,symbol_name)].second.address;
	}else{
		new_val -= EStable[findIndexBySymbolName(EStable,symbol_name)].second.address ;
	} 

	string new_hex = hexToStringLen6(new_val);

	if(new_hex.length()>6 && new_hex[0]=='F' && new_hex[1]=='F'){
		string temp;
		temp = "";
		for(int i=2;i<new_hex.length();i++){
			temp+= new_hex[i];
		}
		new_hex = temp;
	}

	return new_hex;
}

int MTypeRecords(string records){
	string symbol_name="";
	string sym_loc="";
	string sym_len="";

	int i=1;

	while(i<records.length()){
		if(i<=6){
			sym_loc+=records[i];
		}else if(i<=9){
			sym_len+=records[i];
		}else{ 
			symbol_name+=records[i];
		}
		i++;
	}

	int symbolAddr = hexToInt(sym_loc);
	symbolAddr += controlSecAddr;


	string old_val="";
	int ind[3];
	for(i=0;i<3;i++){
		ind[i]=findIndexBySymbolAddr(Memory_objcode,symbolAddr+i);
	}

	for(int v=0;v<3;v++){
		if(ind[v]!=-1){
			old_val+=Memory_objcode[ind[v]].second;
		}
	}
	
	string str_hex = ModificationType(old_val,symbol_name,sym_len);

	int k=0;
	i=0;
	while(i<3){
		if(ind[i]==-1){
			Memory_objcode.push_back({symbolAddr+i,""});
			Memory_objcode.back().second+=str_hex[k];
			k++;
			Memory_objcode.back().second+=str_hex[k];
			k++;
		}else{
			Memory_objcode[ind[i]].second="";
			Memory_objcode[ind[i]].second+=str_hex[k];
			k++;
			Memory_objcode[ind[i]].second+=str_hex[k];
			k++;
		}
		i++;
	}

	return 1;
}

//pass 1 of the assembler
int pass1(){
	
	input_file.open(inputFileName);

	//this file will contain the external symbol table as contructed after pass1;
	pass1File.open("esTableFile.txt");

	pass1File<<"ControlSec\tSymbolName   Addr    \t Length\n";

	//assign starting address for relocation
	controlSecAddr=progAddr; 

    //stores the name of the current control section
	string current_secname=""; 
	int cs_length=0;

	string line="";

	while(getline(input_file,line,'\n')){

		vector<string> line_w=readLineToWords(line);

		if(line_w[0][0]=='H'){

			string csLen="";
			string controlSecAddr_obj="";

			for(int i=0;i<6;i++){
				controlSecAddr_obj+=line_w[1][i];
			}

			for(int i=6;i<line_w[1].length();i++){
				csLen += line_w[1][i];
			}

			string temp;
			temp = "";
			for(int i=1;i<line_w[0].length();i++){
				temp+= line_w[0][i];
			}
			line_w[0] = temp;

			if(findIndexBySymbolName(EStable,line_w[0])>=0){
				cout<<"ERROR : DUPLICATE SECTION FOUND\n";
				return -1;
			}else{
				current_secname=line_w[0];
				int addR = hexToInt(controlSecAddr_obj);
				addR += controlSecAddr;
				es new_es (addR,hexToInt(csLen));
				EStable.push_back({line_w[0],new_es});

				pass1File<<line_w[0]<<" \t\t  \t\t\t ";
                pass1File << intToHex(new_es.address,4) <<" \t\t ";
                pass1File<< intToHex(new_es.length,4)<<" \t\t \n";
			}

			while(getline(input_file,line,'\n')){
				vector<string> records=readLineToWords(line);

                //if end record
				if(records[0][0]=='E'){ 
					break;
				}

                //if define records
				if(records[0][0]=='D'){ 
					DTypeRecords(records);
				}
			}

			controlSecAddr += hexToInt(csLen);
		}

	}
	input_file.close();
	pass1File.close();

	return 1;
}

//pass2 of the loader 
int pass2(){

	input_file.open(inputFileName);

	int cs_len;

	string line="";

	controlSecAddr=progAddr;

	execAddr=progAddr;

	//reading the input file 
	while(getline(input_file,line,'\n')){

		vector<string> line_w=readLineToWords(line);

		if(line_w[0][0]=='H'){

			string temp;
			temp = "";
			for(int i=1;i<line_w[0].length();i++){
				temp+= line_w[0][i];
			}
			line_w[0] = temp;

			string current_secname;
			current_secname=line_w[0];
			int index=findIndexBySymbolName(EStable,current_secname);
			cs_len=EStable[index].second.length;

			while(getline(input_file,line,'\n')){
				vector<string> records=readLineToWords(line);

                //if end of section
				if(records[0][0]=='E'){
					break;
				}

                //if text record
				if(records[0][0]=='T'){ 
					TTypeRecords(records[0]);

				}

                //if modification record
				if(records[0][0]=='M'){
					MTypeRecords(records[0]);
				}
			}

            //if address is specified in the END record 
			if(line[0]=='E'){ 
				if(line.length()>1){
					string temp;
					temp = "";
					for(int i=1;i<line.length();i++){
						temp+= line[i];
					}
					line = temp;
				
					execAddr = controlSecAddr + hexToInt(line);
				}
			}
			controlSecAddr +=cs_len;
		}

	}

	input_file.close();

    return 1;
}

int generate_obj(){
	
	pass2File.open("pass2LinkerFile.txt");

	pass2File<<"Contents Of Memory Address"<<endl;
	
	int addr_counter= progAddr;

	addr_counter-=16;

	int addrEnd =(*Memory_objcode.rbegin()).first;

	int i=0;
	while(addr_counter <= addrEnd){
		int index=0;

		if(i%16==0){
			pass2File<<intToHex(addr_counter,4)<<" \t ";
		}

		if(addr_counter<progAddr){

			pass2File<<'x';
			pass2File<<'x';
			if((i+1)%4==0){
				pass2File<<"\t";
			}

		}else if((index=findIndexBySymbolAddr(Memory_objcode,addr_counter))>=0 && Memory_objcode[index].second!=""){
			pass2File<<Memory_objcode[index].second;
			if((i+1)%4==0){
				pass2File<<" \t";	
			}	
		}else{
			pass2File<<'.';
			pass2File<<'.';
			if((i+1)%4==0){
				pass2File<<"\t";
			}
		}

		if((i+1)%16==0){
			pass2File<<"\n";
		}

		addr_counter++;
		i++;
	}

	while((progAddr-addr_counter)%16!=0){

		pass2File<<'x';
		pass2File<<'x';
		if((i+1)%4==0){
			pass2File<<"\t";
		}

		i++;
		addr_counter++;
	}

	pass2File.close();

	return 1;
}


int main(){

	inputFileName="linker_input.txt";
	
	progAddr=hexToInt("4000");

	pass1();

	cout<<"es table created in pass1File.txt\n";

	pass2();

	cout<<"memory table created in pass2File.txt\n";

	generate_obj();
}
