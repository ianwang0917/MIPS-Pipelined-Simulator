#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <queue>
using namespace std;

stringstream ss;
int stallCount = 0;
struct IFStruct { //  Instruction fetch from memory
    string Instruction; // Full instruction. e.g. "lw $2, 8($0)"
    string Op;
    int PC = 0;
    int Rs = 0; //For load use hazard, implement Rs and Rt for "lw" here.
    int Rt = 0; //For load use hazard, implement Rs and Rt for "lw" here.
    int Rd = 0; //For load use hazard, implement Rs and Rt for "lw" here.
    bool nop = false;
};

struct IDStruct { // Instruction decode & register read
    string Op; // Splited instruction. e.g. "lw", "sw", "add", "sub", "beq
    int Rs = 0;
    int Rt = 0;
    int Rd = 0;
    int ReadData1;
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
    int Rd = 0;
    int ALUResult;
    int ReadData;
    int WriteData;
    int Result;
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
    int Rd = 0;
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
    int registers[32] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    int memory[32] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    queue<string> instructions; // Read instructions from file
    int cycle = 1;
    bool EXHazard_Rs = false;
    bool EXHazard_Rt = false;
    bool MEMHazard_Rs = false;
    bool MEMHazard_Rt = false;
    bool LoadUseHazard = false;

public:
    void excuteIF() {
        if (!instructions.empty()) {
            IF.Instruction = instructions.front();
            instructions.pop();
        }
        else {
            IF.Instruction = "";// 無指令時清空
        }

        string reg1, reg2, reg3_or_offset;
        ss << IF.Instruction;
        ss >> IF.Op;

        if (IF.Op == "lw" || IF.Op == "sw") {
            ss >> reg1 >> reg2;
            IF.Rt = stoi(reg1.substr(1));
            IF.Rs = stoi(reg2.substr(reg2.find('$') + 1, reg2.find(')') - reg2.find('(') - 2));
        }
        else if (IF.Op == "add" || IF.Op == "sub") {
            ss >> reg1 >> reg2 >> reg3_or_offset;
            IF.Rd = stoi(reg1.substr(1));
            IF.Rs = stoi(reg2.substr(1));
            IF.Rt = stoi(reg3_or_offset.substr(1));
        }
        else if (IF.Op == "beq") {
            ss >> reg1 >> reg2 >> reg3_or_offset;
            IF.Rs = stoi(reg1.substr(1));
            IF.Rt = stoi(reg2.substr(1));
        }

        ss.str("");
        ss.clear();
    }

    void excuteID() {
        if (LoadUseHazard) {
            ID.nop = true;
            return; // 插入 nop 停止
        }

        ID.nop = IF.nop;
        ss << IF.Instruction;
        ss >> ID.Op; // stringstream split string by space in default.
        if (ID.Op == "lw") {
            string regPart, memoryPart;
            ss >> regPart;
            ss.ignore();
            ss >> memoryPart;

            ID.Rt = stoi(regPart.substr(1));
            ID.Rs = stoi(memoryPart.substr(memoryPart.find('$') + 1, memoryPart.find(')') - memoryPart.find('(') - 2));
            ID.Immediate = stoi(memoryPart.substr(0, memoryPart.find('(')));
        }
        else if (ID.Op == "sw") {
            string regPart, memoryPart;
            ss >> regPart;
            ss.ignore();
            ss >> memoryPart;

            ID.Rt = stoi(memoryPart.substr(memoryPart.find('$') + 1, memoryPart.find(')') - memoryPart.find('(') - 2));
            ID.Rs = stoi(regPart.substr(1));
            ID.Immediate = stoi(memoryPart.substr(0, memoryPart.find('(')));
        }
        else if (ID.Op == "add") {
            string rd, rs, rt;
            ss >> rd >> rs >> rt;

            ID.Rd = stoi(rd.substr(1));
            ID.Rs = stoi(rs.substr(1));
            ID.Rt = stoi(rt.substr(1));
        }
        else if (ID.Op == "sub") {
            string rd, rs, rt;
            ss >> rd >> rs >> rt;

            ID.Rd = stoi(rd.substr(1));
            ID.Rs = stoi(rs.substr(1));
            ID.Rt = stoi(rt.substr(1));
        }
        else if (ID.Op == "beq") {
            string rs, rt, offset;
            ss >> rs >> rt >> offset;

            ID.Rs = stoi(rs.substr(1));
            ID.Rt = stoi(rt.substr(1));
            ID.Immediate = stoi(offset);
        }
        else if (ID.Op == "") {
        }
        else {
            throw "Unknown instruction!";
        }
        ss.str("");
        ss.clear();
        resetIF();
    }

