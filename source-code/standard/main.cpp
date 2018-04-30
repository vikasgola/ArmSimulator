#include<iostream>
#include<fstream>
#include<algorithm>
#include<vector>
#include<map>
#include<string>
#include"functions.h"

using namespace std;

//  TODO list
// LDR                      done
// STR                      done
// ADD                      done
// SUB                      done
// MUL                      done
// CMP                      done
// BNE                      done
// BGE                      done
// B                        done
// BL                       done
// MOV                      done
// pc                       done
// lr                       done

// ======================================== Declaring some global varibles=================================
vector <string> instii_complete;    // stores all lines of code(but not empty lines)
fstream input , debug , output,latency,proc_info;                // input file is code and debug file is for debugging
int r[16];               // assuming intial value of all registers to 0.
    
int memory[10000],pause;         // memory of 100 words
bool cmp_result,lat = true;        // storing result of cmp instruction for next instruction
string mode = "run";

short int cmp;          // compareble values storage
vector <string> labels;         // stores labels name of functions
vector <int> labelsid;          // labels name storage
int empty =-1;                   // counter  for empty lines
int total_instruction = 0;      // total instructions count

vector<vector <int>> sp,ep;     // vectors for storing starting and ending positions of word in each line
class arm_array{
    public:               // for labels(named pointers) storage
    string name;
    int start;
    int end;
    arm_array(string a,int s, int e){
        name =a;
        start = s;
        end = e;
    }
};
vector <arm_array> arm_lab;     // store my labels here
string box_type = "data";                // store what type of box it is text or data
clock_t tStart;                 // keeping time to execute

int loop_time = 0;
int total_cycles = 0;           // total cycles in code
map <string, int> lat_values,latency_total_instru;
string proc,model;
struct pipeline {
    string instruction;
    string operation;
    vector<int> s;
    vector<int> e;
    int memory;
    int operation_type;
    int memory_offset;
    int alu_result;
    int r1 = -123456789,r2 = -123456789,res,c1 = -1,c2 = -1;
} stages[5];

string pipeline_out;


//  ========================= functions declartions start=======================================



// showing all registers registers 
void show_register(){
    output<<"\t";
    cout<<"\t";    
    for(int i=0;i<14;i++){
        output<<"r"<<i<<" = "<<r[i]<<" | ";
        cout<<"r"<<i<<" = "<<r[i]<<" | ";
        if(i==7) {
            output<<endl;
            output<<"\t";
            cout<<endl;
            cout<<"\t";
        }        
    }
    output<<"r14(lr) = "<<r[14]<<" | ";
    output<<"r15(pc) = "<<r[15]+4<<" ";
    output<<endl;
    output<<"\tTotal cycles completed: "<<total_cycles;
    output<<endl<<endl<<endl;

    cout<<"r14(lr) = "<<r[14]<<" | ";
    cout<<"r15(pc) = "<<r[15]+4<<" ";
    cout<<endl;
    cout<<"\tTotal cycles completed: "<<total_cycles;
    cout<<endl<<endl<<endl;
}

// showing memory for memory related istructions                        assuming memory starts from 1000000
void show_memory(){
    output<<"\t\t\t Memory Addresses: stored Value"<<endl;
    cout<<"\t\t\t Memory Addresses: stored Value"<<endl;
    
    if(arm_lab.size())
    for(int i =0;i<arm_lab.at(arm_lab.size()-1).end;i++){
        output<<1000000+i*4<<": "<<memory[i]<<"\t\t  ";
        cout<<1000000+i*4<<": "<<memory[i]<<"\t\t  ";
        
        if(i%5 == 4 && i != 0){
            output<<endl;
            cout<<endl;
        }                       
    }
    output<<endl<<endl;
    cout<<endl<<endl;
    
}

// showing important message related to instructions
void message_show(string s){
    output<<s<<endl;
    cout<<s<<endl;
}

// showing cmp_result
void show_cmp_result(){
    if(cmp_result == false){
        output<<"\tcmp result = "<<"false"<<endl;
        cout<<"\tcmp result = "<<"false"<<endl;
    }
    else{
        output<<"\tcmp result = "<<"true"<<endl;
        cout<<"\tcmp result = "<<"true"<<endl;
    } 
        
}


