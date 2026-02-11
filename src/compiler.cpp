#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

#include <conio.h>



//////////////////////////// LEXER /////////////////////////



enum TokenType{
    DATATYPE_TK=0,
    RETURN_TK,
    IF_TK,
    WHILE_TK,
    
    SEMICOLON_TK,
    OPEN_PARENTHESIS_TK,
    CLOSE_PARENTHESIS_TK,
    OPEN_BRACKETS_TK,
    CLOSE_BRACKETS_TK,
    PLUS_TK,
    MINUS_TK,
    MULTIPLY_TK,
    DIVIDE_TK,
    SHIFT_RIGHT_TK,
    SHIFT_LEFT_TK,
    EQUAL_TK,
    NOT_TK,
    
    NOT_EQUAL_TK,
    EQUAL_EQUAL_TK,
    GREATER_TK,
    GREATER_EQUAL_TK,
    LESS_TK,
    LESS_EQUAL_TK,
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
    {"byte",DATATYPE_TK},
    {"word",DATATYPE_TK},
    {"return",RETURN_TK},
    {"if",IF_TK},
    {"while",WHILE_TK},
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
        
        char seeChar(int offset=0)
        {

            if(this->globalIndex+offset>=this->globalBuffer.length())
                return 0;
            return this->globalBuffer[this->globalIndex+offset];
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

                        else if(this->seeChar()=='='){
                            if(this->seeChar(1)=='='){
                                this->consumeChar();
                                this->pushToken(EQUAL_EQUAL_TK,"==");
                            }
                            else
                                this->pushToken(EQUAL_TK,"=");
                        }

                        else if(this->seeChar()=='>'){
                            if(this->seeChar(1)=='='){
                                this->consumeChar();
                                this->pushToken(GREATER_EQUAL_TK,">=");
                            }
                            else if(this->seeChar(1)=='>'){
                                this->consumeChar();
                                this->pushToken(SHIFT_RIGHT_TK, ">>");
                            }
                            else
                                this->pushToken(GREATER_TK,">");
                        }

                        else if(this->seeChar()=='<'){
                            if(this->seeChar(1)=='='){
                                this->consumeChar();
                                this->pushToken(LESS_EQUAL_TK,"<=");
                            }
                            else if(this->seeChar(1)=='<'){
                                this->consumeChar();
                                this->pushToken(SHIFT_LEFT_TK, "<<");
                            }

                            else
                                this->pushToken(LESS_TK,"<");
                        }
                        else if(this->seeChar()=='!'){
                            if(this->seeChar(1)=='='){
                                this->consumeChar();
                                this->pushToken(NOT_EQUAL_TK, "!=");
                            }
                            else
                                this->pushToken(NOT_TK, "!");
                        }

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
    NodeFunction,
    NodeDeclareVar,
    NodeAssignVar,
    NodeDeclareArguments,
    NodeArguments,
    NodeReturn,
    NodeIf,
    NodeWhile,
    NodeEnd,
    
};

enum Datatype{
    INT=0,
    SHORT,
    CHAR,
    BYTE,
    WORD,
};

enum Operator{
    PlusOp=0,
    MinusOp,
    MultiplyOp,
    DivideOp,
    EqualOp,
    EqualEqualOp,
    GreaterOp,
    GreaterEqualOp,
    LessOp,
    LessEqualOp,
    NotEqualOp,
    NotOp,
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


struct BlockStruct : StatementStruct
{
    BlockStruct()
    {
        type=NodeBlock;
    }

    std::vector<StatementStruct*> statements;
};

struct ArgumentsStruct : StatementStruct
{
    ArgumentsStruct()
    {
        type=NodeArguments;
    }

    std::vector<ExprStruct*> expressions;
};

struct DeclareArgumentsStruct : StatementStruct
{
    DeclareArgumentsStruct()
    {
        type=NodeDeclareArguments;
    }

