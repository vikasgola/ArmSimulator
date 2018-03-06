#include<iostream>
#include<fstream>
#include<algorithm>
#include<vector>

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
fstream input,debug;                // input file is code and debug file is for debugging
int r[16];               // assuming intial value of all registers to 0.
    
int memory[100];         // memory of 100 words
bool cmp_result;        // storing result of cmp instruction for next instruction

short int cmp;          // compareble values storage
vector <string> labels;         // stores labels name of functions
vector <int> labelsid;          // labels name storage
int empty =0;                   // counter  for empty lines
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
string box_type;                // store what type of box it is text or data

// ================================ functions list============================================

void pushwordinfo(vector<int> l,vector<int> m);
void show_register();
void show_memory();
void message_show(string s);
void show_cmp_result();
void error_show(int row,int col);
void show_details();
bool not_possible_chars(char c);
bool word_check(string check_word);
bool str_is(string st,string li,string li2);
bool word_seprators(char t);
void start_instruction(vector <int> s ,vector <int> e,const string instruc);
bool instructions_checker(string checking);
string remove_space(string correct);
bool fileread();
void addsubmul(char k,string do_this[]);
void ldrstr(char k , string do_this[]);
void bnebge(char k , string do_this[]);


//  ========================= functions declartions start=======================================

//  word position storage
void pushwordinfo(vector<int> l,vector<int> m){
    sp.push_back(l);
    ep.push_back(m);
}


// showing all registers registers 
void show_register(){
    cout<<"\t";
    for(int i=0;i<14;i++){
        cout<<"r"<<i<<" = "<<r[i]<<" | ";
        if(i==7) {
            cout<<endl;
            cout<<"\t";
        }        
    }
    cout<<"r14(lr) = "<<r[14]<<" | ";
    cout<<"r15(pc) = "<<r[15]<<" ";
    cout<<endl<<endl<<endl;
}

// showing memory for memory related istructions                        assuming memory starts from 1000000
void show_memory(){
    cout<<"\t\t\t Memory Addresses: stored Value"<<endl;
    for(int i =0;i<arm_lab.at(arm_lab.size()-1).end;i++){
        cout<<1000000+i*4<<": "<<memory[i]<<"\t\t  ";
    }
    cout<<endl<<endl;
}

// showing important message related to instructions
void message_show(string s){
    cout<<s<<endl;
}

// showing cmp_result
void show_cmp_result(){
    if(cmp_result == false)
        cout<<"\tcmp result = "<<"false"<<endl;
    else 
        cout<<"\tcmp result = "<<"true"<<endl;
}


//  syntax error handling
void error_show(int row,int col){
    cout<<"Syntax error at line: "<<row<<" and column: "<<col<<endl;
    exit(EXIT_FAILURE);
}


