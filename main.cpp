#include<iostream>
#include<fstream>
#include<vector>
#include<regex>

using namespace std;

//  declaring some global varibles
vector <string> instii;
fstream input,debug;
int r[16]; // assuming intial value of all registers to 0.

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
    cout<<endl;
}

//  syntax error handling
void error_show(int row,int col){
    cout<<"Syntax error at line: "<<row<<" and column: "<<col<<endl;
}

// checking for characters which are not possible in instructions
bool not_possible_chars(char c){
    // 64-91 captial chars
    // 96-123 small chars
    // 0-9 numbers
    //  35 #
    // = equality sign
    // : coln

    if((c>64 && c<91) || (c>96 && c<123) || (c>47 && c<58) || c==35 || c==61 || c==58){
        return false;
    }
    return true;
}

bool word_check(string check_word){

    debug<<check_word<<" ";     // words find in instruction printing in debug file

    for(int i=0;i<check_word.size();i++){
        if(not_possible_chars(check_word[i])){
            return false;
        }
    }
    
    return true;
}

//  charcters which can seprate the word in a instructions are "space", "," and "horizontal tab" 
bool word_seprators(char t){
    if( t== ' ' || t==',' || t==9 ){
        return true;
    }
    return false;
}

bool instructions_checker(string checking){

    debug<<"Line "<<instii.size()+1<<": ";      // printing each detected word in debug file.

    vector <int> s,e;

    for(int i=0;i<checking.size();i++){

        if(!word_seprators(checking[i]) && s.size() == e.size()){
            s.push_back(i);
        }else if(word_seprators(checking[i]) && s.size() != e.size() || checking[i]==12){
            e.push_back(i);
            if(!word_check( checking.substr(s.at(s.size()-1) , e.at(s.size()-1) - s.at(s.size()-1)))){
                error_show(instii.size()+1,s.at(s.size()-1));
                return false;
            }
        }
    }

    debug<<endl;

    return true;
}


//  start reading input file from here
bool fileread(){
    
    while(!input.eof()){
        string temp;
        getline(input,temp);    // reading each line from input file one by one

        if(temp.empty()){       // leaving empty lines as they do nothing
            continue;
        }
        if(!instructions_checker(temp)){
            return false;
        }
        instii.push_back(temp);     // adding in instructions list

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

    for(int i=0;i<instii.size();i++){
        cout<<i+1<<": "<<instii.at(i)<<endl;
        show_register();
    }
    return 0;
}