    std::vector<StatementStruct*> statements;
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


struct DeclareVarStruct : StatementStruct
{
        DeclareVarStruct(Datatype d,std::string i,ExprStruct* e)
        {
            type=NodeDeclareVar;
            dtype=d;
            identifier=i;
            expr=nullptr;
            if(e!=nullptr)
            expr=new AssignmentVarStruct(i,e);
        }
        Datatype dtype;
        std::string identifier;
        AssignmentVarStruct* expr;
};


struct DeclareFunctionStruct : StatementStruct
{
        DeclareFunctionStruct(Datatype d,std::string i,DeclareArgumentsStruct* a,BlockStruct* b)
        {
            type=NodeDeclareFunction;
            dtype=d;
            identifier=i;
            args=a;
            block=b;
        }
        Datatype dtype;
        std::string identifier;
        DeclareArgumentsStruct* args;
        BlockStruct* block;
};


struct FunctionStruct : StatementStruct
{
        FunctionStruct(std::string i,ArgumentsStruct* a)
        {
            type=NodeFunction;
            identifier=i;
            args=a;
        }
        std::string identifier;
        ArgumentsStruct* args;
};



struct ReturnStruct : StatementStruct
{
    ReturnStruct(ExprStruct* e)
    {
        type=NodeReturn;
        expression=e;
    }
    ExprStruct* expression;
};

struct IfStruct : StatementStruct
{
    IfStruct(ExprStruct* e,BlockStruct* b){
        type=NodeIf;
        expression=e;
        block=b;
    }
    ExprStruct* expression;
    BlockStruct* block;
};

struct WhileStruct : StatementStruct
{
    WhileStruct(ExprStruct* e,BlockStruct* b){
        type=NodeWhile;
        expression=e;
        block=b;
    }
    ExprStruct* expression;
    BlockStruct* block;
};



struct EndStruct : StatementStruct
{
        EndStruct()
        {
            type=NodeEnd;
        }
};

struct ProgramStruct : StatementStruct
{
        ProgramStruct()
        {
            type=NodeProgram;
        }
        std::vector<StatementStruct*> statements;
};

Operator TokenToOperator(Token t){
if(t.TK==PLUS_TK)return PlusOp;
    switch (t.TK)
    {
        case PLUS_TK:return PlusOp;break;
        case MINUS_TK:return MinusOp;break;
        case MULTIPLY_TK:return MultiplyOp;break;
        case DIVIDE_TK:return DivideOp;break;
        case EQUAL_EQUAL_TK:return EqualEqualOp;break;
        case GREATER_TK:return GreaterOp;break;
        case GREATER_EQUAL_TK:return GreaterEqualOp;break;
        case LESS_TK:return LessOp;break;
        case LESS_EQUAL_TK:return LessEqualOp;break;
        case NOT_EQUAL_TK:return NotEqualOp;break;
        case NOT_TK:return NotOp;break;
        default:return PlusOp;
    }

}

Datatype StringToDatatype(std::string str){
if(str=="int")return INT;
if(str=="short")return SHORT;
if(str=="char")return CHAR;
if(str=="byte")return BYTE;
if(str=="word")return WORD;

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

        bool expectErrorToken(TokenType tt,std::string error)
        {
            if(this->seeToken().TK==tt)
                return (this->consumeToken().TK==tt);
            std::cout<<error;
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
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";*/
            Token t=this->seeToken();
            //std::cout<<"Identifier "<<t.value<<"\n";
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
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";*/
           
            Token t=this->consumeToken();
            //std::cout<<"Number "<<t.value<<"\n";


            this->globalSpace--;
            return (ExprStruct*)(new NumericLiteralStruct(t.value));
        }

        ExprStruct* parsePrimaryExpression()
        {
           /* this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"PrimaryExpression\n";*/
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
           
            //this->globalSpace--;
            return expr;

        }


        ExprStruct* parseMultiplicativeExpression()
        {
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"MultiplicativeExpression\n";*/
            
            ExprStruct* left=this->parsePrimaryExpression();
            
            while((this->seeToken().TK==MULTIPLY_TK||this->seeToken().TK==DIVIDE_TK)&&this->noErrorApear)
            {
                Operator op=TokenToOperator(this->consumeToken());
                ExprStruct* right=this->parsePrimaryExpression();
                left=(ExprStruct*)(new BinaryExprStruct(left,right,op));
            }
            
            //this->globalSpace--;
            return left;
        }

        ExprStruct* parseAdditiveExpression()
        {
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"AdditiveExpression\n";*/
            
            ExprStruct* left=this->parseMultiplicativeExpression();

            while((this->seeToken().TK==PLUS_TK||this->seeToken().TK==MINUS_TK)&&this->noErrorApear)
            {
                Operator op=TokenToOperator(this->consumeToken());
                ExprStruct* right=this->parseMultiplicativeExpression();
                left=(ExprStruct*)(new BinaryExprStruct(left,right,op));
            }


            //this->globalSpace--;
            return left;
        }

        ExprStruct* parseComparisonExpression()
        {
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";*/
            ExprStruct* left=this->parseAdditiveExpression();
            while(this->seeToken().TK==EQUAL_EQUAL_TK||this->seeToken().TK==GREATER_EQUAL_TK||this->seeToken().TK==LESS_EQUAL_TK||this->seeToken().TK==GREATER_TK||this->seeToken().TK==LESS_TK||this->seeToken().TK==NOT_EQUAL_TK)
            {
                Operator op=TokenToOperator(this->consumeToken());
                ExprStruct* right=this->parseAdditiveExpression();
                left=(ExprStruct*)(new BinaryExprStruct(left,right,op));
            }
           

            //this->globalSpace--;
            return left;
        }

        ExprStruct* parseExpression()
        {
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Expression\n";*/
            ExprStruct* expr= this->parseComparisonExpression();

           // this->globalSpace--;
            return expr;

        }

        BlockStruct* parseBlock()
        {
            
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Block\n";*/
            BlockStruct* block=new BlockStruct;

            this->expectToken(OPEN_BRACKETS_TK);
            while(!this->expectToken(CLOSE_BRACKETS_TK)&&this->noErrorApear){
                block->statements.push_back(this->parseStatement());
            }


            //this->globalSpace--;
            return block;
        }

        DeclareVarStruct* parseDeclaration()
        {
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Declaration\n";*/


            if(this->seeToken().TK!=DATATYPE_TK)
                perror("NOT DATATYPE\n");
            Datatype dt=StringToDatatype(this->consumeToken().value);

            if(this->seeToken().TK!=IDENTIFIER_TK)
                perror("NOT IDENTIFIER\n");
            std::string i=this->consumeToken().value;

            this->expectToken(SEMICOLON_TK);

            DeclareVarStruct* var=new DeclareVarStruct(dt,i,nullptr);

           // this->globalSpace--;

            return var;
        }

        AssignmentVarStruct* parseAssignment()
        {
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Assignment\n";*/
            
            if(this->seeToken().TK!=IDENTIFIER_TK)
                perror("NOT IDENTIFIER\n");
            std::string i=this->consumeToken().value;
            if(!this->expectToken(EQUAL_TK))
                perror("NOT IDENTIFIER\n");
            
            ExprStruct *expr=this->parseExpression();
            
            this->expectToken(SEMICOLON_TK);

            AssignmentVarStruct* var=new AssignmentVarStruct(i,expr);
            //this->globalSpace--;
            return var;

        }

        DeclareVarStruct* parseDeclarationAssignment()
        {
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Assignment\n";*/

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

            //this->globalSpace--;
            return var;
        }



        DeclareArgumentsStruct* parseDeclareArguments()
        {
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"DeclareArgs\n";*/

            DeclareArgumentsStruct* args=new DeclareArgumentsStruct();

            this->expectToken(OPEN_PARENTHESIS_TK);
            while(!this->expectToken(CLOSE_PARENTHESIS_TK)&&this->noErrorApear){
                //std::cout<<this->consumeToken().TK;
                args->statements.push_back(this->parseStatement());
                this->expectToken(COMMA_TK);
                //();
            }


            //this->globalSpace--;
            return args;
        }

        ArgumentsStruct* parseArguments(){
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Args\n";*/

            ArgumentsStruct* args=new ArgumentsStruct();

            this->expectToken(OPEN_PARENTHESIS_TK);
            while(!this->expectToken(CLOSE_PARENTHESIS_TK)&&this->noErrorApear){
                //std::cout<<this->consumeToken().TK;
                args->expressions.push_back(this->parseExpression());
                this->expectToken(COMMA_TK);
                //();
            }


            //this->globalSpace--;
            return args;
        }

        DeclareFunctionStruct* parseDeclareFunction()
        {
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"DeclareFunction\n";*/


            //FunctionStruct* fn=new FunctionStruct();
            if(this->seeToken().TK!=DATATYPE_TK)
                perror("NOT DATATYPE\n");
            Datatype d=StringToDatatype(this->consumeToken().value);
            if(this->seeToken().TK!=IDENTIFIER_TK)
                perror("NOT IDENTIFIER\n");
            std::string i=this->consumeToken().value;
            
            DeclareArgumentsStruct* args=this->parseDeclareArguments();
            BlockStruct* b=this->parseBlock();


            //this->globalSpace--;

            return new DeclareFunctionStruct(d,i,args,b);
        }
        FunctionStruct* parseFunction()
        {
            /* this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Function\n";*/

            if(this->seeToken().TK!=IDENTIFIER_TK)
                perror("NOT IDENTIFIER\n");
            std::string i=this->consumeToken().value;
            ArgumentsStruct* arg=this->parseArguments();

            this->expectToken(SEMICOLON_TK);
            //this->globalSpace--;
            return new FunctionStruct(i,arg);
            
        }
        ReturnStruct* parseReturn()
        {
            if(!this->expectToken(RETURN_TK))
                perror("NOT Return\n");

            ExprStruct* expr=this->parseExpression();

            this->expectToken(SEMICOLON_TK);

            return new ReturnStruct(expr);
        }
        IfStruct* parseIf()
        {
            this->expectErrorToken(IF_TK,"No if token on if statement\n");
            this->expectErrorToken(OPEN_PARENTHESIS_TK,"No O parenthesis token on if statement\n");

            ExprStruct* expr=this->parseExpression();
            
            this->expectErrorToken(CLOSE_PARENTHESIS_TK,"No C parenthesis token on if statement\n");
            //std::cout<<this->consumeToken().TK<<"\n";

            BlockStruct* block=this->parseBlock();

            return new IfStruct(expr,block);
        }

        WhileStruct* parseWhile()
        {
            this->expectErrorToken(WHILE_TK,"No while token on while statement\n");
            this->expectErrorToken(OPEN_PARENTHESIS_TK,"No O parenthesis token on while statement\n");

            ExprStruct* expr=this->parseExpression();
            
            this->expectErrorToken(CLOSE_PARENTHESIS_TK,"No C parenthesis token on while statement\n");
            //std::cout<<this->consumeToken().TK<<"\n";

            BlockStruct* block=this->parseBlock();

            return new WhileStruct(expr,block);
        }
        
        StatementStruct* parseStatement(bool global=false)
        {
            /*this->globalSpace++;
            for(int i=0;i<this->globalSpace;i++)
                std::cout<<"  ";
            std::cout<<"Statement\n";*/
            

            StatementStruct* stmt=nullptr;
            
            if(this->seeToken().TK==DATATYPE_TK&&this->seeToken(1).TK==IDENTIFIER_TK)
            {
                if(this->seeToken(2).TK==SEMICOLON_TK||this->seeToken(2).TK==COMMA_TK||this->seeToken(2).TK==CLOSE_PARENTHESIS_TK)
                    stmt=this->parseDeclaration();
                else if(this->seeToken(2).TK==EQUAL_TK)
                    stmt=(StatementStruct*)this->parseDeclarationAssignment();
                else if(this->seeToken(2).TK==OPEN_PARENTHESIS_TK)
                    stmt=this->parseDeclareFunction();
            }

            else if(this->seeToken().TK==IDENTIFIER_TK){
                 if(this->seeToken(1).TK==EQUAL_TK)
                    stmt=(StatementStruct*)this->parseAssignment();
                 else if(this->seeToken(1).TK==OPEN_PARENTHESIS_TK)
                    stmt=this->parseFunction();

            }

            else if(this->seeToken().TK==RETURN_TK&&!global)
                stmt=(StatementStruct*)this->parseReturn();
            else if(this->seeToken().TK==IF_TK&&!global)
                stmt=(StatementStruct*)this->parseIf();
            else if(this->seeToken().TK==WHILE_TK&&!global)
                stmt=(StatementStruct*)this->parseWhile();

            //this->globalSpace--;
            return stmt;
        }

        ProgramStruct* parseProgram()
        {
            ProgramStruct* program = new ProgramStruct();
            while(this->seeToken().TK!=EOF_TK&&this->noErrorApear){
                program->statements.push_back(this->parseStatement(true));
            }
            program->statements.push_back(new EndStruct());

            return program;
            
        }

        StatementStruct* generateAbstractSyntaxTree()
        {
            ProgramStruct* program;
            program=this->parseProgram();
            return (StatementStruct*)program;
        }

};

int indexSpace=0;

void showSpaces(std::string str)
{
    indexSpace++;
for(int i=0;i<indexSpace;i++)
    std::cout<<"  ";
    
    if(str!="")std::cout<<str<<"\n";
}



void seeDatatype(Datatype d)
{
    showSpaces("Datatype :");
    indexSpace--;
    showSpaces("");
    switch (d)
    {
        case INT:std::cout<<" int ";break;
        case SHORT:std::cout<<" short ";break;
        case CHAR:std::cout<<" char ";break;
    }
    std::cout<<"\n";
    indexSpace--;
}

int sizeOfDataType(Datatype d)
{
switch (d)
    {
        case INT:return 4;
        case SHORT:return 2;
        case CHAR:return 1;
        case BYTE:return 1;
        case WORD:return 2;
    }

}


void seeOperator(Operator o)
{
    showSpaces("Operator :");
    indexSpace--;
    showSpaces("");
    switch (o)
    {
        case PlusOp:std::cout<<" + ";break;
        case MinusOp:std::cout<<" - ";break;
        case MultiplyOp:std::cout<<" * ";break;
        case DivideOp:std::cout<<" / ";break;
        case EqualEqualOp:std::cout<<" == ";break;
        case GreaterEqualOp:std::cout<<" >= ";break;
        case LessEqualOp:std::cout<<" <= ";break;
        case GreaterOp:std::cout<<" > ";break;
        case LessOp:std::cout<<" < ";break;
        case NotEqualOp:std::cout<<" != ";break;
        case NotOp:std::cout<<" ! ";break;
    }
    std::cout<<"\n";
    indexSpace--;
}

void seeStatement(StatementStruct* ptr);
void seeFunction(StatementStruct* ptr);

void seeIdentifier(std::string identifier)
{
    showSpaces("Identifier : ");
    indexSpace--;
    showSpaces("");
    std::cout<<identifier<<"\n";
    indexSpace--;
}

void seeNumber(std::string number)
{
    showSpaces("Number : ");
    indexSpace--;
    showSpaces("");
    std::cout<<number<<"\n";
    indexSpace--;
}



void seeExpression(ExprStruct* expr)
{
        
        showSpaces("Expression");
        if(expr->type==NodeNumericLiteral)
            seeNumber(((NumericLiteralStruct*)expr)->value);
        else if(expr->type==NodeIdentifier)
            seeIdentifier(((IdentifierStruct*)expr)->value);
        else if(expr->type==NodeFunction)
            seeFunction((StatementStruct*)expr);

        else if(expr->type==NodeBinaryExpr)
        {
        BinaryExprStruct* biexpr=(BinaryExprStruct*)expr;

            seeExpression(biexpr->left);
            seeOperator(biexpr->op);
            seeExpression(biexpr->right);
        }

        indexSpace--;
}

void seeDeclareArguments(DeclareArgumentsStruct* arg)
{
    showSpaces("DeclareArguments");
    for(auto s: arg->statements){
        seeStatement(s);
    }
    indexSpace--;
}

void seeArguments(ArgumentsStruct* arg)
{
    showSpaces("Arguments");
    for(auto e: arg->expressions)
    {
        seeExpression(e);
    }
    indexSpace--;
}


void seeBlock(BlockStruct* b)
{
    showSpaces("Block");
    for(auto s: b->statements){
        seeStatement(s);
    }
    indexSpace--;
}


void seeDeclareFunction(StatementStruct* ptr)
{
    showSpaces("DeclareFunction");
    DeclareFunctionStruct* fn=(DeclareFunctionStruct*)ptr;
    seeDatatype(fn->dtype);
    seeIdentifier(fn->identifier);
    seeDeclareArguments(fn->args);
    seeBlock(fn->block);
    indexSpace--;
}

void seeFunction(StatementStruct* ptr)
{
    showSpaces("Function");
    FunctionStruct* fn=(FunctionStruct*)ptr;
    seeIdentifier(fn->identifier);
    seeArguments(fn->args);
    indexSpace--;
}

void seeAssignmentVar(StatementStruct* ptr)
{
    showSpaces("AssignVar");
    AssignmentVarStruct *var =(AssignmentVarStruct*)ptr;
    seeIdentifier(var->identifier);
    seeExpression(var->expression);

    indexSpace--;
}


void seeDeclareVar(StatementStruct* ptr)
{
    showSpaces("DeclareVar");
    DeclareVarStruct *var =(DeclareVarStruct*)ptr;
    seeDatatype(var->dtype);
    seeIdentifier(var->identifier);

    if(var->expr!=nullptr)
        seeAssignmentVar(var->expr);

    indexSpace--;
}


void seeReturn(StatementStruct* ptr)
{
    showSpaces("Return");
    ReturnStruct *rt =(ReturnStruct*)ptr;
    seeExpression(rt->expression);

    indexSpace--;
}

void seeIf(StatementStruct* ptr)
{
    showSpaces("If");
    IfStruct* f=(IfStruct*)ptr;
    seeExpression(f->expression);
    seeBlock(f->block);
    indexSpace--;
}

void seeWhile(StatementStruct* ptr)
{
    showSpaces("While");
    WhileStruct* f=(WhileStruct*)ptr;
    seeExpression(f->expression);
    seeBlock(f->block);
    indexSpace--;
}




void seeStatement(StatementStruct* ptr)
{
    if(ptr->type==NodeDeclareFunction){seeDeclareFunction(ptr);}
    if(ptr->type==NodeFunction){seeFunction(ptr);}
    if(ptr->type==NodeDeclareVar){seeDeclareVar(ptr);}
    if(ptr->type==NodeAssignVar){seeAssignmentVar(ptr);}
    if(ptr->type==NodeReturn){seeReturn(ptr);}
    if(ptr->type==NodeIf){seeIf(ptr);}
    if(ptr->type==NodeWhile){seeWhile(ptr);}
}


void seeProgram(StatementStruct* ptr)
{
    ProgramStruct* p=(ProgramStruct*)ptr;
    for(auto s: p->statements)
        seeStatement(s);
}

struct Variable
{
    std::string Identifier;
    Datatype dtype;
    size_t offset;
    int scope;
};

#define A 0
#define B 1
class CodeGenerator
{
    private:
        std::string assemblyString;
        std::vector<Variable> Vars;
        int scopeOnStruct=0;    
    public:

