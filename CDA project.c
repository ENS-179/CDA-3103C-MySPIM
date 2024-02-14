// Wilson Vu, Reed Kurzen, Elishel Sason
// Final Project
// CDA3103C - 23

#include "spimcore.h"

/* ALU */
/* 10 Points */
// The cases were implemented from the ALU Control table from the project file
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult,
         char *Zero) {
  // case 000: Z = A + B
  if (ALUControl == 0) {
    *ALUresult = A + B;
  }
  // case 001: Z = A - B
  else if (ALUControl == 1) {
    *ALUresult = A - B;
  }
  // case 010: if A < B, Z = 1; otherwise, Z = 0
  else if (ALUControl == 2) {
    *ALUresult = ((int)A < (int)B) ? 1 : 0;
  }
  // case 011: if A < B, Z = 1; otherwise, Z = 0  (A and B are unsigned
  // integers)
  else if (ALUControl == 3) {
    *ALUresult = (A < B) ? 1 : 0;
  }
  // case 100: Z = A AND B
  else if (ALUControl == 4) {
    *ALUresult = A & B;
  }
  // case 101: Z = A OR B
  else if (ALUControl == 5) {
    *ALUresult = (A | B);
  }
  // case 110: Z = Shift B left by 16 bits
  else if (ALUControl == 6) {
    *ALUresult = B << 16;
  }
  // case 111: Z = NOT A
  else if (ALUControl == 7) {
    if (A == 0)
      *ALUresult = ~A;
  }

  if (*ALUresult == 0) {
    *Zero = 1;
  } else {
    *Zero = 0;
  }
}

/* instruction fetch */
/* 10 Points */
/*
1. Fetch the instruction addressed by PC from Mem and write it to instruction.
2. Return 1 if a halt condition occurs; otherwise, return 0.
*/
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction) {
  // This is checking if its word alligned
  if (PC % 4 != 0)
    return 1;

  else {
    *instruction = MEM(PC); // MEM(PC) == Mem(PC >> 2)
    return 0;
  }
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,
                           unsigned *r2, unsigned *r3, unsigned *funct,
                           unsigned *offset, unsigned *jsec) {
  /*Partition instruction into several parts (op, r1, r2, r3, funct, offset,
  jsec).
  2. Read line 41 to 47 of spimcore.c for more information.
  */

  *op = (instruction & 0b11111100000000000000000000000000) >> 26;

  *r1 = (instruction & 0b11111000000000000000000000) >> 21;

  *r2 = (instruction & 0b111110000000000000000) >> 16;

  *r3 = (instruction & 0b1111100000000000) >> 11;

  *funct = instruction & 0b111111;

  *offset = instruction & 0b1111111111111111;

  *jsec = instruction & 0b11111111111111111111111111;
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op, struct_controls *controls) {

  // look for op code for all operations in appendix a then "decode them"
  controls->RegDst = 0;
  controls->Jump = 0;
  controls->Branch = 0;
  controls->MemRead = 0;
  controls->MemtoReg = 0;
  controls->ALUOp = 0;
  controls->MemWrite = 0;
  controls->ALUSrc = 0;
  controls->RegWrite = 0;

  if (op == 0) { // r- type
    controls->RegDst = 1;
    controls->ALUOp = 7;
    controls->RegWrite = 1;
    return 0;
  }

  if (op == 35) { // op code for lw
    controls->MemRead = 1;
    controls->MemtoReg = 1;
    controls->ALUSrc = 1;
    controls->RegWrite = 1;
    return 0;
  }

  if (op == 43) { // op code for sw
    controls->MemWrite = 1;
    controls->ALUSrc = 1;
    return 0;
  }

  if (op == 4) { // op code for beq
    controls->RegDst = 2;
    controls->Branch = 1;
    controls->MemtoReg = 2;
    controls->ALUOp = 1;
    controls->ALUSrc = 2;
    return 0;
  }

  if (op == 8) { // op code for addi
    controls->ALUSrc = 1;
    controls->RegWrite = 1;
    return 0;
  }

  if (op == 10) { // op code for slti
    controls->ALUSrc = 1;
    controls->RegWrite = 1;
    controls->ALUOp = 2;
    return 0;
  }

  if (op == 15) { // op code for lui
    controls->RegWrite = 1;
    controls->ALUSrc = 1;
    controls->ALUOp = 6;

    return 0;
  }

  if (op == 11) { // op code for sltiu
    controls->RegWrite = 1;
    controls->ALUSrc = 1;
    controls->ALUOp = 3;
    return 0;
  }

  if (op == 2) { // op code for j
    controls->RegDst = 2;
    controls->Jump = 1;
    controls->Branch = 2;
    controls->MemtoReg = 2;
    controls->ALUSrc = 2;
    return 0;
  }

  // halt flag
  else
    return 1;
}

