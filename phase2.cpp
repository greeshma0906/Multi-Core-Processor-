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
   // int flag;
    string pp[500][1000];
     int branch_flag;
     int pipeRow;
   // int bflag;

public:
    Core() {
        // Initialize register names and indices

        for (int i = 0; i < 32; ++i) {
            registers["x" + to_string(i)] = 0;
        }
        pc = 0;
        branch_flag=1;
    }
   void fill(int x,int y,int iF, int id, int ex, int mem, int wb){
            while(iF!=0){
                if(pp[x][y]=="stall"){
                    y++;
                }
                else{
                    pp[x][y]="stall";
                    y++;
                    iF--;
                }
                //another way....
//                 else {
//     pipeline[x][y] = "IF";  // Fill the cell with the "IF" stage label
//     y++;  // Move to the next cell
//     iF--;  // Decrement the cycle count for the "IF" stage
// }

            }
            while(pp[x][y]=="stall"){
                y++;
            }

            pp[x][y]="IF";
            y++;
            while(id!=0){
                if(pp[x][y]=="stall"){
                    y++;
                }
                else{
                    pp[x][y]="stall";
                    y++;
                    id--;
                }
            }
            while(pp[x][y]=="stall"){
                y++;
            }
            pp[x][y]="ID";
            y++;
            while(ex!=0){
                if(pp[x][y]=="stall"){
                    y++;
                }
                else{
                    pp[x][y]="stall";
                    y++;
                    ex--;
                }
            }
            while(pp[x][y]=="stall"){
                y++;
            }
            pp[x][y]="EX";
            y++;
            while(mem!=0){
                if(pp[x][y]=="stall"){
                    y++;
                }
                else{
                    pp[x][y]="stall";
                    y++;
                    mem--;
                }
            }
            while(pp[x][y]=="stall"){
                y++;
            }
            pp[x][y]="MEM";
            y++;
            while(wb!=0){
                if(pp[x][y]=="stall"){
                    y++;
                }
                else{
                    pp[x][y]="stall";
                    y++;
                    wb--;
                }
            }
            while(pp[x][y]=="stall"){
                y++;
            }
            pp[x][y]="WB";
        }

        string hazard(string ins){
            if(ins.substr(0,4)=="addi"){
                return ins.substr(4,2);
            }
            if(ins.substr(0,3)=="add" && (ins.substr(3,1)!="i")){
                return ins.substr(3,2);
            }
            if(ins.substr(0,3)=="sub"){
                return ins.substr(3,2);
            }
            if(ins.substr(0,3)=="mul"){
                return ins.substr(3,2);
            }
            if(ins.substr(0,3)=="div"){
                return ins.substr(3,2);
            }
            if(ins.substr(0,3)=="slt"){
                return ins.substr(3,2);
            }
            if(ins.substr(0,2)=="lw"){
                return ins.substr(2,2);
            }
            if(ins.substr(0,2)=="sw"){
                return ins.substr(2,2);
            }
            return "nulll";
            //if ..... other functions
        }

        bool branchhazard(string ins){
            bool flag=false;
             if(ins.substr(0,3)=="beq"||ins.substr(0,3)=="bne"||(ins.substr(0,1)=="j"&&ins.substr(1,1)!="r")){
                flag=true;
            }
            return flag;
        }
        void stalls_hazard(int ins_row){
            int IF,ID,EX,MEM;
            int clk_len=0;
            for(int j=1;j<10000;j++){
                if(pp[ins_row][j]=="WB")
                  clk_len=j;
            }
            for(int j=1;j<clk_len;j++){
              if(pp[ins_row][j]=="IF")
              IF=j;
               if(pp[ins_row][j]=="ID")
              ID=j;
               if(pp[ins_row][j]=="EX")
              EX=j;
               if(pp[ins_row][j]=="MEM")
              MEM=j;
            }
            for(int j=IF+1;j<ID;j++){
                if(pp[ins_row][j]=="stall"){

                  pp[ins_row+1][j]="stall";


                }
            }
            for(int j=ID+1;j<EX;j++){
                if(pp[ins_row][j]=="stall"){
                  pp[ins_row+1][j]="stall";
                }
            }
            for(int j=EX+1;j<MEM;j++){
                if(pp[ins_row][j]=="stall"){
                  //fill(ins_row+1,i,0,0,0,1,0);

                  pp[ins_row+1][j]="stall";
                }
            }
            for(int j=MEM+1;j<clk_len;j++){
                if(pp[ins_row][j]=="stall"){
                  //fill(ins_row+1,i,0,0,0,0,1);
                  pp[ins_row+1][j]="stall";
                }
            }

        }
         void fillPipeline(int numb_rows,int flagForwdg){
            int clock1=1;
            int j=0;
            for(int i=0; i<numb_rows; i++){
                j=clock1;

                if(pp[i][0].substr(0,4)=="addi"){
                    if(i!=0 && pp[i][0].substr(6,2) == hazard(pp[i-1][0])){
                         if(flagForwdg==0){//no forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pp[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,2,0,0);
                           }
                            else{
                            fill(i,j,0,0,2,0,0); 
                            }     
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pp[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,0,0,0);
                            }
                            else{
                            fill(i,j,0,0,0,0,0);
                            }
                        }
                    }
                    else if(i==0){
                        fill(i,j,0,0,0,0,0);
                    }
                    else{   //i!=0 and no hazard in previous instruction
                         stalls_hazard(i-1);
                        if(branchhazard(pp[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,0,0,0);
                        }
                        else{
                            fill(i,j,0,0,0,0,0);
                        }
                       
                    }
                }
                if(pp[i][0].substr(0,3)=="add" && pp[i][0].substr(3,1)!="i"){
                    if(pp[i][0].substr(5,2) == hazard(pp[i-1][0]) || pp[i][0].substr(7,2) == hazard(pp[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                            if(branchhazard(pp[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,2,0,0);
                            }
                            else{
                              fill(i,j,0,0,2,0,0);
                            }
                           
                            
                        }
                        else{//with forwarding
                         stalls_hazard(i-1);
                             if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                  fill(i,j,1,0,0,0,0);
                             }
                           
                            else{
                                 fill(i,j,0,0,0,0,0);
                            }
                           
                        }
                       
                    }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pp[i-1][0]) && branch_flag==1){
                             fill(i,j,1,0,0,0,0);
                         }
                            
                        else{
                            fill(i,j,0,0,0,0,0);
                        }
                       
                    }
                }

                if(pp[i][0].substr(0,3)=="sub"){
                    if(pp[i][0].substr(5,2) == hazard(pp[i-1][0]) || pp[i][0].substr(7,2) == hazard(pp[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                             if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                 fill(i,j,1,0,2,0,0);
                             }
                            
                            else{
                                fill(i,j,0,0,2,0,0);
                            }
                            
                            //stalls_hazard(i-1);
                        }
                        else{//with forwarding
                             stalls_hazard(i-1);
                             if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                 fill(i,j,1,0,0,0,0);
                             }
                            
                            else{
                                 fill(i,j,0,0,0,0,0);
                            }
                           
                        }
                      
                    }
                    else{
                        stalls_hazard(i-1);
                       if(branchhazard(pp[i-1][0]) && branch_flag==1){
                           fill(i,j,1,0,0,0,0);
                       }
                            
                        else{
                        fill(i,j,0,0,0,0,0);
                        }
                    }
                }

                if(pp[i][0].substr(0,3)=="mul"){
                    if(pp[i][0].substr(5,2) == hazard(pp[i-1][0]) || pp[i][0].substr(7,2) == hazard(pp[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                           if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,2,0,0);
                           }
                           
                            else{
                                fill(i,j,0,0,2,0,0);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                           if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                           }
                           
                            else{
                              fill(i,j,0,0,0,0,0);
                            }  
                        } 
                    }
                    else{
                         stalls_hazard(i-1);
                        if(branchhazard(pp[i-1][0]) && branch_flag==1){  
                              fill(i,j,1,0,0,0,0);
                        }
                           
                        else{
                          fill(i,j,0,0,0,0,0);
                        }
                        
                    }
                }

                if(pp[i][0].substr(0,3)=="div"){
                    if(pp[i][0].substr(5,2) == hazard(pp[i-1][0]) || pp[i][0].substr(7,2) == hazard(pp[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                            if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,2,0,0);
                            }
                            
                            else{
                                fill(i,j,0,0,2,0,0);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                           if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                           }
                           
                            else{
                            fill(i,j,0,0,0,0,0);
                            }
                        }   
                    }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pp[i-1][0]) && branch_flag==1){  
                             fill(i,j,1,0,0,0,0);
                        }         
                        else{
                             fill(i,j,0,0,0,0,0);
                        }
                       
                    }
                }

                if(pp[i][0].substr(0,3)=="slt"){
                   if(pp[i][0].substr(5,2) == hazard(pp[i-1][0]) || pp[i][0].substr(7,2) == hazard(pp[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                            if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                 fill(i,j,1,0,2,0,0);
                            }
                            
                            else{
                                fill(i,j,0,0,2,0,0);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                           }  
                            else{
                            fill(i,j,0,0,0,0,0);
                            }
                        }
                       
                    }
                    else{
                         stalls_hazard(i-1);
                        if(branchhazard(pp[i-1][0]) && branch_flag==1){   
                             fill(i,j,1,0,0,0,0);
                        }
                            
                        else{
                             fill(i,j,0,0,0,0,0);
                        }
                       
                    }
                }
                if(pp[i][0].substr(0,3)=="beq"){
                     int pc;
                     branch_flag=0;
                     for(int j=0;j<program.size();j++){
                        if(pp[i][0]==program[j]){
                        pc=j;
                        }
                                                    
                     }
                     if(pp[i+1][0]!=program[pc+1])
                     branch_flag=1;
                     else
                     branch_flag=0;
                    if(pp[i][0].substr(3,2) == hazard(pp[i-1][0]) || pp[i][0].substr(5,2) == hazard(pp[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                            if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                 fill(i,j,1,0,2,0,0);
                            }
                            
                            else{
                            fill(i,j,0,0,2,0,0);
                            //stalls_hazard(i-1);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                           if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                           }
                           
                            else{
                            fill(i,j,0,0,0,0,0);
                            }
                        }
                       
                    }
                    else{
                         stalls_hazard(i-1);
                        if(branchhazard(pp[i-1][0]) && branch_flag==1){  
                              fill(i,j,1,0,0,0,0);
                        }
                           
                        else{
                          fill(i,j,0,0,0,0,0);
                        }
                        
                    }
                    

                }
                 if(pp[i][0].substr(0,3)=="bne"){
                    int pc;
                     branch_flag=0;
                     for(int j=0;j<program.size();j++){
                        if(pp[i][0]==program[j])
                            pc=j;
                     }
                     if(pp[i+1][0]!=program[pc+1])
                     branch_flag=1;
                     else
                     branch_flag=0;
                     if(pp[i][0].substr(3,2) == hazard(pp[i-1][0]) || pp[i][0].substr(5,2) == hazard(pp[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                            if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                 fill(i,j,1,0,2,0,0);
                            }
                            
                            else{
                            fill(i,j,0,0,2,0,0);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                           }
                            else{
                            fill(i,j,0,0,0,0,0);
                            }
                        }
                       
                    }
                    else{
                         stalls_hazard(i-1);
                        if(branchhazard(pp[i-1][0]) && branch_flag==1){  
                            fill(i,j,1,0,0,0,0);
                        }
                           
                        else{
                            fill(i,j,0,0,0,0,0);
                        }
                        
                    }
                     

                }
                 if(pp[i][0].substr(0,1)=="j" && pp[i][0].substr(1,1)!="r"){
                    int pc;
                     branch_flag=0;
                     for(int j=0;j<program.size();j++){
                        if(pp[i][0]==program[j])
                            pc=j;
                     }
                     if(pp[i+1][0]!=program[pc+1])
                     branch_flag=1;
                     else
                     branch_flag=0;
                     
                    stalls_hazard(i-1);
                    if(branchhazard(pp[i-1][0]) && branch_flag==1){
                        fill(i,j,1,0,0,0,0);
                    }
                    else{
                        fill(i,j,0,0,0,0,0);
                    }
                }
                  if(pp[i][0].substr(0,2)=="lw"){
                    if(i!=0 && pp[i][0].substr(pp[i][0].length()-3,2) == hazard(pp[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,2,0,0);
                            }
                            else{
                                fill(i,j,0,0,2,0,0);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                            }
                            else{
                                fill(i,j,0,0,0,0,0);
                            }  
                        }
                    }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pp[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,0,0,0);
                        }
                        else{
                            fill(i,j,0,0,0,0,0);
                        }
                    }
                }

                if(pp[i][0].substr(0,2)=="sw"){
                    if(i!=0 && pp[i][0].substr(pp[i][0].length()-3,2) == hazard(pp[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,2,0,0);
                            }
                            else{
                                fill(i,j,0,0,2,0,0);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pp[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                            }
                            else{
                                fill(i,j,0,0,0,0,0);
                            }  
                        }
                    }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pp[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,0,0,0);
                        }
                        else{
                            fill(i,j,0,0,0,0,0);
                        }
                    }
                }

