#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <bitset>
#include <vector>
#include <queue>
using namespace std;

stringstream ss;

struct IFStruct { //  Instruction fetch from memory
    string instruction; // Full instruction. e.g. "lw $2, 8($0)"
    bitset<32> PC;
    bool nop = false;
};

struct IDStruct { // Instruction decode & register read
    string Op; // Split instruction. e.g. "lw", "sw", "add", "sub", "beq
    int Rs;
    int Rt;
    int Rd;
    int Immediate;
    bool nop = false;
};

struct EXStruct { //Execute operation or calculate address
    string Instruction;
    int Rd;
    int Rs;
    int Rt;
    int Immediate;
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
    bool Branch = false;
};

struct WBStruct { // Write result back to register
    string Instruction;
    int Rs;
    int Rt;
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
    int memory[32] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    queue<string> instructions;
    int cycle = 0; 

public:
    void excuteIF() {
        IF.instruction = instructions.front();
        instructions.pop();
    }

    void excuteID() {
        ss << IF.instruction;
        ss >> ID.Op; // stringstream split string by space in default.
        if(ID.Op == "lw") {
            string regPart, memoryPart;
            ss >> regPart;
            ss.ignore();
            ss >> memoryPart;

            ID.Rt = stoi(regPart.substr(1));
            ID.Rs = stoi(memoryPart.substr(memoryPart.find('$') + 1, memoryPart.find(')') - memoryPart.find('(') - 2));
            ID.Immediate = stoi(memoryPart.substr(0, memoryPart.find('(')));
        } else if(ID.Op == "sw") {
            string regPart, memoryPart;
            ss >> regPart;
            ss.ignore();
            ss >> memoryPart;

            ID.Rt = stoi(memoryPart.substr(memoryPart.find('$') + 1, memoryPart.find(')') - memoryPart.find('(') - 2));
            ID.Rs = stoi(regPart.substr(1));
            ID.Immediate = stoi(memoryPart.substr(0, memoryPart.find('(')));
        } else if(ID.Op == "add") {
            string rd, rs, rt;
            ss >> rd;
            ss.ignore();
            ss >> rs;
            ss.ignore();
            ss >> rt;
            
            ID.Rd = stoi(rd.substr(1));
            ID.Rs = stoi(rs.substr(1));
            ID.Rt = stoi(rt.substr(1));
        } else if(ID.Op == "sub") {
            string rd, rs, rt;
            ss >> rd;
            ss.ignore();
            ss >> rs;
            ss.ignore();
            ss >> rt;
            
            ID.Rd = stoi(rd.substr(1));
            ID.Rs = stoi(rs.substr(1));
            ID.Rt = stoi(rt.substr(1));
        } else if(ID.Op == "beq") {
            string rs, rt, offset;
            ss >> rs;
            ss.ignore();
            ss >> rt;
            ss.ignore();
            ss >> offset;
            
            ID.Rs = stoi(rs.substr(1));
            ID.Rt = stoi(rt.substr(1));
            ID.Immediate = stoi(offset);
        } else {
            throw "Unknown instruction!";
        }
        ss.str("");
        ss.clear();
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
        printState();
    }

    void printState() {
        cout << "Clock Cycle " << cycle << ":\n";
        if (!IF.nop) {
            cout << IF.instruction.substr(0, 3) << " IF\n";
        }
        if (!ID.nop) {
            cout << ID.Op << " ID\n";
        }
        if (!EX.nop) {
            cout << EX.Instruction.substr(0, 3) << " EX ";
            cout << "RegDst=" << EX.RegDst << " ALUSrc=" << EX.ALUSrc << " Branch=" << EX.Branch;
            cout << " MemRead=" << EX.MemRead << " MemWrite=" << EX.MemWrite << " RegWrite=" << EX.RegWrite;
            cout << " MemToReg=" << EX.MemtoReg << "\n";
        }
        if (!MEM.nop) {
            cout << MEM.Instruction.substr(0, 3) << " MEM ";
            cout << "Branch=" << MEM.Branch << " MemRead=" << MEM.MemRead << " MemWrite=" << MEM.MemWrite;
            cout << " RegWrite=" << MEM.RegWrite << " MemToReg=" << MEM.MemtoReg << "\n";
        }
        if (!WB.nop) {
            cout << WB.Instruction.substr(0, 3) << " WB ";
            cout << "RegWrite=" << WB.RegWrite << " MemToReg=" << WB.MemtoReg << "\n";
        }
    }

    void readInstructions() {
        fstream file;
        file.open("inputs/test6.txt");
        if (!file) {
            throw "Can't open file";
        }
        string instruction;
        while (getline(file, instruction)) {
            instructions.push(instruction);
        }
        file.close();
    }

    void run() {
        readInstructions();
        while (!instructions.empty()) {
            cycle++;
            excute();
        }
        cout << "Total Cycles: " << cycle << endl;

        cout << "Final Register Values:" << endl;
        for (int i = 0; i < 32; i++) {
            cout << registers[i] << " ";
        }
        cout << endl;

        cout << "Final Memory Values:" << endl;
        for (int i = 0; i < 32; i++) {
            cout << memory[i] << " ";
        }
        cout << endl;
    }
};

int main(void) {
    Pipeline pipeline;
    pipeline.run();
}