/* Read Register */
/* 5 Points */
// If the function is void you dont have to figure out halt or not
// Otherwise return 0 or 1
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1,
                   unsigned *data2) {
  // reads the content from the register array into data1 and data2
  *data1 = Reg[r1];
  *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
// Assign the sign-extended value of offset to extended_value.
void sign_extend(unsigned offset, unsigned *extended_value) {
  /*  16th bit is the sign bit
    During instruction_partition, you may have put all zeros in the upper order
    16 bits. In that case, you would only need to change the value if the sign
    bit is negative.*/
  if ((offset >> 15) == 1) {
    *extended_value = offset | 0b11111111111111110000000000000000;
  } else {
    *extended_value = offset & 0b1111111111111111;
  }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value,
                   unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult,
                   char *Zero) {

  unsigned char ALUControl = ALUOp;

  switch (ALUOp) {
  // Add
  case 0:
    // Sub
  case 1:
  // slt
  case 2:
  // sltu
  case 3:
  // and
  case 4:
  // or
  case 5:
  // shift
  case 6:
    break;
  case 7: // Rest are all the R-Type Instructions
    switch (funct) {
    // add
    case 32:
      ALUControl = 0;
      break;
    // sub
    case 34:
      ALUControl = 1;
      break;
    // and
    case 36:
      ALUControl = 4;
      break;
    // or
    case 37:
      ALUControl = 5;
      break;
    // slt
    case 42:
      ALUControl = 2;
      break;
    // stlu
    case 43:
      ALUControl = 3;
      break;
      // halt if not one of the commands
    default:
      return 1;
    }
    break;
    // if none of the cases halt
  default:
    return 1;
  }

  // ALU called with its B variable assigned the result of the ternary operator
  ALU(data1, (ALUSrc == 1) ? extended_value : data2, ALUControl, ALUresult,
      Zero);

  return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead,
              unsigned *memdata, unsigned *Mem) {
  // Biggest error if you are reading or writing from memory make sure the
  // address is word alligned. If it isnt throw a halt. But only halt if reading
  // or writing from memory. Check if rw memory is asserted. Mem is an array
  // memdata isn't

  if (MemRead == 1) {

    if (ALUresult % 4 == 0 && ALUresult < 65536) { // checking if word alligned

      *memdata = Mem[ALUresult >> 2];
    } else
      return 1; // halt if not word alligned
  }

  if (MemWrite == 1) {
    // what to put in here
    if (ALUresult % 4 == 0 && ALUresult < 65536) { // check if word alligned
      Mem[ALUresult >> 2] = data2; // i think the indexs need work
    } else
      return 1; // halt if not word alligned
  }

  return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata,
                    unsigned ALUresult, char RegWrite, char RegDst,
                    char MemtoReg, unsigned *Reg) {
  /*
  If RegWrite == 1, and MemtoReg == 1, then data is coming from memory
  If RegWrite == 1, and MemtoReg == 0, then data is coming from ALU_result
  If RegWrite == 1, place write data into the register specified by RegDst
*/

  //*reg is register array
  // Check if you have premision to write to the register file (RegWrite == 1)

  if (RegWrite == 1) {
    if (MemtoReg == 1) {
      // data comes from memory
      Reg[r2] = memdata;
    } else if (MemtoReg == 0) {
      // data is coming from ALU_result
      if (RegDst == 1)
        Reg[r3] = ALUresult;
      else
        Reg[r2] = ALUresult;
    }
  }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump,
               char Zero, unsigned *PC) {
  *PC += 4;

  // if for if zero and bracnh == 1?
  // it might be or
  // leaning toward and
  // Jump: Left shift bits of jsec by 2 and use upper 4 bits of PC
  if (Jump == 1) {
    // given in class
    *PC = (*PC & 0xF0000000) | (jsec << 2);
  }

  if (Zero == 1 && Branch == 1) {
    *PC += extended_value << 2;
  }
}

//(*PC & 0xF0000000) | jsec << 2  THIS IS IMPORTANT AND CORRECT  //given in
//class
