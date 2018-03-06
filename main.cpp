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
// B                        
// BL                       done
// MOV                      done
// pc                       done
// lr                       

//  declaring some global varibles
vector <string> instii;
fstream input,debug;
int r[16];               // assuming intial value of all registers to 0.

int memory[100];         // memory of 100 words
bool cmp_result;        // storing result of cmp instruction for next instruction

short int cmp;          // compareble values storage
vector <string> labels;
vector <int> labelsid;          // labels name storage
int empty =0;                   // counter  for empty lines
int total_instruction = 0;

vector<vector <int>> sp,ep;

void pushwordinfo(vector<int> l,vector<int> m){
    sp.push_back(l);
    ep.push_back(m);
}

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
vector <arm_array> arm_lab;

// functions list
bool instructions_checker(string checking,int i);


// showing all registers registers 
void show_register(){
    cout<<"\t";
    for(int i=0;i<16;i++){
        cout<<"r"<<i<<" = "<<r[i]<<" | ";
        if(i==7) {
            cout<<endl;
            cout<<"\t";
        }        
    }
    cout<<"r14(lr) = "<<r[14]<<" | ";
    cout<<"r15(pc) = "<<r[15]<<" ";
    cout<<endl<<endl;
}

// showing memory for memory related istructions
void show_memory(){
    cout<<"\t Memory Addresses: stored Value"<<endl;
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

    debug<<check_word<<" ";     // words find in instruction printing in debug file

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

//  addition, substraction, mov and multiplication instruction handling
void addsubmul(char k,string do_this[]){
    debug<<"\t\t\t Do: "<<k;
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
            error_show(instii.size()+1+ empty,0);
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
            error_show(instii.size()+1 + empty,0);            
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
            error_show(instii.size()+1+empty,0);            
        }
    }else if(k == '='){
        if(do_this[2][0] =='r'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = r[stoi(do_this[2].substr(1,do_this[2].length()-1))];
        }else if(do_this[2][0] =='#'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = stoi(do_this[2].substr(1,do_this[2].length()-1));
        }else {
            error_show(instii.size()+1+empty,0);                        
        }
    }else if(k == 'c'){
        if(do_this[2][0] =='r'){
            cmp = r[stoi(do_this[1].substr(1,do_this[1].length()-1))] - r[stoi(do_this[2].substr(1,do_this[2].length()-1))];
        }else if(do_this[2][0] =='#'){
            cmp = r[stoi(do_this[1].substr(1,do_this[1].length()-1))] - stoi(do_this[2].substr(1,do_this[2].length()-1));
        }else {
            error_show(instii.size()+1+empty,0);                        
        }
    }
}

// bne and bge instructions handling
void bnebge(char k , string do_this[]){
    if(k == 'n'){
        if(cmp != 0){
            cmp_result = true;            
            debug<<endl<<"\t\t\t Going back to top\t\t\t"<<endl;
            ptrdiff_t here = distance(labels.begin() , find(labels.begin(),labels.end(),do_this[1]));
            if(!(here < labels.size() )){
                message_show("label not found");
                exit(EXIT_FAILURE);
            }
            for(int i = labelsid.at(here) + 1; i<instii.size();i++){
                instructions_checker(instii.at(i),i);
            }
            bnebge(k,do_this);            
        }else{
            cmp_result = false;            
            debug<<"=====loop finished====="<<endl;
        }
    }else if(k == 'g'){
        if(cmp >= 0){
            cmp_result = true;
            debug<<endl<<"\t\t\t Going back to top\t\t\t"<<endl;
            ptrdiff_t here = distance(labels.begin() , find(labels.begin(),labels.end(),do_this[1]));
            if(!(here < labels.size() )){
                message_show("label not found");
                exit(EXIT_FAILURE);
            }

            for(int i = labelsid.at(here) + 1; i<instii.size();i++){
                instructions_checker(instii.at(i),i);
            }
            bnebge(k,do_this);            
        }else{
            cmp_result = false;
            debug<<"=====loop finished====="<<endl;
        }
    }else if(k == 'b'){
            ptrdiff_t here = distance(labels.begin() , find(labels.begin(),labels.end(),do_this[1]));
            if(!(here < labels.size() )){
                message_show("label not found");
                exit(EXIT_FAILURE);
            }

            message_show("currently not working. back soon");
            exit(EXIT_SUCCESS);
    }else if(k== 'l'){

    }
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
                message_show("Please define your space or words at the begginnig of code!!!!!!!!!");
                exit(EXIT_FAILURE);
            }else {
                r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = 1000000 + arm_lab.at(i).start;        // starting address is 1000000
            }
        }else if(do_this[2][0] =='[' && do_this[2][do_this[2].length()-1] ==']'){
            r[stoi(do_this[1].substr(1,do_this[1].length()-1))] = memory[(r[stoi(do_this[2].substr(2,do_this[2].length()-2))] -1000000) ];
        }else{
            error_show(instii.size()+1+empty,instii.at(instii.size()-1).length() );                                    
        }
    }else if(k == 's'){
        if(do_this[2][0] =='[' && do_this[2][do_this[2].length()-1] ==']'){
            memory[(r[stoi(do_this[2].substr(2,do_this[2].length()-2))]-1000000)/4] = r[stoi(do_this[1].substr(1,do_this[1].length()-1))];
        }else{
            error_show(instii.size()+1+empty,instii.at(instii.size()-1).length() );                                                
        }
    }

}