//  syntax error handling
void error_show(int row,int col){
    output<<"Syntax error at line: "<<row<<" and column: "<<col<<endl;
    cout<<"Syntax error at line: "<<row<<" and column: "<<col<<endl;
    
    exit(EXIT_FAILURE);
}


// shows the registers and memories after each instruction
void show_details(){
        output<<"\t\t\t\t=========== "<<r[15]<<": "<<instii_complete.at((r[15]-1000)/4)<<" ==========="<<endl;
        cout<<"\t\t\t\t=========== "<<r[15]<<": "<<instii_complete.at((r[15]-1000)/4)<<" ============"<<endl;
        
        if(str_is(instii_complete.at((r[15]-1000)/4 ).substr(0,3),"bne","BNE") || str_is(instii_complete.at((r[15]-1000)/4).substr(0,3),"bge","BGE")){
            show_cmp_result();                                                                      // showing compare result only if there is any cmp
        }else if(str_is(instii_complete.at((r[15]-1000)/4 ).substr(0,3),"ldr","LDR") || str_is(instii_complete.at((r[15]-1000)/4 ).substr(0,3),"str","STR")){
            show_memory();                                                                          // showing memory only in case of load and store
        }

        show_register();
}

void bye(){

    cout<<"\t \t \t \t ===========Summary==========="<<endl;
    output<<"\t \t \t \t ===========Summary==========="<<endl;

    cout<<"Instructions and Cycles details:"<<endl;
    for(map <string,int>::const_iterator it = latency_total_instru.begin();it!= latency_total_instru.end();it++){
        cout<<"\t "<<it->first<<" : "<<it->second<<" cycles \t & "<<it->second/4<<" times"<<endl;
        output<<"\t "<<it->first<<" : "<<it->second<<" cycles \t & "<<it->second/4<<" times"<<endl;        
    }
    cout<<endl;

    cout<<"Memory Details: "<<endl;
    show_memory();

    cout<<"Register Details: "<<endl;
    show_register();

    output<<" Total Instructions executed: "<<total_instruction<<" \t Total cycles: "<<total_cycles<<"\t Empty Lines: "<<empty<<" \n Total Lines: "<<instii_complete.size()+empty<<" \t Execution Time(s) "<<(double)(clock() - tStart)/CLOCKS_PER_SEC<<"\t Average Instruction per cycle: "<<(float)total_instruction/total_cycles<<endl;
    cout<<" Total Instructions executed: "<<total_instruction<<" \t Total cycles: "<<total_cycles<<"\t Empty Lines: "<<empty<<" \n Total Lines: "<<instii_complete.size()+empty<<" \t Execution Time(s) "<<(double)(clock() - tStart)/CLOCKS_PER_SEC<<"\t Average Instruction per cycle: "<<(float)total_instruction/total_cycles<<endl;
    
    cout<<endl;

    cout<<" Total Execution time of ARM code in your pc according to given latency: "<<(float)total_cycles/stoi(proc)<<" Microseconds"<<endl;
    output<<" Total Execution time of ARM code in your pc according to given latency: "<<(float)total_cycles/stoi(proc)<<" Microseconds"<<endl;

    exit(EXIT_SUCCESS);
}


// checking for characters which are not possible in instructions
bool not_possible_chars(char c){
    // 64-91 captial chars
    // 96-123 small chars
    // 0-9 numbers
    //  35 #
    // = equality sign
    // : coln
    // []
    if((c>64 && c<91) || (c>96 && c<123) || (c>47 && c<58) || c==35 || c==61 || c==58 || c=='.' || c=='_' || c == '[' || c == ']'){
        return false;
    }
    return true;
}

//  checking for each word that is acceptable or not
bool word_check(string check_word){

    for(int i=0;i<check_word.size();i++){
        if(not_possible_chars(check_word[i])){
            return false;
        }
    }
    return true;
}

//  check for st equals to li or li2
bool str_is(string st,string li,string li2){
    if(st == li || st == li2){
        return true;
    }
    return false;
}


