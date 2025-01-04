#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <bitset>
#include <vector>
#include <queue>
using namespace std;

struct IFStruct { //  Instruction fetch from memory
    string instruction; // Full instruction. e.g. "lw $2, 8($0)"
    bitset<32> PC;
    bool nop = false;
};

struct IDStruct { // Instruction decode & register read
    string Op; // Split instruction. e.g. "lw", "sw", "add", "sub", "beq
    bitset<5> Rs;
    bitset<5> Rt;
    bitset<5> Rd;
    bitset<16> Immediate;
    bool nop = false;
};

struct EXStruct { //Execute operation or calculate address
    string Instruction;
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
    string Instruction;
    bitset<32> ALUResult;
    bool MemRead = false; // Used in MEM
    bool MemWrite = false; // Used in MEM
    bool RegDst = false;
    bool RegWrite = false;
    bool MemtoReg = false;
    bool nop = false;
};

struct WBStruct { // Write result back to register
    string Instruction;
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
    int registers[32] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int memory[32] = {1};
    queue<string> instructions;
public:
    void excuteIF() {
        IF.instruction = instructions.front();
        instructions.pop();
    }

    void excuteID() {
        stringstream ss(IF.instruction);
        ss >> ID.Op; // stringstream split string by space in default.
        if(ID.Op == "lw") {
            ss >> ID.Rs;
            ss.ignore();
            ss >> ID.Rt;
        } else if(ID.Op == "sw") {
            
        } else if(ID.Op == "add") {

        } else if(ID.Op == "sub") {

        } else if(ID.Op == "beq") {

        } else {
            throw "Unknown instruction!";
        }
        cout << "123";
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

    void readInstructions() {
        fstream file;
        file.open("inputs/test1.txt");
        if(!file){
            throw "Can't open file";
        }
        string instruction;
        while(getline(file, instruction)){
            instructions.push(instruction);
        }
        file.close();
    }

    void run() {
        readInstructions();
        excute();
    }
};

int main(void) {
    Pipeline pipeline;
    pipeline.run();
}