    CodeGenerator()
    {
        this->assemblyString="init: \n jmp #main\n";
    }

    Variable seeVar(std::string identifier)
    {
        for(auto v:Vars)
        {
            if(identifier==v.Identifier&&v.scope<=this->scopeOnStruct){
                std::cout<<"identifier leido: "<<v.Identifier<<" "<<v.offset<<"\n";
            return v;}

        }
        return Vars[0];
    }

    void addOffsetVars(size_t offset)
    {
        for ( auto it = Vars.begin(); it != Vars.end() - 1; ++it) {
            auto& v = *it;
            
            v.offset+=offset;
//            std::cout<<v.Identifier<<v.offset<<" ";
        }

//        std::cout<<"\n";
    }

    void genExpression(StatementStruct* ptr,std::string AorB)
    {
        if(ptr->type==NodeNumericLiteral)
        {
            NumericLiteralStruct* num=(NumericLiteralStruct*)ptr;
            this->assemblyString+=" ld "+AorB+"#"+num->value+"\n";
            return;
        }
        if(ptr->type==NodeIdentifier)
        {
            IdentifierStruct* id=(IdentifierStruct*)ptr;
            this->assemblyString+=" add FP,SP#"+std::to_string(seeVar(id->value).offset)+"\n ld "+AorB+"[FP]\n";
        }
        if(ptr->type==NodeBinaryExpr)
        {
            BinaryExprStruct* biexpr=(BinaryExprStruct*)ptr;
            this->genExpression(biexpr->left,"A");
            this->genExpression(biexpr->right,"B");
            if(biexpr->op==PlusOp)
            {
                this->assemblyString+=" add "+AorB+"\n";
            }
            if(biexpr->op==MinusOp)
            {
                this->assemblyString+=" sub "+AorB+"\n";
            }
        }
        std::cout<<"Type:"<<ptr->type<<"\n";
    }

