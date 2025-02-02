#include <bits/stdc++.h>
class Core
{
public:
    std::unordered_map<std::string, int> registers;
    int pc;
    std::vector<std::string> program;
    std::string pp[500][10000]; // pipeline is taken as 2D array
    int flag1;
    int ppRow;  // row value in 2D array
    int clockk; // after every ins clockk gets updated based on where IF is present in prev ins

public:
    Core()
    {
        // Initialize register names and indices

        for (int i = 0; i < 32; ++i)
        {
            registers["x" + std::to_string(i)] = 0;
        }
        pc = 0;
        flag1 = 0; // always branch is assumed to be not taken..
        int ppRow = 0;
        clockk = 1;
    }
    int execute(std::vector<int> &memory, int flag, std::map<std::string, int> latencies)
    {
        int ppRow = 0;
        while (pc < program.size())
        {
            std::string instruction = program[pc];
            
            if (instruction == "exit")
            {
                // Print register values after sorting

                std::cout << "Register Values after sorting:" << std::endl
                          << std::endl;
                for (auto it = registers.begin(); it != registers.end(); ++it)
                {
                    std::cout << it->first << " = " << it->second << " ";
                }
                std::cout << std::endl
                          << std::endl;
                pp[ppRow][0] = "exit";
                return ppRow;
            }
            std::stringstream ss(instruction);
            std::vector<std::string> parts;
            std::string part;

            while (getline(ss, part, ' '))
            {
                parts.push_back(part);
            }

            std::string opcode = parts[0];
            // cout<<opcode<<endl;
            if (opcode == "sub")
            {
                std::string rd = parts[1];
                std::string rs1 = parts[2];
                std::string rs2 = parts[3];

                // Perform subtraction
                registers[rd] = registers[rs1] - registers[rs2];

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
            if (opcode == "add")
            {
                std::string rd = parts[1];
                std::string rs1 = parts[2];
                std::string rs2 = parts[3];

                // Perform subtraction
                registers[rd] = registers[rs1] + registers[rs2];

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
            else if (opcode == "mul")
            {
                std::string rd = parts[1];
                std::string rs1 = parts[2];
                std::string rs2 = parts[3];

                // Perform multiplication
                registers[rd] = registers[rs1] * registers[rs2];

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }

            else if (opcode == "div")
            {
                std::string rd = parts[1];
                std::string rs1 = parts[2];
                std::string rs2 = parts[3];

                // Perform division
                if (registers[rs2] != 0)
                {
                    registers[rd] = registers[rs1] / registers[rs2];
                }
                else
                {
                    // Handle division by zero
                    std::cout << "Error: Division by zero" << std::endl;
                    // Optionally, set rd to a default value or handle the error in a different way
                }

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }

            else if (opcode == "blt")
            {
                std::string rs1 = parts[1];   // Source register 1
                std::string rs2 = parts[2];   // Source register 2
                std::string label = parts[3]; // Label to branch to if rs1 < rs2
                if (registers[rs1] < registers[rs2])
                {
                    auto it = find(program.begin(), program.end(), label);
                    if (it != program.end())
                    {
                        pc = distance(program.begin(), it) - 1;
                    }
                }

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
            else if (opcode == "loope" || opcode == "swap" || opcode == "min_ind" || opcode == "outerloop" || opcode == "innerloop" || opcode == "swap" || opcode == "leave" || opcode == "loop1" || opcode == "loop" || opcode == "loop2" || opcode == "noswap")
            {
                pc += 1;
            }
            else if (opcode == "bgt")
            {
                std::string rs1 = parts[1];
                std::string rs2 = parts[2];
                std::string label = parts[3];
                if (registers[rs1] > registers[rs2])
                {
                    auto it = find(program.begin(), program.end(), label);
                    if (it != program.end())
                    {
                        pc = distance(program.begin(), it) - 1;
                    }
                }

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
            else if (opcode == "mv")
            {
                std::string rd = parts[1]; // Destination register
                std::string rs = parts[2]; // Source register
                registers[rd] = registers[rs];

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
            else if (opcode == "bge")
            {
                std::string rs1 = parts[1];
                std::string rs2 = parts[2];
                std::string label = parts[3];
                if (registers[rs1] >= registers[rs2])
                {
                    auto it = find(program.begin(), program.end(), label);
                    if (it != program.end())
                    {
                        pc = distance(program.begin(), it) - 1;
                    }
                }

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
            else if (opcode == "li")
            {
                std::string rd = parts[1];
                registers[rd] = std::stoi(parts[2]);

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
            else if (opcode == "addi")
            {
                std::string rd = parts[1];
                std::string rs1 = parts[2];
                registers[rd] = registers[rs1] + stoi(parts[3]);

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
            else if (opcode == "subi")
            {
                std::string rd = parts[1];
                std::string rs1 = parts[2];
                registers[rd] = registers[rs1] - stoi(parts[3]);

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
            else if (opcode == "lw")
            {
                std::string rd = parts[1];      // Destination register
                std::string address = parts[2]; // Memory address
                size_t openBracketPos = address.find('(');
                size_t closeBracketPos = address.find(')');
                if (openBracketPos != std::string::npos && closeBracketPos != std::string::npos)
                {
                    // Extracting the register name from the address string
                    std::string rs = address.substr(openBracketPos + 1, closeBracketPos - openBracketPos - 1);
                    // Extracting the offset from the address string
                    std::string offsetStr = address.substr(0, openBracketPos);
                    int offset = std::stoi(offsetStr);
                    // Calculating the effective address by adding the offset to the value in the register
                    int effectiveAddress = registers[rs] + offset;
                    // Loading the value from memory at the effective address into the destination register
                    registers[rd] = memory[effectiveAddress];
                }

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
            else if (opcode == "sw")
            {
                std::string rs = parts[1];      // Source register
                std::string address = parts[2]; // Memory address
                size_t openBracketPos = address.find('(');
                size_t closeBracketPos = address.find(')');
                if (openBracketPos != std::string::npos && closeBracketPos != std::string::npos)
                {
                    // Extracting the destination register name from the address string
                    std::string rd = address.substr(openBracketPos + 1, closeBracketPos - openBracketPos - 1);
                    // Extracting the offset from the address string
                    std::string offsetStr = address.substr(0, openBracketPos);
                    int offset = std::stoi(offsetStr);
                    // Calculating the effective address by adding the offset to the value in the register
                    int effectiveAddress = registers[rd] + offset;
                    // Storing the value from the source register into memory at the effective address
                    memory[effectiveAddress] = registers[rs];
                }

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
            else if (opcode == "slt")
            {
                std::string rd = parts[1];  // Destination register
                std::string rs1 = parts[2]; // Source register 1
                std::string rs2 = parts[3]; // Source register 2

                // Set the destination register to 1 if source register 1 < source register 2, otherwise set it to 0
                registers[rd] = (registers[rs1] < registers[rs2]) ? 1 : 0;

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
            else if (opcode == "j")
            {
                std::string label = parts[1]; // Target label
                auto it = find(program.begin(), program.end(), label);
                if (it != program.end())
                {
                    pc = distance(program.begin(), it) - 1;
                }

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
            else if (opcode == "bne")
            {
                std::string rs1 = parts[1];
                std::string rs2 = parts[2];
                std::string label = parts[3];
                if (registers[rs1] != registers[rs2])
                {
                    auto it = find(program.begin(), program.end(), label);
                    if (it != program.end())
                    {
                        pc = distance(program.begin(), it) - 1;
                    }
                }

                pp[ppRow][0] = instruction;
                execute_ins(ppRow, flag, latencies);
                ppRow++;
                pc += 1;
            }
        }
    }
    // storing IF,ID etc..in pipeline
    void store_pp(int x, int y, int iF, int id, int ex, int mem, int wb)
    {
        while (iF != 0)
        {
            if (pp[x][y] == "stall")
            {
                y++; // when it sees stall it moves to position after stall..
            }
            else
            {
                pp[x][y] = "stall";
                y++;
                iF--;
            }
        }
        while (pp[x][y] == "stall")
        {
            y++;
        }

        pp[x][y] = "IF"; // fills the location with IF
        y++;
        while (id != 0)
        {
            if (pp[x][y] == "stall")
            {
                y++;
            }
            else
            {
                pp[x][y] = "stall";
                y++;
                id--;
            }
        }
        while (pp[x][y] == "stall")
        {
            y++;
        }
        pp[x][y] = "ID";
        y++;
        while (ex != 0)
        {
            if (pp[x][y] == "stall")
            {
                y++;
            }
            else
            {
                pp[x][y] = "stall";
                y++;
                ex--;
            }
        }
        while (pp[x][y] == "stall")
        {
            y++;
        }
        pp[x][y] = "EX";
        y++;
        while (mem != 0)
        {
            if (pp[x][y] == "stall")
            {
                y++;
            }
            else
            {
                pp[x][y] = "stall";
                y++;
                mem--;
            }
        }
        while (pp[x][y] == "stall")
        {
            y++;
        }
        pp[x][y] = "MEM";
        y++;
        while (wb != 0)
        {
            if (pp[x][y] == "stall")
            {
                y++;
            }
            else
            {
                pp[x][y] = "stall";
                y++;
                wb--;
            }
        }
        while (pp[x][y] == "stall")
        {
            y++;
        }
        pp[x][y] = "WB";
    }
    // function for storing arithmetic instructions in pipeline based on latencies..
    void fillarith(int x, int y, int iF, int id, int ex, int mem, int wb, int latency_val)
    {
        while (iF != 0)
        {
            if (pp[x][y] == "stall")
            {
                y++;
            }
            else
            {
                pp[x][y] = "stall";
                y++;
                iF--;
            }
        }

        while (pp[x][y] == "stall")
        {
            y++;
        }

        pp[x][y] = "IF";
        y++;

        while (id != 0)
        {
            if (pp[x][y] == "stall")
            {
                y++;
            }
            else
            {
                pp[x][y] = "stall";
                y++;
                id--;
            }
        }

        while (pp[x][y] == "stall")
        {
            y++;
        }

        pp[x][y] = "ID";
        y++;

        while (ex != 0)
        {
            if (pp[x][y] == "stall")
            {
                y++;
            }
            else
            {
                pp[x][y] = "stall";
                y++;
                ex--;
            }
        }

        while (pp[x][y] == "stall")
        {
            y++;
        }

        pp[x][y] = "EX";
        y++;

        // Add latency for the EX stage based on the instruction type
        for (int i = 0; i < latency_val; ++i)
        {
            pp[x][y] = "stall";
            y++;
        }

        while (mem != 0)
        {
            if (pp[x][y] == "stall")
            {
                y++;
            }
            else
            {
                pp[x][y] = "stall";
                y++;
                mem--;
            }
        }

        while (pp[x][y] == "stall")
        {
            y++;
        }

        pp[x][y] = "MEM";
        y++;

        while (wb != 0)
        {
            if (pp[x][y] == "stall")
            {
                y++;
            }
            else
            {
                pp[x][y] = "stall";
                y++;
                wb--;
            }
        }

        while (pp[x][y] == "stall")
        {
            y++;
        }

        pp[x][y] = "WB";
    }
    // checkHazard fn returns destination register...
    std::string checkHazard(std::string ins)
    {

        if (ins.substr(0, 4) == "addi")
        {
            return ins.substr(5, 2);
        }
        if (ins.substr(0, 3) == "add" && (ins.substr(3, 1) != "i"))
        {
            return ins.substr(4, 2);
        }
        if (ins.substr(0, 3) == "sub")
        {
            return ins.substr(4, 2);
        }
        if (ins.substr(0, 3) == "mul")
        {
            return ins.substr(4, 2);
        }
        if (ins.substr(0, 3) == "div")
        {
            return ins.substr(4, 2);
        }
        if (ins.substr(0, 3) == "slt")
        {
            return ins.substr(4, 2);
        }
        if (ins.substr(0, 2) == "lw")
        {
            return ins.substr(2, 2);
        }
        if (ins.substr(0, 2) == "sw")
        {
            return ins.substr(3, 2);
        }

        return "false";
        // if ..... other functions
    }
    // prdict fn checks if its a branch instruction and it assumes it as always taken..
    bool predict(std::string ins)
    {
        bool flag = false;
        // if it is branch instruction..then it always returns true.
        if (ins.substr(0, 3) == "beq" || ins.substr(0, 3) == "bne" || (ins.substr(0, 1) == "j" && ins.substr(1, 1) != "r"))
        {
            flag = true;
        }
        return flag;
    }
    // if prev instructions have stalls it transforms all those stalls downwards to present instruction..
    void fill_stalls(int ins_row)
    {
        int IF, ID, EX, MEM;
        int clk_len = 0;
        for (int j = 1; j < 10000; j++)
        {

            if (pp[ins_row][j] == "WB")
            {
                clk_len = j;
            }
        }
        for (int j = 1; j < clk_len; j++)
        {
            if (pp[ins_row][j] == "IF")
                IF = j;
            if (pp[ins_row][j] == "ID")
                ID = j;
            if (pp[ins_row][j] == "EX")
                EX = j;
            if (pp[ins_row][j] == "MEM")
                MEM = j;
        }
        for (int j = IF + 1; j < ID; j++)
        {
            if (pp[ins_row][j] == "stall")
            {

                pp[ins_row + 1][j] = "stall";
            }
        }
        for (int j = ID + 1; j < EX; j++)
        {
            if (pp[ins_row][j] == "stall")
            {
                pp[ins_row + 1][j] = "stall";
            }
        }
        for (int j = EX + 1; j < MEM; j++)
        {
            if (pp[ins_row][j] == "stall")
            {
                pp[ins_row + 1][j] = "stall";
            }
        }
        for (int j = MEM + 1; j < clk_len; j++)
        {
            if (pp[ins_row][j] == "stall")
            {
                pp[ins_row + 1][j] = "stall";
            }
        }
    }

    void execute_ins(int i, int flag, std::map<std::string, int> latencies)
    {
        // cout<<numb_rows<<endl;
        int j = clockk;
        if (pp[i][0].substr(0, 4) == "addi")
        {
            if (i != 0 && pp[i][0].substr(5, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    // check if previous instruction is branch instruction..
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 2, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 2, 0, 0);
                    }
                }
                else
                { // with forwarding

                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 0, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 0, 0, 0);
                    }
                }
            }

            else if (i == 0)
            {
                store_pp(i, j, 0, 0, 0, 0, 0);
            }
            else
            { // i!=0 and no checkHazard in previous instruction

                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    store_pp(i, j, 1, 0, 0, 0, 0);
                }
                else
                {
                    store_pp(i, j, 0, 0, 0, 0, 0);
                }
            }
        }
        else if (pp[i][0].substr(0, 3) == "bgt")
        {
            // Extracting the registers involved in the bgt instruction
            // cout<<"bgt"<<endl;
            std::string reg1 = pp[i][0].substr(3, 2);
            std::string reg2 = pp[i][0].substr(6, 2);

            // Check for hazards with the previous instruction
            if (i != 0 && (reg1 == checkHazard(pp[i - 1][0]) || reg2 == checkHazard(pp[i - 1][0])))
            {
                if (flag == 0)
                { // No forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 2, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 2, 0, 0);
                    }
                }
                else
                { // With forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 0, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 0, 0, 0);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    store_pp(i, j, 1, 0, 0, 0, 0);
                }
                else
                {
                    store_pp(i, j, 0, 0, 0, 0, 0);
                }
            }
        }
        else if (pp[i][0].substr(0, 4) == "subi")
        {
            std::string reg1 = pp[i][0].substr(4, 2);

            // Check for hazards with the previous instruction
            //(it checks if destination reg of prev is same as that of before ins..)
            if (i != 0 && reg1 == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // No forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 2, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 2, 0, 0);
                    }
                }
                else
                { // With forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 0, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 0, 0, 0);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    store_pp(i, j, 1, 0, 0, 0, 0);
                }
                else
                {
                    store_pp(i, j, 0, 0, 0, 0, 0);
                }
            }
        }

        else if (pp[i][0].substr(0, 3) == "add" && pp[i][0].substr(3, 1) != "i")
        {

            int latency_val = latencies["add"];
            if (pp[i][0].substr(5, 2) == checkHazard(pp[i - 1][0]) || pp[i][0].substr(7, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        fillarith(i, j, 1, 0, 2, 0, 0, latency_val);
                    }
                    else
                    {
                        fillarith(i, j, 0, 0, 2, 0, 0, latency_val);
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        fillarith(i, j, 1, 0, 0, 0, 0, latency_val);
                    }

                    else
                    {
                        fillarith(i, j, 0, 0, 0, 0, 0, latency_val);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    fillarith(i, j, 1, 0, 0, 0, 0, latency_val);
                }

                else
                {
                    fillarith(i, j, 0, 0, 0, 0, 0, latency_val);
                }
            }
        }

        else if (pp[i][0].substr(0, 3) == "sub")
        {
            int latency_val = latencies["sub"];
            // cout<<"sub"<<endl;
            if (pp[i][0].substr(5, 2) == checkHazard(pp[i - 1][0]) || pp[i][0].substr(7, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        fillarith(i, j, 1, 0, 2, 0, 0, latency_val);
                    }

                    else
                    {
                        fillarith(i, j, 0, 0, 2, 0, 0, latency_val);
                    }

                    // fill_stalls(i-1);
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        fillarith(i, j, 1, 0, 0, 0, 0, latency_val);
                    }

                    else
                    {
                        fillarith(i, j, 0, 0, 0, 0, 0, latency_val);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    store_pp(i, j, 1, 0, 0, 0, 0);
                }

                else
                {
                    store_pp(i, j, 0, 0, 0, 0, 0);
                }
            }
        }

        else if (pp[i][0].substr(0, 3) == "mul")
        {
            int latency_val = latencies["mul"];
            if (pp[i][0].substr(5, 2) == checkHazard(pp[i - 1][0]) || pp[i][0].substr(7, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        fillarith(i, j, 1, 0, 2, 0, 0, latency_val);
                    }

                    else
                    {
                        fillarith(i, j, 0, 0, 2, 0, 0, latency_val);
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        fillarith(i, j, 1, 0, 0, 0, 0, latency_val);
                    }

                    else
                    {
                        fillarith(i, j, 0, 0, 0, 0, 0, latency_val);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    fillarith(i, j, 1, 0, 0, 0, 0, latency_val);
                }

                else
                {
                    fillarith(i, j, 0, 0, 0, 0, 0, latency_val);
                }
            }
        }

        else if (pp[i][0].substr(0, 3) == "div")
        {
            int latency_val = latencies["div"];
            if (pp[i][0].substr(5, 2) == checkHazard(pp[i - 1][0]) || pp[i][0].substr(7, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        fillarith(i, j, 1, 0, 2, 0, 0, latency_val);
                    }

                    else
                    {
                        fillarith(i, j, 0, 0, 2, 0, 0, latency_val);
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        fillarith(i, j, 1, 0, 0, 0, 0, latency_val);
                    }

                    else
                    {
                        fillarith(i, j, 0, 0, 0, 0, 0, latency_val);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    fillarith(i, j, 1, 0, 0, 0, 0, latency_val);
                }
                else
                {
                    fillarith(i, j, 0, 0, 0, 0, 0, latency_val);
                }
            }
        }

        else if (pp[i][0].substr(0, 3) == "slt")
        {
            // cout<<"slt"<<" ";
            if (pp[i][0].substr(5, 2) == checkHazard(pp[i - 1][0]) || pp[i][0].substr(7, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 2, 0, 0);
                    }

                    else
                    {
                        store_pp(i, j, 0, 0, 2, 0, 0);
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 0, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 0, 0, 0);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    store_pp(i, j, 1, 0, 0, 0, 0);
                }

                else
                {
                    store_pp(i, j, 0, 0, 0, 0, 0);
                }
            }
        }
        else if (pp[i][0].substr(0, 3) == "beq")
        {
            //  cout<<"beq"<<endl;
            int pc;
            flag1 = 0;
            for (int j = 0; j < program.size(); j++)
            {
                if (pp[i][0] == program[j])
                {
                    pc = j;
                }
            }
            if (pp[i + 1][0] != program[pc + 1])
                flag1 = 1;
            else
                flag1 = 0;
            if (pp[i][0].substr(3, 2) == checkHazard(pp[i - 1][0]) || pp[i][0].substr(5, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 2, 0, 0);
                    }

                    else
                    {
                        store_pp(i, j, 0, 0, 2, 0, 0);
                        // fill_stalls(i-1);
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 0, 0, 0);
                    }

                    else
                    {
                        store_pp(i, j, 0, 0, 0, 0, 0);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    store_pp(i, j, 1, 0, 0, 0, 0);
                }

                else
                {
                    store_pp(i, j, 0, 0, 0, 0, 0);
                }
            }
        }
        else if (pp[i][0].substr(0, 3) == "bne")
        {
            // cout<<"bne"<<endl;
            int pc;
            flag1 = 0;
            for (int j = 0; j < program.size(); j++)
            {
                if (pp[i][0] == program[j])
                    pc = j;
            }
            if (pp[i + 1][0] != program[pc + 1])
                flag1 = 1;
            else
                flag1 = 0;
            if (pp[i][0].substr(3, 2) == checkHazard(pp[i - 1][0]) || pp[i][0].substr(5, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 2, 0, 0);
                    }

                    else
                    {
                        store_pp(i, j, 0, 0, 2, 0, 0);
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 0, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 0, 0, 0);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    store_pp(i, j, 1, 0, 0, 0, 0);
                }

                else
                {
                    store_pp(i, j, 0, 0, 0, 0, 0);
                }
            }
        }
        else if (pp[i][0].substr(0, 1) == "j" && pp[i][0].substr(1, 1) != "r")
        {
            //   cout<<"j"<<endl;
            int pc;
            flag1 = 0;
            for (int j = 0; j < program.size(); j++)
            {
                if (pp[i][0] == program[j])
                    pc = j;
            }
            if (pp[i + 1][0] != program[pc + 1])
                flag1 = 1;
            else
                flag1 = 0;

            fill_stalls(i - 1);
            if (predict(pp[i - 1][0]) && flag1 == 1)
            {
                store_pp(i, j, 1, 0, 0, 0, 0);
            }
            else
            {
                store_pp(i, j, 0, 0, 0, 0, 0);
            }
        }
        else if (pp[i][0].substr(0, 3) == "bge")
        {
            // cout<<"bge"<<endl;
            int pc;
            flag1 = 0;
            for (int j = 0; j < program.size(); j++)
            {
                if (pp[i][0] == program[j])
                    pc = j;
            }
            if (pp[i + 1][0] != program[pc + 1])
                flag1 = 1;
            else
                flag1 = 0;
            if (pp[i][0].substr(3, 2) == checkHazard(pp[i - 1][0]) || pp[i][0].substr(5, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 2, 0, 0);
                    }

                    else
                    {
                        store_pp(i, j, 0, 0, 2, 0, 0);
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 0, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 0, 0, 0);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    store_pp(i, j, 1, 0, 0, 0, 0);
                }

                else
                {
                    store_pp(i, j, 0, 0, 0, 0, 0);
                }
            }
        }

        else if (pp[i][0].substr(0, 2) == "lw")
        {
            //   cout<<"lw"<<endl;
            if (i != 0 && pp[i][0].substr(pp[i][0].length() - 3, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 2, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 2, 0, 0);
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 0, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 0, 0, 0);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    store_pp(i, j, 1, 0, 0, 0, 0);
                }
                else
                {
                    store_pp(i, j, 0, 0, 0, 0, 0);
                }
            }
        }

        else if (pp[i][0].substr(0, 2) == "sw")
        {
            // cout<<"sw"<<endl;
            if (i != 0 && pp[i][0].substr(pp[i][0].length() - 3, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 2, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 2, 0, 0);
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 0, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 0, 0, 0);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    store_pp(i, j, 1, 0, 0, 0, 0);
                }
                else
                {
                    store_pp(i, j, 0, 0, 0, 0, 0);
                }
            }
        }

        else if (pp[i][0].substr(0, 3) == "blt")
        {
            // Extracting the registers involved in the blt instruction
            // cout<<"blt"<<endl;
            std::string reg1 = pp[i][0].substr(3, 2);
            std::string reg2 = pp[i][0].substr(6, 2);

            // Check for hazards with the previous instruction
            if (i != 0 && (reg1 == checkHazard(pp[i - 1][0]) || reg2 == checkHazard(pp[i - 1][0])))
            {
                if (flag == 0)
                { // No forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 2, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 2, 0, 0);
                    }
                }
                else
                { // With forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 0, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 0, 0, 0);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    store_pp(i, j, 1, 0, 0, 0, 0);
                }
                else
                {
                    store_pp(i, j, 0, 0, 0, 0, 0);
                }
            }
        }
        else if (pp[i][0] == "exit")
        {
            return;
        }
        else if (pp[i][0].substr(0, 2) == "mv")
        {
            std::string reg1 = pp[i][0].substr(2, 2);
            std::string reg2 = pp[i][0].substr(5, 2);

            // Check for hazards with the previous instruction
            if (i != 0 && (reg1 == checkHazard(pp[i - 1][0]) || reg2 == checkHazard(pp[i - 1][0])))
            {
                if (flag == 0)
                { // No forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 2, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 2, 0, 0);
                    }
                }
                else
                { // With forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        store_pp(i, j, 1, 0, 0, 0, 0);
                    }
                    else
                    {
                        store_pp(i, j, 0, 0, 0, 0, 0);
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    store_pp(i, j, 1, 0, 0, 0, 0);
                }
                else
                {
                    store_pp(i, j, 0, 0, 0, 0, 0);
                }
            }
        }
        else if (pp[i][0].substr(0, 2) == "la")
        { // data and structural hazards not possible in la
            fill_stalls(i - 1);
            if (predict(pp[i - 1][0]) && flag1 == 1)
            {
                store_pp(i, j, 1, 0, 0, 0, 0);
            }
            else
            {
                store_pp(i, j, 0, 0, 0, 0, 0);
            }
        }

        else if (pp[i][0].substr(0, 2) == "jr")
        { // data and structural hazards not possible in jr
            fill_stalls(i - 1);
            if (predict(pp[i - 1][0]) && flag1 == 1)
            {
                store_pp(i, j, 1, 0, 0, 0, 0);
            }
            else
            {
                store_pp(i, j, 0, 0, 0, 0, 0);
            }
        }

        for (int q = 1; q < 10000; q++)
        {
            if (pp[i][q] == "IF")
            {
                clockk = q + 1;
            }
        }

        return;
    }

    void printval(int ppRow, int flag)
    {
        int cnt = 0;
        for (int j = 1; j < 10000; j++)
        {
            if (pp[ppRow - 1][j] == "WB")
            {
                std::cout << "Total number of clock cycles: " << j << std::endl
                          << std::endl;
                cnt = j;
            }
        }

        std::string stallInstruction[5000];
        int count = 0;
        int k = 0;
        for (int i = 0; i < ppRow; i++)
        {
            for (int j = 1; j < 10000; j++)
            {
                if (pp[i][j] == "stall")
                {
                    count++;
                    stallInstruction[k] = pp[i][0];
                }
            }
            k++;
        }

        std::cout << "Total number of stalls: " << count << std::endl
                  << std::endl;
        std::cout << "Instructions: " << ppRow << std::endl
                  << std::endl;
        float ipc = (float)ppRow / cnt;
        std::cout << "IPC(Instructions per cycle is) : " << ipc << std::endl
                  << std::endl;
        return;
    }
};
class Processor
{
public:
    std::vector<int> memory;
    // int clock;
    std::vector<Core> cores;

public:
    Processor()
    {
        memory = std::vector<int>(4096, 0);
        // clock = 0;
        cores = std::vector<Core>(2);
    }
    void send(std::vector<std::string> &program, int coreval)
    {
        cores[coreval].program = program;
    }
    void run(int flag1, int flag2, std::map<std::string, int> latencies)
    {
        int ppRow;
        ppRow = cores[0].execute(memory, flag1, latencies);
        // cores[0].execute_ins(ppRow,flag1,latencies);
        std::cout << "BUBBLE SORT: " << std::endl
                  << std::endl;
        cores[0].printval(ppRow, flag1);
        ppRow = cores[1].execute(memory, flag2, latencies);
        // cores[1].execute_ins(ppRow,flag2,latencies);
        std::cout << "SELECTION SORT: " << std::endl
                  << std::endl;
        cores[1].printval(ppRow, flag2);
        return;
    }
};

