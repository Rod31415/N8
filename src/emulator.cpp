#include <iostream>
#include <fstream>
#include <conio.h>

#define SPNOT 0b1000000000000000 // 0
#define ME 0b0100000000000000    // 1
#define MI 0b0010000000000000    // 1
#define RI 0b0001000000000000    // 1
#define II 0b0000100000000000    // 1
#define PE 0b0000010000000000    // 1
#define PI 0b0000001000000000    // 1
#define RS 0b0000000100000000    // 1

#define A3 0b0000000010000000  // 1
#define A2 0b0000000001000000  // 1
#define A1 0b0000000000100000  // 0
#define A0 0b0000000000010000  // 1
#define HI 0b0000000000001000  // 1
#define SPI 0b0000000000000100 // 1
#define BI 0b0000000000000010  // 1
#define AI 0b0000000000000001  // 0

#define ADD A0
#define AND A1
#define OR A0 | A1
#define XOR A2
#define SUB A0 | A2
#define SPO A1 | A2
#define BO A0 | A1 | A2
#define AO A3
#define RO A0 | A3
#define PO A1 | A3
#define ADC A0 | A1 | A3
#define CO A2 | A3
#define SHL A0 | A2 | A3
#define SHR A1 | A2 | A3
#define IGB A0 | A1 | A2 | A3

#define CI SPNOT

#define PIH PI | HI
#define PIL PI
#define POH PO | HI
#define POL PO
#define MIH MI | HI
#define MIL MI

struct MicrocodeSignalStruct
{
        std::string name;
        unsigned short signal;
        unsigned short notsignal;
};
typedef unsigned char Byte;
typedef unsigned short Word;

struct cpuState
{
        Word Bus;
        Byte A, B, C;
        Word PC, SP, IR, MR;
        Byte MicroStep;
        Byte ZeroF, CarryF;
        Byte RAM[65536];
};
cpuState CPUSTATE = {0, 0, 0, 0, 0};

MicrocodeSignalStruct Signals[] = {
    {"HI", HI},
    {"SPNOT", SPNOT | SPI},
    {"ME", ME},
    {"RI", RI},
    {"II", II},
    {"PE", PE},
    {"RS", RS},
    {"SPI", SPI},
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
    {"CI", SPNOT, SPI},
    {"SHL", SHL, A1},
    {"SHR", SHR, A0},
    {"IGB", IGB},
    {"PIH", PI | HI},
    {"PIL", PI, HI},
    {"POH", PO | HI},
    {"POL", PO, HI},
    {"MIH", MI | HI},
    {"MIL", MI, HI},
};

unsigned short mCode[16384];

void showMicro(unsigned short signals)
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

bool isSignal(std::string name, unsigned short signals)
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
        if (kbhit())
        {
                char a = getch();
                if (a == 0xD)
                        a = 0xA;
                return a;
        }
        return 0;
}

void executeMicroRising(Word signal)
{
        if (isSignal("SPI", signal))
        {
                CPUSTATE.SP -= 1;
                if (isSignal("SPNOT", signal))
                {
                        CPUSTATE.SP += 2;
                }
                CPUSTATE.SP = CPUSTATE.SP | 0xff00;
        }
        if (isSignal("AI", signal))
                CPUSTATE.A = CPUSTATE.Bus;
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
        CPUSTATE.ZeroF = ((CPUSTATE.Bus & 0xFF) == 0);
        CPUSTATE.CarryF = ((CPUSTATE.Bus & 0x100) == 0x100);
        CPUSTATE.Bus = CPUSTATE.Bus & 0xFF;
}

void executeMicroFalling(Word signal)
{
        CPUSTATE.Bus = 0;
        if (isSignal("SPO", signal))
        {
                CPUSTATE.Bus = (CPUSTATE.SP & 0xff);
                if (isSignal("HI", signal))
                        CPUSTATE.Bus = ((CPUSTATE.SP >> 8) & 0xff);
        }
        if (isSignal("AO", signal))
                CPUSTATE.Bus = CPUSTATE.A;
        if (isSignal("BO", signal))
                CPUSTATE.Bus = CPUSTATE.B;
        if (isSignal("CO", signal))
                CPUSTATE.Bus = CPUSTATE.C;
        if (isSignal("RO", signal))
        {
                CPUSTATE.Bus = CPUSTATE.RAM[CPUSTATE.MR];
                if (CPUSTATE.MR == 0x2000)
                {
                        CPUSTATE.Bus = recvByte();
                }
        }
        if (isSignal("POH", signal))
                CPUSTATE.Bus = ((CPUSTATE.PC >> 8) & 0xFF);
        if (isSignal("POL", signal))
                CPUSTATE.Bus = (CPUSTATE.PC & 0xFF);

        if (isSignal("IGB", signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A + 1);
                setFlags();
        }

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
                      *rMCode2 = reinterpret_cast<unsigned char *>(malloc(16384));
        std::ifstream microcodeR1, microcodeR2;
        microcodeR1.open("microcode1.bin", std::ios::binary);
        microcodeR2.open("microcode2.bin", std::ios::binary);

        microcodeR1.read(reinterpret_cast<char *>(rMCode1), 16384);
        microcodeR2.read(reinterpret_cast<char *>(rMCode2), 16384);
        int i = 0, j;

        for (; i < 16384; i++)
        {
                mCode[i] = rMCode1[i] | (rMCode2[i] << 8);
        }
        free(rMCode1);
        free(rMCode2);

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
        for (; 1;)
        {
                for (CPUSTATE.MicroStep = 0; CPUSTATE.MicroStep < 16; CPUSTATE.MicroStep++)
                {
                        //getch();
                        // if(CPUSTATE.IR==118)
                         //showMicro(mCode[CPUSTATE.IR * 16 + CPUSTATE.MicroStep + CPUSTATE.CarryF * 2048 + CPUSTATE.ZeroF * 4096]);
                        executeMicroFalling(mCode[CPUSTATE.IR * 16 + CPUSTATE.MicroStep + CPUSTATE.CarryF * 4096 + CPUSTATE.ZeroF * 8192]);
                        executeMicroRising(mCode[CPUSTATE.IR * 16 + CPUSTATE.MicroStep + CPUSTATE.CarryF * 4096 + CPUSTATE.ZeroF * 8192]);
                }
                /*std::cout <<std::hex<< " IR:" << static_cast<int>(CPUSTATE.IR) << " A:" << static_cast<int>(CPUSTATE.A)
                          << " B:" << static_cast<int>(CPUSTATE.B) << " Z:" << static_cast<int>(CPUSTATE.ZeroF)
                          << " C:" << static_cast<int>(CPUSTATE.CarryF) << " PC:" << static_cast<int>(CPUSTATE.PC)
                          << " SP:" << static_cast<int>(CPUSTATE.SP) <<" ";
                for(i=0;i<16;i++){
                        std::cout<<std::hex<<static_cast<int>(CPUSTATE.RAM[i+0xfff0])<<" ";
                }
                std::cout<<std::dec<< "\n";*/
        }
}
// 8100:03 19 81 5B 00 68 16 81 07 00 20 02 19 81 14 01 07 19 81 62 00 81 62 00 00 1B 81 68 6F 6C 61 20 42 55 45 4E 41 53 20 74 61 72 64 65 73 0A