//  charcters which can seprate the word in a instructions are "space", "," and "horizontal tab" 
bool word_seprators(char t){
    if( t== ' ' || t==',' || t==9 ){
        return true;
    }
    return false;
}



//b, bl, bne and bge instructions handling
void bnebge(char k , string do_this[]){
    if(k == 'n'){                                           // BNE ===========
        if(cmp != 0){
            cmp_result = true;            
            debug<<endl<<"\t\t\t Going back to top\t\t\t"<<endl;
            ptrdiff_t here = distance(labels.begin() , find(labels.begin(),labels.end(),do_this[1]));
            if(!(here < labels.size() )){
                message_show("label not found");
                exit(EXIT_FAILURE);
            }
            show_details();
            r[15] = labelsid.at(here);
            loop_time++;                                                
        }else{
            cmp_result = false;            
            debug<<endl<<"============== loop finished========="<<endl;
            show_details();            
            r[15] = r[15] + 4;
            debug<<endl<<"Loop count: "<<loop_time<<endl;                        
        }
    }else if(k == 'g'){                                        // BGE=============
        if(cmp >= 0){
            cmp_result = true;
            debug<<endl<<"\t\t\t Going back to top\t\t\t"<<endl;
            ptrdiff_t here = distance(labels.begin() , find(labels.begin(),labels.end(),do_this[1]));
            if(!(here < labels.size() )){
                message_show("label not found");
                exit(EXIT_FAILURE);
            }
            show_details();
            r[15] = labelsid.at(here);
            loop_time++;                        
        }else{
            cmp_result = false;            
            
            debug<<endl<<"============== loop finished========="<<endl;
            show_details();            
            r[15] = r[15] + 4;
            debug<<endl<<"Loop count: "<<loop_time<<endl;                        
            
        }
    }else if(k == 'b'){                                         // B==================
            // ptrdiff_t here = distance(labels.begin() , find(labels.begin(),labels.end(),do_this[1]));
            int m;
            for(int i=0;i<instii_complete.size();i++){
                if(do_this[1] + ":" == instii_complete.at(i).substr(sp.at(i).at(0) , ep.at(i).at(0) - sp.at(i).at(0) )){
                    m = i;
                }
            }

            if((m == instii_complete.size() )){
                message_show("label not found");
                exit(EXIT_FAILURE);
            }

            show_details();
            r[15] = 1000 + m*4;

    }else if(k== 'l'){                                               //BL========================
        r[14] = r[15] + 4;
        int m;
        for(int i=0;i<instii_complete.size();i++){
            if(do_this[1] + ":" == instii_complete.at(i).substr(sp.at(i).at(0) , ep.at(i).at(0) - sp.at(i).at(0) )){
                m = i;
            }
        }

        if((m == instii_complete.size() )){
            message_show("label not found");
            exit(EXIT_FAILURE);
        }

        show_details();
        r[15] = 1000 + m*4;        
    }
}



// stage1 of pipeline called instruction fetch
void instructionFetch(vector <int> s ,vector <int> e,const string instruc){
    pipeline_out="\t\t\t\t\t ==Pipeline==\n";

    if(!instruc.empty() ){
        stages[0].instruction = instruc;
        stages[0].s = s;
        stages[0].e = e;
        pipeline_out +=  "\t\t IF: "+stages[0].instruction+"\n";
    }else{
        stages[0].instruction = "";
        pipeline_out += "\t\t IF: Blank\n";
        
    }

    show_details();
    instructionDecode();
    total_cycles++;    
    cout<<pipeline_out<<endl<<endl<<endl;
    stages[1] = stages[0];
    // r[15]+=4;
}


