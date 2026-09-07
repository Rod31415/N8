#include <iostream>
#include <fstream>
#include <conio.h>
#include <iomanip>
#include <thread>
#include <atomic>
#include <windows.h>
#include <chrono>

// #define DEBUG0
// #define DEBUG1
//  #define DEBUG2

//               ZC
// FLAGS: 0b0000000

#define INT 0b100000000000000000000000
#define NOFLG 0b010000000000000000000000
#define XI 0b001000000000000000000000
#define XO 0b000100000000000000000000
#define FPI 0b000010000000000000000000
#define SPI 0b000001000000000000000000
#define CI 0b000000100000000000000000
#define SPNOT 0b000000010000000000000000

#define ME 0b000000001000000000000000 // 0
#define MI 0b000000000100000000000000 // 1
#define RI 0b000000000010000000000000 // 1
#define II 0b000000000001000000000000 // 1
#define PE 0b000000000000100000000000 // 1
#define PI 0b000000000000010000000000 // 1
#define RS 0b000000000000001000000000 // 1
#define A4 0b000000000000000100000000 // 1

#define A3 0b000000000000000010000000    // 1
#define A2 0b000000000000000001000000    // 1
#define A1 0b000000000000000000100000    // 0
#define A0 0b000000000000000000010000    // 1
#define HI 0b000000000000000000001000    // 1
#define SPACT 0b000000000000000000000100 // 1
#define BI 0b000000000000000000000010    // 1
#define AI 0b000000000000000000000001    // 0

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
#define FPO A0 | A1 | A2 | A3

#define SBC A4
#define ROT A0 | A4
#define STI A1 | A4 | HI
#define CLI A1 | A4
#define LIDT A0 | A1 | A4
#define FRI A2 | A4 | HI
#define FRO A2 | A4
#define ACK A0 | A2 | A4
#define IRQO A1 | A2 | A4
#define CONSTF A0 | A1 | A2 | A4
#define OIDT A3 | A4

#define PIH PI | HI
#define PIL PI
#define POH PO | HI
#define POL PO
#define MIH MI | HI
#define MIL MI

#define LIDTH LIDT | HI
#define LIDTL LIDT
#define OIDTH OIDT | HI
#define OIDTL OIDT

bool IsHalted = false;
bool isRevised = false;

enum UniqueSignal
{
        UNIQUE_HI = 0,
        UNIQUE_SPNOT,
        UNIQUE_ME,
        UNIQUE_RI,
        UNIQUE_II,
        UNIQUE_PE,
        UNIQUE_RS,
        UNIQUE_SPI,
        UNIQUE_FPI,
        UNIQUE_SPACT,
        UNIQUE_AI,
        UNIQUE_BI,
        UNIQUE_ADD,
        UNIQUE_AND,
        UNIQUE_OR,
        UNIQUE_XOR,
        UNIQUE_SUB,
        UNIQUE_SPO,
        UNIQUE_BO,
        UNIQUE_AO,
        UNIQUE_RO,
        UNIQUE_ADC,
        UNIQUE_CO,
        UNIQUE_CI,
        UNIQUE_SHL,
        UNIQUE_SHR,
        UNIQUE_FPO,
        UNIQUE_PIH,
        UNIQUE_PIL,
        UNIQUE_POH,
        UNIQUE_POL,
        UNIQUE_MIH,
        UNIQUE_MIL,
        UNIQUE_XI,
        UNIQUE_XO,
        UNIQUE_NOFLG,
        UNIQUE_ROT,
        UNIQUE_SBC,
        UNIQUE_INT,
        UNIQUE_STI,
        UNIQUE_CLI,
        UNIQUE_LIDTH,
        UNIQUE_LIDTL,
        UNIQUE_OIDTH,
        UNIQUE_OIDTL,
        UNIQUE_FRI,
        UNIQUE_FRO,
        UNIQUE_ACK,
        UNIQUE_IRQO,
        UNIQUE_CONSTF,
};

struct MicrocodeSignalStruct
{
        std::string name;
        UniqueSignal usename;
        unsigned int signal;
        unsigned int notsignal;
};
typedef unsigned char Byte;
typedef unsigned short Word;

struct cpuState
{
        Word Bus;
        Byte A, B, C, X;
        Word PC, SP, FP, IR, MR, IDT;
        Byte MicroStep;
        Byte ZeroF, CarryF, IntF, IRQ;
        Byte RAM[65536];
};
cpuState CPUSTATE = {0, 0, 0, 0, 0};

