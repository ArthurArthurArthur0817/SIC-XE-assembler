#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <sstream>
#include <string>
#include <iomanip>



using namespace std;

//十進位轉十六進位
string hexadecimal(int number){
    stringstream stream;
    stream << hex << uppercase<< number;
    return stream.str();
    }

string reverseString(const string& str){
    return string(str.rbegin(), str.rend());
}


// 將整數轉換為二進制表示法的字串
string binaryString(int number){
    string result;
    int width = 0;
    while (number > 0) {
        result.insert(result.begin(), '0' + (number % 2));
        number /= 2;
    }

    if (result.empty()){
        result = "0";
    }

    return result;
}
//十六進位轉十進位
int Decimalahex(string hexStr){
    stringstream ss;
    ss << hex << hexStr;
    int decimalValue;
    ss >> decimalValue;
    return decimalValue;
}

//補0
string sixletter(string num){
    for(int i=num.length();i<6;i++){
        num="0"+num;
    }
    return num;
}

class Pair{
public:
    string symbol, loc;

    Pair(string symbol, string loc){
        this->symbol = symbol;
        this->loc = loc;
    }
    
};

class Data{
public:
    string str_first, str_second, str_third;
    string str, opcode, format;
    Data(string str_first, string str_second, string str_third, string str, string opcode, string format) {
        this->str_first = str_first;
        this->str_second = str_second;
        this->str_third = str_third;
        this->opcode = opcode;
        this->format = format;
        this->str = str;
    }
};