// stage 2 of pipeline
void instructionDecode(){
   

    if(!stages[1].instruction.empty() ){
            // different words in different element of array
        string do_this[stages[1].s.size()];
        for(int j=0;j<stages[1].s.size();j++){
            do_this[j] = stages[1].instruction.substr(stages[1].s.at(j) ,stages[1].e.at(j) - stages[1].s.at(j)); // array of readed word from one instruction
        }
        // saving operation to operation property
            stages[1].operation = do_this[0];   

        // decoding the instruction
        // saving register position of r1 r2 and r3 also constants
        if(str_is(do_this[0] ,"SUB","sub") || str_is(do_this[0] ,"MUL","mul") || str_is(do_this[0] ,"ADD","add") ){
            if(do_this[2][0] =='r' && do_this[3][0] =='r' ){
                stages[1].res =  stoi(do_this[1].substr(1 , do_this[1].length()-1));
                stages[1].r1 = stoi(do_this[2].substr(1,do_this[2].length()-1));
                stages[1].r2 = stoi(do_this[3].substr(1,do_this[3].length()-1));
                stages[1].operation_type = 01;
            }else if(do_this[2][0] =='#' && do_this[3][0] =='#'){
                stages[1].res =  stoi(do_this[1].substr(1 , do_this[1].length()-1));
                stages[1].c1 = stoi(do_this[2].substr(1,do_this[2].length()-1));
                stages[1].c2 = stoi(do_this[3].substr(1,do_this[3].length()-1));
                stages[1].operation_type = 02;
            }else if(do_this[2][0] =='#' && do_this[3][0] =='r'){
                stages[1].res =  stoi(do_this[1].substr(1 , do_this[1].length()-1));
                stages[1].c1 = stoi(do_this[2].substr(1,do_this[2].length()-1));
                stages[1].r2 = stoi(do_this[3].substr(1,do_this[3].length()-1));
                stages[1].operation_type = 03;                        
            }else if(do_this[3][0] =='#' && do_this[2][0] =='r'){
                stages[1].res =  stoi(do_this[1].substr(1 , do_this[1].length()-1));
                stages[1].r1 = stoi(do_this[2].substr(1,do_this[2].length()-1));
                stages[1].c2 = stoi(do_this[3].substr(1,do_this[3].length()-1));
                stages[1].operation_type = 04;
            }else {
                error_show(instii_complete.size()+1+ empty,0);
            }
        } else if(str_is(do_this[0] ,"CMP","cmp") || str_is(do_this[0] ,"MOV","mov")){
            if(do_this[2][0] =='r'){
                stages[1].res = stoi(do_this[1].substr(1,do_this[1].length()-1));
                stages[1].r1 = stoi(do_this[2].substr(1,do_this[2].length()-1));
                stages[1].operation_type = 11;            
            }else if(do_this[2][0] =='#'){
                stages[1].res = stoi(do_this[1].substr(1,do_this[1].length()-1));  //register position
                stages[1].c1 = stoi(do_this[2].substr(1,do_this[2].length()-1)); //constants
                stages[1].operation_type = 12;
            }else {
                error_show(instii_complete.size()+1+empty,0);                        
            }
        } else if(str_is(do_this[0] ,"str","STR")){
            if(do_this[2][0] =='[' && do_this[2][do_this[2].length()-1] ==']'){
                stages[1].memory = (r[stoi(do_this[2].substr(2,do_this[2].length()-2))]-1000000)/4;  //storing memory address
                stages[1].r1 = stoi(do_this[1].substr(1,do_this[1].length()-1));
                stages[1].operation_type = 21;
            }else if(do_this[2][0] =='[' && do_this[3][do_this[3].length()-1] ==']' && do_this[3][0] =='#'){
                stages[1].memory = (r[stoi(do_this[2].substr(2,do_this[2].length()-2))]-1000000)/4;
                stages[1].memory_offset = stoi(do_this[3].substr(1,do_this[2].length()-1))/4;
                stages[1].r1 = stoi(do_this[1].substr(1,do_this[1].length()-1));
                stages[1].operation_type = 22;
            }else{
                error_show( (r[15]-1000)/4+1+empty , instii_complete.at((r[15]-1000)/4).length() );                                    
            }
        }else if(str_is(do_this[0],"ldr","LDR")){
            if(do_this[2][0] =='=' ){
                int i;
                for(i=0;i<arm_lab.size();i++){
                    if(arm_lab.at(i).name == do_this[2].substr(1 , do_this[2].length()-1) ){
                        break;
                    }
                }
                if(i >= arm_lab.size()){
                    message_show("Label Not found!!! or Please write your space or word in next line!!!");
                    exit(EXIT_FAILURE);
                }else {

                    stages[1].res =  stoi(do_this[1].substr(1,do_this[1].length()-1));
                    stages[1].c1 = 1000000 + arm_lab.at(i).start * 4;        // starting address is 1000000
                    stages[1].operation_type = 31;
                }
                
            }else if(do_this[2][0] =='[' && do_this[2][do_this[2].length()-1] ==']'){
                stages[1].memory = (r[stoi(do_this[2].substr(2,do_this[2].length()-2))]-1000000)/4;
                stages[1].res = stoi(do_this[1].substr(1,do_this[1].length()-1));
                stages[1].operation_type = 32;
                //  r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = memory[(r[stoi(do_this[2].substr(2,do_this[2].length()-2))] -1000000)/4 ];
            }else if(do_this[2][0] =='[' && do_this[3][do_this[3].length()-1] ==']' && do_this[3][0] =='#'){
                stages[1].memory = (r[stoi(do_this[2].substr(2,do_this[2].length()-2))]-1000000)/4;
                stages[1].memory_offset = stoi(do_this[3].substr(1,do_this[2].length()-1))/4;
                stages[1].res = stoi(do_this[1].substr(1,do_this[1].length()-1));
                stages[1].operation_type = 33;
                // r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = memory[(r[stoi(do_this[2].substr(2,do_this[2].length()-1)) + stoi(do_this[3].substr(1,do_this[2].length()-1))] -1000000)/4 ];
            }else{
                error_show( (r[15]-1000)/4+1+empty , instii_complete.at((r[15]-1000)/4).length() );                                    
            }
        }
        pipeline_out += "\t\t ID: "+stages[1].instruction+"\n";        
    }else{
        pipeline_out += "\t\t ID: Blank\n";
        
    }

    arithmeticLogicUnit();
    stages[2] = stages[1];
}


