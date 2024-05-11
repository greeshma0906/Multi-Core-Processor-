#define _GLIBCXX_USE_CXX11_ABI 0
#include <bits/stdc++.h>
using namespace std;

int register_values[32]={0};
string REG[32]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"};


string pipeline[500][1000];

class mipsSimulator{
    public:
        int MEM[1024]={0};
        int miss_arr[500];
        //------------------------------------
        int cache1[1024]={0};
        int tag1[1024];
        int cache2[1024]={0}; 
        int tag2[1024];
        int counter1[1024]={0};
        int counter2[1024]={0};
        int cache1Size, cache2Size, block1Size, block2Size;
        int associativity;
        int totalInts1, totalInts2;
        int blockInts1, blockInts2;
        int numblocks1, numblocks2;
        int accessLatency1, accessLatency2; //inputed as number of cycles needed to go to L1 and L2
        int memTime; //number of cycles needed to go to the main memory

        int miss = -1; //this could take only three values
        // miss=0 for a hit in cache l1
        //miss=1 for a miss in l1 and hit in l2;
        //miss=2 for a miss in l1 and l2
        double accessesL1=0;
        double accessesL2=0;
        double totalL1misses=0;
        double totalL2misses=0;
        //--------------------------------------
        int programCounter;
        int NumberOfInstructions;
        int branch_flag;
        int MaxLength;//10000
        vector<string> InputProgram; //to store the input program
        vector<string>Input_ins;
        int mainindex=0;
        struct Memoryword{
            string value;
            string address;//pc line number
        };
        struct Label{
            string labelname;
            string address;
        };
        vector<struct Memoryword>Mem;
        vector<struct Label>labeltable;

        mipsSimulator(string fileName, int Cache1Size,int Cache2Size,int Block1Size,int Block2Size,int AccessLatency1,int AccessLatency2,int MemTime, int Associativity){
        programCounter=0;
        NumberOfInstructions=0;
        MaxLength=10000;
        branch_flag=0;
        ifstream InputFile;
        InputFile.open(fileName.c_str(),ios::in); //open file
        if(!InputFile){ //if open failed
            cout<<"Error: File does not exist or could not be opened"<<endl;
            exit(1);
        }
        string tempString;
        while(getline(InputFile,tempString)){ //read line by line
        //readInstruction(tempString);
            NumberOfInstructions++;
            if(NumberOfInstructions>MaxLength){ ///check number of instructions with maximum allowed
                cout<<"Error: Number of lines in input too large, maximum allowed is "<<MaxLength<<" line"<<endl;
                exit(1);
            }
            InputProgram.push_back(tempString); //store in InputProgram
        }
        InputFile.close();

        //--------------------------------------
        cache1Size=Cache1Size; 
        cache2Size=Cache2Size; 
        block1Size=Block1Size; 
        block2Size=Block2Size;
        associativity=Associativity;
        accessLatency1=AccessLatency1; 
        accessLatency2=AccessLatency2;
        memTime=MemTime;
        totalInts1 = Cache1Size/4;
        totalInts2 = Cache2Size/4;
        blockInts1=Block1Size/4;
        blockInts2=Block2Size/4;
        numblocks1=Cache1Size/Block1Size;
        numblocks2=Cache2Size/Block2Size;
        for(int i=0;i<numblocks1;i++){
            tag1[i]=-1;
        }
        for(int i=0;i<numblocks2;i++){
            tag2[i]=-1;
        }
        for(int i=0; i<500; i++){
            miss_arr[i]=-1;
        }

        //--------------------------------------
        }

        int nearest(int addrs){
            while(addrs%blockInts1!=0){
                addrs--;
            }
            return addrs;
        }

        bool search1(int addrs){
            for(int i=0;i<numblocks1;i++){
                if(tag1[i]==nearest(addrs)){
                   return true; 
                }
            }
            return false;
        }