int main() {
    string Symbol_Table[][3] = {{"STL", "3", "14"},{"LDB", "3", "68"},{"JSUB", "3", "48"},
        {"LDA", "3", "00"},{"COMP", "3", "28"},{"JEQ", "3", "30"},{"J", "3", "3C"}, 
        {"STA", "3", "0C"},{"CLEAR", "2", "B4"},{"LDT", "3", "74"},{"TD", "3", "E0"},
        {"RD", "3", "D8"}, {"COMPR", "2", "A0"}, {"STCH", "3", "54"},{"TIXR", "2", "B8"},
        {"JLT", "3", "38"}, {"STX", "3", "10"},{"RSUB", "3", "4C"},{"LDCH", "3", "50"},
        {"WD", "3", "DC"}   
    };

    vector<Data> List;
    vector<string> Length;
    vector<string> Loc;
    vector<Pair> SYM_TAB;
    vector<string> Object_code;
    vector<int>M_record;
    
    //讀入/輸入檔案
    string file;
    cout<<"Enter the File: ";
    cin>>file;
    ifstream inputFile(file);
    ofstream outFile("TargetCode.txt");
    
    if (!outFile.is_open()){
        cerr << "Error opening target code file." << endl;
        return 1;
    }
    if (!inputFile.is_open()){
        cerr << "Error opening input file." << endl;
        return 1;
    }

    string line;
    int n = 0, DecLoc = 0, j = 0, i = 0, k = 0, len = 0;
    string HexLoc = "0", str1 = " ", str2 = " ", str3 = " ", op = " ", format = " ", base = " ";
    bool isOpCode = false, isLine = false;

    while (getline(inputFile, line)){
        stringstream ss(line);
        string temp;

        while (ss >> temp) {
            if (temp == "START" || temp == "END" || temp == "WORD" || temp == "BYTE" ||
                temp == "RESB" || temp == "RESW" || temp == "BASE") {
                str2 = temp;
                isOpCode = true;
            }
            else{
                string s = temp;
                if(temp.find('+') != string::npos){
                    s = temp.substr(1);
                    len = 1;
                }
                //去Symbol_Table找有沒有對應的Symbol，若有則將opcode,Format,str2賦值
                for(i = 0; i < sizeof(Symbol_Table) / sizeof(Symbol_Table[0]); i++){
                    if (s == Symbol_Table[i][0]){
                        str2 = temp;
                        op = Symbol_Table[i][2];
                        len = len + stoi(Symbol_Table[i][1]);
                        format = to_string(len);
                        isOpCode = true;
                        break;
                    }
                }
            }
            if(temp == "."){
                str1 = ".";
                isLine = true;
            }
            if(str1=="*" && temp.find('=') != string::npos &&
             (temp.find('C') != string::npos ||temp.find('X') != string::npos )){
                str2 = temp;
                isOpCode = true;
            }


            if(str2 == "RSUB"){
                isLine = true;
            }
                
            //若Opcode為false，則表示此為str1，否則就是str3
            if (!isOpCode){
                str1 = temp;
            }
            else if(str2 != "RSUB"){
                ss >> str3;
                isLine = true;
            }

           if (isLine){
                List.push_back(Data(str1, str2, str3, str1 + str2 + str3, op, format));
                Length.push_back(to_string(len));
                str1 = " "; str2 = " "; str3 = " "; len = 0; op = " "; format = " ";
                isLine = false;
                isOpCode = false;
            }
        }
    }

    inputFile.close();
    
    for (i = 0; i < List.size(); i++){
        
        // Calculate loc
        if (List[i].str.find(".") != string::npos || List[i].str.find("BASE") != string::npos ||List[i].str_second.find("END") != string::npos) {
            if (List[i].str.find("BASE") != string::npos)
                base = List[i].str_third;
                Loc.push_back("");
                HexLoc = hexadecimal(DecLoc += stoi(Length[i - 1], nullptr, 16));
        }
        else{
            if(i > 1){
                    HexLoc = hexadecimal(DecLoc += stoi(Length[i - 1], nullptr, 16));               
            }
            
            else{
                HexLoc = "0";
            }
            //長度補齊
            for(int j=HexLoc.length();j<4;j++){
                HexLoc="0"+HexLoc;
            }
            Loc.push_back(HexLoc);
        }

        // Calculate length
        if (List[i].str_second == "BYTE"){
            if (List[i].str_third.find("C") != string::npos){
                string temp = List[i].str_third.substr(List[i].str_third.find('\'') + 1, List[i].str_third.length() - 3);
                Length[i] = to_string(temp.length());
            }
            else if(List[i].str_third.find("X") != string::npos){
                string temp = List[i].str_third.substr(List[i].str_third.find('\'') + 1, List[i].str_third.length() - 3);
                Length[i] = to_string(temp.length()/2);
            }
        }
        else if (List[i].str_second.find("RESW") != string::npos){
            Length[i] = hexadecimal(stoi(List[i].str_third) * 3);   
        }
        else if (List[i].str_second.find("RESB") != string::npos){
            Length[i] = hexadecimal(stoi(List[i].str_third));
        }
        else if (List[i].str_second == "CLEAR"){
            Length[i] = "2";
        }
        else if(List[i].str_second.find("C\'") != string::npos){
                string temp = List[i].str_third.substr(List[i].str_third.find('\'') + 1, List[i].str_third.length() - 3);
                Length[i] = to_string(temp.length());
        }
        else if(List[i].str_second.find("X\'") != string::npos){
                string temp = List[i].str_third.substr(List[i].str_third.find('\'') + 1, List[i].str_third.length() - 3);
                Length[i] = to_string(temp.length()/2);
        }

        // Build SYM_TAB
        if (!List[i].str_first.empty() && List[i].str_first != " " && List[i].str_first.find(".") == string::npos) {
            if (List[i].str_first == base){
                 base = HexLoc;
            } 
                SYM_TAB.push_back(Pair(List[i].str_first, HexLoc));     
        } 
    }

    
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
    // pass2: build target code  
 
    for (i = 0; i < List.size(); i++){
        stringstream s;

        // Format 2 (opcode r1 r2)
        if (List[i].format == "2"){
            s << List[i].opcode;
            string arr[2];
            arr[0]=List[i].str_third[0];
            arr[1]=List[i].str_third[1];
            arr[2]=List[i].str_third[2];
            if(arr[1]==","){   //有兩個暫存器
                for(int  k = 0; k <= 2; k+=2){
                    if (arr[k] == "A"){
                        s << "0";
                    }         
                    else if (arr[k] == "X"){
                        s << "1";
                    } 
                    else if (arr[k] == "S"){
                        s << "4";
                    }    
                    else if (arr[k] == "T"){
                        s << "5";
                    }     
                }
            }
            else{   //只有一個
                if (arr[0] == "A"){
                        s << "0";
                    }         
                else if (arr[0] == "X"){
                        s << "1";
                    } 
                else if (arr[0] == "S"){
                        s << "4";
                    }    
                else if (arr[0] == "T"){
                        s << "5";
                    } 
                s << "0";
            }
        }
        
        // Format 4 (opcode nixbpe address)
        if (List[i].format == "4"){
            string str, nixbpe;
            str=binaryString(stoi(List[i].opcode, nullptr, 16));
            ostringstream tmp;
            tmp <<uppercase<<str;
            str= tmp.str();
           
            if (str == "0"){
                str = "000000";  // LDA needs to append '0' since opcode is 0
                M_record.push_back(i);
            }
            if (List[i].str_third.find("#") != string::npos){
                nixbpe = "010001";
                 M_record.push_back(i);
            } 
            else if (List[i].str_third.find("@") != string::npos){
                nixbpe = "100001";
                 M_record.push_back(i);
            }    
            else if (List[i].str_third.find(",X") != string::npos){
                nixbpe = "111001";
                 M_record.push_back(i);
            }  
            else{
                nixbpe = "110001";
                 M_record.push_back(i);
            }
                

            str = hexadecimal(stoi(str.substr(0, str.length() - 2) + nixbpe, nullptr, 2));
            
            if (str.length() != 3){
                str = str + "0";
            }

            for (k = 0; k < SYM_TAB.size(); k++){
                if (List[i].str_third.find(SYM_TAB[k].symbol) != string::npos){
                    if (SYM_TAB[k].loc.length() != 5) {
                        for (int l = SYM_TAB[k].loc.length(); l < 5; l++){
                             str += "0";
                        }   
                    }
                    str += SYM_TAB[k].loc;
                    break;
                }
            }

            char c[List[i].str_third.size()];
            copy(List[i].str_third.begin(), List[i].str_third.end(), c);
            if (c[1] - '0' >= 0 && c[1] - '0' <= 9 ){
                string hex = hexadecimal(stoi(List[i].str_third.substr(1)));
                if (hex.length() != 5){
                    for (int l = hex.length(); l < 5; l++)
                        str += "0";
                }
                str += hex;
            }
            s << str;

        }
        

        // Format 3 (opcode nicbpe disp)
        if (List[i].format == "3"){
            string str, nixbpe;
            str=binaryString(stoi(List[i].opcode, nullptr, 16));
            for(int j=str.length();j<8;j++){
                str="0"+str;
            }
            

            if (str == "0"){
                str = "000000";
            }
            
            // RSUB
            if (List[i].str_third == " " || List[i].str == "RSUB"){
                nixbpe = "110000";
                str = hexadecimal(stoi(str.substr(0, str.length() - 2) + nixbpe, nullptr, 2));
                str += "000";
            }
            // ,X
            else if(List[i].str_third.find(",X") != string::npos){
                string ta, num2 = Loc[i + 1];
                int disp = 0;
                for (k = 0; k < SYM_TAB.size(); k++){
                    if (List[i].str_third.find(SYM_TAB[k].symbol) != string::npos){
                        ta = SYM_TAB[k].loc;
                        break;
                    }
                }
                disp = stoi(ta, nullptr, 16) - stoi(num2, nullptr, 16);
                if (disp < 2047 && disp > -2048){
                    nixbpe = "111010";         
                    str = hexadecimal(stoi(str.substr(0, str.length() - 2) + nixbpe, nullptr, 2));
                    if (str.length() != 3){
                         str = "0" + str;
                    }     
                    if (hexadecimal(disp).length() != 3){
                        for (int l = hexadecimal(disp).length(); l < 3; l++){
                            str += "0";
                        }
                    }
                   
                }
                else{
                    num2 = base;
                    disp = stoi(ta, nullptr, 16) - stoi(num2, nullptr, 16);
                    nixbpe = "111100";
                    if (str.length() != 3){
                         str = "0" + str;
                    }  
                    str = hexadecimal(stoi(str.substr(0, str.length() - 2) + nixbpe, nullptr, 2)); 
                    if (hexadecimal(disp).length() != 3){
                        for (int l = hexadecimal(disp).length(); l < 3; l++){
                            str += "0";
                        }
                    }
                    str += hexadecimal(disp);   
                }
            }

            // #
            else if(List[i].str_third.find("#") != string::npos){
                string ta, num2 ;
                for(int j=i+1;j < List.size();j++){
                    if(Loc[j][0]>='0' && Loc[j][0]<='9'){
                        num2=Loc[j];
                        break;
                    }
                }
                int disp = 0;
                for (k = 0; k < SYM_TAB.size(); k++){
                    if (List[i].str_third.find(SYM_TAB[k].symbol) != string::npos){
                        ta = SYM_TAB[k].loc;
                        break;
                    }
                }
                //若是 #1,#2形式，直接補在後面
                if(List[i].str_third[1]>='0' && List[i].str_third[1]<='9'){    
                   nixbpe = "010000";
                    str = hexadecimal(stoi(str.substr(0, str.length() - 2) + nixbpe, nullptr, 2));  
                    if(str.length() != 3){
                        str = "0" + str ;
                    }             
                    disp=stoi(List[i].str_third.substr(1,List[i].str_third.length()-1), nullptr, 16); //轉換成10進位
                    if (hexadecimal(disp).length() != 3){
                            for (int l = hexadecimal(disp).length(); l < 3; l++){
                                    str += "0";
                                }      
                    }
                    str += hexadecimal(disp);   
                }
                //若是 #Length形式，要計算disp
                else{
                   disp = stoi(ta, nullptr, 16) - stoi(num2, nullptr, 16);             
                   if (disp < 2047 && disp > -2048){      //使用PC暫存器   
                        nixbpe = "010010";   
                        str = hexadecimal(stoi(str.substr(0, str.length() - 2) + nixbpe, nullptr, 2));
                        if (str.length() != 3){
                            str = "0" + str ;
                        }

                        //若disp長度小於3，則在str後面補0
                        if (hexadecimal(disp).length() != 3){
                            for (int l = hexadecimal(disp).length(); l < 3; l++){
                                    str += "0";
                                }      
                        }

                        if(disp<0){    //若disp小於0，則負數轉換後取最後三個字母即可 (負數的十六進位必是一堆英文)
                            str += hexadecimal(disp).substr(hexadecimal(disp).length()-3,3) ;
                        }
                        else{
                            str += hexadecimal(disp);
                        }    
                    }
                    
                   else{
                    num2 = base;
                    disp = stoi(ta, nullptr, 16) - stoi(num2, nullptr, 16);
                    nixbpe = "110100";
                    if (str.length() != 3){
                         str = "0" + str;
                        }  
                    str = hexadecimal(stoi(str.substr(0, str.length() - 2) + nixbpe, nullptr, 2)); 
                    if (hexadecimal(disp).length() != 3){
                        for (int l = hexadecimal(disp).length(); l < 3; l++){
                            str += "0";
                            }
                        }
                    str += hexadecimal(disp); 
                    } 
                }                 
            }

            // @
            else if(List[i].str_third.find("@") != string::npos){
                string ta, num2 = Loc[i + 1];
                int disp = 0;
                for (k = 0; k < SYM_TAB.size(); k++){
                    if (List[i].str_third.find(SYM_TAB[k].symbol) != string::npos){
                        ta = SYM_TAB[k].loc;
                        break;
                    }
                }
                disp = stoi(ta, nullptr, 16) - stoi(num2, nullptr, 16);
                if (disp < 2047 && disp > -2048){
                    nixbpe = "100010";
                    str = hexadecimal(stoi(str.substr(0, str.length() - 2) + nixbpe, nullptr, 2));
                    if (str.length() != 3){
                        str =  "0"+ str;
                    }
                    if (hexadecimal(disp).length() != 3){
                        for (int l = hexadecimal(disp).length(); l < 3; l++)
                            str += "0";
                    }
                    str += hexadecimal(disp);
                }
                else{
                    num2 = base;
                    disp = stoi(ta, nullptr, 16) - stoi(num2, nullptr, 16);
                    nixbpe = "110100";
                    if (str.length() != 3){
                         str = "0" + str;
                    }  
                    str = hexadecimal(stoi(str.substr(0, str.length() - 2) + nixbpe, nullptr, 2)); 
                    if (hexadecimal(disp).length() != 3){
                        for (int l = hexadecimal(disp).length(); l < 3; l++){
                            str += "0";
                        }
                    }
                    str += hexadecimal(disp); 
                }
            }
            // simple address
            else{
                string ta, num2 = Loc[i + 1];
                int disp = 0;
                //若有Literal
                if(List[i].str_third.find('=')!= string::npos){
                        for(k=0;k<List.size();k++){
                            if(List[k].str_second==List[i].str_third){
                                ta = Loc[k];
                                break;
                            }
                        }
                }
                else{
                    for (k = 0; k < SYM_TAB.size(); k++){
                        if(List[i].str_third.find(SYM_TAB[k].symbol) != string::npos){
                            ta = SYM_TAB[k].loc;
                            break;
                        }
                    }
                }


                
               disp = stoi(ta, nullptr, 16) - stoi(num2, nullptr, 16);
                
                //使用PC暫存器
                if (disp < 2047 && disp > -2048){
                    
                    nixbpe = "110010";
                           
                    str = hexadecimal(stoi(str.substr(0, str.length() - 2) + nixbpe, nullptr, 2));
                   
                    if (str.length() != 3){
                        str = "0" + str ;
                    }    
                    //若disp長度小於3，則在str後面補0
                    if (hexadecimal(disp).length() != 3){
                        for (int l = hexadecimal(disp).length(); l < 3; l++){
                                 str += "0";
                            }      
                    }
                    if(disp<0){    //若disp小於0，則負數轉換後取最後三個字母即可 (負數的十六進位必是一堆英文)
                        str += hexadecimal(disp).substr(hexadecimal(disp).length()-3,3) ;
                    }
                    else{
                        str += hexadecimal(disp);
                    }
                    
                }
                //使用B暫存器
                else{
                    num2 = base;
                    disp = stoi(ta, nullptr, 16) - stoi(num2, nullptr, 16);
                    nixbpe = "110100";
                    if (str.length() != 3){
                         str = "0" + str;
                    }  
                    str = hexadecimal(stoi(str.substr(0, str.length() - 2) + nixbpe, nullptr, 2)); 
                    if (hexadecimal(disp).length() != 3){
                        for (int l = hexadecimal(disp).length(); l < 3; l++){
                            str += "0";
                        }
                    }
                    str += hexadecimal(disp); 
                }
            }
            s << str;
        }

        // 處理C'EOF'
        if(List[i].str_third[0]=='C' && List[i].str_third[1]=='\''){
            string str,t;
            for(int k=2;k<List[i].str_third.size()-1;k++){
                int tmp =(int)List[i].str_third[k];
                str+=hexadecimal(tmp);
            }
            s<<str;
        }
        else if(List[i].str_first[0]=='*'){
            string str,t;
            if(List[i].str_second[1]=='C'){
                for(int k=3;k<List[i].str_second.size()-1;k++){
                    int tmp =(int)List[i].str_second[k];
                    str+=hexadecimal(tmp);
                }  
            }
           if(List[i].str_second[1]=='X'){
                str+=List[i].str_second.substr(3,List[i].str_second.length()-4);
            }
             s<<str;
        }

       Object_code.push_back(s.str());  
    }

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------   

    cout << setw(6) << "位置" << setw(6) << "            " << setw(6) << "原始敘述" << setw(5) << "           " << "目的碼" << "   "<<"Format "<<endl;
    outFile << setw(6) << "位置" << setw(6) << " " << setw(6) << "原始敘述" << setw(5) << " " << "目的碼" << endl;
    cout << "------------------------------------------------" << endl;
//輸出
    for(i = 0; i < List.size(); i++){
             cout << setw(4) << Loc[i]<<" " << left<<setw(7) << List[i].str_first << setw(6) << List[i].str_second
             << setw(10) << List[i].str_third << setw(5) <<"   "<<Object_code[i] << " " <<List[i].format<<endl;
            outFile << setw(6) << Loc[i] << setw(7) << List[i].str_first << setw(6) << List[i].str_second
             << setw(10) << List[i].str_third << setw(5) <<"   "<<Object_code[i] << "   "<<endl;
    }
//Object Program
    //輸出H,T record
    for(i = 0; i < List.size(); i++){
        int flag=0;
        int length=0;
        if(i==0){
            //計算指令總長度
            //長度=尾-頭+1，尾的指令長度=(尾的Loc)+(尾的指令長度-1)       (會多1要扣掉)
            Loc[j]=sixletter(Loc[j]);
            int total_length=0;
            total_length= (Decimalahex(Loc[List.size()-2]) + stoi(Length[List.size()-2])-1) - Decimalahex(Loc[0]) +1;
            outFile<<"H"<<"^"<<List[i].str_first<<setw(6)<<"^"<<Loc[i]<<"^"<<sixletter(hexadecimal(total_length));
            cout<<"H"<<"^"<<List[i].str_first<<setw(6)<<"^"<<Loc[i]<<"^"<<sixletter(hexadecimal(total_length));
        }
        else if(i==List.size()-1){
            break;
        }
        else{
            //每次輸出7個指令
            if(i+7<List.size()-1){
                for(j=i;j<i+7;j++){
                    if(Object_code[j]!=""){  
                        length+=stoi(Length[j]);
                    }  
                }
                cout<<endl;
                outFile<<endl;
                
                for(j=i;j<i+7;j++){
                    if(Object_code[j]!=""){
                        if(flag==0){
                            Loc[j]=sixletter(Loc[j]);
                            if(length<15){
                            cout<<"T"<<"^"<<Loc[j]<<"^"<<"0"<<hexadecimal(length)<<"^"<<Object_code[j];
                            outFile<<"T"<<"^"<<Loc[j]<<"^"<<"0"<<hexadecimal(length)<<"^"<<Object_code[j];
                            }
                            else{
                            cout<<"T"<<"^"<<Loc[j]<<"^"<<hexadecimal(length)<<"^"<<Object_code[j];
                            outFile<<"T"<<"^"<<Loc[j]<<"^"<<hexadecimal(length)<<"^"<<Object_code[j];
                            }
                            flag=1;
                        }
                        else{
                            cout<<"^"<<Object_code[j];
                            outFile<<"^"<<Object_code[j];
                        }
                    }               
                }
                i=i+6;
            }
            //若剩餘指令不到7個
            else{
                cout<<endl;
                outFile<<endl;
                 for(j=i;j<List.size()-1;j++){
                    if(Object_code[j]!=""){  
                        length+=stoi(Length[j]);
                    }  
                }
                for(j=i;j<List.size()-1;j++){
                    if(flag==0){
                        Loc[j]=sixletter(Loc[j]);
                        if(length<15){
                            cout<<"T"<<"^"<<Loc[j]<<"^"<<"0"<<hexadecimal(length)<<"^"<<Object_code[j];
                            outFile<<"T"<<"^"<<Loc[j]<<"^"<<"0"<<hexadecimal(length)<<"^"<<Object_code[j];
                        }
                        else{
                            cout<<"T"<<"^"<<Loc[j]<<"^"<<hexadecimal(length)<<"^"<<Object_code[j];
                            outFile<<"T"<<"^"<<Loc[j]<<"^"<<hexadecimal(length)<<"^"<<Object_code[j];
                        }
                        
                        flag=1;
                    }
                    else{
                         cout<<"^"<<Object_code[j];
                    }   
                }
            }      
        }
    }
    cout<<endl;
    outFile<<endl;
    //M record
    for(i=0;i<M_record.size();i++){
        cout<<"M"<<"^"<<sixletter(hexadecimal(Decimalahex(Loc[M_record[i]])+1))<<"^05"<<endl;
        outFile<<"M"<<"^"<<sixletter(hexadecimal(Decimalahex(Loc[M_record[i]])+1))<<"^05"<<endl;
    }
    //E record
    cout<<"E"<<"^"<<Loc[0]<<endl;
    outFile<<"E"<<"^"<<Loc[0]<<endl;


    outFile.close();

    return 0;
}