void arithmeticLogicUnit(){

    if(!stages[2].instruction.empty() ){

        switch( stages[2].operation_type ){
            case 01:
                if(stages[2].r1 == stages[3].res){
                    stages[2].alu_result = addsubmulcmp(stages[2].operation,stages[3].alu_result,r[stages[2].r2]);
                }else if(stages[2].r2 == stages[3].res){
                    stages[2].alu_result = addsubmulcmp(stages[2].operation,r[stages[2].r1],stages[3].alu_result);
                }else if(stages[2].r1 == stages[4].res){
                    stages[2].alu_result = addsubmulcmp(stages[2].operation, stages[4].alu_result ,r[stages[2].r2]);
                }else if(stages[2].r2 == stages[4].res){
                    stages[2].alu_result = addsubmulcmp(stages[2].operation,r[stages[2].r1],stages[4].alu_result);
                }else{
                    stages[2].alu_result = addsubmulcmp(stages[2].operation,r[stages[2].r1],r[stages[2].r2]);
                }
                break;
            case 02:
                stages[2].alu_result = addsubmulcmp(stages[2].operation,stages[2].c1,stages[2].c2);
                break;
            case 03:
                if(stages[2].r2 == stages[3].res){
                    stages[2].alu_result = addsubmulcmp(stages[2].operation, stages[2].c1 ,stages[3].alu_result);
                }else{
                    stages[2].alu_result = addsubmulcmp(stages[2].operation,stages[2].c1,r[stages[2].r2]);
                }
                break;
            case 04:
                if(stages[2].r1 == stages[3].res){
                    stages[2].alu_result = addsubmulcmp(stages[2].operation, stages[3].alu_result ,stages[2].c2);
                }else{
                    stages[2].alu_result = addsubmulcmp(stages[2].operation,r[stages[2].r1], stages[2].c2);
                }
                break;
            case 11:
                stages[2].alu_result = addsubmulcmp("cmp",r[stages[2].res],r[stages[2].r1]);
                break;
            case 12:
                stages[2].alu_result = addsubmulcmp("cmp",r[stages[2].res],stages[2].c1);
                break;
            case 22:
            case 33:
                stages[2].memory = addsubmulcmp("add",stages[2].memory_offset,stages[2].memory);
                break;          
        }
    
        pipeline_out += "\t\t ALU: "+stages[2].instruction+"\n";
    }else{
        pipeline_out += "\t\t ALU: Blank\n";      
    }

    dataMemory();

    stages[3] = stages[2];
}


