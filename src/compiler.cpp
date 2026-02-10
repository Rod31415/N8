#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

#include <conio.h>



//////////////////////////// LEXER /////////////////////////



enum TokenType{
    DATATYPE_TK=0,
    RETURN_TK,
    
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
    COMMA_TK,

    IDENTIFIER_TK,

    NUMBER_TK,
    EOF_TK,


};


struct reservedKeyWordsStruct{
std::string keyWord;
TokenType tk;
};

reservedKeyWordsStruct reservedKeyWords[]={
    {"int",DATATYPE_TK},
    {"short",DATATYPE_TK},
    {"char",DATATYPE_TK},
    {"void",DATATYPE_TK},
    {"return",RETURN_TK},
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
    {',',COMMA_TK},
};

enum StateType{
    NORMAL_STATE=0,
    ALPHA_STATE,
    NUMERIC_STATE,
    

};




enum TCode{
    FAILESS=0,
    SUCCESS,
};

struct Token{
    TokenType TK;
    std::string value;
};




bool isNumeric(char c){
return (c>='0'&&c<='9');
}

bool isAlpha(char c){
return (c>='A'&&c<='Z')||(c>='a'&&c<='z');
}


bool isAlphaNumeric(char c){
return isAlpha(c)||isNumeric(c);
}





class Lexer
{
    private:
        std::vector<Token> TokenVector;
        
        std::string globalBuffer;
        size_t globalIndex;
        StateType actualState;
        std::string intermediateString="";
        char actualChar;

    public:
        
        Lexer(std::string str){
        this->globalIndex=0;
        this->globalBuffer=str;
        }
        
        void pushToken(TokenType tk,std::string readed)
        {
            this->TokenVector.push_back({tk,readed});
        }

        bool pushDetectedReservedChar(char setChar)
        {
            for(auto c:reservedChars){
                if(setChar==c.uniqueChar)
                {
                    std::string s(1,setChar);
                    this->pushToken(c.tk,s);
                    return true;
                }
            }
            return false;
        }


        bool pushDetectedReservedWords(std::string readed){

            if(readed=="")return false;

            std::string aux;
            size_t pos = readed.find_first_not_of(' ');
            if (pos != std::string::npos)
                aux = readed.substr(pos, readed.length());
            for(auto word:reservedKeyWords)
            {
                if(aux==word.keyWord)
                {
                    this->pushToken(word.tk,aux);
                    return true;
                }
            }
            return false;
        }

        char consumeChar()
        {
            return this->globalBuffer[this->globalIndex++];
        }
        
        char seeChar()
        {
            return this->globalBuffer[this->globalIndex];
        }
        
        std::vector<Token> Tokenize(){

            this->actualState=NORMAL_STATE;
            while(true){

                this->actualChar=this->seeChar();
    
                if(this->actualChar=='\n')
                    this->actualChar=this->consumeChar();
                
                this->actualChar=this->seeChar();

                if(this->actualChar==0)break;
                switch(this->actualState){
                    case NORMAL_STATE:
                        if(isAlpha(this->actualChar)){
                            this->actualState=ALPHA_STATE;
                            this->intermediateString=this->actualChar;}
                        else if(isNumeric(this->actualChar)){
                            this->actualState=NUMERIC_STATE;
                            this->intermediateString=this->actualChar;}
                        else{
                            this->pushDetectedReservedChar(this->actualChar);
                        }
                        this->consumeChar();
                    break;
                    case ALPHA_STATE:
                        if(!isAlphaNumeric(this->actualChar)){
                            if(!this->pushDetectedReservedWords(this->intermediateString))
                                this->pushToken(IDENTIFIER_TK,this->intermediateString);

                            this->actualState=NORMAL_STATE;
                            this->intermediateString="";
                        }
                        else
                            this->consumeChar();
                        
                        this->intermediateString+=actualChar;
                    
                    break;
                    case NUMERIC_STATE:
                    
                        if(!isNumeric(this->actualChar)){
                            this->pushToken(NUMBER_TK,this->intermediateString);
                            this->actualState=NORMAL_STATE;
                            this->intermediateString="";
                        }
                        else
                            this->consumeChar();
                    
                        this->intermediateString+=this->actualChar;
                    break;
                }
        //std::cout<< intermediateString<<"\n";

            }
        this->pushToken(EOF_TK,"EOF");

        return this->TokenVector;
        }
};



///////////////////////// PARSER ////////////////////////

enum NodeType{
    NodeProgram,
    NodeBlock
};

enum Datatype{
    INT=0,
    SHORT,
    CHAR,
    VOID,
};

struct StatementStruct
{
        NodeType type;
};

struct Expr : StatementStruct
{

};

struct BlockStruct
{
        StatementStruct** statements;
};

struct FunctionStruct
{

        BlockStruct* block;
};

struct DeclarationStruct
{
        Datatype type;
        std::string identifier;
};

struct AssignmentStruct
{
        Datatype type;
        std::string identifier;
        Expr* expression;
};



struct ProgramStruct{
        StatementStruct** statements;
};