//  start doing work of each instructions by detacting what instruction is that
void start_instruction(vector <int> s ,vector <int> e,const string instruc){
    string do_this[s.size()];
    for(int j=0;j<s.size();j++){
        do_this[j] = instruc.substr(s.at(j) ,e.at(j) - s.at(j)); // array of readed word from one instruction
    }

    if(str_is(do_this[0] ,"ADD","add")){
        addsubmul('+',do_this);                                 // add instruction work
    }else if(str_is(do_this[0] ,"SUB","sub")){
        addsubmul('-',do_this);                                 // sub instruction work
    }else if(str_is(do_this[0] ,"MUL","mul")){
        addsubmul('*',do_this);                                 // mul instruction work
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
    }
    else if( do_this[0][do_this[0].length()-1] ==':'){
         if(s.size()==1){
            labels.push_back(do_this[0].substr(0,do_this[0].length()-1) );
            labelsid.push_back( instii.size() );
        }else {
            message_show("\tPlease write your space or word in next line!!!");
            exit(-1);
        }
    }else if(do_this[0][0] == '.' ){
        if(do_this[0] == ".space"){
            if(arm_lab.size()!=0){
                arm_array a(labels.at(labels.size()-1), arm_lab.at(arm_lab.size()-1).end ,stoi(do_this[1]));
                arm_lab.push_back(a);                
            }
            else{
                arm_array a(labels.at(labels.size()-1), 0 ,stoi(do_this[1]));
                arm_lab.push_back(a);
            }
        }
    }
}


//  charcters which can seprate the word in a instructions are "space", "," and "horizontal tab" 
bool word_seprators(char t){
    if( t== ' ' || t==',' || t==9 ){
        return true;
    }
    return false;
}

// instruction checker frunction for calling function
bool instructions_checker(string checking,int i){

    debug<<"Line "<<i+1<<": ";      // printing each detected word in debug file.

    vector <int> s,e; // vector to store word starting point and end point

    for(int i=0;i<checking.length();i++){
        if( (!word_seprators(checking[i])) && s.size() == e.size()){
            s.push_back(i);
        }else if( word_seprators(checking[i]) && s.size() != e.size()){
            e.push_back(i);
            debug<<checking.substr(s.at(s.size()-1) ,e.at(e.size()-1) - s.at(s.size()-1))<<" ";            
        }else if(i == checking.length()-1 && s.size() != e.size() ){
            e.push_back(i+1);
            debug<<checking.substr(s.at(s.size()-1) ,e.at(e.size()-1) - s.at(s.size()-1))<<" ";                        
        }
    }
    total_instruction++;
    start_instruction(s,e,checking);    // sending to do work for instruction    
    
    cout<<i+1<<": "<<instii.at(i)<<endl;
    if(str_is(checking.substr(0,3),"bne","BNE") || str_is(checking.substr(0,3),"bge","BGE")){
        show_cmp_result();
    }else if(str_is(checking.substr(0,3),"ldr","LDR") || str_is(checking.substr(0,3),"str","STR")){
        show_memory();
    }
    
    r[15]=1000+4*i+4;
    
    show_register();
    debug<<endl;
}


//  seprating words from each instruction line and checking for errors
bool instructions_checker(string checking){

    debug<<"Line "<<instii.size()+1<<": ";      // printing each detected word in debug file.

    vector <int> s,e; // vector to store word starting point and end point

    for(int i=0;i<checking.length();i++){
        if( (!word_seprators(checking[i])) && s.size() == e.size()){
            s.push_back(i);
        }else if( word_seprators(checking[i]) && s.size() != e.size()){
            e.push_back(i);
            if(!word_check( checking.substr(s.at(s.size()-1) ,e.at(e.size()-1) - s.at(s.size()-1))) ){
                error_show(instii.size()+1+ empty,s.at(s.size()-1));
                return false;
            }
        }else if(i == checking.length()-1 && s.size() != e.size() ){
            e.push_back(i+1);
            if(!word_check( checking.substr(s.at(s.size()-1) ,e.at(e.size()-1) - s.at(s.size()-1))) ){  // sending for checking error
                error_show(instii.size()+1+empty,s.at(s.size()-1));   // showing where is with line and column
                return false;  // if error stop doing work
            }
        }
    }

    if(s.size() == e.size()){
        start_instruction(s,e,checking);    // sending to do work for instruction    
        
        // pushwordinfo(s,e);
        debug<<endl;
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
    int t = 0;
    while(!input.eof()){
        string temp;
        getline(input,temp);    // reading each line from input file one by one

        temp = remove_space(temp);

        if(temp.empty()){
            empty++;
            continue;
        }
        
        if(!instructions_checker(temp)){
            return false;
        }
        instii.push_back(temp);     // adding in instructions list
        
        cout<<t+1<<": "<<instii.at(instii.size()-1)<<endl;
        if(str_is(temp.substr(0,3),"bne","BNE") || str_is(temp.substr(0,3),"bge","BGE")){
            show_cmp_result();
        }else if(str_is(temp.substr(0,3),"ldr","LDR") || str_is(temp.substr(0,3),"str","STR")){
            show_memory();
        }
        t++;
        r[15]=1000+4*t;
        total_instruction++;

        show_register();
    }
    return true;
}


int main(){
    input.open("input.txt",ios::in);        
    debug.open("debug.txt",ios::out);
    
    debug<<"=======This is debug file for checking errors and how is code working and doing it's work.======="<<endl;

    if(!fileread()){
        return -1;
    }

    // start working
    // for(int i=0;i<instii.size();i++){
    //     start_instruction(sp.at(i) , ep.at(i) , instii.at(i));    // sending to do work for instruction    
    // }

    cout<<"\t Total Instructions executed: "<<total_instruction<<"\t Empty Lines: "<<empty<<" \t Total Lines: "<<instii.size()<<endl;

    return 0;
}