        bool search2(int addrs){
            for(int i=0;i<numblocks2;i++){
                if(tag2[i]==nearest(addrs)){
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

        void incrementcounter1(int adrs){
            for(int i=0;i<numblocks1;i++){
                if(tag1[i]==adrs){
                    counter1[i]++;
                    break;
                }
            }
        }

        void L2toL1(int addrs){
            int i,j,count;
            for(i=0;i<numblocks2;i++){
                if(tag2[i]==nearest(addrs))
                    break;
            }
            int k1,k2;
            for(j=0;j<numblocks1;j++){
                if(tag1[j]==-1){
                    tag1[j]=tag2[i];
                    tag2[i]=-1;

                    k1=blockInts1*j;
                    k2=blockInts2*i;
                    for(int k=0; k<blockInts1; k++){
                        cache1[k1]=cache2[k2];
                        cache2[k2]=0;
                        k1++;
                        k2++;
                    }
                    counter2[i]=0;
                    counter1[j]=1;
                    break;
                }
            }
            if(j==numblocks1){
                //lru
                //check with the counter in cache1, the least one will be pushed 
                //to cache2(if full, we again apply lru for cache2) and then we put 
                // new one in the place that has been emptied.
                int tempcount1, temptag1, tempInts1[blockInts1];
                int minIndex = minimum(counter1, numblocks1);
                j=minIndex;
                k1=blockInts1*j;
                k2=blockInts2*i;
                //copy the contents of the block that is being kicked out of L1
                //as these are to be put in L2
                temptag1 = tag1[j];
                tempcount1 = counter1[j];
                for(int m=0; m<blockInts1; m++){
                    tempInts1[m]= cache1[k1];
                    k1++;
                }
                //copy from L2 to L1
                tag1[j]=tag2[i];
                k1=blockInts1*j;
                k2=blockInts2*i;
                for(int k=0; k<blockInts1; k++){
                    cache1[k1]=cache2[k2];
                    cache2[k2]=0;
                    k1++;
                    k2++;
                }
                counter1[j]=counter2[i];
                counter1[j]++;
                //copy back the contents of temp into the block that was emptied in L2
                tag2[i] = temptag1;
                counter2[i] = tempcount1;
                k2=blockInts2*i;
                for(int m=0; m<blockInts2; m++){
                    cache2[k2] = tempInts1[m];
                    k2++;
                }
            }
        }

        void memtoL1(int addrs){
            int j,k1,k2;
            for(j=0;j<numblocks1;j++){
                if(tag1[j]==-1){
                    k1 = blockInts1*j;
                    tag1[j]=nearest(addrs);
                    counter1[j]=1;
                    for(int k=0; k<blockInts1; k++){
                        cache1[k1]=MEM[addrs];
                        k1++;
                        addrs++;
                    }
                    break;
                }
            }
            int i; //i for cache L2
            if(j==numblocks1){  //if L1 is full
                //lru
                int minIndex1 = minimum(counter1, numblocks1);
                int tempcount1, temptag1, tempInts1[blockInts1];
                j=minIndex1;
                k1=blockInts1*j;
                //copy the contents of the block that is being kicked out of L1
                //as these are to be put in L2
                temptag1 = tag1[j];
                tempcount1 = counter1[j];
                for(int m=0; m<blockInts1; m++){
                    tempInts1[m]= cache1[k1];
                    k1++;
                }
                //bring new block from them memory and put it cache
                tag1[j]=nearest(addrs);
                counter1[j]=1;
                for(int k=0; k<blockInts1; k++){
                    cache1[k1]=MEM[addrs];
                    k1++;
                    addrs++;
                }
                //copy back contents in temp to L2
                for(i=0;i<numblocks2;i++){
                    if(tag2[i]==-1){
                        tag2[i] = temptag1;
                        counter2[i] = tempcount1;
                        k2=blockInts2*i;
                        for(int m=0; m<blockInts2; m++){
                            cache2[k2] = tempInts1[m];
                            k2++;
                        }
                        break;
                    }
                }
                if(i==numblocks2){  //if L2 is full, apply lru again
                    int minIndex2 = minimum(counter2, numblocks2);
                    i=minIndex2;
                    tag2[i] = temptag1;
                    counter2[i] = tempcount1;
                    k2=blockInts2*i;
                    for(int m=0; m<blockInts2; m++){
                        cache2[k2] = tempInts1[m];
                        k2++;
                    }
                }
            }
        }

        void updateInL1(int addrs,int val){
            int j;
            for(j=0;j<numblocks1;j++){
                if(tag1[j]==nearest(addrs))
                    break;
            }
            int k1 = blockInts1*j;
            cache1[k1 + (addrs-nearest(addrs))] = val;
        }

        void updateInL2(int addrs,int val){
            int i;
            for(i=0;i<numblocks2;i++){
                if(tag1[i]==nearest(addrs))
                    break;
            }
            int k1 = blockInts2*i;
            cache2[k1 + (addrs-nearest(addrs))] = val;
        }

        string readInstruction(string str){
            if(str.find("#")!=-1){ //remove comments
                    str=str.substr(0,str.find("#"));
                    }
            str.erase(remove(str.begin(), str.end(), ' '), str.end());
            str.erase(remove(str.begin(), str.end(), ','), str.end());
            str.erase(remove(str.begin(), str.end(), '$'), str.end());
           return str;
        }

        string readArrayinstruction(string str){
            str.erase(remove(str.begin(), str.end(), ' '), str.end());
           return str;
        }

        void reportError(int line_number){
            cout<<"Error found in :"<<(line_number+1)<<": "<<InputProgram[line_number]<<endl;
        }

        void preprocess(){
            int i=0,j=0;
            int current_section=-1; //current_section=0 - data section, current_section=1 - text section
            int index; //to hold index of ".data"
            int flag=0; //whether "..data" found
            //string current_instruction="";
            int dataStart=0; //line number for start of data section
            int textStart=0;

            for(int k=0;k<InputProgram.size();k++){
                if(InputProgram[k]=="main:")
                mainindex=k;
            }
            int p_count=mainindex+1;
            for(int k=p_count;k<InputProgram.size();k++){
                string current_instrucn=readInstruction(InputProgram[k]);
                Input_ins.push_back(current_instrucn);
            }

            for(i=0;i<NumberOfInstructions;i++){
                string current_instruction="";
                current_instruction=InputProgram[i];
                current_instruction = readInstruction(current_instruction);
                index=current_instruction.find(".data");
                if(index==-1)
                continue;
                else if(flag==0){
                    flag=1;
                    current_section=0;
                    dataStart=i;
                }
                else if(flag==1){
                    cout<<"Multiple instances of .data found"<<endl;
                    exit(1);
                }
            }
            int wordindex,arrayindex;
            if(current_section==0){
                for(i=dataStart+1;i<NumberOfInstructions;i++){
                    string current_instruction="";
                    current_instruction=InputProgram[i];
                    current_instruction = readArrayinstruction(current_instruction);
                    arrayindex=current_instruction.find(":");//array:.word9315
                    wordindex=current_instruction.find(".word");
                    int storeline;
                    if(wordindex==-1 && arrayindex==-1){
                        if(current_instruction.find(".text")==-1){ //if text section has not started
                            cout<<"Error: Unexpected symbol in data section"<<endl;
                        }
                        else{
                            break;
                        }
                    }
                    else{
                        string num=current_instruction.substr(arrayindex+6);//9,3,11,35,2,411
                        //lets assume array values are <10
                        int k=0;
                        while(num.length()!=0){
                            int found=num.find(',');
                            if(found != string::npos){
                                MEM[k]=stoi(num.substr(0,found));
                                num=num.substr(found+1);
                                k++;
                            }
                            else{
                                MEM[k]=stoi(num.substr(0));
                                num=num.substr(num.length());
                            }
                        }
                    } 
                }
            }
            int textIndex=0;
            int textFlag=0;

            for(i=programCounter;i<NumberOfInstructions;i++)
            {
                string current_instruction=InputProgram[i];
                current_instruction = readInstruction(current_instruction);
                if(current_instruction=="")
                {
                    continue;
                }
                textIndex=current_instruction.find(".text"); //find text section similar as above
                if(textIndex==-1)
                {
                    continue;
                }
                else if(textFlag==0)
                {
                    textFlag=1;
                    current_section=1;
                    textStart=i;
                }
                else if(textFlag==1)
                {
                    cout<<"Error: Multiple instances of .text"<<endl;
                    reportError(i);
                }
            }
            if(current_section!=1){ //if text section not found
                cout<<"Error: Text section does not exist or found unknown string"<<endl;
                exit(1);
            }
            if(InputProgram[textStart+1]!=".globl main"){
                cout<<"Error: No (.globl main) found"<<endl;
                exit(1);
            }
            int foundmain=0;
            int main_index=0,labelindex=-1;
            if(InputProgram[textStart+2]!="main:"){
                cout<<"Error: No main found"<<endl;
                exit(1);
            }
            else{
                foundmain=1;
                main_index=textStart+2;
            }
            for(int i=main_index+1;i<NumberOfInstructions;i++){
                string current_instruction=InputProgram[i];
                current_instruction = readInstruction(current_instruction);
                labelindex=current_instruction.find(":");
                if(labelindex==0){
                    cout<<"Error : Label name expected"<<endl;
                    reportError(i);
                }
                else if(labelindex==-1){
                    continue;
                }
                else{
                    j=labelindex-1;
                    string temp="";
                    temp=current_instruction.substr(0,j+1);
                    Label templabel;
                    templabel.labelname=temp;
                    templabel.address=to_string(i+1);
                    labeltable.push_back(templabel);
                }
            }
            for(i=0;labeltable.size()>0 && i<(labeltable.size()-1);i++) //check for duplicates
            {
                if(labeltable[i].labelname==labeltable[i+1].labelname)
                {
                    cout<<"Error: One or more labels are repeated"<<endl;
                    exit(1);
                }
            }
        }

        void processInstruction(string current_instruction){
            if(current_instruction.substr(0,3)=="add" && current_instruction.substr(3,1)!="i"){
                int reg_store[3]={-1};
                for(int i=0;i<32;i++){
                    if(current_instruction.substr(3,2)==REG[i])
                        reg_store[0]=i;
                    if(current_instruction.substr(5,2)==REG[i])
                        reg_store[1]=i;
                    if(current_instruction.substr(7,2)==REG[i])
                        reg_store[2]=i;
                }
                register_values[reg_store[0]]= register_values[reg_store[1]]+ register_values[reg_store[2]];
                programCounter++;
                return;
            }
            if(current_instruction.substr(0,3)=="sub"){
                int reg_store[3]={-1};
                for(int i=0;i<32;i++){
                    if(current_instruction.substr(3,2)==REG[i])
                        reg_store[0]=i;
                    if(current_instruction.substr(5,2)==REG[i])
                        reg_store[1]=i;
                    if(current_instruction.substr(7,2)==REG[i])
                        reg_store[2]=i;

                }
               
                     register_values[reg_store[0]]= register_values[reg_store[1]]-register_values[reg_store[2]];
                      programCounter++;
                      return;
            }
            if(current_instruction.substr(0,3)=="mul"){
                int reg_store[3]={-1};
                for(int i=0;i<32;i++){
                    if(current_instruction.substr(3,2)==REG[i])
                        reg_store[0]=i;
                    if(current_instruction.substr(5,2)==REG[i])
                        reg_store[1]=i;
                    if(current_instruction.substr(7,2)==REG[i])
                        reg_store[2]=i;
                }        
                     register_values[reg_store[0]]= register_values[reg_store[1]]*register_values[reg_store[2]];
                      programCounter++;
                      return;
            }
            if(current_instruction.substr(0,3)=="div"){
                int reg_store[3]={-1};
                for(int i=0;i<32;i++){
                    if(current_instruction.substr(3,2)==REG[i])
                        reg_store[0]=i;
                    if(current_instruction.substr(5,2)==REG[i])
                        reg_store[1]=i;
                    if(current_instruction.substr(7,2)==REG[i])
                        reg_store[2]=i;

                }
               
                     register_values[reg_store[0]]= register_values[reg_store[1]]/register_values[reg_store[2]];
                      programCounter++;
                      return;
            }
            if(current_instruction.substr(0,4)=="addi"){//addit2t34
                string rs,rd,imm;
                int immediate;
                rd=current_instruction.substr(4,2);
                if(current_instruction.substr(6,2)!="ze"){
                    rs=current_instruction.substr(6,2);
                    imm=current_instruction.substr(8);
                    immediate=stoi(imm);
                }
                else{
                     rs=current_instruction.substr(6,4);
                     imm=current_instruction.substr(10);
                    immediate=stoi(imm);
                }
                int reg_store[2]={-1};
                for(int i=0;i<32;i++){
                    if(rd==REG[i])
                        reg_store[0]=i;
                    if(rs==REG[i])
                        reg_store[1]=i;
                }
                register_values[reg_store[0]]=immediate+register_values[reg_store[1]];
                programCounter++;
                return;
            }       
            if(current_instruction.substr(0,3)=="beq"){
                string st;
                int reg_store[2]={-1};
                if(current_instruction.substr(5,2)=="ze"){
                    for(int i=0;i<32;i++){
                        if(current_instruction.substr(3,2)==REG[i])
                            reg_store[0]=i;
                        if(current_instruction.substr(5,4)==REG[i]) //beqt0zeroLABEL
                            reg_store[1]=i;
                    }
                    st = current_instruction.substr(9);
                }
                else{
                    for(int i=0;i<32;i++){
                        if(current_instruction.substr(3,2)==REG[i])
                            reg_store[0]=i;
                        if(current_instruction.substr(5,2)==REG[i])
                            reg_store[1]=i;
                    }
                    st = current_instruction.substr(7);
                }

                string addr;
                for(int i=0;i<labeltable.size();i++){
                    if(labeltable[i].labelname==st){
                        addr=labeltable[i].address;
                    }
                }
                if(register_values[reg_store[0]]==register_values[reg_store[1]]){
                    programCounter=stoi(addr) + 1;
                }
                else{
                    programCounter++;
                }
                return;
            }
            if(current_instruction.substr(0,3)=="bne"){
                string st;
                int reg_store[2]={-1};
                if(current_instruction.substr(5,2)=="ze"){  //bnet2t3LABEL
                    for(int i=0;i<32;i++){
                        if(current_instruction.substr(3,2)==REG[i])
                            reg_store[0]=i;
                        if(current_instruction.substr(5,4)==REG[i]) //beqt0zeroLABEL
                            reg_store[1]=i;
                    }
                    st = current_instruction.substr(9);
                }
                else{
                    for(int i=0;i<32;i++){
                        if(current_instruction.substr(3,2)==REG[i])
                            reg_store[0]=i;
                        if(current_instruction.substr(5,2)==REG[i])
                            reg_store[1]=i;
                    }
                    st = current_instruction.substr(7);
                }
                string addr;
                for(int i=0;i<labeltable.size();i++){
                    if(labeltable[i].labelname==st){
                        addr=labeltable[i].address;
                    }
                }
                if(register_values[reg_store[0]]!=register_values[reg_store[1]]){
                    programCounter=stoi(addr) + 1;
                }
                else{
                    programCounter++;
                }
                return;
            }
            if(current_instruction.substr(0,1)=="j" && current_instruction.substr(1,1)!="r"){
              
                string st = current_instruction.substr(1);
                string addr;
                for(int i=0;i<labeltable.size();i++){
                    if(labeltable[i].labelname==st){
                        addr=labeltable[i].address;
                    }
                }
               programCounter=stoi(addr) + 1;
               return;

            }
            
            if(current_instruction.substr(0,2)=="lw"){
                string rd,rs,offset;
                rd=current_instruction.substr(2,2);
                int index=current_instruction.find("(");
                rs=current_instruction.substr(index+1,2);
                offset=current_instruction.substr(4,index-4);
                int offs = stoi(offset);
                int value;
                int reg_store[2]={-1};
                for(int i=0;i<32;i++){
                    if(rs==REG[i])
                        reg_store[0]=i;
                    else if(rd==REG[i])
                        reg_store[1]=i;
                }
                value = register_values[reg_store[0]];
                register_values[reg_store[1]] = MEM[(offs + value)/4];
                programCounter++;

                //----------------------------------------
                int adrs;
                adrs = (offs + value)/4;
                if(search1(adrs) == true){//hit in L1
                    miss=0;
                    incrementcounter1(adrs);
                    accessesL1++;
                }
                else if(search2(adrs) == true){//hit in L2 but was a miss in L1
                    miss=1; 
                    totalL1misses++;
                    L2toL1(adrs);
                    accessesL1++;
                    accessesL2++; 
                }
                else{
                    miss=2;
                    totalL1misses++;
                    totalL2misses++;
                    memtoL1(adrs);
                    accessesL1++;
                    accessesL2++;
                }
                //----------------------------------------
                return;
            }

            if(current_instruction.substr(0,2)=="sw"){
                  string rd,rs,offset;
                  rs=current_instruction.substr(2,2);
                  int index=current_instruction.find("(");
                  rd=current_instruction.substr(index+1,2);
                  offset=current_instruction.substr(4,index-4);
                int offs = stoi(offset);
                int value;

                int reg_store[2]={-1};
                for(int i=0;i<32;i++){
                    if(rs==REG[i])
                        reg_store[0]=i;
                    else if(rd==REG[i])
                        reg_store[1]=i;
                }                                     
                value = register_values[reg_store[1]];
                MEM[(offs + value)/4]=register_values[reg_store[0]];
                programCounter++;

                //---------------------------------------
                int adrs, value1;
                value1 = register_values[reg_store[0]];
                adrs = (offs + value)/4;
                if(search1(adrs) == true){//hit in L1
                    miss=0;
                    incrementcounter1(adrs);
                    updateInL1(adrs,value1);
                    accessesL1++;
                }
                else if(search2(adrs) == true){//hit in L2 but was a miss in L1
                    miss=1; 
                    L2toL1(adrs);
                    updateInL2(adrs,value1); 
                    totalL1misses++;
                    accessesL1++;
                    accessesL2++; 
                }
                else{
                    miss=2;
                    memtoL1(adrs);
                    totalL1misses++;
                    totalL2misses++;
                    accessesL1++;
                    accessesL2++;
                }
                //----------------------------------------
                return;
            }

            if(current_instruction.substr(0,3)=="slt"){
                string rd,src1,src2;
                rd=current_instruction.substr(3,2);
                src1=current_instruction.substr(5,2);
                src2=current_instruction.substr(7,2);
                int reg_store[3]={-1};
                for(int i=0;i<32;i++){
                    if(src1==REG[i])
                    reg_store[0]=i;
                    else if(src2==REG[i])
                    reg_store[1]=i;
                    else if(rd==REG[i])
                    reg_store[2]=i;
                }
                if(register_values[reg_store[0]]<register_values[reg_store[1]]){
                    register_values[reg_store[2]]=1;
                }
                else{
                     register_values[reg_store[2]]=0;
                }
                programCounter++;
                return;
            }
            if(current_instruction.substr(0,2)=="la"){ //las0array
                for(int i=0;i<32;i++){
                    if(current_instruction.substr(2,2)==REG[i]){
                        register_values[i]=0; //MEM[0]=0;
                        break;
                    }
                }
                programCounter++;
                return;
            }
            if(current_instruction.substr(0,2)=="jr"){
                programCounter++;
                return;
            }
        }

        void fill(int x,int y,int iF, int id, int ex, int mem, int wb){
            while(iF!=0){
                if(pipeline[x][y]=="stall"){
                    y++;
                }
                else{
                    pipeline[x][y]="stall";
                    y++;
                    iF--;
                }
            }
            while(pipeline[x][y]=="stall"){
                y++;
            }

            pipeline[x][y]="IF";
            y++;
            while(id!=0){
                if(pipeline[x][y]=="stall"){
                    y++;
                }
                else{
                    pipeline[x][y]="stall";
                    y++;
                    id--;
                }
            }
            while(pipeline[x][y]=="stall"){
                y++;
            }
            pipeline[x][y]="ID";
            y++;
            while(ex!=0){
                if(pipeline[x][y]=="stall"){
                    y++;
                }
                else{
                    pipeline[x][y]="stall";
                    y++;
                    ex--;
                }
            }
            while(pipeline[x][y]=="stall"){
                y++;
            }
            pipeline[x][y]="EX";
            y++;
            while(mem!=0){
                if(pipeline[x][y]=="stall"){
                    y++;
                }
                else{
                    pipeline[x][y]="stall";
                    y++;
                    mem--;
                }
            }
            while(pipeline[x][y]=="stall"){
                y++;
            }
            if(pipeline[x][0].substr(0,2)=="lw" || pipeline[x][0].substr(0,2)=="sw"){
                if(miss_arr[x]==0){//miss=?
                    int temp_time=accessLatency1;
                    while(temp_time!=0){
                        pipeline[x][y]="MEM";
                        y++;
                        temp_time--;
                    }
                }
                else if(miss_arr[x]==1){
                    int temp_time=accessLatency1+accessLatency2;
                    while(temp_time!=0){
                        pipeline[x][y]="MEM";
                        y++;
                        temp_time--;
                    }
                }
                else{
                    int temp_time=accessLatency1+accessLatency2+memTime;
                    while(temp_time!=0){
                        pipeline[x][y]="MEM";
                        y++;
                        temp_time--;
                    }
                }
            }
            else{
            pipeline[x][y]="MEM";
            y++;
            }
            while(wb!=0){
                if(pipeline[x][y]=="stall"){
                    y++;
                }
                else{
                    pipeline[x][y]="stall";
                    y++;
                    wb--;
                }
            }
            while(pipeline[x][y]=="stall"){
                y++;
            }
            pipeline[x][y]="WB";
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

            return "null";
            //if ..... other functions
        }

        bool branchhazard(string ins){
            bool flag=false;
             if(ins.substr(0,3)=="beq"||ins.substr(0,3)=="bne"||(ins.substr(0,1)=="j"&&ins.substr(1,1)!="r")){
                flag=true;
            }
            return flag;
        }
      
        int memory_hazard(int ins_row){
           int result=0;
           if(pipeline[ins_row][0].substr(0,2)=="lw"||pipeline[ins_row][0].substr(0,2)=="sw"){
               int temp_miss=miss_arr[ins_row];
               if(temp_miss==0){
                  result=accessLatency1-1;
               }
               else if(temp_miss==1){
                   result=accessLatency1+accessLatency2-1;
               }
               else{
                   result=accessLatency1+accessLatency2+memTime-1;
               }
               return result;
           }
           else{
               return result;
           }
       }
        
        void stalls_hazard(int ins_row){
            int IF,ID,EX,MEM;
            int clk_len=0;
            for(int j=1;j<1000;j++){
                if(pipeline[ins_row][j]=="WB")
                  clk_len=j;
            }
            for(int j=1;j<clk_len;j++){
              if(pipeline[ins_row][j]=="IF")
              IF=j;
               if(pipeline[ins_row][j]=="ID")
              ID=j;
               if(pipeline[ins_row][j]=="EX")
              EX=j;
               if(pipeline[ins_row][j]=="MEM")
              MEM=j;
            }
            for(int j=IF+1;j<ID;j++){
                if(pipeline[ins_row][j]=="stall"){

                  pipeline[ins_row+1][j]="stall";


                }
            }
            for(int j=ID+1;j<EX;j++){
                if(pipeline[ins_row][j]=="stall"){
                  pipeline[ins_row+1][j]="stall";
                }
            }
            for(int j=EX+1;j<MEM;j++){
                if(pipeline[ins_row][j]=="stall"){
                  //fill(ins_row+1,i,0,0,0,1,0);

                  pipeline[ins_row+1][j]="stall";
                }
            }
            for(int j=MEM+1;j<clk_len;j++){
                if(pipeline[ins_row][j]=="stall"){
                  //fill(ins_row+1,i,0,0,0,0,1);
                  pipeline[ins_row+1][j]="stall";
                }
            }

        }

        void fillPipeline(int numb_rows,int flagForwdg){
            int clock1=1;
            int j=0;
            for(int i=0; i<numb_rows; i++){
                j=clock1;

                if(pipeline[i][0].substr(0,4)=="addi"){
                     //no of cycles-1 if it is a lw or sw
                    if(i!=0 && pipeline[i][0].substr(6,2) == hazard(pipeline[i-1][0])){
                       //if its a lw or sw..then it returns time 
                       int res=memory_hazard(i-1);
                         if(flagForwdg==0){//no forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                if(res!=0){//suppose 4 cycles res=3
                                   fill(i,j,1,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,1,0,2,0,0);
                                }
                           }
                            else{
                            if(res!=-1){
                                   fill(i,j,0,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,0,0,2,0,0);
                                }
                            }     
                        }
                        //doubtful in case of forwarding
                        else{//with forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                             if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                            }
                            else{
                             if(res!=-1){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                            }
                        }
                    }
                    else if(i==0){
                        fill(i,j,0,0,0,0,0);
                    }
                    else{   //i!=0 and no hazard in previous instruction
                    int res=memory_hazard(i-1);
                         stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            if(res!=-1){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,res,0,0);
                                }
                        }
                        else{
                             if(res!=-1){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,res,0,0);
                                }
                        }
                       
                    }
                }
                if(pipeline[i][0].substr(0,3)=="add" && pipeline[i][0].substr(3,1)!="i"){
                    int res=memory_hazard(i-1);
                    if(pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(7,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            if(res!=0){
                                   fill(i,j,1,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,1,0,2,0,0);
                                }
                            }
                            else{
                                if(res!=0){
                                   fill(i,j,0,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,0,0,2,0,0);
                                }
                            }
                           
                            
                        }
                        else{//with forwarding
                         stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                   if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                            }
                           
                        }
                       
                    }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            // fill(i,j,1,0,0,0,0);
                             if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                         }
                            
                        else{
                            //fill(i,j,0,0,0,0,0);
                            if(res!=0){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                        }
                       
                    }
                }

                if(pipeline[i][0].substr(0,3)=="sub"){
                    int res=memory_hazard(i-1);
                    if(pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(7,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            if(res!=0){
                                   fill(i,j,1,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,1,0,2,0,0);
                                }
                            }
                            else{
                                if(res!=0){
                                   fill(i,j,0,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,0,0,2,0,0);
                                }
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                            }
                           
                        }
                      
                    }
                    else{
                         stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            // fill(i,j,1,0,0,0,0);
                             if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                         }
                            
                        else{
                            //fill(i,j,0,0,0,0,0);
                            if(res!=0){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                        }
                    }
                }

                if(pipeline[i][0].substr(0,3)=="mul"){
                    int res=memory_hazard(i-1);
                    if(pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(7,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                           if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,2,0,0);
                           }
                           
                            else{
                                fill(i,j,0,0,2,0,0);
                            }
                        }
                        else{//with forwarding
                           stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                            }
                           
                        } 
                    }
                    else{
                         stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            // fill(i,j,1,0,0,0,0);
                             if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                         }
                            
                        else{
                            //fill(i,j,0,0,0,0,0);
                            if(res!=0){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                        }
                        
                    }
                }

                if(pipeline[i][0].substr(0,3)=="div"){
                    int res=memory_hazard(i-1);
                    if(pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(7,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                          stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,1,0,2,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,0,0,2,0,0);
                                }
                            }
                           
                        }
                        else{//with forwarding
                             stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                            }
                             
                      }
                }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            // fill(i,j,1,0,0,0,0);
                             if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                         }
                            
                        else{
                            //fill(i,j,0,0,0,0,0);
                            if(res!=0){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                        }
                       
                    }
                }

                if(pipeline[i][0].substr(0,3)=="slt"){
                    int res=memory_hazard(i-1);
                   if(pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(7,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                            stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,1,0,2,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,0,0,2,0,0);
                                }
                            }
                        }
                        else{//with forwarding
                           stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                            }
                        }
                       
                    }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            // fill(i,j,1,0,0,0,0);
                             if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                         }
                            
                        else{
                            //fill(i,j,0,0,0,0,0);
                            if(res!=0){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                        }
                       
                       
                    }
                }
                if(pipeline[i][0].substr(0,3)=="beq"){
                     int pc;
                     int res=memory_hazard(i-1);
                     branch_flag=0;
                     for(int j=0;j<Input_ins.size();j++){
                        if(pipeline[i][0]==Input_ins[j]){
                        pc=j;
                        }
                                                    
                     }
                     if(pipeline[i+1][0]!=Input_ins[pc+1])
                     branch_flag=1;
                     else
                     branch_flag=0;
                    if(pipeline[i][0].substr(3,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                            stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,1,0,2,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,0,0,2,0,0);
                                }
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                            }
                        }
                       
                    }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            // fill(i,j,1,0,0,0,0);
                             if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                         }
                            
                        else{
                            //fill(i,j,0,0,0,0,0);
                            if(res!=0){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                        }
                        
                        
                    }
                    

                }
                 if(pipeline[i][0].substr(0,3)=="bne"){
                    int pc;
                    int res=memory_hazard(i-1);
                     branch_flag=0;
                     for(int j=0;j<Input_ins.size();j++){
                        if(pipeline[i][0]==Input_ins[j])
                            pc=j;
                     }
                     if(pipeline[i+1][0]!=Input_ins[pc+1])
                     branch_flag=1;
                     else
                     branch_flag=0;
                     if(pipeline[i][0].substr(3,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                           stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,1,0,2,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,0,0,2,0,0);
                                }
                            }
                        }
                        else{//with forwarding
                           stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,1,0,2,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,0,0,2,0,0);
                                }
                            }
                        }
                       
                    }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            // fill(i,j,1,0,0,0,0);
                             if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                         }
                            
                        else{
                            //fill(i,j,0,0,0,0,0);
                            if(res!=0){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                        }
                        
                        
                    }
                     

                }
                 if(pipeline[i][0].substr(0,1)=="j" && pipeline[i][0].substr(1,1)!="r"){
                    int pc;
                     branch_flag=0;
                     for(int j=0;j<Input_ins.size();j++){
                        if(pipeline[i][0]==Input_ins[j])
                            pc=j;
                     }
                     if(pipeline[i+1][0]!=Input_ins[pc+1])
                     branch_flag=1;
                     else
                     branch_flag=0;
                     int res=memory_hazard(i-1);
                    stalls_hazard(i-1);
                    if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                        //fill(i,j,1,0,0,0,0);
                         if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }

                    }
                    else{
                        //fill(i,j,0,0,0,0,0);
                         if(res!=0){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                    }
                }
                  if(pipeline[i][0].substr(0,2)=="lw"){
                      int res=memory_hazard(i-1);
                    if(i!=0 && pipeline[i][0].substr(pipeline[i][0].length()-3,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                            stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,1,0,2,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,0,0,2,0,0);
                                }
                            }
                        }
                        else{//with forwarding
                              stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                            }
                        }
                    }
                    else{
                       stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            // fill(i,j,1,0,0,0,0);
                             if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                         }
                            
                        else{
                            //fill(i,j,0,0,0,0,0);
                            if(res!=0){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                        }
                    }
                }

                if(pipeline[i][0].substr(0,2)=="sw"){
                    int res=memory_hazard(i-1);
                    if(i!=0 && pipeline[i][0].substr(pipeline[i][0].length()-3,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                            stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,1,0,2,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res+2,0,0);
                                }
                                else{
                                    fill(i,j,0,0,2,0,0);
                                }
                            }
                        }
                        else{//with forwarding
                           stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                             }
                           
                            else{
                                if(res!=-1){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                            }
                        }
                    }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            // fill(i,j,1,0,0,0,0);
                             if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }
                         }
                            
                        else{
                            //fill(i,j,0,0,0,0,0);
                            if(res!=0){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                        }
                       
                       
                    }
                }


                if(pipeline[i][0].substr(0,2)=="la"){   //data and structural hazards not possible in la
                         int res=memory_hazard(i-1);
                    stalls_hazard(i-1);
                    if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                        //fill(i,j,1,0,0,0,0);
                         if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }

                    }
                    else{
                        //fill(i,j,0,0,0,0,0);
                         if(res!=0){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                    }
                }

                if(pipeline[i][0].substr(0,2)=="jr"){
                       //data and structural hazards not possible in jr
                       int res=memory_hazard(i-1);
                    stalls_hazard(i-1);
                    if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                        //fill(i,j,1,0,0,0,0);
                         if(res!=0){
                                   fill(i,j,1,0,res,0,0);
                                }
                                else{
                                    fill(i,j,1,0,0,0,0);
                                }

                    }
                    else{
                        //fill(i,j,0,0,0,0,0);
                         if(res!=0){
                                   fill(i,j,0,0,res,0,0);
                                }
                                else{
                                    fill(i,j,0,0,0,0,0);
                                }
                    }
                }

                for(int q=1; q<1000; q++){
                    if(pipeline[i][q]=="IF"){
                            clock1=q+1;
                    }
                }
            }
        }

        void execute(int flag){
            preprocess();
            //int mainindex;
            for(int i=1;i<=NumberOfInstructions-1;i++){
                if(InputProgram[i]=="main:"){
                mainindex=i;
                break;
                }
            }
            programCounter=mainindex+2;

            int pipeRow = 0;
            while(programCounter<=NumberOfInstructions){
                string current_instruction = readInstruction(InputProgram[programCounter-1]);
                miss=-1; 
                processInstruction(current_instruction);//miss= 0 1 2
                miss_arr[pipeRow]=miss;

                //cout << current_instruction << endl;
                pipeline[pipeRow][0]=current_instruction;
                pipeRow++;
            }

            fillPipeline(pipeRow, flag);

            int cnt=0;
            for(int j=1;j<1000;j++){
                if(pipeline[pipeRow-1][j] == "WB"){
                    cout << "Total number of clock cycles: " << j << endl<<endl;
                    cnt=j;
                }
            }

            string stallInstruction[500];
            int count=0;
            int k=0;
            for(int i=0; i<pipeRow ;i++){
                for(int j=1;j<1000;j++){
                    if(pipeline[i][j]=="stall"){
                        count++;
                        stallInstruction[k] = pipeline[i][0];
                    }
                }
                k++;
            }
 
            cout << "Total number of stalls: " << count <<endl<<endl;
            float ipc=(float)pipeRow/cnt;
            cout<<"IPC(Instructions per cycle is) :"<<ipc<<endl<<endl;

            cout << "List of instructions for which stalls occur: " << endl<<endl<<endl;

            for(int i=0;i<k;i++){
                if(stallInstruction[i]!=""){
                    cout << stallInstruction[i] << endl;
                }
            }

            cout << "Miss rate for cache L1: " << totalL1misses/accessesL1 << endl;
            cout << "Miss rate for cache L2: " << totalL2misses/accessesL2 << endl;
        
            cout<<endl<<endl;
             cout<<"MEMORY:"<<endl;
                for(int i=0;i<1024;i++){
                    if(MEM[i]!=0){
                         cout<<MEM[i]<<" ";
                    }
                }
                cout<<endl;
            return;
        }
};
int main(){
    cout<<"Welcome to Team dynamic MIPS SIMULATOR!!"<<endl;

    int Cache1Size, Cache2Size, Block1Size, Block2Size, AccessLatency1, AccessLatency2, MemTime, associativity;

    cout << "Enter the cache sizes of L1 and L2: " << endl;
    cin >> Cache1Size >>  Cache2Size;
    cout << "Enter the block sizes of L1 and L2: " << endl;
    cin >> Block1Size >>  Block2Size;
    cout << "Enter the accociativity: " << endl;
    cin >> associativity;
    cout << "Enter the access latencies of L1 and L2: " << endl;
    cin >>  AccessLatency1 >> AccessLatency2;
    cout << "Enter the memory access time: " << endl;
    cin >> MemTime;

    //mipsSimulator simulator("BubbleSort.asm");
    mipsSimulator simulator("mipsBubblesort.asm",Cache1Size,Cache2Size,Block1Size,Block2Size,AccessLatency1,AccessLatency2,MemTime,associativity);

    int flagFrwd;
    cout << "ENTER 1 for Forwarding and 0 for NO forwarding" << endl;
    cin >> flagFrwd;
    simulator.execute(flagFrwd);
}

/*
TESTCASE:

Enter the cache sizes of L1 and L2:
16 32
Enter the block sizes of L1 and L2:
8 8
Enter the accociativity:
2
Enter the access latencies of L1 and L2:
2 3
Enter the memory access time:
5
ENTER 1 for Forwarding and 0 for NO forwarding
0

*/
