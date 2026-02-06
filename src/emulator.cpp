#include <iostream>
#include <fstream>
#include <conio.h>
#include <iomanip>
/*
#define DEBUG0
#define DEBUG1
#define DEBUG2
*/
#define NOFLG 0b001000000000000000000000
#define XI    0b000100000000000000000000
#define XO    0b000010000000000000000000
#define FPI   0b000001000000000000000000
#define SPI   0b000000100000000000000000
#define CI    0b000000010000000000000000

#define SPNOT 0b000000001000000000000000  // 0
#define ME    0b000000000100000000000000  // 1
#define MI    0b000000000010000000000000  // 1
#define RI    0b000000000001000000000000  // 1
#define II    0b000000000000100000000000  // 1
#define PE    0b000000000000010000000000  // 1
#define PI    0b000000000000001000000000  // 1
#define RS    0b000000000000000100000000  // 1

#define A3    0b000000000000000010000000 // 1
#define A2    0b000000000000000001000000 // 1
#define A1    0b000000000000000000100000 // 0
#define A0    0b000000000000000000010000 // 1
#define HI    0b000000000000000000001000 // 1
#define SPACT 0b000000000000000000000100 // 1
#define BI    0b000000000000000000000010 // 1
#define AI    0b000000000000000000000001 // 0

#define ADD A0
#define AND A1
#define OR A0|A1
#define XOR A2
#define SUB A0|A2
#define SPO A1|A2
#define BO A0|A1|A2
#define AO A3
#define RO A0|A3
#define PO A1|A3
#define ADC A0|A1|A3
#define CO A2|A3
#define SHL A0|A2|A3
#define SHR A1|A2|A3
#define FPO A0|A1|A2|A3


#define PIH PI | HI
#define PIL PI
#define POH PO | HI
#define POL PO
#define MIH MI | HI
#define MIL MI

struct MicrocodeSignalStruct
{
        std::string name;
        unsigned int signal;
        unsigned int notsignal;
};
typedef unsigned char Byte;
typedef unsigned short Word;

struct cpuState
{
        Word Bus;
        Byte A, B, C, X;
        Word PC, SP, FP, IR, MR;
        Byte MicroStep;
        Byte ZeroF, CarryF;
        Byte RAM[65536];
};
cpuState CPUSTATE = {0, 0, 0, 0, 0};

MicrocodeSignalStruct Signals[] = {
    {"HI", HI},
    {"SPNOT", SPNOT},
    {"ME", ME},
    {"RI", RI},
    {"II", II},
    {"PE", PE},
    {"RS", RS},
    {"SPI",SPI},
    {"FPI",FPI},
    {"SPACT", SPACT},
    {"AI", AI},
    {"BI", BI},
    {"ADD", ADD, A1 | A2 | A3},
    {"AND", AND, A0 | A2 | A3},
    {"OR", OR, A2 | A3},
    {"XOR", XOR, A0 | A1 | A3},
    {"SUB", SUB, A1 | A3},
    {"SPO", SPO, A0 | A3},
    {"BO", BO, A3},
    {"AO", AO, A0 | A1 | A2},
    {"RO", RO, A1 | A2},
    {"ADC", ADC, A2},
    {"CO", CO, A0 | A1},
    {"CI", CI},
    {"SHL", SHL, A1},
    {"SHR", SHR, A0},
    {"FPO", FPO},
    {"PIH", PI | HI},
    {"PIL", PI, HI},
    {"POH", PO | HI,A0|A2},
    {"POL", PO, HI|A0|A2},
    {"MIH", MI | HI},
    {"MIL", MI, HI},
    {"XI", XI},
    {"XO", XO},
    {"NOFLG",NOFLG},
};

unsigned int mCode[16384];

void showMicro(unsigned int signals)
{
        std::cout << "( ";
        for (auto sig : Signals)
        {
                if ((signals & sig.signal) != sig.signal)
                        continue;
                else if ((signals & sig.notsignal) != 0)
                        continue;
                else
                        std::cout << sig.name << " ";
        }
        std::cout << ")";
}

bool isSignal(std::string name, unsigned int signals)
{
        for (auto sig : Signals)
        {
                if (name != sig.name)
                        continue;
                if ((signals & sig.signal) != sig.signal)
                        continue;
                else if ((signals & sig.notsignal) != 0)
                        continue;
                else
                        return true;
        }
        return false;
}

void sendByte(Byte data) { std::cout << data; }
Byte recvByte()
{       
/*#ifndef DEBUG2
        if (kbhit())
        {
                char a = getch();
                if (a == 0xD)
                        a = 0xA;
                return a;
        }
        return 0;
#else*/ 
            char a=getch();
            return a;
/*#endif*/
}

bool enableFlags=true;