    void excuteEX() {
        EX.nop = ID.nop;
        EX.Op = ID.Op;


        // 在 EX 階段處理 forwarding
        int forwardRsValue = registers[ID.Rs];
        int forwardRtValue = registers[ID.Rt];

        if (EXHazard_Rs) forwardRsValue = EX.ALUResult;
        else if (MEMHazard_Rs) forwardRsValue = MEM.ALUResult;

        if (EXHazard_Rt) forwardRtValue = EX.ALUResult;
        else if (MEMHazard_Rt) forwardRtValue = MEM.ALUResult;


        if (EX.RegWrite && EX.Rd != 0 && EX.Rd == ID.Rs) {
            ID.ReadData1 = EX.ALUResult; // Forward EX 結果給 ID
        }
        if (MEM.RegWrite && MEM.Rd != 0 && MEM.Rd == ID.Rs) {
            ID.ReadData1 = MEM.ReadData; // Forward MEM 結果給 ID
        }

        if (EX.Op == "beq") {
            if (forwardRsValue == forwardRtValue) {
                IF.PC += EX.Immediate; // 更新 PC。
                resetIF();
                resetID();
            }
        }



        if (EX.Op == "lw") {
            EX.Rs = ID.Rs;
            EX.Rt = ID.Rt;
            EX.Immediate = ID.Immediate;
            EX.RegDst = 0; EX.ALUSrc = 1; EX.MemtoReg = 1; EX.RegWrite = 1; EX.MemRead = 1; EX.MemWrite = 0; EX.Branch = 0;
            EX.ALUResult = forwardRsValue + EX.Immediate / 4;
        }
        else if (EX.Op == "sw") {
            EX.Rs = ID.Rs;
            EX.Rt = ID.Rt;
            EX.Immediate = ID.Immediate;
            EX.RegDst = 0; EX.ALUSrc = 1; EX.MemtoReg = 0; EX.RegWrite = 0; EX.MemRead = 0; EX.MemWrite = 1; EX.Branch = 0;
            EX.ALUResult = forwardRsValue + EX.Immediate / 4;
        }
        else if (EX.Op == "add" || EX.Op == "sub") {
            EX.Rd = ID.Rd;
            EX.Rs = ID.Rs;
            EX.Rt = ID.Rt;
            EX.RegDst = 1; EX.ALUSrc = 0; EX.MemtoReg = 0; EX.RegWrite = 1; EX.MemRead = 0; EX.MemWrite = 0; EX.Branch = 0;

            if (EX.Op == "add") {
                EX.ALUResult = forwardRsValue + forwardRtValue;
            }
            else if (EX.Op == "sub") {
                EX.ALUResult = forwardRsValue - forwardRtValue;
            }
        }
       

        resetID();
    }


    void excuteMEM() {
        
        MEM.nop = EX.nop;
        MEM.Op = EX.Op;
        MEM.Rs = EX.Rs;
        MEM.Rt = EX.Rt;
        MEM.Rd = EX.Rd;
        MEM.ALUResult = EX.ALUResult;
        MEM.RegDst = EX.RegDst;
        MEM.MemtoReg = EX.MemtoReg;
        MEM.RegWrite = EX.RegWrite;
        MEM.MemRead = EX.MemRead;
        MEM.MemWrite = EX.MemWrite;
        MEM.Branch = EX.Branch;

        if (MEM.MemRead) {
            // Load 指令：從記憶體讀取數據
            MEM.ReadData = memory[MEM.ALUResult];
        }
        else if (MEM.MemWrite) {
            // Store 指令：將資料寫入記憶體
            memory[MEM.ALUResult] = registers[MEM.Rs];
        }
        else {
        }
        
        resetEX();
    }

