#include <iostream>
#include <bitset>
#include <vector>
using namespace std;

struct IFStruct { //  Instruction fetch from memory
    bitset<32> PC;
    bool nop;
};

struct IDStruct { // Instruction decode & register read
    bitset<32> Instruction;
    bool nop;
};

struct EXEStruct { //Execute operation or calculate address
    bitset<5> Rs;
    bitset<5> Rt;
    bitset<5> Rd;
    bitset<16> Immediate;
    bitset<32> ALUResult;
    bitset<32> ReadData1;
    bitset<32> ReadData2;
    bool ALUSrc; // Used in EXE
    bool Branch;  // Used in EXE
    bool MemRead;
    bool MemWrite;
    bool RegDst;
    bool RegWrite;
    bool MemtoReg;
    bool nop;
};

struct MEMStruct { // Access memory operand
    bitset<32> ALUResult;
    bool MemRead; // Used in MEM
    bool MemWrite; // Used in MEM
    bool RegDst;
    bool RegWrite;
    bool MemtoReg;
    bool nop;
};

struct WBStruct { // Write result back to register
    bitset<5> Rs;
    bitset<5> Rt;
    bool RegDst; // Used in WB
    bool RegWrite; // Used in WB
    bool MemtoReg; // Used in WB
    bool nop;
};

int main(void) {
    vector<string> instructions = {
        "lw $2, 8($0)"
    };
}