void dataMemory(){

    if(!stages[3].instruction.empty() ){
        switch(stages[3].operation_type){
            case 32:
            case 33:
                stages[3].alu_result = memory[stages[3].memory];
                break;
            case 31:
                stages[3].alu_result = stages[3].c1;
        }
        pipeline_out += "\t\t DM: "+stages[3].instruction+"\n";        
    }else{
        pipeline_out += "\t\t DM: Blank\n";
        
    }
    writeBack();
    // show_details();
    stages[4] = stages[3];
}

void writeBack(){
    
    if(!stages[4].instruction.empty() ){
        switch((stages[4].operation_type - (stages[4].operation_type)%10)/10){
            case 0:
            case 3:
                r[stages[4].res] = stages[4].alu_result;
                break;
        }
        pipeline_out += "\t\t WB: "+stages[4].instruction+"\n";        
    }else{
        pipeline_out += "\t\t WB: Blank\n";       
    }
    
}

int addsubmulcmp(string k ,int r1 ,int r2){
    if(str_is(k ,"add","ADD")){
        return r1 + r2;
    }else if(str_is(k ,"sub","SUB")){
        return r1 - r2;        
    }else if(str_is(k ,"mul","MUL")){
        return r1 - r2;        
    }else if(str_is(k ,"cmp","CMP")){
        return r1 - r2;        
    }

    return -1;
}


bool not_instructions(vector <int> s ,vector <int> e,const string instruc){
    
    string do_this[s.size()];
    
    for(int j=0;j<s.size();j++){
        do_this[j] = instruc.substr(s.at(j) ,e.at(j) - s.at(j)); // array of readed word from one instruction
    }

    if(box_type == ".text" && do_this[0] != ".data" && do_this[0] != ".end" ){
        total_instruction++;
        if(str_is(do_this[0] ,"swi","SWI")){                    // swi exit
            if(do_this[1] == "SWI_Exit" ){
                total_instruction--;
                // r[15] = r[15] - 4;                
                exitingProgram();
            }
            r[15] = r[15] + 4;
        }if( do_this[0][do_this[0].length()-1] ==':'){
            total_instruction--;
            if(s.size()==1){
                labels.push_back(do_this[0].substr(0,do_this[0].length()-1) );          //labels names saving 
                labelsid.push_back(r[15]);                                              // saving there id's
            }else {
                message_show("\tPlease write your space or word in next line!!!");
                exit(EXIT_FAILURE);
            }
            r[15] = r[15] +4;
        }else if(do_this[0][do_this[0].length()-1] !=':'){
            return false;
        }else{
            // error --------------------- show
            cout<<"Error at: "<<1 + (r[15]-1000)/4<<"\n In : "<<instruc<<endl<<"\t\t at "<<do_this[0]<<endl;
            exit(EXIT_FAILURE);    
        }    
    }else if(box_type == ".data" && do_this[0] != ".text" && do_this[0] != ".end" ){


        if( do_this[0][do_this[0].length()-1] ==':'){
            if(s.size()==1){
                labels.push_back(do_this[0].substr(0,do_this[0].length()-1) );          //labels names saving 
                labelsid.push_back(r[15]);                                              // saving there id's
            }
        }else if(do_this[0] == ".space"){
    
            if(arm_lab.size()!=0){       
                arm_array a(labels.at(labels.size()-1), arm_lab.at(arm_lab.size()-1).end ,arm_lab.at(arm_lab.size()-1).end+stoi(do_this[1]));    // making space for data
                arm_lab.push_back(a);                
            }
            else{          
                arm_array a(labels.at(labels.size()-1), 0 ,stoi(do_this[1]));                       // making space for data
                arm_lab.push_back(a);
            }
        }
            
        r[15] = r[15] + 4;         
        
    }else if(do_this[0][0] == '.' ){                                    // change box type 
        if(do_this[0] == ".equ" ){
            //    here equ like instruction
        }else if(do_this[0] == ".text" ){
            if(box_type!= ".data"){
                box_type = ".data";
                ptrdiff_t here = distance(instii_complete.begin() , find(instii_complete.begin(),instii_complete.end(),".data"));
                if(here != instii_complete.size() )
                    r[15] = 1000 + 4 *here;
                else
                    box_type = ".text";                    
            }else 
                box_type = ".text";

        }else if(do_this[0] == ".data" ){                       // always doing work first with .data
             if(box_type == ".text" ){
                exitingProgram();              
             }else{
                box_type = ".data";                 
             }
        }else if(do_this[0] == ".end" && box_type == ".data"){
            ptrdiff_t here = distance(instii_complete.begin() , find(instii_complete.begin(),instii_complete.end(),".text"));
            r[15] = 1000 + 4*here;
            box_type = ".text";
        }else if(do_this[0] == ".end" && box_type == ".text"){
            bye();
        }
        r[15] = r[15]+4; 
    }else{
        // error --------------------- show
        cout<<"Source of error can be '.text'"<<endl;
        cout<<"Error at: "<<1 + (r[15]-1000)/4<<"\n In : "<<instruc<<endl<<"\t\t at "<<do_this[0]<<endl;
        exit(EXIT_FAILURE); 
    }

    return true;
}

