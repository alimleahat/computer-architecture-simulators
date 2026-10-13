#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

// Internal ALU control codes.
#define ADDU (1)
#define SUBU (3)
#define AND (4)
#define OR  (5)
#define NOR (7)

// Memory size.
// In reality, the memory size should be 2^32, but for this lab and space reasons,
// we keep it as this large number, but the memory is still 32-bit addressable.
#define MemSize (65536)


class RF
{
  public:
    bitset<32> ReadData1, ReadData2; 
    RF()
    { 
      Registers.resize(32);  
      Registers[0] = bitset<32> (0);  
    }

    void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
    {   
      ReadData1 = Registers[RdReg1.to_ulong()];
      ReadData2 = Registers[RdReg2.to_ulong()];
      if ((WrtEnable.to_ulong() == 1) && (WrtReg.to_ulong() != 0)) {
        Registers[WrtReg.to_ulong()] = WrtData;
      }          
    }

    void OutputRF()
    {
      ofstream rfout;
      rfout.open("RFresult.txt",std::ios_base::app);
      if (rfout.is_open())
      {
        rfout<<"A state of RF:"<<endl;
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


class ALU
{
  public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {   
      unsigned int op1 = oprand1.to_ulong();
      unsigned int op2 = oprand2.to_ulong();
      int ALUint = ALUOP.to_ulong();

      if (ALUint == ADDU){
        ALUresult = op1 + op2;
      }
      else if (ALUint == SUBU){
        ALUresult = op1 - op2;
      }
      else if (ALUint == AND){
        ALUresult = op1 & op2;
      }
      else if (ALUint == OR){
        ALUresult = op1 | op2;
      }
      else if (ALUint == NOR){
        ALUresult = ~(op1 | op2);
      }
      return ALUresult;
    }            
};


class INSMem
{
  public:
    bitset<32> Instruction;
    INSMem()
    {       IMem.resize(MemSize); 
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

    bitset<32> ReadMemory (bitset<32> ReadAddress) 
    {    
      int ReadAdd = ReadAddress.to_ulong();
      string b0 = IMem[ReadAdd].to_string();
      string b1 = IMem[ReadAdd + 1].to_string();
      string b2 = IMem[ReadAdd + 2].to_string();
      string b3 = IMem[ReadAdd + 3].to_string();
      Instruction = bitset<32>(b0 + b1 + b2 + b3);


      return Instruction;     
    }     

  private:
    vector<bitset<8> > IMem;

};

class DataMem    
{
  public:
    bitset<32> readdata;  
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
    bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
    {    

      int Add = Address.to_ulong();

      if (readmem == 1){
        string b0 = DMem[Add].to_string();
        string b1 = DMem[Add + 1].to_string();
        string b2 = DMem[Add + 2].to_string();
        string b3 = DMem[Add + 3].to_string();
        readdata = bitset<32>(b0 + b1 + b2 + b3);
      }

  
      if (writemem == 1){
        string wrt = WriteData.to_string();

        string wrt1 = wrt.substr(0, 8);
        string wrt2 = wrt.substr(8, 8);
        string wrt3 = wrt.substr(16, 8);
        string wrt4 = wrt.substr(24, 8);

        DMem[Add] = bitset<8> (wrt1);
        DMem[Add + 1] = bitset<8> (wrt2);
        DMem[Add + 2] = bitset<8> (wrt3);
        DMem[Add + 3] = bitset<8> (wrt4);

      }
      
      return readdata;     
    }   

    void OutputDataMem()
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



int main()
{
  RF myRF;
  ALU myALU;
  INSMem myInsMem;
  DataMem myDataMem;
  bitset<32> PC(0); // 32-bit program counter, initialized to 0

  while (1)  // TODO: implement!
  {

    bitset<32> instruction = myInsMem.ReadMemory(PC);

    if (instruction.to_ulong() == 0xFFFFFFFF) {
      break;
    }

    string inst = instruction.to_string();
    string opcode_b = inst.substr(0, 6);
    int op = bitset<6>(opcode_b).to_ulong();

    string rs_b = inst.substr(6, 5);
    int rs = bitset<5>(rs_b).to_ulong();

    string rt_b = inst.substr(11, 5);
    int rt = bitset<5>(rt_b).to_ulong();

    string rd_b = inst.substr(16, 5);
    int rd = bitset<5>(rd_b).to_ulong();

    string immediate_b = inst.substr(16, 16);
    unsigned int immediate = bitset<16>(immediate_b).to_ulong();

    string funct_b = inst.substr(26, 6);
    int funct = bitset<6>(funct_b).to_ulong();

    if (op == 0x00){

      myRF.ReadWrite(
        bitset<5>(rs),
        bitset<5>(rt),
        bitset<5>(0),
        bitset<32>(0),
        bitset<1>(0)
      );

      if (funct == 0x21){

        bitset<32> result = myALU.ALUOperation(
            ADDU,
            myRF.ReadData1,
            myRF.ReadData2
        );
        myRF.ReadWrite(
            bitset<5>(0),
            bitset<5>(0),
            bitset<5>(rd),
            result,
            bitset<1>(1)
        );
      }
      else if (funct == 0x23) {
        bitset<32> result = myALU.ALUOperation(
          SUBU,
          myRF.ReadData1,
          myRF.ReadData2
      );
      myRF.ReadWrite(
          bitset<5>(0),
          bitset<5>(0),
          bitset<5>(rd),
          result,
          bitset<1>(1)
      );
      }
      else if (funct == 0x24) {
        bitset<32> result = myALU.ALUOperation(
          AND,
          myRF.ReadData1,
          myRF.ReadData2
      );
      myRF.ReadWrite(
          bitset<5>(0),
          bitset<5>(0),
          bitset<5>(rd),
          result,
          bitset<1>(1)
      );
      }
      else if (funct == 0x25) {
        bitset<32> result = myALU.ALUOperation(
          OR,
          myRF.ReadData1,
          myRF.ReadData2
      );
      myRF.ReadWrite(
          bitset<5>(0),
          bitset<5>(0),
          bitset<5>(rd),
          result,
          bitset<1>(1)
      );
      }
      else if (funct == 0x27) {
        bitset<32> result = myALU.ALUOperation(
          NOR,
          myRF.ReadData1,
          myRF.ReadData2
      );
      myRF.ReadWrite(
          bitset<5>(0),
          bitset<5>(0),
          bitset<5>(rd),
          result,
          bitset<1>(1)
      );
      }
      PC = bitset<32>(PC.to_ulong() + 4);
    }
  else if (op == 0x09) {

    myRF.ReadWrite(
        bitset<5>(rs),
        bitset<5>(0),
        bitset<5>(0),
        bitset<32>(0),
        bitset<1>(0)
    );

    long signedImm = immediate;

    if (immediate & 0x8000) {
        signedImm -= 0x10000;
    }

    bitset<32> imm32((unsigned long)signedImm);

    bitset<32> result = myALU.ALUOperation(
        ADDU,
        myRF.ReadData1,
        imm32
    );

    myRF.ReadWrite(
        bitset<5>(0),
        bitset<5>(0),
        bitset<5>(rt),
        result,
        bitset<1>(1)
    );

    PC = bitset<32>(PC.to_ulong() + 4);
}
  else if (op == 0x04) {

    myRF.ReadWrite(
        bitset<5>(rs),
        bitset<5>(rt),
        bitset<5>(0),
        bitset<32>(0),
        bitset<1>(0)
    );

    long signedImm = immediate;

    if (immediate & 0x8000) {
        signedImm -= 0x10000;
    }

    if (myRF.ReadData1 == myRF.ReadData2) {
        long newPC = (long)PC.to_ulong() + 4 + signedImm * 4;
        PC = bitset<32>(newPC);
    }
    else {
        PC = bitset<32>(PC.to_ulong() + 4);
    }
  }
  else if (op == 0x02) {

    bitset<32> PCplus4(PC.to_ulong() + 4);

    string jumpAddress =
        PCplus4.to_string().substr(0, 4) + inst.substr(6, 26) + "00";

    PC = bitset<32>(jumpAddress);
  }
  else if (op == 0x23) {

    myRF.ReadWrite(
        bitset<5>(rs),
        bitset<5>(0),
        bitset<5>(0),
        bitset<32>(0),
        bitset<1>(0)
    );

    long signedImm = immediate;

    if (immediate & 0x8000) {
        signedImm -= 0x10000;
    }

    bitset<32> imm32((unsigned long)signedImm);

    bitset<32> address = myALU.ALUOperation(
        ADDU,
        myRF.ReadData1,
        imm32
    );

    bitset<32> data = myDataMem.MemoryAccess(
        address,
        bitset<32>(0),
        bitset<1>(1),
        bitset<1>(0)
    );

    myRF.ReadWrite(
        bitset<5>(0),
        bitset<5>(0),
        bitset<5>(rt),
        data,
        bitset<1>(1)
    );

    PC = bitset<32>(PC.to_ulong() + 4);
}
else if (op == 0x2B) {

  myRF.ReadWrite(
      bitset<5>(rs),
      bitset<5>(rt),
      bitset<5>(0),
      bitset<32>(0),
      bitset<1>(0)
  );

  long signedImm = immediate;

  if (immediate & 0x8000) {
      signedImm -= 0x10000;
  }

  bitset<32> imm32((unsigned long)signedImm);

  bitset<32> address = myALU.ALUOperation(
      ADDU,
      myRF.ReadData1,
      imm32
  );

  myDataMem.MemoryAccess(
      address,
      myRF.ReadData2,
      bitset<1>(0),
      bitset<1>(1)
  );

  PC = bitset<32>(PC.to_ulong() + 4);
}

    myRF.OutputRF(); // dump RF;    
  }
  myDataMem.OutputDataMem(); // dump data mem

  return 0;
}
