#include <iostream>
#include <bitset>
#include <vector>
using namespace std;

struct IFStruct { //  Instruction fetch from memory
    bitset<32> PC;
    bool nop = false;
};

struct IDStruct { // Instruction decode & register read
    bitset<32> Instruction;
    bool nop = false;
};

struct EXStruct { //Execute operation or calculate address
    bitset<5> Rs;
    bitset<5> Rt;
    bitset<5> Rd;
    bitset<16> Immediate;
    bitset<32> ALUResult;
    bitset<32> ReadData1;
    bitset<32> ReadData2;
    bool ALUSrc = false; // Used in EX
    bool Branch = false;  // Used in EX
    bool MemRead = false;
    bool MemWrite = false;
    bool RegDst = false;
    bool RegWrite = false;
    bool MemtoReg = false;
    bool nop = false;
};

struct MEMStruct { // Access memory operand
    bitset<32> ALUResult;
    bool MemRead = false; // Used in MEM
    bool MemWrite = false; // Used in MEM
    bool RegDst = false;
    bool RegWrite = false;
    bool MemtoReg = false;
    bool nop = false;
};

struct WBStruct { // Write result back to register
    bitset<5> Rs;
    bitset<5> Rt;
    bool RegDst = false; // Used in WB
    bool RegWrite = false; // Used in WB
    bool MemtoReg = false; // Used in WB
    bool nop = false;
};

class Pipeline {
private:
    IFStruct IF;
    IDStruct ID;
    EXStruct EX;
    MEMStruct MEM;
    WBStruct WB;
public:
    void excuteIF() {

    }

    void excuteID() {

    }

    void excuteEX() {

    }

    void excuteMEM() {

    }

    void excuteWB() {

    }

    void excute() {
        excuteIF();
        excuteID();
        excuteEX();
        excuteMEM();
        excuteWB();
    }

    void printState() {

    }

};

int main(void) {
    vector<string> instructions = {
        "lw $2, 8($0)"
    };

    
}