// shows the registers and memories after each instruction
void show_details(){
        cout<<r[15]<<": "<<instii_complete.at((r[15]-1000)/4)<<endl;
        
        if(str_is(instii_complete.at((r[15]-1000)/4 ).substr(0,3),"bne","BNE") || str_is(instii_complete.at((r[15]-1000)/4).substr(0,3),"bge","BGE")){
            show_cmp_result();                                                                      // showing compare result only if there is any cmp
        }else if(str_is(instii_complete.at((r[15]-1000)/4 ).substr(0,3),"ldr","LDR") || str_is(instii_complete.at((r[15]-1000)/4 ).substr(0,3),"str","STR")){
            show_memory();                                                                          // showing memory only in case of load and store
        }

        show_register();
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


//  addition, substraction, compare , mov and multiplication instruction handling
void addsubmul(char k,string do_this[]){
    if(k == '+'){
        if(do_this[2][0] =='r' && do_this[3][0] =='r' ){
            r[stoi(do_this[1].substr(1 , do_this[1].length()-1))] = r[stoi(do_this[2].substr(1,do_this[2].length()-1))] + r[stoi(do_this[3].substr(1,do_this[3].length()-1))];    
        }else if(do_this[2][0] =='#' && do_this[3][0] =='#'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = stoi(do_this[2].substr(1,do_this[2].length()-1)) + stoi(do_this[3].substr(1,do_this[3].length()-1));    
        }else if(do_this[2][0] =='#' && do_this[3][0] =='r'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = stoi(do_this[2].substr(1,do_this[2].length()-1)) + r[stoi(do_this[3].substr(1,do_this[3].length()-1))];    
        }else if(do_this[3][0] =='#' && do_this[2][0] =='r'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = r[stoi(do_this[2].substr(1,do_this[2].length()-1))] + stoi(do_this[3].substr(1,do_this[3].length()-1));    
        }else {
            error_show(instii_complete.size()+1+ empty,0);
        }
    }else if(k == '-'){
        if(do_this[2][0] =='r' && do_this[3][0] =='r' ){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = r[stoi(do_this[2].substr(1,do_this[2].length()-1))] - r[stoi(do_this[3].substr(1,do_this[3].length()-1))];    
        }else if(do_this[2][0] =='#' && do_this[3][0] =='#'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = stoi(do_this[2].substr(1,do_this[2].length()-1)) - stoi(do_this[3].substr(1,do_this[3].length()-1));    
        }else if(do_this[2][0] =='#' && do_this[3][0] =='r'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = stoi(do_this[2].substr(1,do_this[2].length()-1)) - r[stoi(do_this[3].substr(1,do_this[3].length()-1))];    
        }else if(do_this[3][0] =='#' && do_this[2][0] =='r'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = r[stoi(do_this[2].substr(1,do_this[2].length()-1))] - stoi(do_this[3].substr(1,do_this[3].length()-1));    
        }else {
            error_show(instii_complete.size()+1 + empty,0);            
        }
    }else if(k == '*'){
        if(do_this[2][0] =='r' && do_this[3][0] =='r' ){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = r[stoi(do_this[2].substr(1,do_this[2].length()-1))] * r[stoi(do_this[3].substr(1,do_this[3].length()-1))];    
        }else if(do_this[2][0] =='#' && do_this[3][0] =='#'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = stoi(do_this[2].substr(1,do_this[2].length()-1)) * stoi(do_this[3].substr(1,do_this[3].length()-1));    
        }else if(do_this[2][0] =='#' && do_this[3][0] =='r'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = stoi(do_this[2].substr(1,do_this[2].length()-1)) * r[stoi(do_this[3].substr(1,do_this[3].length()-1))];    
        }else if(do_this[3][0] =='#' && do_this[2][0] =='r'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = r[stoi(do_this[2].substr(1,do_this[2].length()-1))] * stoi(do_this[3].substr(1,do_this[3].length()-1));    
        }else {
            error_show(instii_complete.size()+1+empty,0);            
        }
    }else if(k == '='){
        if(do_this[2][0] =='r'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = r[stoi(do_this[2].substr(1,do_this[2].length()-1))];
        }else if(do_this[2][0] =='#'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = stoi(do_this[2].substr(1,do_this[2].length()-1));
        }else {
            error_show(instii_complete.size()+1+empty,0);                        
        }
    }else if(k == 'c'){
        if(do_this[2][0] =='r'){
            cmp = r[stoi(do_this[1].substr(1,do_this[1].length()-1))] - r[stoi(do_this[2].substr(1,do_this[2].length()-1))];
        }else if(do_this[2][0] =='#'){
            cmp = r[stoi(do_this[1].substr(1,do_this[1].length()-1))] - stoi(do_this[2].substr(1,do_this[2].length()-1));
        }else {
            error_show(instii_complete.size()+1+empty,0);                        
        }
    }
    show_details();
    debug<<"\t\t\t Do: "<<k<<endl;
    r[15] = r[15] + 4;
}

// ldr and str insruction working here
void ldrstr(char k , string do_this[]){
    if(k=='l'){
        if(do_this[2][0] =='=' ){
            int i;
            for(i=0;i<arm_lab.size();i++){
                if(arm_lab.at(i).name == do_this[2].substr(1 , do_this[2].length()-1) ){
                    break;
                }
            }
            if(i >= arm_lab.size()){
                message_show("label Not found");
                exit(EXIT_FAILURE);
            }else {
                r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = 1000000 + arm_lab.at(i).start;        // starting address is 1000000
            }
        }else if(do_this[2][0] =='[' && do_this[2][do_this[2].length()-1] ==']'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = memory[(r[stoi(do_this[2].substr(2,do_this[2].length()-2))] -1000000)/4 ];
        }else{
            error_show( (r[15]-1000)/4+1+empty , instii_complete.at((r[15]-1000)/4).length() );                                    
        }
    }else if(k == 's'){
        if(do_this[2][0] =='[' && do_this[2][do_this[2].length()-1] ==']'){
            memory[(r[stoi(do_this[2].substr(2,do_this[2].length()-2))]-1000000)/4] = r[stoi(do_this[1].substr(1,do_this[1].length()-1))];
        }else{
            error_show( (r[15]-1000)/4+1+empty , instii_complete.at((r[15]-1000)/4).length() );                                    
        }
    }

    show_details();
    r[15] = r[15]+4;
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
        }else{
            cmp_result = false;            
            debug<<endl<<"============== loop finished========="<<endl;
            show_details();            
            r[15] = r[15] + 4;
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
        }else{
            cmp_result = false;            
            
            debug<<endl<<"============== loop finished========="<<endl;
            show_details();            
            r[15] = r[15] + 4;
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


//  start doing work of each instructions by detacting what instruction is that
void start_instruction(vector <int> s ,vector <int> e,const string instruc){
    string do_this[s.size()];
    for(int j=0;j<s.size();j++){
        do_this[j] = instruc.substr(s.at(j) ,e.at(j) - s.at(j)); // array of readed word from one instruction
    }

    debug<<"Line "<<r[15]<<": "<<instruc<<endl;      // printing each detected word in debug file.
    
    // box type is text
    if(box_type == ".text" && do_this[0] != ".data" && do_this[0] != ".end" ){
        if(str_is(do_this[0] ,"swi","SWI")){                    // swi exit
            if(do_this[1] == "SWI_Exit" ){
                show_details();
                cout<<"\t Total Instructions executed: "<<total_instruction<<"\t Empty Lines: "<<empty<<" \t Total Lines: "<<instii_complete.size()<<endl;

                exit(EXIT_SUCCESS);
            }
            r[15] = r[15] + 4;
        }else if(str_is(do_this[0] ,"ADD","add")){
            addsubmul('+',do_this);                                 // add instruction work
        }else if(str_is(do_this[0] ,"SUB","sub")){
            addsubmul('-',do_this);                                 // sub instruction work
        }else if(str_is(do_this[0] ,"MUL","mul")){
            addsubmul('*',do_this);                                  // mul instruction work
        }else if(str_is(do_this[0],"MOV","mov")){
            addsubmul('=',do_this);                                 // mov instruction work
        }else if(str_is(do_this[0],"CMP","cmp")){
            addsubmul('c',do_this);                                 // cmp instruction work
        }else if(str_is(do_this[0],"bne","BNE")){
            bnebge('n',do_this);
        }else if(str_is(do_this[0],"bge","BGE")){
            bnebge('g',do_this);
        }else if(str_is(do_this[0],"ldr","LDR")){
            ldrstr('l',do_this);
        }else if(str_is(do_this[0],"str","STR")){
            ldrstr('s',do_this);
        }else if(str_is(do_this[0],"b","B")){
            bnebge('b',do_this);
        }else if(str_is(do_this[0],"bl","BL")){
            bnebge('l',do_this);
        }else if( do_this[0][do_this[0].length()-1] ==':'){
         if(s.size()==1){
            labels.push_back(do_this[0].substr(0,do_this[0].length()-1) );          //labels names saving 
            labelsid.push_back(r[15]);                                              // saving there id's
        }else {
            message_show("\tPlease write your space or word in next line!!!");
            exit(EXIT_FAILURE);
        }

        r[15] = r[15] +4;
        
        }else{
            show_details();
            r[15] = r[15] + 4;            
        }
    }else if(box_type == ".data" && do_this[0] != ".text" && do_this[0] != ".end" ){
        
        if( do_this[0][do_this[0].length()-1] ==':'){
            if(s.size()==1){
                labels.push_back(do_this[0].substr(0,do_this[0].length()-1) );          //labels names saving 
                labelsid.push_back(r[15]);                                              // saving there id's
            }
        }else if(do_this[0] == ".space"){
            if(arm_lab.size()!=0){
                
                arm_array a(labels.at(labels.size()-1), arm_lab.at(arm_lab.size()-1).end ,stoi(do_this[1]));    // making space for data
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
                r[15] = 1000 + 4 *here;
            }else 
                box_type = ".text";

        }else if(do_this[0] == ".data" ){                       // always doing work first with .data
            box_type = ".data";
        }else if(do_this[0] == ".end" && box_type == ".data"){
                ptrdiff_t here = distance(instii_complete.begin() , find(instii_complete.begin(),instii_complete.end(),".text"));
                r[15] = 1000 + 4 *here;
                box_type = ".text";
        }else if(do_this[0] == ".end" && box_type == ".text"){
            cout<<"\t Total Instructions executed: "<<total_instruction<<"\t Empty Lines: "<<empty<<" \t Total Lines: "<<instii_complete.size()<<endl;
            exit(EXIT_SUCCESS);
        }
        r[15] = r[15]+4; 
    }else{
        show_details();
        r[15] = r[15]+4;
    }

    total_instruction++;

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
        pushwordinfo(s,e);                                      // storing each work start and end point in line
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
    return true;
}


// main function
int main(){
    input.open("input.txt",ios::in);                // input file
    debug.open("debug.txt",ios::out);
    
    debug<<"=======This is debug file for checking errors and how is code working and doing it's work.======="<<endl;

    if(!fileread()){                                // reading file 
        return -1;
    }

    r[15] = 1000;                                   // starting value of pc register

    // start working
    while((r[15]-1000)/4 < instii_complete.size()){
        start_instruction(sp.at((r[15]-1000)/4) , ep.at((r[15]-1000)/4) , instii_complete.at((r[15]-1000)/4));    // sending to do work for instruction          
    }

    return 0;
}