void executeMicroRising(unsigned int signal)
{
        if (isSignal("SPACT", signal))
        {
                CPUSTATE.SP -= 1;
                if (isSignal("SPNOT", signal))
                {
                        CPUSTATE.SP += 2;
                }
                CPUSTATE.SP = CPUSTATE.SP | 0xff00;
        }
        if (isSignal("SPI", signal)){

                CPUSTATE.SP = CPUSTATE.Bus;
                CPUSTATE.SP = CPUSTATE.SP | 0xff00;
        }
        if (isSignal("FPI", signal)){

                CPUSTATE.FP = CPUSTATE.Bus;
                CPUSTATE.FP = CPUSTATE.FP | 0xff00;
        }
        if (isSignal("AI", signal))
                CPUSTATE.A = CPUSTATE.Bus;
        if (isSignal("XI", signal))
                CPUSTATE.X = CPUSTATE.Bus;
        if (isSignal("BI", signal))
                CPUSTATE.B = CPUSTATE.Bus;
        if (isSignal("CI", signal))
                CPUSTATE.C = CPUSTATE.Bus;
        if (isSignal("RI", signal))
        {
                CPUSTATE.RAM[CPUSTATE.MR] = CPUSTATE.Bus;
                if (CPUSTATE.MR == 0x2000)
                {
                        sendByte(CPUSTATE.Bus);
                }

        }
        if (isSignal("II", signal))
                CPUSTATE.IR = CPUSTATE.Bus;
        if (isSignal("MIH", signal))
                CPUSTATE.MR = (CPUSTATE.MR & 0xFF) | (CPUSTATE.Bus << 8);
        if (isSignal("MIL", signal))
                CPUSTATE.MR = (CPUSTATE.MR & 0xFF00) | (CPUSTATE.Bus);
        if (isSignal("PIH", signal))
                CPUSTATE.PC = (CPUSTATE.PC & 0xFF) | (CPUSTATE.Bus << 8);
        if (isSignal("PIL", signal))
                CPUSTATE.PC = (CPUSTATE.PC & 0xFF00) | (CPUSTATE.Bus);

        if (isSignal("ME", signal))
                CPUSTATE.MR++;
        if (isSignal("PE", signal))
                CPUSTATE.PC++;

        if (isSignal("RS", signal))
                CPUSTATE.MicroStep = 17;
}

void setFlags()
{
        if(enableFlags){
        CPUSTATE.ZeroF = ((CPUSTATE.Bus & 0xFF) == 0);
        CPUSTATE.CarryF = ((CPUSTATE.Bus & 0x100) == 0x100);}
        CPUSTATE.Bus = CPUSTATE.Bus & 0xFF;
        enableFlags=true;
}

void executeMicroFalling(unsigned int signal)
{
        CPUSTATE.Bus = 0;
        if (isSignal("SPO", signal))
        {
                CPUSTATE.Bus = (CPUSTATE.SP & 0xff);
                if (isSignal("HI", signal))
                        CPUSTATE.Bus = ((CPUSTATE.SP >> 8) & 0xff);
        }
        if (isSignal("FPO", signal))
        {
                CPUSTATE.Bus = (CPUSTATE.FP & 0xff);
                if (isSignal("HI", signal))
                        CPUSTATE.Bus = ((CPUSTATE.FP >> 8) & 0xff);
        }
        if (isSignal("AO", signal))
                CPUSTATE.Bus = CPUSTATE.A;
        if (isSignal("BO", signal))
                CPUSTATE.Bus = CPUSTATE.B;
        if (isSignal("CO", signal))
                CPUSTATE.Bus = CPUSTATE.C;
        if (isSignal("XO", signal))
                CPUSTATE.Bus = CPUSTATE.X;
        if (isSignal("RO", signal))
        {
                CPUSTATE.Bus = CPUSTATE.RAM[CPUSTATE.MR];
                if (CPUSTATE.MR == 0x2000)
                {
                        CPUSTATE.Bus = recvByte();
                }
        }
        if(isSignal("NOFLG",signal))
            enableFlags=false;
        if (isSignal("POH", signal))
                CPUSTATE.Bus = ((CPUSTATE.PC >> 8) & 0xFF);
        if (isSignal("POL", signal))
                CPUSTATE.Bus = (CPUSTATE.PC & 0xFF);

        if (isSignal("ADD", signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A + CPUSTATE.B);
                setFlags();
        }
        if (isSignal("SUB", signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A + (255 - CPUSTATE.B) + 1);
                // std::cout << "SUB is " << static_cast<unsigned int>((CPUSTATE.Bus)) << " ";
                setFlags();
        }
        if (isSignal("AND", signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A & CPUSTATE.B);
                setFlags();
        }
        if (isSignal("OR", signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A | CPUSTATE.B);
                setFlags();
        }
        if (isSignal("XOR", signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A ^ CPUSTATE.B);
                setFlags();
        }
        if (isSignal("ADC", signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A + CPUSTATE.B + CPUSTATE.CarryF);
                setFlags();
        }
        if (isSignal("SHL", signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A << 1);
                setFlags();
        }
        if (isSignal("SHR", signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A >> 1);
                setFlags();
        }
}