//  seprating words from each instruction line and checking for errors
bool instructions_checker(string checking){

    vector <int> s,e; // vector to store word starting point and end point

    for(int i=0;i<checking.length();i++){
        if( (!word_seprators(checking[i])) && s.size() == e.size()){
            s.push_back(i);
        }else if( word_seprators(checking[i]) && s.size() != e.size()){
            e.push_back(i);
            if(!word_check( checking.substr(s.at(s.size()-1) ,e.at(e.size()-1) - s.at(s.size()-1))) ){
                error_show(instii_complete.size()+1+ empty , s.at(s.size()-1));
                return false;
            }
        }else if(i == checking.length()-1 && s.size() != e.size() ){
            e.push_back(i+1);
            if(!word_check( checking.substr(s.at(s.size()-1) ,e.at(e.size()-1) - s.at(s.size()-1))) ){  // sending for checking error
                error_show(instii_complete.size()+1+empty , s.at(s.size()-1));   // showing where is with line and column
                return false;  // if error stop doing work
            }
        }
    }

    if(s.size() == e.size()){
        sp.push_back(s);
        ep.push_back(e);                                    // storing each work start and end point in line
        return true;
    }else {
        debug<<"Something wrong happen!"<<endl;
        return false;
    }
}

// removing unncesary extra space
string remove_space(string correct){
    if(correct.empty()){
        return correct;
    }
    if(correct[0] == 9){
        return remove_space(correct.substr(1,correct.size()-1));
    }else{
        return correct;
    }
}
//  start reading input file from here
bool fileread(){
    // int t = 0;
    while(!input.eof()){
        string temp;
        getline(input,temp);    // reading each line from input file one by one
        temp = remove_space(temp);                                          // removing extra space in starting 

        if(temp.empty()){                                   // not storing the blank lines
            empty++;
            continue;
        }
        if(!instructions_checker(temp)){                    // sending instruction for checkig error
            break;
        }
        instii_complete.push_back(temp);     // adding in instructions list
        
    }
    if(input.eof() != true ){
        return false;
    }
    return true;
}

bool latfile(){

    while(!latency.eof()){
        string temp;
        getline(latency,temp);    // reading each line from input file one by one
        temp = remove_space(temp);                                          // removing extra space in starting 

        if(temp.empty()){                                   // not storing the blank lines
            empty++;
            continue;
        }
        if(temp.find("=")){
            lat_values.insert(pair<string,int> (temp.substr( 0 ,temp.find("=")-1) ,stoi(temp.substr(temp.find("=")+1,temp.find(";")-1))));
            latency_total_instru.insert(pair<string,int> (temp.substr( 0 ,temp.find("=")-1) ,0));
        }
    }
    return true;
}

