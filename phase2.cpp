#include <iostream>
#include <vector>
#include<string>
#include<algorithm>
#include<sstream>
#include<fstream>
#include<unordered_map>
#include<bits/stdc++.h>
using namespace std;
class Core {
public:
    unordered_map<string, int> registers; 
    int pc;
    vector<string> program;
    int flag;
    string pp[500][1000];
    int bflag;
public:
    Core() {
        // Initialize register names and indices

        for (int i = 0; i < 32; ++i) {
            registers["x" + to_string(i)] = 0;
        }
        pc = 0;
    }
    void fill(int x, int y, int If, int id, int ex, int mem, int wb) {
    try {
        while (If != 0) {
            pp[x][y] = "stall"; // if If is not equal to 0, then consider it as stall
            y++;
            If--;
        }

        pp[x][y] = "IF"; // if If is equal to 0, then consider it as IF
        y++;
        while (id != 0) {
            pp[x][y] = "stall"; // if id is not equal to 0, then consider it as stall
            y++;
            id--;
        }

        pp[x][y] = "ID"; // if id is to 0, then consider it as ID
        y++;
        while (ex != 0) {
            pp[x][y] = "stall"; // if ex is not equal to 0, then consider it as stall
            y++;
            ex--;
        }
        pp[x][y] = "EX"; // if ex is to 0, then consider it as EX
        y++;
        while (mem != 0) {
            pp[x][y] = "stall"; // if mem is not equal to 0, then consider it as stall
            y++;
            mem--;
        }
        pp[x][y] = "MEM"; // if mem is to 0, then consider it as MEM
        y++;
        while (wb != 0) {
            pp[x][y] = "stall"; // if wb is not equal to 0, then consider it as stall
            y++;
            wb--;
        }
        pp[x][y] = "WB"; // if wb is to 0, then consider it as WB
        while (pp[x][y].find("stall") != std::string::npos) {
            pp[x + 1][y] = "stall"; // if a cell is stall, then continue the whole column as stall
            x++;
        }
    } catch (const std::exception& e) {
        // for first instruction
        pp[x][y] = "IF";
        y++;
        pp[x][y] = "ID";
        y++;
        pp[x][y] = "EX";
        y++;
        pp[x][y] = "MEM";
        y++;
        pp[x][y] = "WB";
    }
}

