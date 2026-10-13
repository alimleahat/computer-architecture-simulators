#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;

#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab csa23, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;  
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;  
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;  
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;    
    bool        nop;    
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;     
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF(int cycle)
		{
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF (Cycle="<<cycle<<")\t"<<endl;
                
				for (int j = 0; j<32; j++)
				{        
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();               
		} 
			
	private:
		vector<bitset<32> >Registers;	
};

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
            ifstream imem;
			string line;
			int i=0;
			imem.open("imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{      
					IMem[i] = bitset<8>(line);
					i++;
				}                    
			}
            else cout<<"Unable to open file";
			imem.close();                     
		}
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public:
        bitset<32> ReadData;  
        DataMem()
        {
            DMem.resize(MemSize); 
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {      
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {     
                    dmemout << DMem[j]<<endl;
                }
                     
            }
            else cout<<"Unable to open file";
            dmemout.close();               
        }             
      
    private:
		vector<bitset<8> > DMem;      
};  

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}
 
unsigned long extract_bits(bitset<32> instruction, int bit1, int bit2){

    bitset <32> result{};

    for (int i = bit1; i <= bit2; i++){
        result[i - bit1] = instruction[i];
    }

    return result.to_ulong();
}

int main()
{
    
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    stateStruct state{};
    state.IF.nop = false;
    state.ID.nop = true;
    state.EX.nop = true;
    state.MEM.nop = true;
    state.WB.nop = true;

    int cycle = 0;


			
             
    while (1) {

        stateStruct newState = state;
        bool branchTaken = false;

        /* --------------------- WB stage --------------------- */

        if(state.WB.wrt_enable && !state.WB.nop&& state.WB.Wrt_reg_addr.to_ulong() != 0) {
            
            myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
        }

        /* --------------------- MEM stage --------------------- */
        if(!state.MEM.nop)
        {
            if (state.MEM.wrt_mem){
                myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);
            }
            else if (state.MEM.rd_mem){
                newState.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);
        
            }
            else {
                newState.WB.Wrt_data = state.MEM.ALUresult;
            }
        newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
        newState.WB.wrt_enable = state.MEM.wrt_enable;
        newState.WB.Rs = state.MEM.Rs;
        newState.WB.Rt = state.MEM.Rt;

        }
        newState.WB.nop = state.MEM.nop;


        /* --------------------- EX stage --------------------- */
     
        if (!state.EX.nop){

            bitset<32> operand1 = state.EX.Read_data1;
            bitset<32> operand2 = state.EX.Read_data2;

            if (!state.WB.nop && state.WB.wrt_enable && state.WB.Wrt_reg_addr.to_ulong() != 0) {

                if (state.WB.Wrt_reg_addr == state.EX.Rs)
                    operand1 = state.WB.Wrt_data;

                if (state.WB.Wrt_reg_addr == state.EX.Rt)
                    operand2 = state.WB.Wrt_data;

            }
            if (!state.MEM.nop && state.MEM.wrt_enable && !state.MEM.rd_mem && state.MEM.Wrt_reg_addr.to_ulong() != 0) {
                    
                if (state.MEM.Wrt_reg_addr == state.EX.Rs)
                    operand1 = state.MEM.ALUresult;
                    
                if (state.MEM.Wrt_reg_addr == state.EX.Rt)
                    operand2 = state.MEM.ALUresult;
            }

            if (state.EX.is_I_type){

                int offset = state.EX.Imm.to_ulong();

                if (state.EX.Imm[15]) {
                    offset = offset - 65536;
                }

                newState.MEM.ALUresult = operand1.to_ulong() + offset;
            }
            else {
                if (state.EX.alu_op){
                    newState.MEM.ALUresult = operand1.to_ulong() + operand2.to_ulong();
                }
                else {
                    newState.MEM.ALUresult = operand1.to_ulong() - operand2.to_ulong();

                }
            }
            newState.MEM.rd_mem = state.EX.rd_mem;
            newState.MEM.wrt_mem = state.EX.wrt_mem;
            newState.MEM.wrt_enable = state.EX.wrt_enable;
            newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
            newState.MEM.Store_data = operand2;
            newState.MEM.Rs = state.EX.Rs;
            newState.MEM.Rt = state.EX.Rt;
        }
        newState.MEM.nop = state.EX.nop;


        /* --------------------- ID stage --------------------- */

        newState.EX.nop = state.ID.nop;

        bool loadUseStall = false;

        if (!state.ID.nop && !state.EX.nop && state.EX.rd_mem && state.EX.wrt_enable && state.EX.Wrt_reg_addr.to_ulong() != 0) {

            unsigned long opcode = extract_bits(state.ID.Instr, 26, 31);
            bitset<5> rs = extract_bits(state.ID.Instr, 21, 25);
            bitset<5> rt = extract_bits(state.ID.Instr, 16, 20);

            bool usesRs = (opcode == 0x00 || opcode == 0x23 || opcode == 0x2B);

            bool usesRt = (opcode == 0x00 || opcode == 0x2B);

            loadUseStall = (usesRs && rs == state.EX.Wrt_reg_addr) || (usesRt && rt == state.EX.Wrt_reg_addr);
        }
        if (loadUseStall) {
            newState.EX.nop = true;
        }

        else if (!state.ID.nop){
            newState.EX.Rs = extract_bits(state.ID.Instr, 21, 25);
            newState.EX.Rt = extract_bits(state.ID.Instr, 16, 20);
            unsigned long Rd = extract_bits(state.ID.Instr, 11, 15);
            newState.EX.Imm = extract_bits(state.ID.Instr, 0, 15);
            newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
            newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
            unsigned long opcode = extract_bits(state.ID.Instr, 26, 31);
            unsigned long funct = extract_bits(state.ID.Instr, 0, 5);

            switch (opcode) {
                case 0x00:
                    newState.EX.is_I_type = false;
                    newState.EX.rd_mem = false;
                    newState.EX.wrt_mem = false;
                    newState.EX.wrt_enable = true;
                    newState.EX.Wrt_reg_addr = Rd;
                    if (funct == 0x21){
                    
                        newState.EX.alu_op = true;
                    }

                    else if (funct == 0x23){

                    newState.EX.alu_op = false;
                    }
                    else {
                        cout << "Unsupported funct" << endl;
                        return 1;
                    }

                    break;
            
                case 0x23:

                    newState.EX.is_I_type = true;
                    newState.EX.rd_mem = true;
                    newState.EX.wrt_mem = false;
                    newState.EX.wrt_enable = true;
                    newState.EX.Wrt_reg_addr = newState.EX.Rt;
                    newState.EX.alu_op = true;
                    
                    break;

                case 0x2B:

                    newState.EX.is_I_type = true;
                    newState.EX.rd_mem = false;
                    newState.EX.wrt_mem = true;
                    newState.EX.wrt_enable = false;
                    newState.EX.Wrt_reg_addr = 0;
                    newState.EX.alu_op = true;

                    break;
                
                case 0x05:

                    newState.EX.nop = true;

                    if (newState.EX.Read_data1 != newState.EX.Read_data2){
                        int offset = newState.EX.Imm.to_ulong();

                        if (newState.EX.Imm[15]) {
                            offset = offset - 65536;
                        }

                        long long target = state.IF.PC.to_ulong();
                        target = target + offset * 4;

                        newState.ID.nop = true;
                        branchTaken = true;
                        newState.IF.PC = target;

                    }
                
                    break;
            
                default:

                cout << "Unsupported opcode" << endl;
                return 1;
            }

        }

        
        /* --------------------- IF stage --------------------- */
        if (loadUseStall) {
            newState.IF = state.IF;
            newState.ID = state.ID;
        }

        else if (branchTaken){

        }
        else if (state.IF.nop) {

            newState.ID.nop = state.IF.nop;
            newState.IF.nop = true;

        }
        else {
            bitset<32> instruction = myInsMem.readInstr(state.IF.PC);
            unsigned long opcode = extract_bits(instruction, 26, 31);

            if (opcode == 0x3F){
                newState.IF.nop = true;
                newState.ID.nop = true;
            }
            else {
                newState.IF.nop = false;
                newState.ID.nop = false;
                newState.ID.Instr = instruction;
                newState.IF.PC = state.IF.PC.to_ulong() + 4;

            }
            

        }
             
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;
        
        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
       
        state = newState; /*** The end of the cycle and updates the current state with the values calculated in this cycle. csa23 ***/ 
        
        myRF.outputRF(cycle); // dump RF;

        cycle++;
    }
    	
	myDataMem.outputDataMem(); // dump data mem 
	
	return 0;
}

// a06ab8e681fd21c64d696c93bec4b1ef