MicrocodeSignalStruct Signals[] = {
    {"HI", UNIQUE_HI, HI},
    {"SPNOT", UNIQUE_SPNOT, SPNOT},
    {"ME", UNIQUE_ME, ME},
    {"RI", UNIQUE_RI, RI},
    {"II", UNIQUE_II, II},
    {"PE", UNIQUE_PE, PE},
    {"RS", UNIQUE_RS, RS},
    {"SPI", UNIQUE_SPI, SPI},
    {"FPI", UNIQUE_FPI, FPI},
    {"SPACT", UNIQUE_SPACT, SPACT},
    {"AI", UNIQUE_AI, AI},
    {"BI", UNIQUE_BI, BI},
    {"ADD", UNIQUE_ADD, ADD, A1 | A2 | A3 | A4},
    {"AND", UNIQUE_AND, AND, A0 | A2 | A3 | A4},
    {"OR", UNIQUE_OR, OR, A2 | A3 | A4},
    {"XOR", UNIQUE_XOR, XOR, A0 | A1 | A3 | A4},
    {"SUB", UNIQUE_SUB, SUB, A1 | A3 | A4},
    {"SPO", UNIQUE_SPO, SPO, A0 | A3 | A4},
    {"BO", UNIQUE_BO, BO, A3 | A4},
    {"AO", UNIQUE_AO, AO, A0 | A1 | A2 | A4},
    {"RO", UNIQUE_RO, RO, A1 | A2 | A4},
    {"ADC", UNIQUE_ADC, ADC, A2 | A4},
    {"CO", UNIQUE_CO, CO, A0 | A1 | A4},
    {"CI", UNIQUE_CI, CI},
    {"SHL", UNIQUE_SHL, SHL, A1 | A4},
    {"SHR", UNIQUE_SHR, SHR, A0 | A4},
    {"FPO", UNIQUE_FPO, FPO, A4},
    {"PIH", UNIQUE_PIH, PI | HI},
    {"PIL", UNIQUE_PIL, PI, HI},
    {"POH", UNIQUE_POH, PO | HI, A0 | A2 | A4},
    {"POL", UNIQUE_POL, PO, HI | A0 | A2 | A4},
    {"MIH", UNIQUE_MIH, MI | HI},
    {"MIL", UNIQUE_MIL, MI, HI},
    {"XI", UNIQUE_XI, XI},
    {"XO", UNIQUE_XO, XO},
    {"NOFLG", UNIQUE_NOFLG, NOFLG},
    {"ROT", UNIQUE_ROT, ROT, A0 | A1 | A2 | A3},
    {"SBC", UNIQUE_SBC, SBC, A1 | A2 | A3},
    {"INT", UNIQUE_INT, INT},
    {"STI", UNIQUE_STI, STI, A0 | A2 | A3},
    {"CLI", UNIQUE_CLI, CLI, A0 | A2 | A3 | HI},
    {"LIDTL", UNIQUE_LIDTL, LIDTL, A2 | A3 | HI},
    {"LIDTH", UNIQUE_LIDTH, LIDTH, A2 | A3},
    {"OIDTL", UNIQUE_OIDTL, OIDTL, A0 | A1 | A2 | HI},
    {"OIDTH", UNIQUE_OIDTH, OIDTH, A0 | A1 | A2},
    {"FRI", UNIQUE_STI, FRI, A0 | A1 | A3},
    {"FRO", UNIQUE_CLI, FRO, A0 | A1 | A3 | HI},
    {"ACK", UNIQUE_ACK, ACK, A1 | A3},
    {"IRQO", UNIQUE_IRQO, IRQO, A0 | A3},
    {"CONST", UNIQUE_CONSTF, CONSTF, A3},
};

unsigned int mCode[16384];

#define IRQ_TIMER 0b00000001
#define IRQ_KEYBOARD 0b00000010
#define IRQ_UART 0b00000100

std::atomic<char> last_key{0};
std::atomic<bool> irq_timer, irq_keyboard, irq_uart, ack;

std::atomic<Byte> irq, irq_address;