    std::string hazard(std::string ins) {
    if (ins.substr(0, 3).find("add") != std::string::npos && ins.substr(3, 1).find("i") == std::string::npos) {
        return ins.substr(3, 2); // return destination register
    }
    if (ins.substr(0, 3).find("sub") != std::string::npos && ins.substr(3, 1).find("i") == std::string::npos) {
        return ins.substr(3, 2); // return destination register
    }
    if (ins.substr(0, 4).find("addi") != std::string::npos) {
        return ins.substr(4, 2); // return destination register
    }
    if (ins.substr(0, 4).find("subi") != std::string::npos) {
        return ins.substr(4, 2); // return destination register
    }
    if (ins.substr(0, 2).find("lw") != std::string::npos) {
        return ins.substr(2, 2); // return destination register
    }
    if (ins.substr(0, 2).find("sw") != std::string::npos) {
        return ins.substr(2, 2); // return destination register
    }
    if (ins.substr(0, 3).find("slt") != std::string::npos) {
        return ins.substr(3, 2); // return destination register
    }
    if (ins.substr(0, 3).find("sgt") != std::string::npos) {
        return ins.substr(3, 2); // return destination register
    }
    if (ins.substr(0, 2).find("li") != std::string::npos) {
        return ins.substr(2, 2); // return destination register
    }

    return "null";
}
bool branchhazard(std::string ins) {
    bool flag = false;
    if (ins.substr(0, 3) == "bne" || ins.substr(0, 3) == "jal") {
        flag = true; // true if it is branch instruction
    }
    return flag;
}

void hazard1(int row) {
    int IF = 0, ID = 0, EX = 0, MEM = 0;
    int clk = 0;
    for (int j = 1; j < 1000; j++) {
        if (!pp[row][j].empty()) {
            if (pp[row][j].find("WB") != std::string::npos) {
                clk = j; // index at which WB is seen
            }
        }
    }
    for (int j = 1; j < clk; j++) {
        if (!pp[row][j].empty()) {
            if (pp[row][j].find("IF") != std::string::npos) {
                IF = j; // index at which IF is seen
            }
            if (pp[row][j].find("ID") != std::string::npos) {
                ID = j; // index at which ID is seen
            }
            if (pp[row][j].find("EX") != std::string::npos) {
                EX = j; // index at which EX is seen
            }
            if (pp[row][j].find("MEM") != std::string::npos) {
                MEM = j; // index at which MEM is seen
            }
        }
    }
    for (int j = IF + 1; j < ID; j++) {
        if (pp[row][j].find("stall") != std::string::npos) {
            pp[row + 1][j] = "stall"; // if a cell is stall, then continue the whole column as stall
        }
    }
    for (int j = ID + 1; j < EX; j++) {
        if (pp[row][j].find("stall") != std::string::npos) {
            pp[row + 1][j] = "stall"; // if a cell is stall, then continue the whole column as stall
        }
    }
    for (int j = EX + 1; j < MEM; j++) {
        if (pp[row][j].find("stall") != std::string::npos) {
            pp[row + 1][j] = "stall"; // if a cell is stall, then continue the whole column as stall
        }
    }
    for (int j = MEM + 1; j < clk; j++) {
        if (pp[row][j].find("stall") != std::string::npos) {
            pp[row + 1][j] = "stall"; // if a cell is stall, then continue the whole column as stall
        }
    }
}
      void pipeline(int row, int flag, vector<string> &program)
      {
        int clk=1;
        int j=0;
        pp[row][0]=program[pc];
        for(int i=0;i<row;i++)
        {
            j=clk;
            if(pp[i][0]!="null")
            {
                if (pp[i][0].substr(0, 4).find("addi") != std::string::npos)
                {
                    if(i!=0 && pp[i][0].substr(6,8).find(hazard(pp[i-1][0])) )
                    {
                        if(flag==0)
                        {
                            hazard1(i-1);
                             if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 2, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 2, 0, 0);
                            }
                        }
                         else {// forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 0, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 0, 0, 0);
                            }
                        }
                    }
                    else if (i == 0) {
                        fill(i, j, 0, 0, 0, 0, 0);
                    }
                    else{
                        hazard1(i-1);
                        if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                            fill(i, j, 1, 0, 0, 0, 0);
                        } else {
                            fill(i, j, 0, 0, 0, 0, 0);
                        }
                    }
                }
                if (pp[i][0].substr(0, 3).find("sub") && !pp[i][0].substr(3, 4).find("i")) {
                    // if current instruction is sub
                    if (pp[i][0].substr(5, 7).find(hazard(pp[i - 1][0]))
                            || pp[i][0].substr(7, 9).find(hazard(pp[i - 1][0]))) {
                        // if the source register is destination register for previous instruction
                        if (flag == 0) {// non-forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 2, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 2, 0, 0);
                            }

                        } else {// forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 0, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 0, 0, 0);
                            }
                        }
                    } else {
                        hazard1(i - 1);
                        if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                            fill(i, j, 1, 0, 0, 0, 0);
                        } else {
                            fill(i, j, 0, 0, 0, 0, 0);
                        }
                    }
                }
                if (pp[i][0].substr(0, 3).find("slt")) {
                    // if current instruction is slt
                    if (pp[i][0].substr(5, 7).find(hazard(pp[i - 1][0]))
                            || pp[i][0].substr(7, 9).find(hazard(pp[i - 1][0]))) {
                        // if the source register is destination register for previous instruction
                        if (flag == 0) {// non-forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 2, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 2, 0, 0);
                            }
                        } else {// forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 0, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 0, 0, 0);
                            }
                        }
                    } else {
                        hazard1(i - 1);
                        if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                            fill(i, j, 1, 0, 0, 0, 0);
                        } else {
                            fill(i, j, 0, 0, 0, 0, 0);
                        }

                    }
                }
                if (pp[i][0].substr(0, 3).find("sgt")) {
                    // if current instruction is sgt
                    if (pp[i][0].substr(5, 7).find(hazard(pp[i - 1][0]))
                            || pp[i][0].substr(7, 9).find(hazard(pp[i - 1][0]))) {
                        // if the source register is destination register for previous instruction
                        if (flag == 0) {// non-forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 2, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 2, 0, 0);
                            }
                        } else {// forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 0, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 0, 0, 0);
                            }
                        }
                    } else {
                        hazard1(i - 1);
                        if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                            fill(i, j, 1, 0, 0, 0, 0);
                        } else {
                            fill(i, j, 0, 0, 0, 0, 0);
                        }

                    }
                }
                if (pp[i][0].substr(0, 3).find("bne")) {
                    // if current instruction is bne
                    int pc = 0;
                    bflag = 0;
                    for (int j1 = 0; j1 < arr1.length(); j1++) {
                        if (pp[i][0].find(arr1[j]))
                            pc = j;
                    }
                    if (pp[i + 1][0] != "null") {
                        if (!pp[i + 1][0].find(arr1[pc + 1]))
                            bflag = 1;
                        else
                            bflag = 0;
                    }

                    if (pp[i][0].substr(3, 5).find(hazard(pp[i - 1][0]))
                            || pp[i][0].substr(5, 7).find(hazard(pp[i - 1][0]))) {
                        // if the source register is destination register for previous instruction
                        if (flag == 0) {// non-forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 2, 0, 0);
                            }

                            else {
                                fill(i, j, 0, 0, 2, 0, 0);
                            }
                        } else {// forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 0, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 0, 0, 0);
                            }
                        }

                    } else {
                        hazard1(i - 1);
                        if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                            fill(i, j, 1, 0, 0, 0, 0);
                        }

                        else {
                            fill(i, j, 0, 0, 0, 0, 0);
                        }

                    }

                }
                if (pp[i][0].substr(0, 2).find("lw")) {
                    // if current instruction is lw
                    if (i != 0 && pp[i][0].substr(pp[i][0].length() - 3,
                            pp[i][0].length() - 1).contains(hazard(pp[i - 1][0]))) {
                        // if the source register is destination register for previous instruction
                        if (flag == 0) {// non-forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 2, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 2, 0, 0);
                            }
                        } else {// forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 0, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 0, 0, 0);
                            }
                        }
                    } else {
                        hazard1(i - 1);
                        if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                            fill(i, j, 1, 0, 0, 0, 0);
                        } else {
                            fill(i, j, 0, 0, 0, 0, 0);
                        }
                    }
                }
                if (pp[i][0].substr(0, 2).find("sw")) {
                    // if current instruction is sw
                    if (i != 0 && pp[i][0].substr(pp[i][0].length() - 3,
                            pp[i][0].length() - 1).find(hazard(pp[i - 1][0]))) {
                        // if the source register is destination register for previous instruction
                        if (flag == 0) {// non-forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && flag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 2, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 2, 0, 0);
                            }
                        } else {// forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 1, 0, 0, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 0, 0, 0);
                            }
                        }
                    } else {
                        hazard1(i - 1);
                        if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                            fill(i, j, 1, 0, 0, 0, 0);
                        } else {
                            fill(i, j, 0, 0, 0, 0, 0);
                        }
                    }
                }
                if (pp[i][0].substr(0, 2).find("li")) {
                    // if current instruction is li
                    if (i != 0) {
                        if (flag == 0) {// non-forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 0, 0, 0, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 0, 0, 0);
                            }
                        } else {// forwarding
                            hazard1(i - 1);
                            if (branchhazard(pp[i - 1][0]) && bflag == 1) {// if branch instruction
                                fill(i, j, 0, 0, 0, 0, 0);
                            } else {
                                fill(i, j, 0, 0, 0, 0, 0);
                            }
                        }
                    } else {
                        fill(i, j, 0, 0, 0, 0, 0);
                    }
                }

            }

            }
        }
          
      }

    void execute(vector<int> &memory) {
      while (pc < program.size()) {
        pipeline(pc,flag,program);
        string instruction = program[pc];
        if (instruction == "exit") {
            //Print register values after sorting
            cout << "Register Values after sorting:" << endl;
            for (auto it = registers.begin(); it != registers.end(); ++it) {
                cout << it->first << " = " << it->second <<" ";
            }
            cout<<endl;
            return;
        }
           stringstream ss(instruction);
        vector<string> parts;
        string part;

        while (getline(ss, part, ' ')) {
            parts.push_back(part);
        }
     
        string opcode = parts[0];
        //cout<<opcode<<endl;
        if (opcode == "sub") {
            string rd = parts[1];
            string rs1 = parts[2];
            string rs2 = parts[3];
            
            // Perform subtraction
           registers[rd]=registers[rs1]-registers[rs2];
        } 
        else if(opcode=="blt")
        {
                string rs1 = parts[1]; // Source register 1
         string rs2 = parts[2]; // Source register 2
         string label = parts[3]; // Label to branch to if rs1 < rs2
         if (registers[rs1] < registers[rs2]) {
        auto it = find(program.begin(), program.end(), label);
        if (it != program.end()) {
            pc = distance(program.begin(), it)-1;
        } 
        }
        }
        else if(opcode=="loope"||opcode=="swap"||opcode=="min_ind"||opcode=="outerloop"||opcode=="innerloop"||opcode=="swap"||opcode=="leave"|| opcode =="loop1"|| opcode =="loop"|| opcode =="loop2"|| opcode=="noswap")
        {
            
        }
        else if(opcode=="bgt")
        {
            string rs1 = parts[1];
                string rs2 = parts[2];
                string label = parts[3];
                if (registers[rs1] > registers[rs2]) {
                    auto it = find(program.begin(), program.end(), label);
                    if (it != program.end()) {
                        pc = distance(program.begin(), it) - 1;
                    }
                }
        }
        else if(opcode=="mv")
        {
           string rd = parts[1]; // Destination register
          string rs = parts[2]; // Source register
          registers[rd] = registers[rs];

        }
           else if(opcode=="bge")
        {
            string rs1 = parts[1];
                string rs2 = parts[2];
                string label = parts[3];
                if (registers[rs1] >= registers[rs2]) {
                    auto it = find(program.begin(), program.end(), label);
                    if (it != program.end()) {
                        pc = distance(program.begin(), it) - 1;
                    }
                }
        }
        else if(opcode=="li")
        {
            string rd=parts[1];
            registers[rd]=stoi(parts[2]);
        }
       else if(opcode=="addi")
       {
          string rd = parts[1];
          string rs1 = parts[2];
          registers[rd]=registers[rs1]+stoi(parts[3]);
       }
        else if(opcode=="subi")
       {
          string rd = parts[1];
          string rs1 = parts[2];
          registers[rd]=registers[rs1]-stoi(parts[3]);
       }
       else if(opcode=="lw")
       {
          string rd = parts[1]; // Destination register
          string address = parts[2]; // Memory address
           size_t openBracketPos = address.find('(');
          size_t closeBracketPos = address.find(')');
          if (openBracketPos != string::npos && closeBracketPos != string::npos) {
        // Extracting the register name from the address string
        string rs = address.substr(openBracketPos + 1, closeBracketPos - openBracketPos - 1);
        // Extracting the offset from the address string
        string offsetStr = address.substr(0, openBracketPos);
        int offset = stoi(offsetStr);
        // Calculating the effective address by adding the offset to the value in the register
        int effectiveAddress = registers[rs] + offset;
        // Loading the value from memory at the effective address into the destination register
        registers[rd] = memory[effectiveAddress];
          } 
       }
       else if(opcode=="sw")
       {
           string rs = parts[1]; // Source register
            string address = parts[2]; // Memory address
          size_t openBracketPos = address.find('(');
          size_t closeBracketPos = address.find(')');
        if (openBracketPos != string::npos && closeBracketPos != string::npos) {
        // Extracting the destination register name from the address string
        string rd = address.substr(openBracketPos + 1, closeBracketPos - openBracketPos - 1);
        // Extracting the offset from the address string
        string offsetStr = address.substr(0, openBracketPos);
        int offset = stoi(offsetStr);
        // Calculating the effective address by adding the offset to the value in the register
        int effectiveAddress = registers[rd] + offset;
        // Storing the value from the source register into memory at the effective address
        memory[effectiveAddress] = registers[rs];
          }
       }
         else if(opcode=="slt")
         {
             string rd = parts[1]; // Destination register
             string rs1 = parts[2]; // Source register 1
             string rs2 = parts[3]; // Source register 2

    // Set the destination register to 1 if source register 1 < source register 2, otherwise set it to 0
            registers[rd] = (registers[rs1] < registers[rs2]) ? 1 : 0;
         }
         else if(opcode=="j")
         {
               string label = parts[1]; // Target label
            auto it = find(program.begin(), program.end(), label);
            if (it != program.end()) {
                pc = distance(program.begin(), it)-1;
            }
         }
           else if(opcode=="bne")
         {
            string rs1=parts[1];
            string rs2=parts[2];
            string label=parts[3];
            if(registers[rs1]!=registers[rs2])
            {
                 auto it = find(program.begin(), program.end(), label);
            if (it != program.end()) {
                pc = distance(program.begin(), it)-1;
            }
            }
         }
          
          //cout<<program[pc]<<endl;
          pc+=1;
        }
       
    }
};
class Processor {
public:
    vector<int> memory;
   // int clock;
    vector<Core> cores;

public:
    Processor()
    {
        memory = vector<int>(4096, 0);
       //clock = 0;
       cores = vector<Core>(2);
    }
    void send(vector<string> & program,int coreval)
    {
       cores[coreval].program=program;
    }
    void run() {
        for(int i=0;i<2;i++)
        {
          cores[i].execute(memory);

        }
        return;
    }
};

