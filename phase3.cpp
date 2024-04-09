#include <bits/stdc++.h>
using namespace std;
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
    int missarr[500];
     int cache[1024]={0};
     int tag[1024];
     int counter[1024]={0};
     int cachesize,blocksize,associativity;
     int totalins,blockins,numblocks;
     int accesslatency,memtime;
     int miss=-1;
     double accesscache=0;
     double totalmisses=0;
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
    void assignval(int cacheSize, int blockSize,int Associativity, int accessLatency,int memTime)
    {
         cachesize=cacheSize;  
        blocksize=blockSize; 
        associativity=Associativity;
        accesslatency=accessLatency; 
        memtime=memTime;
        totalins = cacheSize/4;
        blockins=blockSize/4;
        numblocks=cacheSize/blockSize;
        for(int i=0;i<numblocks;i++){
            tag[i]=-1;
        }
        for(int i=0; i<500; i++){
            missarr[i]=-1;
        }
    }
    int nearest(int addrs){
            while(addrs%blockins!=0){
                addrs--;
            }
            return addrs;
        }

        bool search(int addrs){
            for(int i=0;i<numblocks;i++){
                if(tag[i]==nearest(addrs)){
                   return true; 
                }
            }
            return false;
        }
       int minimum(int arr[], int n){
            int min=arr[0];
            for(int i=0; i<n; i++){
                if(arr[i]<=min)
                    min=arr[i];
            }
            int j;
            for(j=0; j<n; j++){
                if(arr[j]==min)
                    break;
            }
            return j;
        }

        void incrementcounter(int adrs){
            for(int i=0;i<numblocks;i++){
                if(tag[i]==adrs){
                    counter[i]++;
                    break;
                }
            }
        } 
        void memtoCache(int addrs,vector<int>& memory){
            int j,k1;
            for(j=0;j<numblocks;j++){
                if(tag[j]==-1){
                    k1 = blockins*j;
                    tag[j]=nearest(addrs);
                    counter[j]=1;
                    for(int k=0; k<blockins; k++){
                        cache[k1]=memory[addrs];
                        k1++;
                        addrs++;
                    }
                    break;
                }
            }
            if(j==numblocks){  //if cache is full
                //lru
                int minIndex = minimum(counter, numblocks);
                j=minIndex;
                k1=blockins*j;
                //bring new block from them memory and put it cache
                tag[j]=nearest(addrs);
                counter[j]=1;
                for(int k=0; k<blockins; k++){
                    cache[k1]=memory[addrs];
                    k1++;
                    addrs++;
                }
            }
        }
        void updateInCache(int addrs, int val) {
        int j;
        for (j = 0; j < numblocks; j++) {
            if (tag[j] == nearest(addrs))
                break;
        }
        int k1 = blockins * j;
        cache[k1 + (addrs - nearest(addrs))] = val;
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
                int offset;
                string rs;
                if (openBracketPos != std::string::npos && closeBracketPos != std::string::npos)
                {
                    // Extracting the register name from the address string
                    std::string rs = address.substr(openBracketPos + 1, closeBracketPos - openBracketPos - 1);
                    // Extracting the offset from the address string
                    std::string offsetStr = address.substr(0, openBracketPos);
                     offset = std::stoi(offsetStr);
                    // Calculating the effective address by adding the offset to the value in the register
                    int effectiveAddress = registers[rs] + offset;
                    // Loading the value from memory at the effective address into the destination register
                    registers[rd] = memory[effectiveAddress];
                }

                pp[ppRow][0] = instruction;
                int adrs;
               adrs = (offset + registers[rs])/4;
              if(search(adrs) == true){//hit in L1
               miss=0;
             incrementcounter(adrs);
             accesscache++;
           }
              else{
                 miss=1; 
                  totalmisses++;
                  memtoCache(adrs,memory);
                accesscache++;
               }
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
                string rd;
                int offset;
                if (openBracketPos != std::string::npos && closeBracketPos != std::string::npos)
                {
                    // Extracting the destination register name from the address string
                    std::string rd = address.substr(openBracketPos + 1, closeBracketPos - openBracketPos - 1);
                    // Extracting the offset from the address string
                    std::string offsetStr = address.substr(0, openBracketPos);
                     offset = std::stoi(offsetStr);
                    // Calculating the effective address by adding the offset to the value in the register
                    int effectiveAddress = registers[rd] + offset;
                    // Storing the value from the source register into memory at the effective address
                    memory[effectiveAddress] = registers[rs];
                }

                pp[ppRow][0] = instruction;
                int adrs, value1;
               value1 = registers[rs];
               adrs = (offset +registers[rd])/4;
             if(search(adrs) == true) { // hit in L1
              miss = 0;
             incrementcounter(adrs);
            updateInCache(adrs, value1);
            accesscache++;
           } else {
              miss = 1;
               memtoCache(adrs,memory);
              totalmisses++;
              accesscache++;
            }
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
    int memory_hazard(int ins_row) {
    int result = 0;
    if (pp[ins_row][0].substr(0, 2) == "lw" || pp[ins_row][0].substr(0, 2) == "sw") {
        int temp_miss = missarr[ins_row];
        if (temp_miss == 0) {
            result = accesslatency - 1; // Access latency for cache
        } else {
            result = accesslatency + memtime - 1; // Access latency for cache + Memory access time
        }
        return result;
    } else {
        return result;
    }
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
                int temp=memory_hazard(i-1);
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    // check if previous instruction is branch instruction..
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                       if(temp!=0){//suppose 4 cycles temp=3
                                   store_pp(i,j,1,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,2,0,0);
                                }
                    }
                    else
                    {
                        if(temp!=-1){
                                   store_pp(i,j,0,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,2,0,0);
                                }
                    }
                }
                else
                { // with forwarding

                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        if(temp!=0){
                                   store_pp(i,j,1,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,0,0,0);
                                }
                    }
                    else
                    {
                        if(temp!=-1){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,0,0,0);
                                }
                    }
                }
            }

            else if (i == 0)
            {
                store_pp(i, j, 0, 0, 0, 0, 0);
            }
            else
            { // i!=0 and no checkHazard in previous instruction
                int temp=memory_hazard(i-1);
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    if(temp!=-1){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,temp,0,0);
                                }
                }
                else
                {
                    if(temp!=-1){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,temp,0,0);
                                }
                }
            }
        }
        else if (pp[i][0].substr(0, 3) == "bgt")
        {
            // Extracting the registers involved in the bgt instruction
            // cout<<"bgt"<<endl;
            std::string reg1 = pp[i][0].substr(3, 2);
            std::string reg2 = pp[i][0].substr(6, 2);
             int temp = memory_hazard(i - 1);

            // Check for hazards with the previous instruction
            if (i != 0 && (reg1 == checkHazard(pp[i - 1][0]) || reg2 == checkHazard(pp[i - 1][0])))
            {
                if (flag == 0)
                { // No forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        if (temp != 0) {
                    store_pp(i, j, 1, 0, temp + 2, 0, 0); // With branch hazard
                } else {
                    store_pp(i, j, 1, 0, 2, 0, 0); // With branch hazard, no memory hazard
                }
                    }
                    else
                    {
                        if (temp != -1) {
                    store_pp(i, j, 0, 0, temp + 2, 0, 0); // Without branch hazard
                } else {
                    store_pp(i, j, 0, 0, 2, 0, 0); // Without branch hazard, no memory hazard
                }
                    }
                }
                else
                { // With forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                         if (temp != 0) {
                    store_pp(i, j, 1, 0, temp, 0, 0); // With branch hazard
                } else {
                    store_pp(i, j, 1, 0, 0, 0, 0); // With branch hazard, no memory hazard
                }
                    }
                    else
                    {
                        if (temp != -1) {
                    store_pp(i, j, 0, 0, temp, 0, 0); // Without branch hazard
                } else {
                    store_pp(i, j, 0, 0, 0, 0, 0); // Without branch hazard, no memory hazard
                }
                    }
                }
            }
            else if (i == 0) {
        store_pp(i, j, 0, 0, 0, 0, 0); // No hazard in the first instruction
    }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                     if (temp != -1) {
                store_pp(i, j, 0, 0, temp, 0, 0); // With branch hazard
            } else {
                store_pp(i, j, 0, 0, temp, 0, 0); // With branch hazard, no memory hazard
            }
                }
                else
                {
                   if (temp != -1) {
                store_pp(i, j, 0, 0, temp, 0, 0); // Without branch hazard
            } else {
                store_pp(i, j, 0, 0, 0, 0, 0); // Without branch hazard, no memory hazard
            }
                }
            }
        }
        else if (pp[i][0].substr(0, 4) == "subi")
        {
            std::string reg1 = pp[i][0].substr(4, 2);
              int temp = memory_hazard(i - 1);
            // Check for hazards with the previous instruction
            //(it checks if destination reg of prev is same as that of before ins..)
            if (i != 0 && reg1 == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // No forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        if (temp != 0) {
                    store_pp(i, j, 1, 0, temp + 2, 0, 0); // With branch hazard
                } else {
                    store_pp(i, j, 1, 0, 2, 0, 0); // With branch hazard, no memory hazard
                }
                    }
                    else
                    {
                        if (temp != -1) {
                    store_pp(i, j, 0, 0, temp + 2, 0, 0); // Without branch hazard
                } else {
                    store_pp(i, j, 0, 0, 2, 0, 0); // Without branch hazard, no memory hazard
                }
                    }
                }
                else
                { // With forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        if (temp != 0) {
                    store_pp(i, j, 1, 0, temp, 0, 0); // With branch hazard
                } else {
                    store_pp(i, j, 1, 0, 0, 0, 0); // With branch hazard, no memory hazard
                }
                    }
                    else
                    {
                        if (temp != -1) {
                    store_pp(i, j, 0, 0, temp, 0, 0); // Without branch hazard
                } else {
                    store_pp(i, j, 0, 0, 0, 0, 0); // Without branch hazard, no memory hazard
                }
                    }
                }
            }
             else if (i == 0) {
        store_pp(i, j, 0, 0, 0, 0, 0); // No hazard in the first instruction
    }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                   if (temp != -1) {
                store_pp(i, j, 0, 0, temp, 0, 0); // With branch hazard
            } else {
                store_pp(i, j, 0, 0, temp, 0, 0); // With branch hazard, no memory hazard
            }
                }
                else
                {
                   if (temp != -1) {
                store_pp(i, j, 0, 0, temp, 0, 0); // Without branch hazard
            } else {
                store_pp(i, j, 0, 0, 0, 0, 0); // Without branch hazard, no memory hazard
            }
                }
            }
        }

        else if (pp[i][0].substr(0, 3) == "add" && pp[i][0].substr(3, 1) != "i")
        {
               int temp=memory_hazard(i-1);

            int latency_val = latencies["add"];
            if (pp[i][0].substr(5, 2) == checkHazard(pp[i - 1][0]) || pp[i][0].substr(7, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        if(temp!=0){
                                   fillarith(i,j,1,0,temp+2,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,1,0,2,0,0,latency_val);
                                }
                    }
                    else
                    {
                        if(temp!=0){
                                   fillarith(i,j,0,0,temp+2,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,0,0,2,0,0,latency_val);
                                }
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        if(temp!=0){
                                   fillarith(i,j,1,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,1,0,0,0,0,latency_val);
                                }
                    }

                    else
                    {
                         if(temp!=-1){
                                   fillarith(i,j,0,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,0,0,0,0,0,latency_val);
                                }
                        
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                   if(temp!=0){
                                   fillarith(i,j,1,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,1,0,0,0,0,latency_val);
                                }
                }

                else
                {
                    if(temp!=0){
                                   fillarith(i,j,0,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,0,0,0,0,0,latency_val);
                                }
                }
            }
        }

        else if (pp[i][0].substr(0, 3) == "sub")
        {
            int latency_val = latencies["sub"];
             int temp=memory_hazard(i-1);
            // cout<<"sub"<<endl;
            if (pp[i][0].substr(5, 2) == checkHazard(pp[i - 1][0]) || pp[i][0].substr(7, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                         if(temp!=0){
                                   fillarith(i,j,1,0,temp+2,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,1,0,2,0,0,latency_val);
                                }
                    }

                    else
                    {
                        if(temp!=0){
                                   fillarith(i,j,0,0,temp+2,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,0,0,2,0,0,latency_val);
                                }
                    }

                    // fill_stalls(i-1);
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        if(temp!=0){
                                   fillarith(i,j,1,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,1,0,0,0,0,latency_val);
                                }
                    }

                    else
                    {
                        if(temp!=-1){
                                   fillarith(i,j,0,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,0,0,0,0,0,latency_val);
                                }
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                     if(temp!=0){
                                   fillarith(i,j,1,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,1,0,0,0,0,latency_val);
                                }
                }

                else
                {
                     if(temp!=0){
                                   fillarith(i,j,0,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,0,0,0,0,0,latency_val);
                                }
                }
            }
        }

        else if (pp[i][0].substr(0, 3) == "mul")
        {
            int latency_val = latencies["mul"];
             int temp=memory_hazard(i-1);
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
                      if(temp!=0){
                                   fillarith(i,j,1,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,1,0,0,0,0,latency_val);
                                }
                    }

                    else
                    {
                        if(temp!=-1){
                                   fillarith(i,j,0,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,0,0,0,0,0,latency_val);
                                }
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    if(temp!=0){
                                   fillarith(i,j,1,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,1,0,0,0,0,latency_val);
                                }
                }

                else
                {
                   if(temp!=0){
                                   fillarith(i,j,0,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,0,0,0,0,0,latency_val);
                                }
                }
            }
        }

        else if (pp[i][0].substr(0, 3) == "div")
        {
              int temp=memory_hazard(i-1);
            int latency_val = latencies["div"];
            if (pp[i][0].substr(5, 2) == checkHazard(pp[i - 1][0]) || pp[i][0].substr(7, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        if(temp!=0){
                                   fillarith(i,j,1,0,temp+2,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,1,0,2,0,0,latency_val);
                                }
                    }

                    else
                    {
                        if(temp!=-1){
                                   fillarith(i,j,0,0,temp+2,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,0,0,2,0,0,latency_val);
                                }
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        if(temp!=0){
                                   fillarith(i,j,1,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,1,0,0,0,0,latency_val);
                                }
                    }

                    else
                    {
                         if(temp!=-1){
                                   fillarith(i,j,0,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,0,0,0,0,0,latency_val);
                                }
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    if(temp!=0){
                                   fillarith(i,j,1,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,1,0,0,0,0,latency_val);
                                }
                }
                else
                {
                    if(temp!=0){
                                   fillarith(i,j,0,0,temp,0,0,latency_val);
                                }
                                else{
                                    fillarith(i,j,0,0,0,0,0,latency_val);
                                }
                }
            }
        }

        else if (pp[i][0].substr(0, 3) == "slt")
        {
            // cout<<"slt"<<" ";
              int temp=memory_hazard(i-1);
            if (pp[i][0].substr(5, 2) == checkHazard(pp[i - 1][0]) || pp[i][0].substr(7, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        if(temp!=0){
                                   store_pp(i,j,1,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,2,0,0);
                                }
                    }

                    else
                    {
                        if(temp!=-1){
                                   store_pp(i,j,0,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,2,0,0);
                                }
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                      if(temp!=0){
                                   store_pp(i,j,1,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,0,0,0);
                                }
                    }
                    else
                    {
                        if(temp!=-1){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,0,0,0);
                                }
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                     if(temp!=0){
                                   store_pp(i,j,1,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,0,0,0);
                                }
                }

                else
                {
                    if(temp!=0){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,0,0,0);
                                }
                }
            }
        }
        else if (pp[i][0].substr(0, 3) == "beq")
        {
            //  cout<<"beq"<<endl;
            int pc;
            flag1 = 0;
            int temp=memory_hazard(i-1);
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
                       if(temp!=0){
                                   store_pp(i,j,1,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,2,0,0);
                                }
                    }

                    else
                    {
                        if(temp!=-1){
                                   store_pp(i,j,0,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,2,0,0);
                                }
                        // fill_stalls(i-1);
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                         if(temp!=0){
                                   store_pp(i,j,1,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,0,0,0);
                                }
                    }

                    else
                    {
                       if(temp!=-1){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,0,0,0);
                                }
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                     if(temp!=0){
                                   store_pp(i,j,1,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,0,0,0);
                                }
                }

                else
                {
                     if(temp!=0){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,0,0,0);
                                }
                }
            }
        }
        else if (pp[i][0].substr(0, 3) == "bne")
        {
            // cout<<"bne"<<endl;
            int pc;
            int temp=memory_hazard(i-1);
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
                         if(temp!=0){
                                   store_pp(i,j,1,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,2,0,0);
                                }
                    }

                    else
                    {
                      if(temp!=-1){
                                   store_pp(i,j,0,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,2,0,0);
                                }
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                         if(temp!=0){
                                   store_pp(i,j,1,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,2,0,0);
                                }
                    }
                    else
                    {
                        if(temp!=-1){
                                   store_pp(i,j,0,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,2,0,0);
                                }
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                     if(temp!=0){
                                   store_pp(i,j,1,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,0,0,0);
                                }
                }

                else
                {
                     if(temp!=0){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,0,0,0);
                                }
                }
            }
        }
        else if (pp[i][0].substr(0, 1) == "j" && pp[i][0].substr(1, 1) != "r")
        {
            //   cout<<"j"<<endl;
            int pc;
            flag1 = 0;
            int temp=memory_hazard(i-1);
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
                if(temp!=0){
                                   store_pp(i,j,1,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,0,0,0);
                                }
            }
            else
            {
                 if(temp!=0){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,0,0,0);
                                }
            }
        }
        else if (pp[i][0].substr(0, 3) == "bge")
        {
            // cout<<"bge"<<endl;
            int pc;
            int temp = memory_hazard(i - 1);
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
                         if (temp != 0) {
                    store_pp(i, j, 1, 0, temp + 2, 0, 0); // With branch hazard
                } else {
                    store_pp(i, j, 1, 0, 2, 0, 0); // With branch hazard, no memory hazard
                }
                    }

                    else
                    {
                        if (temp != -1) {
                    store_pp(i, j, 0, 0, temp + 2, 0, 0); // Without branch hazard
                } else {
                    store_pp(i, j, 0, 0, 2, 0, 0); // Without branch hazard, no memory hazard
                }
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        if (temp != 0) {
                    store_pp(i, j, 1, 0, temp, 0, 0); // With branch hazard
                } else {
                    store_pp(i, j, 1, 0, 0, 0, 0); // With branch hazard, no memory hazard
                }
                    }
                    else
                    {
                        if (temp != -1) {
                    store_pp(i, j, 0, 0, temp, 0, 0); // Without branch hazard
                } else {
                    store_pp(i, j, 0, 0, 0, 0, 0); // Without branch hazard, no memory hazard
                }
                    }
                }
            }
            else if (i == 0) {
        store_pp(i, j, 0, 0, 0, 0, 0); // No hazard in the first instruction
    }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    if (temp != -1) {
                store_pp(i, j, 0, 0, temp, 0, 0); // With branch hazard
            } else {
                store_pp(i, j, 0, 0, temp, 0, 0); // With branch hazard, no memory hazard
            }
                }

                else
                {
                    if (temp != -1) {
                store_pp(i, j, 0, 0, temp, 0, 0); // Without branch hazard
            } else {
                store_pp(i, j, 0, 0, 0, 0, 0); // Without branch hazard, no memory hazard
            }
                }
            }
        }

        else if (pp[i][0].substr(0, 2) == "lw")
        {
            //   cout<<"lw"<<endl;
             int temp=memory_hazard(i-1);
            if (i != 0 && pp[i][0].substr(pp[i][0].length() - 3, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        if(temp!=0){
                                   store_pp(i,j,1,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,2,0,0);
                                }
                    }
                    else
                    {
                       if(temp!=-1){
                                   store_pp(i,j,0,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,2,0,0);
                                }
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                       if(temp!=0){
                                   store_pp(i,j,1,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,0,0,0);
                                }
                    }
                    else
                    {
                       if(temp!=-1){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,0,0,0);
                                }
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                   if(temp!=0){
                                   store_pp(i,j,1,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,0,0,0);
                                }
                }
                else
                {
                     if(temp!=0){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,0,0,0);
                                }
                }
            }
        }

        else if (pp[i][0].substr(0, 2) == "sw")
        {
            // cout<<"sw"<<endl;
             int temp=memory_hazard(i-1);
            if (i != 0 && pp[i][0].substr(pp[i][0].length() - 3, 2) == checkHazard(pp[i - 1][0]))
            {
                if (flag == 0)
                { // no forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                                 if(temp!=0){
                                   store_pp(i,j,1,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,2,0,0);
                                }
                    }
                    
                    else
                    {
                         if(temp!=-1){
                                   store_pp(i,j,0,0,temp+2,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,2,0,0);
                                }
                    }
                }
                else
                { // with forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                         if(temp!=0){
                                   store_pp(i,j,1,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,0,0,0);
                                }
                    }
                    else
                    {
                         if(temp!=-1){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,0,0,0);
                                }
                    }
                }
            }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                     if(temp!=0){
                                   store_pp(i,j,1,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,0,0,0);
                                }
                
                }
                else
                {
                     if(temp!=0){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,0,0,0);
                                }
                }
            }
        }

        else if (pp[i][0].substr(0, 3) == "blt")
        {
            // Extracting the registers involved in the blt instruction
            // cout<<"blt"<<endl;
            std::string reg1 = pp[i][0].substr(3, 2);
            std::string reg2 = pp[i][0].substr(6, 2);
            int temp = memory_hazard(i - 1);

            // Check for hazards with the previous instruction
            if (i != 0 && (reg1 == checkHazard(pp[i - 1][0]) || reg2 == checkHazard(pp[i - 1][0])))
            {
                if (flag == 0)
                { // No forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                       if (temp != 0) {
                    store_pp(i, j, 1, 0, temp + 2, 0, 0); // With branch hazard
                } else {
                    store_pp(i, j, 1, 0, 2, 0, 0); // With branch hazard, no memory hazard
                }
                    }
                    else
                    {
                       if (temp != -1) {
                    store_pp(i, j, 0, 0, temp + 2, 0, 0); // Without branch hazard
                } else {
                    store_pp(i, j, 0, 0, 2, 0, 0); // Without branch hazard, no memory hazard
                }
                    }
                }
                else
                { // With forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                         if (temp != 0) {
                    store_pp(i, j, 1, 0, temp, 0, 0); // With branch hazard
                } else {
                    store_pp(i, j, 1, 0, 0, 0, 0); // With branch hazard, no memory hazard
                }
                    }
                    else
                    {
                        if (temp != -1) {
                    store_pp(i, j, 0, 0, temp, 0, 0); // Without branch hazard
                } else {
                    store_pp(i, j, 0, 0, 0, 0, 0); // Without branch hazard, no memory hazard
                }
                    }
                }
            }
             else if (i == 0) {
        store_pp(i, j, 0, 0, 0, 0, 0); // No hazard in the first instruction
    }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                    if (temp != -1) {
                store_pp(i, j, 0, 0, temp, 0, 0); // With branch hazard
            } else {
                store_pp(i, j, 0, 0, temp, 0, 0); // With branch hazard, no memory hazard
            }
                }
                else
                {
                     if (temp != -1) {
                store_pp(i, j, 0, 0, temp, 0, 0); // Without branch hazard
            } else {
                store_pp(i, j, 0, 0, 0, 0, 0); // Without branch hazard, no memory hazard
            }
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
             int temp = memory_hazard(i - 1);

            // Check for hazards with the previous instruction
            if (i != 0 && (reg1 == checkHazard(pp[i - 1][0]) || reg2 == checkHazard(pp[i - 1][0])))
            {
                if (flag == 0)
                { // No forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                        if (temp != 0) {
                    store_pp(i, j, 1, 0, temp + 2, 0, 0); // With branch hazard
                } else {
                    store_pp(i, j, 1, 0, 2, 0, 0); // With branch hazard, no memory hazard
                }
                    }
                    else
                    {
                         if (temp != -1) {
                    store_pp(i, j, 0, 0, temp + 2, 0, 0); // Without branch hazard
                } else {
                    store_pp(i, j, 0, 0, 2, 0, 0); // Without branch hazard, no memory hazard
                }
                    }
                }
                else
                { // With forwarding
                    fill_stalls(i - 1);
                    if (predict(pp[i - 1][0]) && flag1 == 1)
                    {
                         if (temp != 0) {
                    store_pp(i, j, 1, 0, temp, 0, 0); // With branch hazard
                } else {
                    store_pp(i, j, 1, 0, 0, 0, 0); // With branch hazard, no memory hazard
                }
                    }
                    else
                    {
                        if (temp != -1) {
                    store_pp(i, j, 0, 0, temp, 0, 0); // Without branch hazard
                } else {
                    store_pp(i, j, 0, 0, 0, 0, 0); // Without branch hazard, no memory hazard
                }
                    }
                }
            }
            else if (i == 0) {
        store_pp(i, j, 0, 0, 0, 0, 0); // No hazard in the first instruction
    }
            else
            {
                fill_stalls(i - 1);
                if (predict(pp[i - 1][0]) && flag1 == 1)
                {
                     if (temp != -1) {
                store_pp(i, j, 0, 0, temp, 0, 0); // With branch hazard
            } else {
                store_pp(i, j, 0, 0, temp, 0, 0); // With branch hazard, no memory hazard
            }
                }
                else
                {
                     if (temp != -1) {
                store_pp(i, j, 0, 0, temp, 0, 0); // Without branch hazard
            } else {
                store_pp(i, j, 0, 0, 0, 0, 0); // Without branch hazard, no memory hazard
            }
                }
            }
        }
        else if (pp[i][0].substr(0, 2) == "la")
        { // data and structural hazards not possible in la
           int temp=memory_hazard(i-1);
            fill_stalls(i - 1);
            if (predict(pp[i - 1][0]) && flag1 == 1)
            {
              if(temp!=0){
                                   store_pp(i,j,1,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,0,0,0);
                                }


            }
            else
            {
                if(temp!=0){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,0,0,0);
                                }
            }
        }

        else if (pp[i][0].substr(0, 2) == "jr")
        { // data and structural hazards not possible in jr
           int temp=memory_hazard(i-1);
            fill_stalls(i - 1);
            if (predict(pp[i - 1][0]) && flag1 == 1)
            {
                 if(temp!=0){
                                   store_pp(i,j,1,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,1,0,0,0,0);
                                }

            }
            else
            {
                 if(temp!=0){
                                   store_pp(i,j,0,0,temp,0,0);
                                }
                                else{
                                    store_pp(i,j,0,0,0,0,0);
                                }
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
        cout<<"totalmisses"<<totalmisses<<endl;
        cout << "Miss rate for cache: " << totalmisses/accesslatency << endl;
        return;
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
    void run(int flag1, int flag2, std::map<std::string, int> latencies,int cacheSize,int blockSize,int Associativity,int accessLatency,int memTime) {
        
        int pipeRow;
       // cores[0].core( flag1,  flag2,  latencies,int cacheSize,int blockSize,int Associativity,int accessLatency,int memTime)
       cores[0].assignval(cacheSize, blockSize,Associativity, accessLatency,memTime);
        pipeRow = cores[0].execute(memory, flag1,latencies);
        std::cout << "BUBBLE SORT: " << std::endl;
        cores[0].printval(pipeRow, flag1);
        cores[1].assignval(cacheSize, blockSize,Associativity, accessLatency,memTime);
        pipeRow = cores[1].execute(memory, flag2,latencies);
          std::cout<<pipeRow<<std::endl;
        std::cout << "SELECTION SORT: " << std::endl;
        cores[1].printval(pipeRow, flag2);
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
   // cout << endl;
    bubble_input.close();
    // for(int i=0;i<bubble_asmLines.size();i++)
    // {
    //     cout<<bubble_asmLines[i]<<endl;
    // }
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
     sim.send(selection_asmLines, 1); // Load bubble sort program into core 0
     sim.send(bubble_asmLines, 0);
     int flag1;
    int flag2;
    std::cout << "enter 1 for forwarding 0 for non forwarding for bubble sort"
              << " ";
    std::cin >> flag1;
    std::cout << "enter 1 for forwarding 0 for non forwarding for selection sort"
              << " ";
    std::cin >> flag2;
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
    int cacheSize, blockSize, accessLatency,memTime, Associativity;

    cout << "Enter the cache size: " << endl;
    cin >> cacheSize;
    cout << "Enter the block size: " << endl;
    cin >> blockSize;
    cout << "Enter the accociativity: " << endl;
    cin >> Associativity;
    cout << "Enter the access latencies: " << endl;
    cin >> accessLatency;
    cout << "Enter the memory access time: " << endl;
    cin >> memTime;
    sim.run(flag1, flag2, latencies,cacheSize,blockSize,Associativity,accessLatency,memTime);
    std::cout << "memory values:"
              << " ";
    for (int i = 0; i < 9; i++)
    {
        std::cout << sim.memory[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "selection Sort Result: ";
    for (int i = 0; i < selection_values.size(); i++)
    {
        std::cout << sim.memory[i] << " ";
    }
    std::cout << std::endl;

    // Print selection sort result
    std::cout << "Bubble Sort Result: ";
    for (int i = 0; i < bubble_values.size(); i++)
    {
        std::cout << sim.memory[i + selection_values.size()] << " ";
    }
    std::cout << std::endl;

    return 0;
}
