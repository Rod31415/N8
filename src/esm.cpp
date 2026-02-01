#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <algorithm>

#define noToken 32
#define noReturn 0x10000

#define endLabel ':'
#define lineBreak '\n'

struct Instruction
{
        std::string mnem;
        int operands;
};

struct MemInst
{
        int opcode;
        std::string operand;
        bool msb;
        int add;
        int lineDefined;
};

struct Label
{
        std::string name;
        int index;
        int scopeindex;
        bool global;
};

MemInst MemInsts[65536];
size_t InstIndex = 0;

Label Labels[65536];
size_t LabelIndex = 0;
size_t LabelScopeIndex = 0;

Instruction Instructions[] = {
{"NOP",0},
{"LD A#",1},
{"LD A&",2},
{"LD A%",2},
{"LD B#",1},
{"LD B&",2},
{"LD B%",2},
{"ST A&",2},
{"ST A%",2},
{"ST B&",2},
{"ST B%",2},
{"ST &SP&",2},
{"STW &SP&",2},
{"MOV A,B",0},
{"MOV B,A",0},
{"MOVH SP,A",0},
{"MOV SP,A",0},
{"MOVH SP,B",0},
{"MOV SP,B",0},
{"MOV &SP,A",0},
{"MOV &SP,B",0},
{"MOVH &SP,AB",0},
{"GET AB",0},
{"ADD &",2},
{"ADD A#",1},
{"ADD A&",2},
{"ADD A%",2},
{"ADD A",0},
{"ADD B#",1},
{"ADD B&",2},
{"ADD B%",2},
{"ADD B",0},
{"ADC &",2},
{"ADC A#",1},
{"ADC A&",2},
{"ADC A%",2},
{"ADC A",0},
{"ADC B#",1},
{"ADC B&",2},
{"ADC B%",2},
{"ADC B",0},
{"SUB &",2},
{"SUB A#",1},
{"SUB A&",2},
{"SUB A%",2},
{"SUB A",0},
{"SUB B#",1},
{"SUB B&",2},
{"SUB B%",2},
{"SUB B",0},
{"AND &",2},
{"AND A#",1},
{"AND A&",2},
{"AND A%",2},
{"AND A",0},
{"AND B#",1},
{"AND B&",2},
{"AND B%",2},
{"AND B",0},
{"OR &",2},
{"OR A#",1},
{"OR A&",2},
{"OR A%",2},
{"OR A",0},
{"OR B#",1},
{"OR B&",2},
{"OR B%",2},
{"OR B",0},
{"XOR &",2},
{"XOR A#",1},
{"XOR A&",2},
{"XOR A%",2},
{"XOR A",0},
{"XOR B#",1},
{"XOR B&",2},
{"XOR B%",2},
{"XOR B",0},
{"SHL4 &",2},
{"SHL4 A&",2},
{"SHL4",0},
{"SHL2 &",2},
{"SHL2 A&",2},
{"SHL2",0},
{"SHL &",2},
{"SHL A&",2},
{"SHL",0},
{"SHR4 &",2},
{"SHR4 A&",2},
{"SHR4",0},
{"SHR2 &",2},
{"SHR2 A&",2},
{"SHR2",0},
{"SHR &",2},
{"SHR A&",2},
{"SHR",0},
{"CMP A#",1},
{"CMP A&",2},
{"CMP A%",2},
{"CMP B#",1},
{"CMP B&",2},
{"CMP B%",2},
{"CMP",0},
{"JMP #",2},
{"JMP &",2},
{"JMP %",2},
{"JPC #",2},
{"JPC &",2},
{"JPC %",2},
{"JPZ #",2},
{"JPZ &",2},
{"JPZ %",2},
{"JNC #",2},
{"JNC &",2},
{"JNC %",2},
{"JNZ #",2},
{"JNZ &",2},
{"JNZ %",2},
{"JSR #",2},
{"BEQ #",2},
{"BNE #",2},
{"BCS #",2},
{"BCC #",2},
{"RTS",0},
{"PSH A",0},
{"PSH B",0},
{"PSH SP",0},
{"PSHB #",1},
{"PSHB &",2},
{"PSHW #",2},
{"PSHW &",2},
{"POP A",0},
{"POP B",0},
{"POPW AB",0},
{"POPB &",2},
{"POPW &",2},
{"INC A",0},
{"INC SP",0},
{"INCW SP",0},
{"INCD SP",0},
{"DEC SP",0},
{"DECW SP",0},
{"DECD SP",0},




};

