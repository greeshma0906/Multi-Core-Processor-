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

public:
    Core() {
        // Initialize register names and indices

        for (int i = 0; i < 32; ++i) {
            registers["x" + to_string(i)] = 0;
        }
        pc = 0;
    }

    void execute(vector<int> &memory) {
      while (pc < program.size()) {
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
       // cout<<pc<<endl;
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