    void genAssignVar(StatementStruct* ptr)
    {
        AssignmentVarStruct* var=(AssignmentVarStruct*)ptr;
        Variable v=seeVar(var->identifier);
        this->genExpression((StatementStruct*)var->expression,"A");
        this->assemblyString+=" add FP,SP#"+std::to_string(v.offset)+"\n st A[FP]\n";

    }

    void genDeclareVar(StatementStruct* ptr)
    {
        DeclareVarStruct* var=(DeclareVarStruct*)ptr;
        this->assemblyString+=" sub SP#"+std::to_string(sizeOfDataType(var->dtype))+"\n";
        Vars.push_back({var->identifier,var->dtype,0,this->scopeOnStruct});
        addOffsetVars(sizeOfDataType(var->dtype));
        if(var->expr!=nullptr){
            this->genAssignVar((StatementStruct*)var->expr);
        }

    }

    void genDeclareFunction(StatementStruct* ptr)
    {
        DeclareFunctionStruct* fn=(DeclareFunctionStruct*)ptr;
        this->assemblyString+=fn->identifier+":\n";
        this->assemblyString+=" mov FP,A\n psh A\n mov SP,FP\n";

        int size=0;
        for(auto a:fn->args->statements)
        {
            DeclareVarStruct* deV=(DeclareVarStruct*)a;
            size+=sizeOfDataType(deV->dtype);
        }
            
            this->genBlock(fn->block);
        
    }

