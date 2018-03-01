#include<iostream>
#include<fstream>
#include<vector>

using namespace std;

//  declaring some global varibles
vector <string> instii;
fstream input,output;
int r[16];

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

void error_show(){

}

bool word_checker(string check_word){

    return true;
}

bool instructions_checker(string checking){
    vector <int> s,e;
    for(int i=0;i<checking.size();i++){
        if(checking[i] != ' ' && s.size() == e.size()){
            s.push_back(i);
        }else if(checking[i] == ' ' && s.size() != e.size()){
            e.push_back(i);
            if(!word_checker( checking.substr(s.at(0),e.at(0)-s.at(0)))){
                break;
            }
        }
    }

    return true;
}

void fileread(){
    instii;
    while(!input.eof()){
        string temp;
        getline(input,temp);
        if(!instructions_checker(temp)){
            break;
        }
        instii.push_back(temp);
    }
}


int main(){
    input.open("input.txt",ios::in);
    output.open("output.txt",ios::out);

    fileread();

    for(int i=0;i<instii.size();i++){
        cout<<i+1<<": "<<instii.at(i)<<endl;
        show_register();
    }
    return 0;
}