void listenKey()
{
        LARGE_INTEGER frequency;
        LARGE_INTEGER start;
        LARGE_INTEGER end;

        QueryPerformanceFrequency(&frequency);

        QueryPerformanceCounter(&start);

        while (true)
        {
                QueryPerformanceCounter(&end);

                long long elapsedTicks = end.QuadPart - start.QuadPart;
                long long microseconds = (elapsedTicks * 1000000) / frequency.QuadPart;
                
                if (microseconds > 15000)
        {
                irq = true;
                irq_timer = true;
                QueryPerformanceFrequency(&frequency);
                QueryPerformanceCounter(&start);
        }
                if (_kbhit())
                {
                        last_key = _getch();
                        irq = true;
                        irq_keyboard = true;
                        /*std::cout << "IRQ: KEYBOARD\n";*/
                }
                if (ack == true)
                {
                        if (irq_timer)
                        {
                                irq_address = 0;
                                irq_timer = false;
                        }
                        else if (irq_keyboard)
                        {
                                irq_address = 2;
                                irq_keyboard = false;
                        }
                        else if (irq_uart)
                        {
                                irq_address = 4;
                        }
                        ack = false;
                        irq = false;
                }

                std::this_thread::yield();
        }
}

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

bool isSignal(UniqueSignal name, unsigned int signals)
{
        /*for (auto sig : Signals)
        {
                if (name != sig.usename)
                        continue;
                if ((signals & sig.signal) != sig.signal)
                        continue;
                else if ((signals & sig.notsignal) != 0)
                        continue;
                else
                        return true;
        }*/

        if ((signals & Signals[name].notsignal) == 0 && (signals & Signals[name].signal) == Signals[name].signal)
                return true;
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
        char a = getch();
        if (a == 27)
        {
                IsHalted = true;
        }
        return a;
        /*#endif*/
}

bool enableFlags = true;

void executeMicroRising(unsigned int signal)
{
        if (isSignal(UNIQUE_SPACT, signal))
        {
                CPUSTATE.SP -= 1;
                if (isSignal(UNIQUE_SPNOT, signal))
                {
                        CPUSTATE.SP += 2;
                }
                CPUSTATE.SP = CPUSTATE.SP | 0xff00;
        }
        if (isSignal(UNIQUE_SPI, signal))
        {

                CPUSTATE.SP = CPUSTATE.Bus;
                CPUSTATE.SP = CPUSTATE.SP | 0xff00;
        }
        if (isSignal(UNIQUE_FPI, signal))
        {

                CPUSTATE.FP = CPUSTATE.Bus;
                CPUSTATE.FP = CPUSTATE.FP | 0xff00;
        }
        if (isSignal(UNIQUE_AI, signal))
                CPUSTATE.A = CPUSTATE.Bus;
        if (isSignal(UNIQUE_XI, signal))
                CPUSTATE.X = CPUSTATE.Bus;
        if (isSignal(UNIQUE_BI, signal))
                CPUSTATE.B = CPUSTATE.Bus;
        if (isSignal(UNIQUE_CI, signal))
                CPUSTATE.C = CPUSTATE.Bus;
        if (isSignal(UNIQUE_RI, signal))
        {
                CPUSTATE.RAM[CPUSTATE.MR] = CPUSTATE.Bus;
                if (CPUSTATE.MR == 0x2000)
                {
                        sendByte(CPUSTATE.Bus);
                }
        }
        if (isSignal(UNIQUE_II, signal))
                CPUSTATE.IR = CPUSTATE.Bus;
        if (isSignal(UNIQUE_MIH, signal))
                CPUSTATE.MR = (CPUSTATE.MR & 0xFF) | (CPUSTATE.Bus << 8);
        if (isSignal(UNIQUE_MIL, signal))
                CPUSTATE.MR = (CPUSTATE.MR & 0xFF00) | (CPUSTATE.Bus);
        if (isSignal(UNIQUE_PIH, signal))
                CPUSTATE.PC = (CPUSTATE.PC & 0xFF) | (CPUSTATE.Bus << 8);
        if (isSignal(UNIQUE_PIL, signal))
                CPUSTATE.PC = (CPUSTATE.PC & 0xFF00) | (CPUSTATE.Bus);
        if (isSignal(UNIQUE_FRI, signal))
        {
                CPUSTATE.CarryF = (CPUSTATE.Bus & 0b01) == 0b01;
                CPUSTATE.ZeroF = (CPUSTATE.Bus & 0b10) == 0b10;
        }

        if (isSignal(UNIQUE_ME, signal))
                CPUSTATE.MR++;
        if (isSignal(UNIQUE_PE, signal))
                CPUSTATE.PC++;

        if (isSignal(UNIQUE_STI, signal))
                CPUSTATE.IntF = true;
        if (isSignal(UNIQUE_CLI, signal))
                CPUSTATE.IntF = false;

        if (isSignal(UNIQUE_LIDTH, signal))
                CPUSTATE.IDT = (CPUSTATE.IDT & 0xFF) | (CPUSTATE.Bus << 8);
        if (isSignal(UNIQUE_LIDTL, signal))
                CPUSTATE.IDT = (CPUSTATE.IDT & 0xFF00) | (CPUSTATE.Bus);

        if (isSignal(UNIQUE_RS, signal))
                CPUSTATE.MicroStep = 17;
}