    void genStatement(StatementStruct* ptr)
    {
    if(ptr->type==NodeDeclareFunction){this->genDeclareFunction(ptr);}
    if(ptr->type==NodeFunction){this->assemblyString+="FUNCTION\n";}//seeFunction(ptr);}
    if(ptr->type==NodeDeclareVar){this->genDeclareVar(ptr);}
    if(ptr->type==NodeAssignVar){this->genAssignVar(ptr);}
    if(ptr->type==NodeReturn){this->assemblyString+="RETURN\n";}//seeReturn(ptr);}
    if(ptr->type==NodeIf){this->assemblyString+="IF\n";}//seeIf(ptr);}
    if(ptr->type==NodeWhile){this->assemblyString+="WHILE\n";}//seeWhile(ptr);}

    }
    
    void genBlock(StatementStruct* ptr)
    {
        this->scopeOnStruct++;
        BlockStruct* p=(BlockStruct*)ptr;
        for(auto s: p->statements)
            this->genStatement(s);
        this->scopeOnStruct--;
    }   


    void genProgram(StatementStruct* ptr)
    {
        ProgramStruct* p=(ProgramStruct*)ptr;
        for(auto s: p->statements)
            this->genStatement(s);

    }

    std::string gen(StatementStruct* ptr){
        this->genProgram(ptr);
        return this->assemblyString;
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

        StatementStruct *ptr=Parser.generateAbstractSyntaxTree();

        //seeProgram(ptr);

        CodeGenerator CodeGenerator;

        std::cout<<CodeGenerator.gen(ptr);
        /*for(auto t:tk){
        std::cout<<t.TK<<" "<<t.value<<"\n";
        }*/

              

}
