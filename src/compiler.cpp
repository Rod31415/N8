#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>








enum TokenType{
    INT_TK=0,
    SHORT_TK,
    CHAR_TK,
    VOID_TK,
    RETURN_TK,
    MAIN_TK,
    
    SEMICOLON_TK,
    OPEN_PARENTHESIS_TK,
    CLOSE_PARENTHESIS_TK,
    OPEN_BRACKETS_TK,
    CLOSE_BRACKETS_TK,
    PLUS_TK,
    MINUS_TK,
    MULTIPLY_TK,
    DIVIDE_TK,
    EQUAL_TK,

    IDENTIFIER_TK,

    NUMBER_TK,


};


struct reservedKeyWordsStruct{
std::string keyWord;
TokenType tk;
};

reservedKeyWordsStruct reservedKeyWords[]={
    {"int",INT_TK},
    {"short",SHORT_TK},
    {"char",CHAR_TK},
    {"void",VOID_TK},
    {"return",RETURN_TK},
    {"main",MAIN_TK},
};


struct reservedCharsStruct{
char uniqueChar;
TokenType tk;
};

reservedCharsStruct reservedChars[]={
    {';',SEMICOLON_TK},
    {'(',OPEN_PARENTHESIS_TK},
    {')',CLOSE_PARENTHESIS_TK},
    {'{',OPEN_BRACKETS_TK},
    {'}',CLOSE_BRACKETS_TK},
    {'+',PLUS_TK},
    {'-',MINUS_TK},
    {'*',MULTIPLY_TK},
    {'/',DIVIDE_TK},
    {'=',EQUAL_TK},
};

enum StateType{
    NORMAL_STATE=0,
    ALPHA_STATE,
    NUMERIC_STATE,
    

};




enum TCode{
    SUCCESS,
    FAILESS
};

struct Token{
    TokenType TK;
    std::string value;
};

std::vector<Token> TokenVector;



bool isNumeric(char c){
return (c>='0'&&c<='9');
}

bool isAlpha(char c){
return (c>='A'&&c<='Z')||(c>='a'&&c<='z');
}


bool isAlphaNumeric(char c){
return isAlpha(c)||isNumeric(c);
}



void pushToken(TokenType tk,std::string readed){
TokenVector.push_back({tk,readed});
}

bool pushDetectedReservedChar(char actualChar){
    for(auto c:reservedChars){
        if(actualChar==c.uniqueChar){
            std::string s(1,actualChar);
        pushToken(c.tk,s);
        return true;}
    }
    return false;
}


bool pushDetectedReservedWords(std::string readed){

if(readed=="")return false;

std::string aux;
size_t pos = readed.find_first_not_of(' ');
        if (pos != std::string::npos)
        aux = readed.substr(pos, readed.length());
        for(auto word:reservedKeyWords){
            if(aux==word.keyWord){
                pushToken(word.tk,readed);
                return true;
            }
        }
    return false;
}

std::string globalBuffer="";
size_t globalIndex=0;
void setGlobalBuffer(std::string str){
    globalBuffer=str;
}

char consumeChar(){
    return globalBuffer[globalIndex++];
}

char seeChar(){
    return globalBuffer[globalIndex];
}


TCode Tokenize(std::string buffer){
    StateType actualState=NORMAL_STATE;
    std::string intermediateString="";
    setGlobalBuffer(buffer);
    char actualChar; 
    while(true){

    actualChar=seeChar();
    
    if(actualChar=='\n')actualChar=consumeChar();
    actualChar=seeChar();

    if(actualChar==0)break;

        switch(actualState){
            case NORMAL_STATE:
                        if(isAlpha(actualChar)){
                            actualState=ALPHA_STATE;
                            intermediateString=actualChar;}
                        else if(isNumeric(actualChar)){
                            actualState=NUMERIC_STATE;
                            intermediateString=actualChar;}
                        else{
                            pushDetectedReservedChar(actualChar);
                        }
                        consumeChar();
                    break;
            case ALPHA_STATE:
                    if(!isAlphaNumeric(actualChar)){
                        if(!pushDetectedReservedWords(intermediateString))
                            pushToken(IDENTIFIER_TK,intermediateString);

                        actualState=NORMAL_STATE;
                        intermediateString="";
                    }
                    else{
                        consumeChar();
                    }
                    intermediateString+=actualChar;
                    
                    break;
            case NUMERIC_STATE:
                    
                    if(!isNumeric(actualChar)){
                        pushToken(NUMBER_TK,intermediateString);
                        actualState=NORMAL_STATE;
                        intermediateString="";
                    }
                    else{
                    consumeChar();
                    }
                    intermediateString+=actualChar;

                    break;
        }
        //std::cout<< intermediateString<<"\n";

    }

    return SUCCESS;


}


int main(int argc, char** argv){
    if (argc == 1)
        {
                std::cout << "compiler <filename>";
                return 1;
        }

        std::ifstream file(argv[1]);
        std::stringstream ssbuff;
        ssbuff<<file.rdbuf();
        std::string buffer=ssbuff.str();
        
        Tokenize(buffer);
        for(auto t:TokenVector){
        std::cout<<t.TK<<" "<<t.value<<"\n";
        }

              

}