int main()
{
    Processor sim;
    std::ifstream bubble_input("bubblesort.asm");
    if (!bubble_input.is_open())
    {
        std::cerr << "Failed to open bubblesort.asm" << std::endl;
        return 1;
    }

    std::string bubble_line;
    std::vector<std::string> bubble_asmLines;
    // vector<string> temp;
    std::vector<int> bubble_values;
    bool dataSection1 = false;

    while (getline(bubble_input, bubble_line))
    {
        if (!bubble_line.empty())
        {
            if (bubble_line == ".data" || bubble_line == ".text" || bubble_line == "main:")
            {
                // temp.push_back(line);
                continue;
            }
            else if (bubble_line.find(".word") != std::string::npos)
            {
                size_t pos = bubble_line.find(".word");
                if (pos != std::string::npos)
                {
                    // Extract the substring after ".word"
                    std::string valuesStr = bubble_line.substr(pos + 6); // 6 is the length of ".word" plus a space

                    // Create a string stream to parse the values
                    std::istringstream iss(valuesStr);
                    int value;

                    // Read each value and store it in the vector
                    while (iss >> value)
                    {
                        bubble_values.push_back(value);
                    }
                }
            }
            else
            {
                // Process other lines
                size_t pos = bubble_line.find(":");
                if (pos != std::string::npos)
                {
                    // If a colon is found, extract the word before the colon
                    std::string word = bubble_line.substr(0, pos);
                    bubble_asmLines.push_back(word);
                }
                else
                {
                    // If no colon is found, simply add the line to asmLines
                    bubble_asmLines.push_back(bubble_line);
                }
            }
        }
    }
    // cout << endl;
    bubble_input.close();
    // for(int i=0;i<bubble_asmLines.size();i++)
    // {
    //     cout<<bubble_asmLines[i]<<endl;
    // }
    std::cout << std::endl;
    std::ifstream selection_input("selectionsort.asm");
    if (!selection_input.is_open())
    {
        std::cerr << "Failed to open" << std::endl;
        return 1;
    }

    std::string selection_line;
    std::vector<std::string> selection_asmLines;
    // vector<string> temp;
    std::vector<int> selection_values;
    bool dataSection2 = false;

    while (getline(selection_input, selection_line))
    {
        if (!selection_line.empty())
        {
            if (selection_line == ".data" || selection_line == ".text" || selection_line == "main:")
            {
                // temp.push_back(line);
                continue;
            }
            else if (selection_line.find(".word") != std::string::npos)
            {
                size_t pos = selection_line.find(".word");
                if (pos != std::string::npos)
                {
                    // Extract the substring after ".word"
                    std::string valuesStr = selection_line.substr(pos + 6); // 6 is the length of ".word" plus a space

                    // Create a string stream to parse the values
                    std::istringstream iss(valuesStr);
                    int value;

                    // Read each value and store it in the vector
                    while (iss >> value)
                    {
                        selection_values.push_back(value);
                    }
                }
            }
            else
            {
                // Process other lines
                size_t pos = selection_line.find(":");
                if (pos != std::string::npos)
                {
                    // If a colon is found, extract the word before the colon
                    std::string word = selection_line.substr(0, pos);
                    selection_asmLines.push_back(word);
                }
                else
                {
                    // If no colon is found, simply add the line to asmLines
                    selection_asmLines.push_back(selection_line);
                }
            }
        }
    }
    selection_input.close();

    for (int i = 0; i < selection_values.size(); i++)
    {
        sim.memory[i] = selection_values[i];
    }
    for (int i = 0; i < bubble_values.size(); i++)
    {
        sim.memory[i + selection_values.size()] = bubble_values[i];
    }
    sim.send(selection_asmLines, 1); // Load bubble sort program into core 0
    sim.send(bubble_asmLines, 0);
    int flag1;
    int flag2;
    std::cout << "Enter 1 for FORWARDING and 0 for NON FORWARDING for BUBBLE SORT: "
              << " " << std::endl;
    std::cin >> flag1;
    if (flag1 > 1 || flag1 < 0)
    {
        std::cout << "INVALID KEY ENTERED" << std::endl;
        return 0;
    }
    std::cout << "Enter 1 for FORWARDING 0 for NON FORWARDING for SELECTION SORT: "
              << " " << std::endl;
    std::cin >> flag2;
    if (flag2 > 1 || flag2 < 0)
    {
        std::cout << "INVALID KEY ENTERED" << std::endl;
        return 0;
    }
    std::map<std::string, int> latencies;
    std::cout << "Enter latencies for arithmetic operations:" << std::endl;
    std::cout << "ADD: ";
    int addLatency;
    std::cin >> addLatency;
    latencies["add"] = addLatency;

    std::cout << "SUB: ";
    int subLatency;
    std::cin >> subLatency;
    latencies["sub"] = subLatency;

    std::cout << "MUL: ";
    int mulLatency;
    std::cin >> mulLatency;
    latencies["mul"] = mulLatency;

    std::cout << "DIV: ";
    int divLatency;
    std::cin >> divLatency;
    latencies["div"] = divLatency;
    sim.run(flag1, flag2, latencies);
    std::cout << "MEMORY VALUES: "
              << " ";
    for (int i = 0; i < 9; i++)
    {
        std::cout << sim.memory[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "Selection Sort Result: " << std::endl
              << std::endl;
    for (int i = 0; i < selection_values.size(); i++)
    {
        std::cout << sim.memory[i] << " ";
    }
    std::cout << std::endl
              << std::endl;

    // Print selection sort result
    std::cout << "Bubble Sort Result: " << std::endl
              << std::endl;
    for (int i = 0; i < bubble_values.size(); i++)
    {
        std::cout << sim.memory[i + selection_values.size()] << " ";
    }
    std::cout << std::endl
              << std::endl;

    return 0;
}