void proc_details(){
    while(!proc_info.eof()){
        string temp;
        getline(proc_info,temp);    // reading each line from input file one by one
        if(temp.find("cpu MHz") != -1 ){
            proc = temp.substr(temp.find(":")+1 , temp.size() - temp.find(":")); 
            break;
        }else if(temp.find("model name") != -1){
            model = temp;
        }
    }
}


// main function
int main(){
    tStart = clock();

    string inname = "input.txt";

    cout<<"Please Type Your Input file name:"<<endl;
    cin>>inname;

    if(inname.empty()){
        cout<<"Input file set to default : input.txt"<<endl;
        inname = "input.txt";
    }

    input.open("input/" + inname,ios::in);                // input file
    
    if(!input.is_open()){
        cout<<"Please try again with correct input file name.\nBYE!!!"<<endl;
        exit(EXIT_SUCCESS);
    }

    debug.open("output/debug.txt",ios::out);
    output.open("output/output.txt" , ios::out);
    latency.open("input/latency.txt",ios::in);
    proc_info.open("/proc/cpuinfo",ios::in);

    proc_details();

    debug<<"=======This is debug file for checking errors and how is code working and doing it's work.======="<<endl;

    if(!fileread()){                                // reading file 
        return -1;
    }

    if(!latfile()){
        lat = false;
    }

    r[15] = 1000;                                   // starting value of pc register

    cout<<endl<<endl;
    cout<<"Full screen your terminal for better experience :-)"<<endl<<endl;
    cout<<"========== Your processor details are given below & it will be used to calculate the arm code run time(total execution time of arm code)========= "<<endl<<"\t \t "<<model<<endl<<"\t \t CPU Frequency: "<<proc<<endl<<endl;
    cout<<"==========Run mode will run code all at once and u can check your code output in output.txt.=============\n ===============Debug mode will run your code instruction by instruction============================"<<endl;
    cout<<endl<<"run or debug??? : ";
    cin>>mode;
    cout<<endl<<endl<<endl;
    
    // start working
    while((r[15]-1000)/4 < instii_complete.size()){
        // cout<<(r[15]-1000)/4<<": "+instii_complete.at((r[15]-1000)/4)<<endl;
        // checking for not instructions and not sending them to pipeline
        if(not_instructions(sp.at((r[15]-1000)/4) , ep.at((r[15]-1000)/4) , instii_complete.at((r[15]-1000)/4))){
            continue;
        }else if(remove_space(mode) == "run" || box_type != ".text"){
            // sending to instruction fetch stage of pipeline
            instructionFetch(sp.at((r[15]-1000)/4) , ep.at((r[15]-1000)/4) , instii_complete.at((r[15]-1000)/4));
            r[15]+=4;    
        }else{
            cout<<"To exit from Debug mode type 'q'"<<endl; 
            if(getchar() != 'q'){
                instructionFetch(sp.at((r[15]-1000)/4) , ep.at((r[15]-1000)/4) , instii_complete.at((r[15]-1000)/4));
                r[15]+=4;    
            }
            else 
                mode = "run";
        }
        
    }


    return 0;
}


void exitingProgram(){
        int i=5;
    instii_complete.at((r[15] - 1000)/4) = "Exiting";
    // r[15] = 1000 + instii_complete.size()*4 - 4;
    while(i--){
        if(remove_space(mode) == "run" || box_type != ".text"){
            // sending to instruction fetch stage of pipeline
            instructionFetch(sp.at(0),ep.at(0),"");
        }else{
            cout<<"To exit from Debug mode type 'q'"<<endl; 
            if(getchar() != 'q'){
                instructionFetch(sp.at(0),ep.at(0),"");
            }
            else 
                mode = "run";
        }
    }
    getchar();

    instii_complete.pop_back();
    total_cycles--;
    bye();

    cout<<"Warning:--- There was no swi_exit also not .end \n It might create an error in future."<<endl;

}