enum TokenType
{
        INST,
        LABEL,
        VALUE,
        MSB,
        LSB,
        ADD,
        SUB
};

struct TokenStruct
{
        std::string value;
        TokenType type;
        int data;
};

int ADDR[65536];
size_t addrIndex = 0;

int detectInstructions(std::string aux, char c)
{
        size_t pos = aux.find_first_not_of(' ');
        if (pos == std::string::npos)
                return -1;
        aux = aux.substr(pos, aux.length());
        std::transform(aux.begin(), aux.end(), aux.begin(),
                       [](unsigned char c)
                       { return std::toupper(c); });
        // printf("|%d|%s|\n",c,aux.c_str());
        int index = 0;
        for (auto s : Instructions)
        {
                if (aux.find(s.mnem) != std::string::npos)
                {
                        if (c != '\n')
                                for (auto ns : Instructions)
                                {

                                        if (ns.mnem.find(aux) != std::string::npos && s.mnem.compare(ns.mnem))
                                        {
                                                return -1;
                                        }
                                }

                        return index;
                }
                index++;
        }
        return -1;
}
int scopeInstruction = 0;
int setindexLabel = 0;
int setScope(int index)
{
        int scope = 0;
        for (size_t i = setindexLabel; i < LabelIndex; i++)
        {
                if (index >= Labels[i].index)
                {
                        scope = Labels[i].scopeindex;
                        setindexLabel = i;
                }
        }
        return scope;
}

int detectLabel(std::string label)
{
        int index = 0;

        for (auto l : Labels)
        {
                if (label.find(l.name) != std::string::npos) //&&( scopeInstruction == Labels[index].scopeindex||Labels[index].global))
                {

                        if (l.name == "")
                                return -1;
                        if (scopeInstruction == l.scopeindex || l.global)
                                return Labels[index].index;
                }
                index++;
        }
        return -1;
}

enum TokenizeState
{
        NORMAL = 0,
        OPERAND,
        COMMENT,
        ORG,
        DB,
        STRING,
        RES,
        PLUS,
        MINUS
};

int getValue(std::string op)
{
        char *pd, *ph;
        strtol(op.c_str(), &pd, 10);
        strtol(op.c_str(), &ph, 0);
        // printf("P:%d|%d|%s|\n", *pd,*ph, op.c_str());
        if (op.c_str()[0] == '\'')
        {

                if (op.c_str()[1] == '\'')
                {
                        return 32;
                }
                return op.c_str()[1];
        }
        else if ((ph == op.c_str() || *ph != 0) && (pd == op.c_str() || *pd != 0))
        {

                return noReturn;
        }
        else if (op.find("0x") != std::string::npos)
        {
                return std::stoi(op.substr(2, op.length()), 0, 16);
        }
        else
        {
                return std::stoi(op);
        }
}

int lineIndex = 0;

