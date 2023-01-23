#include <stdio.h>
#include <string.h>
#include <omp.h>

int func(int);
int exec(unsigned long long int, int);
void dis(unsigned long long int);

omp_lock_t lock;

int constants[] = { -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7 };

enum op_codes { halt=0, add, sub, shl, shr, and, or, not, xor, nand, nor, xnor, rotr, rotl, rev };

int func(int arg){
  return (arg ^0x17) + 5;
}

int main(int argc, char** argv){
  // TODO: implement cli arg parsing instead of static vals
  
  int solutions[16];
  
  for(int i = 0; i < 16; ++i){
    solutions[i] = func(constants[i]);
  }

  omp_init_lock(&lock);

#pragma omp parallel for
  for(unsigned long long int i = 0; i <= (unsigned long long int)-1; ++i){

    // since 0x00 is halt, 0x0f001213 == 0xf498001213...fill the gaps
    int flag = 0;
    for(int j = 7; j >= 0; --j){
      if(flag == 1){
	*((char*)&i + j) |= 0x01;
      }else if((*((char*)&i + j) | 0x0f) == 0){
	flag = 1;
	*((char*)&i + j) |= 0x01;
      }
    }

    for(int k = 1; k < 16; ++k){
      if(exec(i, constants[k]) != solutions[k]){
	break;
      }else if(k == 15){

	for(int l = -128; l < 128; ++l){
	  if(exec(i, l) != func(l)){
	    break;
	  }else if(l == 127){
	    omp_set_lock(&lock);
	    printf("found: 0x%llx\n\n", i);
	    dis(i);
	    printf("\n");
	    omp_unset_lock(&lock);
	  }
	}
	
      }
    }
  }
  return 0;
}

void dis(unsigned long long int prog){
  char opcode;
  unsigned long long int operand;

  // move to a char*/snprintf/printf form so 0 doesn't mean user param
  
  while(1){
    opcode = prog & 0xf;
    prog = prog >> 4;
      
    switch(opcode) {
      
    case halt:
      return;
      
    case add:
      operand = constants[prog & 0xf];
      printf("add %lld\n", operand);
      break;

    case sub:
      operand = constants[prog & 0xf];
      printf("sub %lld\n", operand);
      break;

    case shl:
      operand = constants[prog & 0xf];
      printf("shl %lld\n", operand);
      break;

    case shr:
      operand = constants[prog & 0xf];
      printf("shr %lld\n", operand);
      break;

    case and:
      operand = constants[prog & 0xf];
      printf("and %lld\n", operand);
      break;

    case or:
      operand = constants[prog & 0xf];
      printf("or %lld\n", operand);
      break;

    case not:
      printf("not\n");
      break;
      
    case xor:
      operand = constants[prog & 0xf];
      printf("xor %lld\n", operand);
      break;

    case nand:
      operand = constants[prog & 0xf];
      printf("nand %lld\n", operand);
      break;

    case nor:
      operand = constants[prog & 0xf];
      printf("nor %lld\n", operand);
      break;

    case xnor:
      operand = constants[prog & 0xf];
      printf("xnor %lld\n", operand);
      break;      
      
    case rotr:
      operand = prog & 0xf;
      printf("rotr %lld\n", operand);
      break;

    case rotl:
      operand = prog & 0xf;
      printf("rotl %lld\n", operand);
      break;

    case rev:
      printf("rev\n");
      break;
      
      // find a final operation for the last opcode 0xf
      
    default:
      printf("invalid\n");
      break;
    }
    prog = prog >> 4;
  }
}

int exec(unsigned long long int prog, int param){
  char opcode;
  int operand;
  int result = 0;
  
  while(1){
    opcode = prog & 0xf;
    prog = prog >> 4;
      
    switch(opcode) {
      
    case halt:
      return result;
      
    case add:
      operand = constants[prog & 0xf];
      if(operand == 0){
	operand = param;
      }
      result += operand;
      break;

    case sub:
      operand = constants[prog & 0xf];
      if(operand == 0){
	operand = param;
      }
      result -= operand;
      break;

    case shl:
      operand = constants[prog & 0xf];
      result =  result << operand;
      break;

    case shr:
      operand = constants[prog & 0xf];
      result =  result >> operand;
      break;

    case and:
      operand = constants[prog & 0xf];
      if(operand == 0){
	operand = param;
      }
      result &= operand;
      break;

    case or:
      operand = constants[prog & 0xf];
      if(operand == 0){
	operand = param;
      }
      result |= operand;
      break;

    case not:
      operand = constants[prog & 0xf];
      if(operand == 0){
	operand = param;
      }
      result = ~(result);
      break;
      
    case xor:
      operand = constants[prog & 0xf];
      if(operand == 0){
	operand = param;
      }
      result ^= operand;
      break;

    case nand:
      operand = constants[prog & 0xf];
      if(operand == 0){
	operand = param;
      }
      result = ~(result & operand);
      break;

    case nor:
      operand = constants[prog & 0xf];
      if(operand == 0){
	operand = param;
      }
      result = ~(result | operand);
      break;

    case xnor:
      operand = constants[prog & 0xf];
      if(operand == 0){
	operand = param;
      }
      result = ~(result ^ operand);
      break;

    case rotr:
      operand = prog & 0xf;
      result = (result >> operand) & (result << (sizeof(result) - operand));
      break;

    case rotl:
      operand = prog & 0xf;
      result = (result << operand) & (result >> (sizeof(result) - operand));
      break;

    case rev:
      {
	int tmp = 0;
	while(result > 0){
	  tmp <<= 1;
	  if((result & 1) == 1){
	    tmp ^= 1;
	  }
	  result >>= 1;
	}
	result = tmp;
      }
      break;
      
    default:
      break;
    }
    prog = prog >> 4;
  }
}