class Parser
{
    private:
        Token* TokenArray;
        Token actualToken;
        bool noErrorApear;
        int globalSpace;

    public:
        Parser(std::vector<Token> tk)
        {
            this->TokenArray=new Token[tk.size()];
            std::copy_n(tk.begin(),tk.size(),this->TokenArray);
            this->noErrorApear=true;
            this->globalSpace=0;
        }
        
        Token consumeToken()
        {
            return *this->TokenArray++;
        }

        Token seeToken(int offset=0)
        {
            return *(this->TokenArray+offset);
        }

        bool expectToken(TokenType tt)
        {
            if(this->seeToken().TK==tt){
            //std::cout<<this->seeToken().TK<<"\n";
                return (this->consumeToken().TK==tt);}
            else
                return false;
        }

        bool peekToken(TokenType tt)
        {
            return (this->seeToken().TK==tt);
        }
        
        void perror(std::string str)
        {
            std::cout<<str<<"\n";
            this->noErrorApear=false;
        }
        
        void parseBinaryExpression()
        {
        }

        void parseExpression()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Expression\n";


            
           

            this->globalSpace--;

        }

        void parseBlock()
        {
            
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Block\n";


            this->expectToken(OPEN_BRACKETS_TK);
            while(!this->expectToken(CLOSE_BRACKETS_TK)&&this->noErrorApear){
                this->parseStatement();
            }


            this->globalSpace--;
        }

        void parseDeclaration()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Declaration\n";


            if(!this->expectToken(DATATYPE_TK))
                perror("NOT DATATYPE\n");
            if(!this->expectToken(IDENTIFIER_TK))
                perror("NOT IDENTIFIER\n");
            this->expectToken(SEMICOLON_TK);
            this->globalSpace--;

        }

        void parseAssignment()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Assignment\n";


            if(!this->expectToken(DATATYPE_TK))
                perror("NOT DATATYPE\n");
            if(!this->expectToken(IDENTIFIER_TK))
                perror("NOT IDENTIFIER\n");
            if(!this->expectToken(EQUAL_TK))
                perror("NOT IDENTIFIER\n");
            
            this->parseExpression();

            this->globalSpace--;

        }


        void parseDeclareArguments()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"DeclareArgs\n";


            this->expectToken(OPEN_PARENTHESIS_TK);
            while(!this->expectToken(CLOSE_PARENTHESIS_TK)&&this->noErrorApear){
                //std::cout<<this->consumeToken().TK;
                this->parseStatement();
                this->expectToken(COMMA_TK);
                //getchar();
            }


            this->globalSpace--;
            
        }

        void parseArguments(){
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Args\n";


            this->expectToken(OPEN_PARENTHESIS_TK);
            while(!this->expectToken(CLOSE_PARENTHESIS_TK)&&this->noErrorApear){
                //std::cout<<this->consumeToken().TK;
                this->expectToken(IDENTIFIER_TK);
                this->expectToken(COMMA_TK);
                //getchar();
            }


            this->globalSpace--;

        }

        void parseDeclareFunction()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"DeclareFunction\n";


            //FunctionStruct* fn=new FunctionStruct();
            if(!this->expectToken(DATATYPE_TK))
                perror("NOT DATATYPE\n");
            if(!this->expectToken(IDENTIFIER_TK))
                perror("NOT IDENTIFIER\n");
            this->parseDeclareArguments();
            this->parseBlock();


            this->globalSpace--;
        }
        void parseFunction(){
             this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Function\n";


            //FunctionStruct* fn=new FunctionStruct();
            if(!this->expectToken(IDENTIFIER_TK))
                perror("NOT IDENTIFIER\n");
            this->parseArguments();

            this->expectToken(SEMICOLON_TK);

        }
        
        void parseStatement()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Statement\n";


            //StatementStruct* a=new StatementStruct();
            
            if(seeToken().TK==DATATYPE_TK&&seeToken(1).TK==IDENTIFIER_TK)
            {
                if(this->seeToken(2).TK==SEMICOLON_TK||this->seeToken(2).TK==COMMA_TK||this->seeToken(2).TK==CLOSE_PARENTHESIS_TK)
                    this->parseDeclaration();
                               else if(this->seeToken(2).TK==OPEN_PARENTHESIS_TK)
                    this->parseDeclareFunction();
            }

            if(seeToken().TK==IDENTIFIER_TK){
                 if(this->seeToken(1).TK==EQUAL_TK)
                    this->parseAssignment();
                 else if(this->seeToken(1).TK==OPEN_PARENTHESIS_TK)
                    this->parseFunction();

            }


            this->globalSpace--;
        }

        void parseProgram()
        {
            while(this->seeToken().TK!=EOF_TK&&this->noErrorApear){
                this->parseStatement();
            }
            
        }

        void generateAbstractSyntaxTree()
        {
            this->parseProgram();

        }

};



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
        
        Lexer Lexer(buffer);

        std::vector<Token> tk=Lexer.Tokenize();

        Parser Parser(tk);

        Parser.generateAbstractSyntaxTree();


        for(auto t:tk){
        std::cout<<t.TK<<" "<<t.value<<"\n";
        }

              

}