void Tokenize(std::string buffer)
{
        std::string aux = "";
        TokenizeState state = NORMAL;
        int op = -1, byteIndex = 0;
        for (auto c : buffer)
        {
                if (c == '\n')
                        lineIndex++;
                switch (state)
                {

                case NORMAL:

                        op = detectInstructions(aux, c);

                        if (op > -1)
                        {

                                MemInsts[InstIndex].opcode = op;
                                MemInsts[InstIndex].lineDefined = lineIndex;
                                state = OPERAND;
                                if (Instructions[op].operands == 0)
                                {
                                        InstIndex++;
                                        state = NORMAL;
                                }
                                byteIndex += Instructions[op].operands + 1;

                                aux = "";
                        }

                        if (c == ':')
                        {
                                Labels[LabelIndex].name = aux.substr(0, aux.length());
                                Labels[LabelIndex].index = byteIndex;
                                Labels[LabelIndex].global = true;
                                Labels[LabelIndex++].scopeindex = LabelScopeIndex;
                                if (Labels[LabelIndex - 1].name.find(".") != std::string::npos)
                                {
                                        Labels[LabelIndex - 1].scopeindex = --LabelScopeIndex;
                                        Labels[LabelIndex - 1].global = false;
                                }
                                LabelScopeIndex++;
                                // printf("Label|%s|%d|\n",Labels[LabelIndex-1].name.c_str(),byteIndex);
                                aux = "";
                                break;
                        }
                        if (c == ';')
                        {
                                state = COMMENT;
                        }
                        if (aux.find(".org ") != std::string::npos)
                        {
                                state = ORG;
                                aux = "";
                        }
                        if (aux.find(".db ") != std::string::npos)
                        {
                                state = DB;
                                aux = "";
                        }
                        if (aux.find(".string ") != std::string::npos)
                        {
                                state = STRING;
                                aux = "";
                        }
                        if (aux.find(".res ") != std::string::npos)
                        {
                                state = RES;
                                aux = "";
                        }
                        if (aux.find(".addr") != std::string::npos)
                        {

                                ADDR[addrIndex++] = byteIndex;
                                aux = "";
                                break;
                        }

                        if (c == '\n' || c == ';')
                        {
                                aux = "";
                                break;
                        }

                        aux += c;
                        break;
                case OPERAND:

                        if (c == '\n' || c == '+' || c == '-' || c == ';')
                        {
                                MemInsts[InstIndex].msb = false;
                                if (aux.find(">") != std::string::npos)
                                        MemInsts[InstIndex].msb = true;

                                MemInsts[InstIndex].add = noReturn;
                                aux.erase(remove_if(aux.begin(), aux.end(), isspace), aux.end());

                                MemInsts[InstIndex++].operand = aux;
                                state = NORMAL;
                                if (c == '+')
                                        state = PLUS;
                                else if (c == '-')
                                        state = MINUS;
                                aux = "";
                                break;
                        }
                        aux += c;
                        break;
                case PLUS:
                        if (c == '\n' || c == ';')
                        {
                                aux.erase(remove_if(aux.begin(), aux.end(), isspace), aux.end());
                                MemInsts[InstIndex - 1].add = std::stoi(aux);
                                state = NORMAL;
                                aux = "";
                                break;
                        }
                        aux += c;
                        break;
                case MINUS:
                        if (c == '\n' || c == ';')
                        {
                                aux.erase(remove_if(aux.begin(), aux.end(), isspace), aux.end());
                                MemInsts[InstIndex - 1].add = -std::stoi(aux);
                                state = NORMAL;
                                aux = "";
                                break;
                        }
                        aux += c;
                        break;
                case COMMENT:
                        if (c == '\n')
                                state = NORMAL;
                        break;
                case ORG:
                        if (c == '\n')
                        {

                                InstIndex = getValue(aux) - byteIndex + InstIndex;
                                byteIndex = getValue(aux);
                                // printf("ORG VALUE |%04x|\n", InstIndex);
                                state = NORMAL;
                                aux = "";
                                break;
                        }
                        aux += c;
                        break;
                case DB:
                        if (c == '\"')
                        {
                                state=STRING;
                                aux="";
                                break;
                        }
                        else if (c == '\n' || c == ',')
                        {

                                MemInsts[InstIndex].opcode = -2;
                                MemInsts[InstIndex++].operand = aux;
                                // printf("|%s|", aux.c_str());
                                state = NORMAL;
                                aux = "";
                                byteIndex++;
                                if (c == ',')
                                {
                                        state = DB;
                                }

                                break;
                        }
                        aux += c;
                        break;
                case STRING:

                        if (c == '\"')
                        {
                                break;
                        }
                        if (c != ','&&c!='\n')
                        {
                                MemInsts[InstIndex].opcode = -2;
                                aux = "'";
                                aux += c;
                                aux += "'";

                                MemInsts[InstIndex++].operand = aux;
                                byteIndex++;
                        }
                        else if (c == ',')
                        {
                                state = DB;
                                aux = "";
                        }
                        else
                        {
                                state = NORMAL;
                                aux = "";
                        }
                        printf("|%s", aux.c_str());
                        break;
                case RES:
                        //printf("#%s",aux.c_str());
                        if (c == '\n')
                        {
                                printf("ORG VALUE |%04x|\n", InstIndex);
                                 printf("BYT VALUE |%04x|\n", byteIndex);
                                InstIndex += getValue(aux);
                                byteIndex += getValue(aux);
                                 printf("ORG VALUE |%04x|\n", InstIndex);
                                 printf("BYT VALUE |%04x|\n", byteIndex);
                                state = NORMAL;
                                aux = "";
                                break;
                        }
                        aux += c;
                        break;
                }
        }
}
bool onFile = true;
int main(int argc, char **argv)
{

        if (argc == 1)
        {
                std::cout << "esm <filename> <options>";
                return 1;
        }

        std::ifstream file(argv[1]);
        if (argc == 3)
        {
                onFile = false;
        }

        std::string filebuffer = "", line;

        while (std::getline(file, line))
        {
                filebuffer += line;
                filebuffer += "\n";
        }

        for (int i = 0; i < 65536; i++)
        {
                MemInsts[i].opcode = 0;
        }

        Tokenize(filebuffer);
        std::cout << "\nLabels:\n";
        for (size_t i = 0; i < LabelIndex; i++)
        {
                std::cout << Labels[i].name << " - " << std::hex << Labels[i].index << " - " << Labels[i].scopeindex << " - " << Labels[i].global << "\n";
        }
        std::cout << "\nInstructions:\n";

        std::ofstream fileOut;
        if (onFile)
                fileOut.open("RAM.bin", std::ios::binary);
        int indexByte = 0;
        for (size_t i = 0; i < InstIndex; i++)
        {

                scopeInstruction = setScope(indexByte);
                int operand = detectLabel(MemInsts[i].operand);
                // if(Instructions[MemInsts[i].opcode].operands != 0)
                // printf("|op:%s- %d|",MemInsts[i].operand.c_str(),operand);
                if (operand == -1 && (Instructions[MemInsts[i].opcode].operands != 0 || MemInsts[i].opcode == -2))
                {
                        // printf(" / %d %d / ",Instructions[MemInsts[i].opcode].operands,MemInsts[i].opcode);
                        operand = getValue(MemInsts[i].operand);
                        if (operand == noReturn)
                        {
                                printf("<Syntax Error>Label not defined on line %d \"%s\"\n", MemInsts[i].lineDefined + 1, MemInsts[i].operand.c_str());
                                return 0;
                        }
                }
                if (MemInsts[i].add != noReturn)
                {
                        operand += MemInsts[i].add;
                }
                unsigned int newwrite;
                 
                 //printf(" %04x :", i);

                if (MemInsts[i].opcode != -2)
                {
                        indexByte++;

                        newwrite = MemInsts[i].opcode & 255;

                        /*if (MemInsts[i].opcode != 0)
                                printf(" %s - %02x", Instructions[MemInsts[i].opcode].mnem.c_str(), newwrite);
                        */
                        if (MemInsts[i].opcode != 0)
                                printf(" %02X", newwrite);
                        if (onFile)
                                fileOut.write(reinterpret_cast<char *>(&newwrite), 1);
                }
                else
                {
                        // printf(" DB - ");
                }

                if (Instructions[MemInsts[i].opcode].operands >= 1 || MemInsts[i].opcode == -2)
                {

                        // if (MemInsts[i].opcode != 0)

                        newwrite = operand & 255;
                        if (MemInsts[i].msb)
                                newwrite = (operand >> 8) & 255;

                        printf(" %02X", newwrite);
                        if (onFile)
                                fileOut.write(reinterpret_cast<char *>(&newwrite), 1);
                        indexByte++;
                }
                if (Instructions[MemInsts[i].opcode].operands == 2)
                {

                        // if (MemInsts[i].opcode != 0)
                        newwrite = (operand >> 8) & 255;

                        printf(" %02X", newwrite);
                        if (onFile)
                                fileOut.write(reinterpret_cast<char *>(&newwrite), 1);
                        indexByte++;
                }

                // if (MemInsts[i].opcode != 0)
                 //printf("\n");
        }
}
