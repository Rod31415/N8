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

            if(this->globalIndex>=this->globalBuffer.length())
                return 0;
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
    NodeProgram=0,
    NodeBlock,
    NodeBinaryExpr,
    NodeNumericLiteral,
    NodeIdentifier,
    NodeDeclareFunction,
    NodeCallFunction,
    NodeDeclareVar,
    NodeAssignVar,
};

enum Datatype{
    INT=0,
    SHORT,
    CHAR,
    VOID,
};

enum Operator{
    PlusOp=0,
    MinusOp,
    MultiplyOp,
    DivideOp,
    EqualOp,
    GreaterOp,
    GreaterEqualOp,
    LessOp,
    LessEqualOp,
};

struct StatementStruct
{
        NodeType type;
};

struct ExprStruct : StatementStruct
{
        
};

struct IdentifierStruct : StatementStruct
{
    IdentifierStruct(std::string v)
    {
    type=NodeIdentifier;
    value=v;
    }
    std::string value;
};


struct NumericLiteralStruct : StatementStruct
{
    NumericLiteralStruct(std::string v)
    {
    type=NodeNumericLiteral;
    value=v;
    }
    std::string value;
};

struct BinaryExprStruct : ExprStruct
{
    BinaryExprStruct(ExprStruct* l,ExprStruct* r,Operator o)
    {
    type=NodeBinaryExpr;
    left=l;
    right=r;
    op=o;
    }
    ExprStruct *left;
    ExprStruct *right;
    Operator op;
};


struct BlockStruct 
{
    std::vector<StatementStruct*> statements;
};

struct ArgumentsStruct 
{
    std::vector<ExprStruct*> expressions;
};


struct DeclareVarStruct : StatementStruct
{
        DeclareVarStruct(Datatype d,std::string i,ExprStruct* e)
        {
        type=NodeDeclareVar;
        dtype=d;
        identifier=i;
        expr=e;
        }
        Datatype dtype;
        std::string identifier;
        ExprStruct* expr;
};


struct DeclareFunctionStruct : StatementStruct
{
        DeclareFunctionStruct(Datatype d,std::string i)
        {
        type=NodeDeclareFunction;
        dtype=d;
        identifier=i;
        }
        Datatype dtype;
        std::string identifier;
        DeclareVarStruct** args;
        BlockStruct* block;
};


struct FunctionStruct : StatementStruct
{
        FunctionStruct(std::string i,ArgumentsStruct* a)
        {
            identifier=i;
            args=a;
        }
        std::string identifier;
        ArgumentsStruct* args;
};


struct AssignmentVarStruct : StatementStruct
{
        AssignmentVarStruct(std::string i,ExprStruct* e)
        {
        type=NodeAssignVar;
        identifier=i;
        expression=e;
        }

        std::string identifier;
        ExprStruct* expression;
};



struct ProgramStruct{
        StatementStruct** statements;
};

Operator TokenToOperator(Token t){
if(t.TK==PLUS_TK)return PlusOp;
    switch (t.TK)
    {
        case PLUS_TK:return PlusOp;break;
        case MINUS_TK:return MinusOp;break;
        case MULTIPLY_TK:return MultiplyOp;break;
        case DIVIDE_TK:return DivideOp;break;
        default:return PlusOp;
    }

}

Datatype StringToDatatype(std::string str){
if(str=="int")return INT;
if(str=="short")return SHORT;
if(str=="char")return CHAR;
if(str=="void")return VOID;
return INT;
}

class Parser
{
    private:
        Token* TokenArray;
        size_t pos;
        size_t size;
        Token actualToken;
        bool noErrorApear;
        int globalSpace;

    public:
        Parser(std::vector<Token> tk)
        {
            this->size=tk.size();
            this->TokenArray=new Token[this->size];
            std::copy_n(tk.begin(),tk.size(),this->TokenArray);
            this->noErrorApear=true;
            this->globalSpace=0;
            this->pos=0;
        }
        
        Token consumeToken()
        {
            return this->TokenArray[this->pos++];
        }

        Token seeToken(int offset=0)
        {
            if(this->pos+offset>this->size) return {EOF_TK,""};
            return this->TokenArray[this->pos+offset];
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
        
        ExprStruct* showIdentifier()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            Token t=this->seeToken();
            std::cout<<"Identifier "<<t.value<<"\n";
            if(this->seeToken(1).TK==OPEN_PARENTHESIS_TK){
                ExprStruct* fn=(ExprStruct*)this->parseFunction();
            this->globalSpace--;
            return fn;
            }
                
            this->globalSpace--;
            this->consumeToken();
            return (ExprStruct*)(new IdentifierStruct(t.value));

        }

        ExprStruct* showNumeric(){
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
           
            Token t=this->consumeToken();
            std::cout<<"Number "<<t.value<<"\n";


            this->globalSpace--;
            return (ExprStruct*)(new NumericLiteralStruct(t.value));
        }

        ExprStruct* parsePrimaryExpression()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"PrimaryExpression\n";
            ExprStruct* expr=nullptr;
            if(this->seeToken().TK==IDENTIFIER_TK)
                expr = this->showIdentifier();
            else if(this->seeToken().TK==NUMBER_TK)
                expr = this->showNumeric();
            else if(this->seeToken().TK==OPEN_PARENTHESIS_TK)
            {
                this->expectToken(OPEN_PARENTHESIS_TK);
                expr = this->parseExpression();
                this->expectToken(CLOSE_PARENTHESIS_TK);
            }
           
