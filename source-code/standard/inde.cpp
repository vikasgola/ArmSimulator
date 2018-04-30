
bool not_instructions(vector <int> s ,vector <int> e,const string instruc){
    string do_this[s.size()];
    
    for(int j=0;j<s.size();j++){
        do_this[j] = instruc.substr(s.at(j) ,e.at(j) - s.at(j)); // array of readed word from one instruction
    }

    if(box_type == ".text" && do_this[0] != ".data" && do_this[0] != ".end" ){
        total_instruction++;
        if( do_this[0][do_this[0].length()-1] ==':'){
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
            r[15] = r[15] +4;
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
            box_type = ".data";
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


// pipeline=============================================

void instructionFetch(vector <int> s ,vector <int> e,const string instruc){
    stages[0].instruction = instruc;
    stages[0].s = s;
    stages[0].e = e;
    instructionDecode();
    stages[1] = stages[0];
}

void instructionFetch(){
    
}

void instructionDecode(){
    string do_this[stages[1].s.size()];
    for(int j=0;j<stages[1].s.size();j++){
        do_this[j] = stages[1].instruction.substr(stages[1].s.at(j) ,stages[1].e.at(j) - stages[1].s.at(j)); // array of readed word from one instruction
    }

    stages[1].operation = do_this[0];
    // if(str_is(do_this[0] ,"ADD","add") || str_is(do_this[0] ,"sub","SUB") || str_is(do_this[0] ,"mul","MUL")){
    //     if(do_this[2][0] =='r' && do_this[3][0] =='r' ){
    //         stages[1].res =  stoi(do_this[1].substr(1 , do_this[1].length()-1));
    //         stages[1].r1 = stoi(do_this[2].substr(1,do_this[2].length()-1));
    //         stages[1].r2 =  stoi(do_this[3].substr(1,do_this[3].length()-1));    
    //     }else if(do_this[2][0] =='#' && do_this[3][0] =='#'){

    //         stages[1].res = stoi(do_this[1].substr(1,do_this[1].length()-1));
    //         stages[1].c1 = stoi(do_this[2].substr(1,do_this[2].length()-1)); 
    //         stages[1].c2 = stoi(do_this[3].substr(1,do_this[3].length()-1));    
        
    //     }else if(do_this[2][0] =='#' && do_this[3][0] =='r'){
    //         stages[1].res = stoi(do_this[1].substr(1,do_this[1].length()-1));
    //         stages[1].c1 = stoi(do_this[2].substr(1,do_this[2].length()-1)); 
    //         stages[1].r2 =  stoi(do_this[3].substr(1,do_this[3].length()-1));    
    //     }else if(do_this[3][0] =='#' && do_this[2][0] =='r'){
    //         stages[1].res = stoi(do_this[1].substr(1,do_this[1].length()-1));
    //         stages[1].r1 = stoi(do_this[2].substr(1,do_this[2].length()-1)); 
    //         stages[1].c2 = stoi(do_this[3].substr(1,do_this[3].length()-1));    
    //     }else {
    //         error_show(instii_complete.size()+1+ empty,0);
    //     }
    // }
    r[15]+=4;
}


struct pipeline {
    string instruction;
    string operation;
    int operation_type;
    vector<int> s;
    vector<int> e;
    int r1 = -1,r2 = -1,res,c1 = -1,c2 = -1;
} stages[5];
