#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <queue>
using namespace std;

stringstream ss;

struct IFStruct { //  Instruction fetch from memory
    string Instruction; // Full instruction. e.g. "lw $2, 8($0)"
    int PC = 0;
    bool nop = false;
};

struct IDStruct { // Instruction decode & register read
    string Op; // Split instruction. e.g. "lw", "sw", "add", "sub", "beq
    int Rs = 0;
    int Rt = 0;
    int Rd = 0;
    int Immediate = 0;
    bool nop = false;
};

struct EXStruct { //Execute operation or calculate address
    string Op;
    int Rd = 0;
    int Rs = 0;
    int Rt = 0;
    int Immediate;
    int ALUResult;
    int ReadData1;
    int ReadData2;
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
    string Op;
    int Rs = 0;
    int Rt = 0;
    int ALUResult;
    bool MemRead = false; // Used in MEM
    bool MemWrite = false; // Used in MEM
    bool RegDst = false;
    bool RegWrite = false;
    bool MemtoReg = false;
    bool Branch = false;
    bool nop = false;
};

struct WBStruct { // Write result back to register
    string Op;
    int Rs = 0;
    int Rt = 0;
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
    int cycle = 1; 

public:
    void excuteIF() {
        if(!instructions.empty()) {
            IF.Instruction = instructions.front();
            instructions.pop();
        }
    }

    void excuteID() {
        ss << IF.Instruction;
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
        } else if (ID.Op == "") {   
        } else {
            throw "Unknown instruction!";
        }
        ss.str("");
        ss.clear();
        resetIF();
    }

    void excuteEX() {
        EX.Op = ID.Op;
        if(EX.Op == "lw") {
            EX.Rs = ID.Rs;
            EX.Rt = ID.Rt;
            EX.Immediate = ID.Immediate;
            EX.RegDst = 0; EX.ALUSrc = 1, EX.MemtoReg = 1, EX.RegWrite = 1, EX.MemRead = 1, EX.MemWrite = 0, EX.Branch = 0;
        } else if(EX.Op == "sw") {
            EX.Rs = ID.Rs;
            EX.Rt = ID.Rt;
            EX.Immediate = ID.Immediate;
            // Actually,RegDst and MemtoReg are don't care.
            EX.RegDst = 0; EX.ALUSrc = 1, EX.MemtoReg = 0, EX.RegWrite = 0, EX.MemRead = 0, EX.MemWrite = 1, EX.Branch = 0; 
        } else if(EX.Op == "add" || EX.Op == "sub") {
            EX.Rd = ID.Rd;
            EX.Rs = ID.Rs;
            EX.Rt = ID.Rt;
            EX.RegDst = 1; EX.ALUSrc = 0, EX.MemtoReg = 0, EX.RegWrite = 1, EX.MemRead = 0, EX.MemWrite = 0, EX.Branch = 0;
            if(EX.Op == "add") {
                memory[EX.Rd] = memory[EX.Rs] + memory[EX.Rt];
            } else if(EX.Op == "sub") {
                memory[EX.Rd] = memory[EX.Rs] - memory[EX.Rt];
            }
        } else if(EX.Op == "beq") {
            EX.Rd = ID.Rd;
            EX.Rs = ID.Rs;
            EX.Immediate = ID.Immediate;
            // Actually,RegDst and MemtoReg are don't care.
            // Branch should check in ID.
            EX.RegDst = 0; EX.ALUSrc = 1, EX.MemtoReg = 0, EX.RegWrite = 0, EX.MemRead = 0, EX.MemWrite = 1, EX.Branch = 0; 
        }
        resetID();
    }

    void excuteMEM() {
        MEM.Op = EX.Op;
        MEM.Rs = MEM.Rt;
        MEM.Rt = EX.Rt;
        MEM.ALUResult = EX.ALUResult;
        MEM.RegDst = EX.RegDst;
        MEM.MemtoReg = EX.MemtoReg;
        MEM.RegWrite = EX.RegWrite;
        MEM.MemRead = EX.MemRead;
        MEM.MemWrite = EX.MemWrite;
        resetEX();
    }

    void excuteWB() {
        WB.Op = MEM.Op;
        WB.Rs = MEM.Rs;
        WB.Rt = MEM.Rt;
        WB.RegDst = MEM.RegDst;
        WB.RegWrite = MEM.RegWrite;
        WB.MemtoReg = MEM.MemtoReg;
        resetMEM();
    }

    void resetIF() { IF = IFStruct(); }
    void resetID() { ID = IDStruct(); }
    void resetEX() { EX = EXStruct(); }
    void resetMEM() { MEM = MEMStruct(); }
    void resetWB() { WB = WBStruct(); }

    void excute() {
        excuteWB();
        excuteMEM();
        excuteEX();
        excuteID();
        excuteIF();
        printState();
    }

    void printState() {
        cout << "Clock Cycle " << cycle << ":\n";
        if (IF.Instruction != "") {
            string IFInstruction;
            ss << IF.Instruction;
            ss >> IFInstruction;
            cout << IFInstruction << " IF\n";
            ss.str("");
            ss.clear();
        }
        if (ID.Op != "") {
            cout << ID.Op << " ID\n";
        }
        if (EX.Op != "") {
            cout << EX.Op << " EX ";
            cout << "RegDst=" << EX.RegDst << " ALUSrc=" << EX.ALUSrc << " Branch=" << EX.Branch;
            cout << " MemRead=" << EX.MemRead << " MemWrite=" << EX.MemWrite << " RegWrite=" << EX.RegWrite;
            cout << " MemToReg=" << EX.MemtoReg << "\n";
        }
        if (MEM.Op != "") {
            cout << MEM.Op << " MEM ";
            cout << "Branch=" << MEM.Branch << " MemRead=" << MEM.MemRead << " MemWrite=" << MEM.MemWrite;
            cout << " RegWrite=" << MEM.RegWrite << " MemToReg=" << MEM.MemtoReg << "\n";
        }
        if (WB.Op != "") {
            cout << WB.Op << " WB ";
            cout << "RegWrite=" << WB.RegWrite << " MemToReg=" << WB.MemtoReg << "\n";
        }
    }

    void printFinal() {
        cout << "\n##Final Result:\nTotal Cycles: " << cycle << "\n";

        cout << "Final Register Values:\n";
        for (int i = 0; i < 32; i++) {
            cout << registers[i] << " ";
        }
        cout << "\n";

        cout << "Final Memory Values:\n";
        for (int i = 0; i < 32; i++) {
            cout << memory[i] << " ";
        }
        cout << "\n";
    }

    void readInstructions() {
        fstream file;
        file.open("inputs/test1.txt");
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
        while (true) {
            excute();
            if(IF.Instruction == "" && ID.Op == "" && EX.Op == "" && MEM.Op == "" && WB.Op == "") {
                break;
            }
            cycle++;
        }
        printFinal();
    }
};

int main(void) {
    Pipeline pipeline;
    pipeline.run();
}