if(pipeline[i][0].substr(0, 3) == "blt") {
    // Extracting the registers involved in the blt instruction
    string reg1 = pipeline[i][0].substr(3, 2);
    string reg2 = pipeline[i][0].substr(6, 2);

    // Check for hazards with the previous instruction
    if(i != 0 && (reg1 == hazard(pipeline[i-1][0]) || reg2 == hazard(pipeline[i-1][0]))) {
        if(flagForwdg == 0) { // No forwarding
            stalls_hazard(i-1);
            if(branchhazard(pipeline[i-1][0]) && branch_flag == 1) {
                fill(i, j, 1, 0, 2, 0, 0);
            } else {
                fill(i, j, 0, 0, 2, 0, 0);
            }
        } else { // With forwarding
            stalls_hazard(i-1);
            if(branchhazard(pipeline[i-1][0]) && branch_flag == 1) {
                fill(i, j, 1, 0, 0, 0, 0);
            } else {
                fill(i, j, 0, 0, 0, 0, 0);
            }
        }
    } else {
        stalls_hazard(i-1);
        if(branchhazard(pipeline[i-1][0]) && branch_flag == 1) {
            fill(i, j, 1, 0, 0, 0, 0);
        } else {
            fill(i, j, 0, 0, 0, 0, 0);
        }
    }
}
if(pipeline[i][0].substr(0, 2) == "mv") {
    // Extracting the registers involved in the mv instruction
    string reg1 = pipeline[i][0].substr(2, 2);
    string reg2 = pipeline[i][0].substr(5, 2);

    // Check for hazards with the previous instruction
    if(i != 0 && (reg1 == hazard(pipeline[i-1][0]) || reg2 == hazard(pipeline[i-1][0]))) {
        if(flagForwdg == 0) { // No forwarding
            stalls_hazard(i-1);
            if(branchhazard(pipeline[i-1][0]) && branch_flag == 1) {
                fill(i, j, 1, 0, 2, 0, 0);
            } else {
                fill(i, j, 0, 0, 2, 0, 0);
            }
        } else { // With forwarding
            stalls_hazard(i-1);
            if(branchhazard(pipeline[i-1][0]) && branch_flag == 1) {
                fill(i, j, 1, 0, 0, 0, 0);
            } else {
                fill(i, j, 0, 0, 0, 0, 0);
            }
        }
    } else {
        stalls_hazard(i-1);
        if(branchhazard(pipeline[i-1][0]) && branch_flag == 1) {
            fill(i, j, 1, 0, 0, 0, 0);
        } else {
            fill(i, j, 0, 0, 0, 0, 0);
        }
    }
}
                if(pp[i][0].substr(0,2)=="la"){   //data and structural hazards not possible in la
                        stalls_hazard(i-1);
                        if(branchhazard(pp[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,0,0,0);
                        }
                        else{
                            fill(i,j,0,0,0,0,0);
                        }
                }

                if(pp[i][0].substr(0,2)=="jr"){   //data and structural hazards not possible in jr
                    stalls_hazard(i-1);
                    if(branchhazard(pp[i-1][0]) && branch_flag==1){
                        fill(i,j,1,0,0,0,0);
                    }
                    else{
                        fill(i,j,0,0,0,0,0);
                    }
                }

                for(int q=1; q<10000; q++){
                    if(pp[i][q]=="IF"){
                            clock1=q+1;
                    }
                }
            }
            return;
        }


    void execute(vector<int> &memory,int flag) {
        int pipeRow = 0;
      while (pc < program.size()) {
        string instruction = program[pc];
        //  pp[pipeRow][0]=instruction;
        //         pipeRow++;

        if (instruction == "exit") {
            //Print register values after sorting
            cout << "Register Values after sorting:" << endl;
            for (auto it = registers.begin(); it != registers.end(); ++it) {
                cout << it->first << " = " << it->second <<" ";
            }
            cout<<endl;
            int cnt=0;
            for(int j=1;j<10000;j++){
                if(pp[pipeRow-1][j] == "WB"){
                    cout << "Total number of clock cycles: " << j << endl<<endl;
                    cnt=j;
                }
            }

            string stallInstruction[5000];
            int count=0;
            int k=0;
            for(int i=0; i<pipeRow ;i++){
                for(int j=1;j<10000;j++){
                    if(pp[i][j]=="stall"){
                        count++;
                        stallInstruction[k] = pp[i][0];
                    }
                }
                k++;
            }
 
            cout << "Total number of stalls: " << count <<endl<<endl;
            float ipc=(float)pipeRow/cnt;
            cout<<"IPC(Instructions per cycle is) :"<<ipc<<endl<<endl;

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
          
          pp[pipeRow][0]=instruction;
          // fillPipeline(pipeRow, flag);
            pipeRow++;
          pc+=1;
        
        }
       // pipeRow=0;
       fillPipeline(pipeRow, flag);
    //    int cnt=0;
    //         for(int j=1;j<10000;j++){
    //             if(pp[pipeRow-1][j] == "WB"){
    //                 cout << "Total number of clock cycles: " << j << endl<<endl;
    //                 cnt=j;
    //             }
    //         }

    //         string stallInstruction[5000];
    //         int count=0;
    //         int k=0;
    //         for(int i=0; i<pipeRow ;i++){
    //             for(int j=1;j<10000;j++){
    //                 if(pp[i][j]=="stall"){
    //                     count++;
    //                     stallInstruction[k] = pp[i][0];
    //                 }
    //             }
    //             k++;
    //         }
 
    //         cout << "Total number of stalls: " << count <<endl<<endl;
    //         float ipc=(float)pipeRow/cnt;
    //         cout<<"IPC(Instructions per cycle is) :"<<ipc<<endl<<endl;

    }
   // void printval()
    // {
    //      for (int l = 0; l <pipeRow; l++) {
    //             for (int l1 = 0; l1 < l + 6; l1++) {
    //                 cout<<(pp[l][l1] + " "); // printing pipeline 2d array
    //             }
    //            cout<<endl;
    //         }
    //        cout<<endl;
    // }
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
    void run(int flag) {
        for(int i=0;i<2;i++)
        {
          cores[i].execute(memory,flag);
         // cores[i].printval();
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
   // cout << endl;
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
     int flag;
     cout<<"enter 1 for forwarding 0 for non forwarding"<<" ";
     cin>>flag;
     sim.run(flag);
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
