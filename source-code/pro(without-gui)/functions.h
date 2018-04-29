#ifndef FUNCTIONS
#define FUNCTIONS

#include<iostream>
#include<vector>
#include<string>
#include<fstream>

using namespace std;


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
void instructionFetch(vector <int> s ,vector <int> e,const string instruc);
void instructionDecode();
bool latfile();
void proc_details();


#endif