int main()
{
    Processor sim;
    ifstream bubble_input("bubblesort.asm");
    if (!bubble_input.is_open()) {
        cerr << "Failed to open bubblesort.asm" << endl;
        return 1;
    }

    string bubble_line;
    vector<string> bubble_asmLines;
    //vector<string> temp;
    vector<int> bubble_values;
    bool dataSection1 = false;

    while (getline(bubble_input, bubble_line)) {
        if (!bubble_line.empty()) {
            if (bubble_line == ".data" || bubble_line == ".text" || bubble_line == "main:") {
               // temp.push_back(line);
                continue;
            } 
            else if (bubble_line.find(".word") != string::npos) {
                size_t pos = bubble_line.find(".word");
                if (pos != string::npos) {
                    // Extract the substring after ".word"
                    string valuesStr = bubble_line.substr(pos + 6); // 6 is the length of ".word" plus a space

                    // Create a string stream to parse the values
                    istringstream iss(valuesStr);
                    int value;

                    // Read each value and store it in the vector
                    while (iss >> value) {
                        bubble_values.push_back(value);
                    }
                }
            } else {
                // Process other lines
                size_t pos = bubble_line.find(":");
                if (pos != string::npos) {
                    // If a colon is found, extract the word before the colon
                    string word = bubble_line.substr(0, pos);
                    bubble_asmLines.push_back(word);
                } else {
                    // If no colon is found, simply add the line to asmLines
                    bubble_asmLines.push_back(bubble_line);
                }
            }
        }
    }
    cout << endl;
    bubble_input.close();

    ifstream selection_input("selectionsort.asm");
    if (!selection_input.is_open()) {
        cerr << "Failed to open" << endl;
        return 1;
    }

    string selection_line;
    vector<string> selection_asmLines;
    //vector<string> temp;
    vector<int> selection_values;
    bool dataSection2 = false;

    while (getline(selection_input,selection_line)) {
        if (!selection_line.empty()) {
            if (selection_line == ".data" || selection_line == ".text" || selection_line == "main:") {
               // temp.push_back(line);
                continue;
            } 
            else if (selection_line.find(".word") != string::npos) {
                size_t pos = selection_line.find(".word");
                if (pos != string::npos) {
                    // Extract the substring after ".word"
                    string valuesStr = selection_line.substr(pos + 6); // 6 is the length of ".word" plus a space

                    // Create a string stream to parse the values
                    istringstream iss(valuesStr);
                    int value;

                    // Read each value and store it in the vector
                    while (iss >> value) {
                        selection_values.push_back(value);
                    }
                }
            } else {
                // Process other lines
                size_t pos = selection_line.find(":");
                if (pos != string::npos) {
                    // If a colon is found, extract the word before the colon
                    string word =selection_line.substr(0, pos);
                    selection_asmLines.push_back(word);
                } else {
                    // If no colon is found, simply add the line to asmLines
                    selection_asmLines.push_back(selection_line);
                }
            }
        }
    }
    selection_input.close();

    for(int i=0;i<selection_values.size();i++)
    {
       sim.memory[i]=selection_values[i];
    }
    for(int i = 0; i < bubble_values.size(); i++) {
    sim.memory[i + selection_values.size()] = bubble_values[i];
}
     sim.send(selection_asmLines, 0); // Load bubble sort program into core 0
     sim.send(bubble_asmLines, 1);
     sim.run();
     cout<<"memory values:"<<" ";
     for(int i=0;i<9;i++)
     {
        cout<<sim.memory[i]<<" ";
     }
     cout<<endl;
     cout << "selection Sort Result: ";
    for (int i = 0; i < selection_values.size(); i++) {
        cout << sim.memory[i] << " ";
    }
    cout << endl;

    // Print selection sort result
    cout << "Bubble Sort Result: ";
    for (int i = 0; i < bubble_values.size(); i++) {
        cout << sim.memory[i + selection_values.size()] << " ";
    }
    cout << endl;

return 0;

}
    