            this->globalSpace--;
            return expr;

        }


        ExprStruct* parseMultiplicativeExpression()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"MultiplicativeExpression\n";
            
            ExprStruct* left=this->parsePrimaryExpression();
            
            while((this->seeToken().TK==MULTIPLY_TK||this->seeToken().TK==DIVIDE_TK)&&this->noErrorApear)
            {
                Operator op=TokenToOperator(this->consumeToken());
                ExprStruct* right=this->parsePrimaryExpression();
                left=(ExprStruct*)(new BinaryExprStruct(left,right,op));
            }
            
            this->globalSpace--;
            return left;
        }

        ExprStruct* parseAdditiveExpression()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"AdditiveExpression\n";
            
            ExprStruct* left=this->parseMultiplicativeExpression();

            while((this->seeToken().TK==PLUS_TK||this->seeToken().TK==MINUS_TK)&&this->noErrorApear)
            {
                Operator op=TokenToOperator(this->consumeToken());
                ExprStruct* right=this->parseMultiplicativeExpression();
                left=(ExprStruct*)(new BinaryExprStruct(left,right,op));
            }


            this->globalSpace--;
            return left;
        }

        ExprStruct* parseComparisonExpression()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"ComparisonExpression\n";
            this->parseAdditiveExpression();
            while(this->seeToken().TK==EQUAL_TK&&this->seeToken(1).TK==EQUAL_TK)
            {
                this->consumeToken();
                this->consumeToken();
                this->parseAdditiveExpression();
            }
           

            this->globalSpace--;

        }

        ExprStruct* parseExpression()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Expression\n";
            ExprStruct* expr= this->parseAdditiveExpression();

            this->globalSpace--;
            return expr;

        }

        BlockStruct* parseBlock()
        {
            
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Block\n";
            BlockStruct* block=new BlockStruct;

            this->expectToken(OPEN_BRACKETS_TK);
            while(!this->expectToken(CLOSE_BRACKETS_TK)&&this->noErrorApear){
                block->statements.push_back(this->parseStatement());
            }


            this->globalSpace--;
            return block;
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

        StatementStruct* parseAssignment()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Assignment\n";

            if(!this->expectToken(IDENTIFIER_TK))
                perror("NOT IDENTIFIER\n");
            if(!this->expectToken(EQUAL_TK))
                perror("NOT IDENTIFIER\n");
            
            ExprStruct *expr=this->parseExpression();
            
            this->expectToken(SEMICOLON_TK);

            this->globalSpace--;
            return expr;

        }

        DeclareVarStruct* parseDeclarationAssignment()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Assignment\n";

            if(this->seeToken().TK!=DATATYPE_TK)
                perror("NOT DATATYPE\n");
            Datatype dt=StringToDatatype(this->consumeToken().value);
            if(this->seeToken().TK!=IDENTIFIER_TK)
                perror("NOT IDENTIFIER\n");
            std::string i=this->consumeToken().value;
            if(!this->expectToken(EQUAL_TK))
                perror("NOT IDENTIFIER\n");
            
            ExprStruct* expr=this->parseExpression();
            
            this->expectToken(SEMICOLON_TK);
            
            DeclareVarStruct* var=new DeclareVarStruct(dt,i,expr);

            this->globalSpace--;
            return var;
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

        ArgumentsStruct* parseArguments(){
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Args\n";

            ArgumentsStruct* args=new ArgumentsStruct();

            this->expectToken(OPEN_PARENTHESIS_TK);
            while(!this->expectToken(CLOSE_PARENTHESIS_TK)&&this->noErrorApear){
                //std::cout<<this->consumeToken().TK;
                args->expressions.push_back(this->parseExpression());
                this->expectToken(COMMA_TK);
                //getchar();
            }


            this->globalSpace--;
            return args;
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
        FunctionStruct* parseFunction(){
             this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Function\n";

            if(this->seeToken().TK!=IDENTIFIER_TK)
                perror("NOT IDENTIFIER\n");
            std::string i=this->consumeToken().value;
            ArgumentsStruct* arg=this->parseArguments();

            this->expectToken(SEMICOLON_TK);
            return new FunctionStruct(i,arg);
            
        }
        
        StatementStruct* parseStatement()
        {
            this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Statement\n";


            StatementStruct* stmt=nullptr;
            
            if(seeToken().TK==DATATYPE_TK&&seeToken(1).TK==IDENTIFIER_TK)
            {
                if(this->seeToken(2).TK==SEMICOLON_TK||this->seeToken(2).TK==COMMA_TK||this->seeToken(2).TK==CLOSE_PARENTHESIS_TK)
                    this->parseDeclaration();
                else if(this->seeToken(2).TK==EQUAL_TK)
                    stmt=(StatementStruct*)this->parseDeclarationAssignment();
                else if(this->seeToken(2).TK==OPEN_PARENTHESIS_TK)
                    this->parseDeclareFunction();
            }

            getchar();
            if(seeToken().TK==IDENTIFIER_TK){
                 if(this->seeToken(1).TK==EQUAL_TK)
                    stmt=(StatementStruct*)this->parseAssignment();
                 else if(this->seeToken(1).TK==OPEN_PARENTHESIS_TK)
                    this->parseFunction();

            }


            this->globalSpace--;
            return stmt;
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