    void excuteWB() {
        WB.nop = MEM.nop;
        WB.Op = MEM.Op;
        WB.Rs = MEM.Rs;
        WB.Rt = MEM.Rt;
        WB.Rd = MEM.Rd;
        WB.RegDst = MEM.RegDst;
        WB.RegWrite = MEM.RegWrite;
        WB.MemtoReg = MEM.MemtoReg;

        if (WB.RegWrite) {

            if (WB.MemtoReg) {
                // 從記憶體寫回暫存器 (lw 指令)
                registers[WB.Rt] = MEM.ReadData;
            }
            else {
                // 從 ALU 結果寫回暫存器 (R-type 指令如 add/sub)
                registers[WB.Rd] = MEM.ALUResult;
            }
        }


        resetMEM();
    }

    void resetIF() { IF = IFStruct(); }
    void resetID() { ID = IDStruct(); }
    void resetEX() { EX = EXStruct(); }
    void resetMEM() { MEM = MEMStruct(); }
    void resetWB() { WB = WBStruct(); }

    void detectEXHazard() { // Should Forwarding
        if (EX.RegWrite == true && EX.Rd != 0 && EX.Rd == ID.Rs) {
            /* if (EX/MEM.RegWrite and (EX/MEM.RegisterRd ≠ 0)
            and (EX/MEM.RegisterRd = ID/EX.RegisterRs)) ForwardA = 10 */
            EXHazard_Rs = true;
        }
        else {
            EXHazard_Rs = false;
        }
        if (EX.RegWrite == true && EX.Rd != 0 && EX.Rd == ID.Rt) {
            /* if (EX/MEM.RegWrite and (EX/MEM.RegisterRd ≠ 0)
            and (EX/MEM.RegisterRd = ID/EX.RegisterRt)) ForwardB = 10 */
            EXHazard_Rt = true;
        }
        else {
            EXHazard_Rt = false;
        }
    }

    void detectMEMHazard() { // Should Forwarding
        if (MEM.RegWrite == true && MEM.Rd != 0 && MEM.Rd == ID.Rs && !EXHazard_Rs) {
            /* if (MEM/WB.RegWrite and (MEM/WB.RegisterRd ≠ 0)
            and not (EX/MEM.RegWrite and (EX/MEM.RegisterRd ≠ 0)
            and (EX/MEM.RegisterRd = ID/EX.RegisterRs))
            and (MEM/WB.RegisterRd = ID/EX.RegisterRs)) ForwardA = 01 */
            MEMHazard_Rs = true;
        }
        else {
            MEMHazard_Rs = false;
        }

        if (MEM.RegWrite == true && MEM.Rd != 0 && MEM.Rd == ID.Rt && !EXHazard_Rt) {
            /*if (MEM/WB.RegWrite and (MEM/WB.RegisterRd ≠ 0)
            and not (EX/MEM.RegWrite and (EX/MEM.RegisterRd ≠ 0)
            and (EX/MEM.RegisterRd = ID/EX.RegisterRt))
            and (MEM/WB.RegisterRd = ID/EX.RegisterRt)) ForwardB = 01*/
            MEMHazard_Rt = true;
        }
        else {
            MEMHazard_Rt = false;
        }
    }