int main(int argc, char **argv)
{
        /*if (argc != 2)
        {
                std::cout << "emulator <filename>\n";
                return 1;
        }*/

        unsigned char *rMCode1 = reinterpret_cast<unsigned char *>(malloc(16384)),
                      *rMCode2 = reinterpret_cast<unsigned char *>(malloc(16384)),
                      *rMCode3 = reinterpret_cast<unsigned char *>(malloc(16384));
        
        std::ifstream microcodeR1, microcodeR2, microcodeR3;
        microcodeR1.open("microcode1.bin", std::ios::binary);
        microcodeR2.open("microcode2.bin", std::ios::binary);
        microcodeR3.open("microcode3.bin", std::ios::binary);

        microcodeR1.read(reinterpret_cast<char *>(rMCode1), 16384);
        microcodeR2.read(reinterpret_cast<char *>(rMCode2), 16384);
        microcodeR3.read(reinterpret_cast<char *>(rMCode3), 16384);
        int i = 0, j;

        for (; i < 16384; i++)
        {
                mCode[i] = rMCode1[i] | (rMCode2[i] << 8) | (rMCode3[i]<<16);
        }
        free(rMCode1);
        free(rMCode2);
        free(rMCode3);

        std::ifstream file("RAM.bin", std::ios::binary);
        file.seekg(0, file.end);
        size_t length = file.tellg();
        char *code = new char[length + 1];
        file.seekg(0, file.beg);
        file.read(code, length);

        for (i = 0; i < length; i++)
        {
                CPUSTATE.RAM[i] = code[i];
        }

        CPUSTATE.PC = 0;
        CPUSTATE.MR = 0;
        CPUSTATE.IR = 0;
        CPUSTATE.SP = 0xff00;
        CPUSTATE.FP = 0xff00;
        for (; 1;)
        {
                int c;
                for (CPUSTATE.MicroStep = 0; CPUSTATE.MicroStep < 16; CPUSTATE.MicroStep++)
                {
#ifdef DEBUG0

                        c=getch();
                        // if(CPUSTATE.IR==118)
                        showMicro(mCode[CPUSTATE.IR * 16 + CPUSTATE.MicroStep + CPUSTATE.CarryF * 4096 + CPUSTATE.ZeroF * 8192]);
#endif
                        executeMicroFalling(mCode[CPUSTATE.IR * 16 + CPUSTATE.MicroStep + CPUSTATE.CarryF * 4096 + CPUSTATE.ZeroF * 8192]);
                        executeMicroRising(mCode[CPUSTATE.IR * 16 + CPUSTATE.MicroStep + CPUSTATE.CarryF * 4096 + CPUSTATE.ZeroF * 8192]);
                }
#ifdef DEBUG1
                std::cout <<std::hex<< "\nIR:" << static_cast<int>(CPUSTATE.IR) << " A:" << static_cast<int>(CPUSTATE.A)
                          << " B:" << static_cast<int>(CPUSTATE.B) << " Z:" << static_cast<int>(CPUSTATE.ZeroF)
                          << " C:" << static_cast<int>(CPUSTATE.CarryF) << " PC:" << static_cast<int>(CPUSTATE.PC)
                          << " SP:" << static_cast<int>(CPUSTATE.SP) <<"\n"
                          << " FP:" << static_cast<int>(CPUSTATE.FP) <<"\n";
                        if(CPUSTATE.SP>=0xfff0){
                            int s=CPUSTATE.SP-0xfff0;
                            for(i=0;i<s;i++)std::cout<<"   ";
                            std::cout<<"P\n";
                        }
                        if(CPUSTATE.FP>=0xfff0){
                            int s=CPUSTATE.FP-0xfff0;
                            for(i=0;i<s;i++)std::cout<<"   ";
                            std::cout<<"F\n";
                        }
                for(i=0;i<16;i++){
                        std::cout<<std::hex<<std::setw(2)<<std::setfill('0')<<static_cast<int>(CPUSTATE.RAM[i+0xfff0])<<" ";
                }
                std::cout<<std::dec<< "\n";
               

#endif
                if(CPUSTATE.IR==0x96||c==27)
                    break;
        }
}
// 8100:03 19 81 5B 00 68 16 81 07 00 20 02 19 81 14 01 07 19 81 62 00 81 62 00 00 1B 81 68 6F 6C 61 20 42 55 45 4E 41 53 20 74 61 72 64 65 73 0A