void setFlags()
{
        if (enableFlags)
        {
                CPUSTATE.ZeroF = ((CPUSTATE.Bus & 0xFF) == 0);
                CPUSTATE.CarryF = ((CPUSTATE.Bus & 0x100) == 0x100);
        }
        CPUSTATE.Bus = CPUSTATE.Bus & 0xFF;
        enableFlags = true;
}

void executeMicroFalling(unsigned int signal)
{
        CPUSTATE.Bus = 0;
        if (isSignal(UNIQUE_SPO, signal))
        {
                CPUSTATE.Bus = (CPUSTATE.SP & 0xff);
                if (isSignal(UNIQUE_HI, signal))
                        CPUSTATE.Bus = ((CPUSTATE.SP >> 8) & 0xff);
        }
        if (isSignal(UNIQUE_FPO, signal))
        {
                CPUSTATE.Bus = (CPUSTATE.FP & 0xff);
                if (isSignal(UNIQUE_HI, signal))
                        CPUSTATE.Bus = ((CPUSTATE.FP >> 8) & 0xff);
        }
        if (isSignal(UNIQUE_AO, signal))
                CPUSTATE.Bus = CPUSTATE.A;
        if (isSignal(UNIQUE_BO, signal))
                CPUSTATE.Bus = CPUSTATE.B;
        if (isSignal(UNIQUE_CO, signal))
                CPUSTATE.Bus = CPUSTATE.C;
        if (isSignal(UNIQUE_XO, signal))
                CPUSTATE.Bus = CPUSTATE.X;
        if (isSignal(UNIQUE_RO, signal))
        {
                CPUSTATE.Bus = CPUSTATE.RAM[CPUSTATE.MR];
                if (CPUSTATE.MR == 0x2000)
                {
                        CPUSTATE.Bus = last_key;
                        last_key = 0;
                }
        }
        if (isSignal(UNIQUE_NOFLG, signal))
                enableFlags = false;
        if (isSignal(UNIQUE_POH, signal))
                CPUSTATE.Bus = ((CPUSTATE.PC >> 8) & 0xFF);
        if (isSignal(UNIQUE_POL, signal))
                CPUSTATE.Bus = (CPUSTATE.PC & 0xFF);
        if (isSignal(UNIQUE_FRO, signal))
        {
                CPUSTATE.Bus = (CPUSTATE.CarryF | (CPUSTATE.ZeroF << 1)) & 0b11;
        }
        if (isSignal(UNIQUE_CONSTF, signal))
                CPUSTATE.Bus = 0xF1;
        if (isSignal(UNIQUE_IRQO, signal))
                CPUSTATE.Bus = irq_address;
        if (isSignal(UNIQUE_ACK, signal))
                ack = true;

        if (isSignal(UNIQUE_OIDTH, signal))
                CPUSTATE.Bus = ((CPUSTATE.IDT >> 8) & 0xFF);
        if (isSignal(UNIQUE_OIDTL, signal))
                CPUSTATE.Bus = (CPUSTATE.IDT & 0xFF);

        if (isSignal(UNIQUE_ADD, signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A + CPUSTATE.B);
                setFlags();
        }
        if (isSignal(UNIQUE_SUB, signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A + (255 - CPUSTATE.B) + 1);
                setFlags();
        }
        if (isSignal(UNIQUE_SBC, signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A + (255 - CPUSTATE.B) + CPUSTATE.CarryF);
                setFlags();
        }
        if (isSignal(UNIQUE_AND, signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A & CPUSTATE.B);
                setFlags();
        }
        if (isSignal(UNIQUE_OR, signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A | CPUSTATE.B);
                setFlags();
        }
        if (isSignal(UNIQUE_XOR, signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A ^ CPUSTATE.B);
                setFlags();
        }
        if (isSignal(UNIQUE_ADC, signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A + CPUSTATE.B + CPUSTATE.CarryF);
                setFlags();
        }
        if (isSignal(UNIQUE_SHL, signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A << 1);
                if (isSignal(UNIQUE_ROT, signal))
                        CPUSTATE.Bus = CPUSTATE.Bus | CPUSTATE.CarryF;
                setFlags();
        }
        if (isSignal(UNIQUE_SHR, signal))
        {
                CPUSTATE.Bus = (CPUSTATE.A >> 1);
                if (isSignal(UNIQUE_ROT, signal))
                        CPUSTATE.Bus = CPUSTATE.Bus | (CPUSTATE.CarryF << 7);
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
        std::thread asyncKey(listenKey);

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
                mCode[i] = rMCode1[i] | (rMCode2[i] << 8) | (rMCode3[i] << 16);
        }
        free(rMCode1);
        free(rMCode2);
        free(rMCode3);

        std::ifstream file("RAM.lcf", std::ios::binary);
        file.seekg(0, file.end);
        size_t length = file.tellg();
        char *code = new char[length + 1];
        file.seekg(0, file.beg);
        file.read(code, length);

        unsigned short initMachineCode = code[1] * 0x100 | code[0] & 0x00FF;
        // std::cout<<initMachineCode;

        for (i = 0; i < length - initMachineCode; i++)
        {
                CPUSTATE.RAM[i] = code[i + initMachineCode];
                // std::cout<<std::hex<<static_cast<unsigned int>(CPUSTATE.RAM[i])<<" ";
        }

        CPUSTATE.PC = 0;
        CPUSTATE.MR = 0;
        CPUSTATE.IR = 0;
        CPUSTATE.SP = 0xff00;
        CPUSTATE.FP = 0xff00;
        for (; 1;)
        {
                int c;
                /*if(CPUSTATE.IR==0xf0){

                }*/

                if (irq && CPUSTATE.IntF)
                {
                        // std::cout << "##############################IRQ: " << (irq == 1) << "\n";
                        CPUSTATE.IR = 0xf0;
                        /*std::cout << "On IRQ\n";*/
                }

                for (CPUSTATE.MicroStep = 0; CPUSTATE.MicroStep < 16; CPUSTATE.MicroStep++)
                {
#ifdef DEBUG0

                        // if(CPUSTATE.IR==118)
                        showMicro(mCode[CPUSTATE.IR * 16 + CPUSTATE.MicroStep + CPUSTATE.CarryF * 4096 + CPUSTATE.ZeroF * 8192]);
#endif
                        /*if (CPUSTATE.IR == 0xf0 || CPUSTATE.IR == 0xf1)
                        {
                                std::cout << std::hex << "\nIR:" << static_cast<int>(CPUSTATE.IR) << " A:" << static_cast<int>(CPUSTATE.A)
                                          << " B:" << static_cast<int>(CPUSTATE.B) << " Zf:" << static_cast<int>(CPUSTATE.ZeroF)
                                          << " Cf:" << static_cast<int>(CPUSTATE.CarryF) << " X:" << static_cast<int>(CPUSTATE.X)
                                          << " C:" << static_cast<int>(CPUSTATE.C) << " PC:" << static_cast<int>(CPUSTATE.PC)
                                          << " IDT:" << static_cast<int>(CPUSTATE.IDT) << " SP:" << static_cast<int>(CPUSTATE.SP) << "\n"
                                          << " FP:" << static_cast<int>(CPUSTATE.FP) << "\n";
                        }*/

                        if (isRevised)
                                showMicro(mCode[CPUSTATE.IR * 16 + CPUSTATE.MicroStep + CPUSTATE.CarryF * 4096 + CPUSTATE.ZeroF * 8192]);
                        executeMicroFalling(mCode[CPUSTATE.IR * 16 + CPUSTATE.MicroStep + CPUSTATE.CarryF * 4096 + CPUSTATE.ZeroF * 8192]);
                        executeMicroRising(mCode[CPUSTATE.IR * 16 + CPUSTATE.MicroStep + CPUSTATE.CarryF * 4096 + CPUSTATE.ZeroF * 8192]);
                }

#ifdef DEBUG1
                // getchar();
                std::cout << std::hex << "\nIR:" << static_cast<int>(CPUSTATE.IR) << " A:" << static_cast<int>(CPUSTATE.A)
                          << " B:" << static_cast<int>(CPUSTATE.B) << " Zf:" << static_cast<int>(CPUSTATE.ZeroF)
                          << " Cf:" << static_cast<int>(CPUSTATE.CarryF) << " X:" << static_cast<int>(CPUSTATE.X)
                          << " C:" << static_cast<int>(CPUSTATE.C) << " PC:" << static_cast<int>(CPUSTATE.PC)
                          << " IDT:" << static_cast<int>(CPUSTATE.IDT) << " SP:" << static_cast<int>(CPUSTATE.SP) << "\n"
                          << " FP:" << static_cast<int>(CPUSTATE.FP) << "\n";
                if (CPUSTATE.SP >= 0xfff0)
                {
                        int s = CPUSTATE.SP - 0xfff0;
                        for (i = 0; i < s; i++)
                                std::cout << "   ";
                        std::cout << "P\n";
                }
                if (CPUSTATE.FP >= 0xfff0)
                {
                        int s = CPUSTATE.FP - 0xfff0;
                        for (i = 0; i < s; i++)
                                std::cout << "   ";
                        std::cout << "F\n";
                }
                for (i = 0; i < 16; i++)
                {
                        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(CPUSTATE.RAM[i + 0xfff0]) << " ";
                }
                std::cout << std::dec << "\n";

#endif
                if (CPUSTATE.IR == 0xfc)
                {
                        std::cout << "BREAK POINT " << static_cast<char *>(&code[code[CPUSTATE.RAM[CPUSTATE.PC - 1] + 2]]) << "\n";
                        std::cout << std::hex << "\nIR:" << static_cast<int>(CPUSTATE.IR) << " A:" << static_cast<int>(CPUSTATE.A)
                                  << " B:" << static_cast<int>(CPUSTATE.B) << " Z:" << static_cast<int>(CPUSTATE.ZeroF)
                                  << " C:" << static_cast<int>(CPUSTATE.CarryF) << " PC:" << static_cast<int>(CPUSTATE.PC)
                                  << " SP:" << static_cast<int>(CPUSTATE.SP) << "\n"
                                  << " FP:" << static_cast<int>(CPUSTATE.FP) << "\n";
                        if (CPUSTATE.SP >= 0xffe0)
                        {
                                int s = CPUSTATE.SP - 0xffe0;
                                for (i = 0; i < s; i++)
                                        std::cout << "   ";
                                std::cout << "P\n";
                        }
                        if (CPUSTATE.FP >= 0xffe0)
                        {
                                int s = CPUSTATE.FP - 0xffe0;
                                for (i = 0; i < s; i++)
                                        std::cout << "   ";
                                std::cout << "F\n";
                        }
                        for (i = 0; i < 32; i++)
                        {
                                std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(CPUSTATE.RAM[i + 0xffe0]) << " ";
                        }
                        std::cout << std::dec << "\n";
                        std::cout << "-------------------------\n";
                }
                if (CPUSTATE.IR == 0xfe || isRevised)
                {

                        std::cout << std::hex << "\nIR:" << static_cast<int>(CPUSTATE.IR) << " A:" << static_cast<int>(CPUSTATE.A)
                                  << " B:" << static_cast<int>(CPUSTATE.B) << " Z:" << static_cast<int>(CPUSTATE.ZeroF)
                                  << " C:" << static_cast<int>(CPUSTATE.CarryF) << " PC:" << static_cast<int>(CPUSTATE.PC)
                                  << " SP:" << static_cast<int>(CPUSTATE.SP) << "\n"
                                  << " FP:" << static_cast<int>(CPUSTATE.FP) << "\n";
                        if (CPUSTATE.SP >= 0xffe0)
                        {
                                int s = CPUSTATE.SP - 0xffe0;
                                for (i = 0; i < s; i++)
                                        std::cout << "   ";
                                std::cout << "P\n";
                        }
                        if (CPUSTATE.FP >= 0xffe0)
                        {
                                int s = CPUSTATE.FP - 0xffe0;
                                for (i = 0; i < s; i++)
                                        std::cout << "   ";
                                std::cout << "F\n";
                        }
                        for (i = 0; i < 32; i++)
                        {
                                std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(CPUSTATE.RAM[i + 0xffe0]) << " ";
                        }
                        std::cout << std::dec << "\n";
                        std::cout << "-------------------------\n";
                        char a = getch();
                        if (a == 27)
                        {
                                IsHalted = true;
                        }
                        isRevised = false;
                }
                if (CPUSTATE.IR == 0xfd)
                        isRevised = true;

                if (CPUSTATE.IR == 0xff || IsHalted)
                {
                        std::cout << "\nProgram halted!!\n";
                        break;
                }
        }
        asyncKey.join();
}
// 8100:03 19 81 5B 00 68 16 81 07 00 20 02 19 81 14 01 07 19 81 62 00 81 62 00 00 1B 81 68 6F 6C 61 20 42 55 45 4E 41 53 20 74 61 72 64 65 73 0A