    void detectLoadUseHazard() { // Should Stall (unavoidable)
        if (EX.Op == "lw" && (EX.Rt == ID.Rs || EX.Rt == ID.Rt || ID.Op == "beq" || ID.Op == "sw")) {
            LoadUseHazard = true;
            
        }
        else if ((EX.Op == "add" || EX.Op == "sub") && (EX.Rt == ID.Rs || EX.Rt == ID.Rt || ID.Op == "sw")) {
            LoadUseHazard = false;
        }
       else if (MEM.Op == "lw" && (MEM.Rd == ID.Rs || MEM.Rd == ID.Rt || ID.Op == "beq")) {
            LoadUseHazard = true;
        }
       else if ((EX.Op == "add" || EX.Op == "sub") && (EX.Rt == ID.Rs || EX.Rt == ID.Rt || ID.Op == "beq")) {
            LoadUseHazard = true;
        }
        else {
            LoadUseHazard = false;
        }
    
    }


    void excute() { // Forward implement, backword pull data.
        detectLoadUseHazard();
        detectEXHazard();
        detectMEMHazard();
        excuteWB();
        excuteMEM();
        if (!LoadUseHazard) { // 如果沒有 Load-Use Hazard 才執行解碼和取指
            excuteEX();
            excuteID();
            excuteIF();
        }
    }

    void printState() {
        cout << "Clock Cycle " << cycle << ":\n";
        if (WB.Op != "") {
            cout << WB.Op << " WB ";
            if (WB.Op == "sw" || WB.Op == "beq") {
                cout << "RegWrite=" << WB.RegWrite << " MemToReg=X" << "\n";
            }
            else {
                cout << "RegWrite=" << WB.RegWrite << " MemToReg=" << WB.MemtoReg << "\n";
            }
        }
        if (MEM.Op != "") {
            cout << MEM.Op << " MEM ";
            if (MEM.Op == "sw" ) {
                cout << "Branch=" << MEM.Branch << " MemRead=" << MEM.MemRead << " MemWrite=" << MEM.MemWrite;
                cout << " RegWrite=" << MEM.RegWrite << " MemToReg= X" << "\n";
            }
            else if (MEM.Op == "beq") {
                cout << "Branch=1" << " MemRead=" << MEM.MemRead << " MemWrite=" << MEM.MemWrite;
                cout << " RegWrite=" << MEM.RegWrite << " MemToReg= X" << "\n";
            }
            else {
                cout << "Branch=" << MEM.Branch << " MemRead=" << MEM.MemRead << " MemWrite=" << MEM.MemWrite;
                cout << " RegWrite=" << MEM.RegWrite << " MemToReg=" << MEM.MemtoReg << "\n";
            }
        }
        if (EX.Op != "") {
            cout << EX.Op << " EX ";
            if (EX.Op == "sw" || EX.Op == "beq") {
                cout << "RegDst=X" << " ALUSrc=" << EX.ALUSrc << " Branch=" << EX.Branch;
                cout << " MemRead=" << EX.MemRead << " MemWrite=" << EX.MemWrite << " RegWrite=" << EX.RegWrite;
                cout << " MemToReg=X" << "\n";
            }
            else {
                cout << "RegDst=" << EX.RegDst << " ALUSrc=" << EX.ALUSrc << " Branch=" << EX.Branch;
                cout << " MemRead=" << EX.MemRead << " MemWrite=" << EX.MemWrite << " RegWrite=" << EX.RegWrite;
                cout << " MemToReg=" << EX.MemtoReg << "\n";

            }

        }
        if (ID.Op != "") {
            
            cout << ID.Op << " ID\n"; 
           
        }
        if (IF.Instruction != "" && !IF.nop) {
            string IFInstruction;
            ss << IF.Instruction;
            ss >> IFInstruction;
            cout << IFInstruction << " IF\n";
            ss.str("");
            ss.clear();
        }
    }

    void printFinal() {
        cout << "\n##Final Result:\nTotal Cycles: " << cycle - 1 << "\n";

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
            if (IF.Instruction == "" && ID.Op == "" && EX.Op == "" && MEM.Op == "" && WB.Op == "") {
                break;
            }
            printState();
            cycle++;
        }
        printFinal();
    }
};

int main(void) {
    Pipeline pipeline;
    pipeline.run();
}