#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

const int MAX_MEM_SIZE  = (1 << 13);

void fetchStage(int *icode, int *ifun, int *rA, int *rB, wordType *valC, wordType *valP) {
	wordType pcAddress = getPC();
	byteType firstByte = getByteFromMemory(pcAddress);
	// gets the most significant nibble (left-most)
	*icode = (firstByte >> 4) & 0xf;
	// gets the least significant nibble (right-most)
	*ifun = firstByte & 0xf;
	
	if ((*icode == IRMOVQ) || (*icode == MRMOVQ) || (*icode == RMMOVQ)) {
		byteType secondByte = getByteFromMemory(pcAddress + 1);
		wordType eightBytes = getWordFromMemory(pcAddress + 2);
		// gets the most significant nibble (left-most)
		*rA = (secondByte >> 4) & 0xf;
		// gets the least significant nibble (right-most)
		*rB = secondByte & 0x0f;

		*valC = eightBytes;
		*valP = pcAddress + 10;
	}
	else if (*icode == NOP || *icode == HALT || *icode == RET) {
		*valP = pcAddress + 1;
	}
	else if (*icode == RRMOVQ || *icode == OPQ || *icode == PUSHQ ||*icode == POPQ) {
		byteType secondByte = getByteFromMemory(pcAddress + 1);
		*rB = secondByte & 0xf;
		*rA = (secondByte >> 4) & 0xf;
		*valP = pcAddress + 2;
	}
  else if (*icode == JXX || *icode == CALL) {
    wordType eightBytes = getWordFromMemory(pcAddress + 1);
    *valC = eightBytes;
    *valP = pcAddress + 9;
  }
}

void decodeStage(int icode, int rA, int rB, wordType *valA, wordType *valB) {
 	if (icode == RRMOVQ) {
		*valA = getRegister(rA);
	}
  else if (icode == RMMOVQ || icode == OPQ) {
    *valA = getRegister(rA);
    *valB = getRegister(rB);
  }
  else if (icode == MRMOVQ) {
    *valB = getRegister(rB);
  }
  else if (icode == CALL) {
    *valB = getRegister(RSP); 
  }
  else if (icode == RET || icode == POPQ) {
    *valA = getRegister(RSP);
    *valB = getRegister(RSP);
  }
  else if (icode == PUSHQ) {
    *valA = getRegister(rA);
    *valB = getRegister(RSP);
  }
}

void executeStage(int icode, int ifun, wordType valA, wordType valB, wordType valC, wordType *valE, bool *Cnd) {
	if (icode == IRMOVQ) {
		*valE = 0 + valC;
	}
	else if (icode == RRMOVQ) {
		*valE = 0 + valA;
	}
  else if ((icode == RMMOVQ) || (icode == MRMOVQ)) {
    *valE = valB + valC;
  }
  else if (icode == OPQ) {
    if (ifun == ADD){ //OR if(Cond(ADD)){ ?
    *valE = valB + valA;
    overflowFlag = ((valA<0) == (valB<0)) && ((*valE<0) != (valA<0));
    }
    else if (ifun == SUB){
    *valE = valB - valA;
    /* If two "two's compliment" #'s are subtracted and their signs are different, 
    then overflow occurs if and only if the result has the same sign as the subtrahend. */
    overflowFlag = ((valA<0) != (valB<0)) && ((*valE<0) == (valA<0));
    }
    else if (ifun == AND){
    *valE = valB & valA;
    }
    else if (ifun == XOR){
    *valE = valB ^ valA;
    }
    // Set other two CC's
    signFlag = (*valE < 0);
    zeroFlag = (*valE == 0);
    setFlags(signFlag, zeroFlag, overflowFlag);
  }
  else if (icode == JXX) {
    *Cnd = Cond(ifun);
  }
  else if (icode == CALL || icode == PUSHQ) {
    *valE = valB - 8;
  }
  else if (icode == RET|| icode == POPQ) {
    *valE = valB + 8;
  }
}

void memoryStage(int icode, wordType valA, wordType valP, wordType valE, wordType *valM) {
  if (icode == RMMOVQ || icode == PUSHQ) {
    setWordInMemory(valE, valA);
  }
  else if (icode == MRMOVQ) {
    *valM = getWordFromMemory(valE);
  }
  else if (icode == CALL) {
    setWordInMemory(valE, valP);
  }
  else if (icode == RET || icode == POPQ) {
    *valM = getWordFromMemory(valA);
  }
}

void writebackStage(int icode, int rA, int rB, wordType valE, wordType valM) {
 if (icode == IRMOVQ || icode == RRMOVQ || icode == OPQ) {
	 setRegister(rB, valE);
 }
 else if (icode == MRMOVQ) {
   setRegister(rA, valM);
 }
 else if (icode == CALL || icode == RET || icode == PUSHQ) {
   setRegister(RSP, valE);
 }
 else if (icode == POPQ) {
   setRegister(RSP, valE);
   setRegister(rA, valM);
 }
}

void pcUpdateStage(int icode, wordType valC, wordType valP, bool Cnd, wordType valM) {
  if (icode == IRMOVQ || icode == NOP || icode == RRMOVQ || icode == RMMOVQ || icode == MRMOVQ || icode == OPQ || icode == PUSHQ || icode == POPQ) {
		setPC(valP);
	}
	else if (icode == HALT) {
		setPC(valP);
		setStatus(STAT_HLT);
	}
  else if (icode == JXX) {
    if (Cnd) {
      setPC(valC);
    } else {
      setPC(valP);
    }
  }
  else if (icode == CALL) {
    setPC(valC);
  }
  else if (icode == RET) {
    setPC(valM);
  }
}

void stepMachine(int stepMode) {
  /* FETCH STAGE */
  int icode = 0, ifun = 0;
  int rA = 0, rB = 0;
  wordType valC = 0;
  wordType valP = 0;
 
  /* DECODE STAGE */
  wordType valA = 0;
  wordType valB = 0;

  /* EXECUTE STAGE */
  wordType valE = 0;
  bool Cnd = 0;

  /* MEMORY STAGE */
  wordType valM = 0;

  fetchStage(&icode, &ifun, &rA, &rB, &valC, &valP);
  applyStageStepMode(stepMode, "Fetch", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

  decodeStage(icode, rA, rB, &valA, &valB);
  applyStageStepMode(stepMode, "Decode", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  executeStage(icode, ifun, valA, valB, valC, &valE, &Cnd);
  applyStageStepMode(stepMode, "Execute", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  memoryStage(icode, valA, valP, valE, &valM);
  applyStageStepMode(stepMode, "Memory", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  writebackStage(icode, rA, rB, valE, valM);
  applyStageStepMode(stepMode, "Writeback", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  pcUpdateStage(icode, valC, valP, Cnd, valM);
  applyStageStepMode(stepMode, "PC update", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

  incrementCycleCounter();
}

/** 
 * main
 * */
int main(int argc, char **argv) {
  int stepMode = 0;
  FILE *input = parseCommandLine(argc, argv, &stepMode);

  initializeMemory(MAX_MEM_SIZE);
  initializeRegisters();
  loadMemory(input);

  applyStepMode(stepMode);
  while (getStatus() != STAT_HLT) {
    stepMachine(stepMode);
    applyStepMode(stepMode);
#ifdef DEBUG
    printMachineState();
    printf("\n");
#endif
  }
  printMachineState();
  return 0;
}