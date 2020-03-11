// ECE-5984 S20 Assignment 1: FunctionInfo.cpp
// PID: sunnywadkar
////////////////////////////////////////////////////////////////////////////////

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"

#include <iostream>

using namespace llvm;

namespace {
  class FunctionInfo : public FunctionPass {
  public:
    static char ID;
    FunctionInfo() : FunctionPass(ID) { }
    ~FunctionInfo() { }

    // We don't modify the program, so we preserve all analyses
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }

    // Do some initialization
    bool doInitialization(Module &M) override {
      errs() << "5984 Function Information Pass\n";
      outs()<<"Name,\tArgs,\tCalls,\tBlocks,\tInsns,\tAdd/Sub,\tMul/Div,\tBr(Cond),\tBr(UnCond)"<<"\n";
      return false;
    }

    // Print output for each function
    bool runOnFunction(Function &F) override {
      int bbcount = 0;
      int instcount = 0;
      int callcount = 0;
      int argscount = 0;
      int addcount = 0;
      int mulcount = 0;
      int branchcount = 0;
      int unconditional_branch_count = 0;
      //loop through each basic block
      for(BasicBlock &BB : F)
      {
        //loop through each instruction in a basic block
	      for(Instruction &II : BB)
	      {
		      Instruction *I = &II;
		      if((I->getOpcode() == Instruction::Add) || (I->getOpcode() == Instruction::Sub)) // If the instruction is an add/sub instruction
		      {
			      addcount++;
		      }
		      if((I->getOpcode() == Instruction::Mul) || (I->getOpcode() == Instruction::UDiv) || (I->getOpcode() == Instruction::SDiv)) // If the instruction is an mul/div instruction
		      {
			      mulcount++;
		      }
		      if(BranchInst* BI = dyn_cast<BranchInst>(I)) // If instruction is branch instruction
		      {
		      	if(BI->isConditional()) // Is conditional branch
		      	{
			      branchcount++;
		      	}
		     	 else if(BI->isUnconditional()) // Is unconditional branch
		      	{
			      unconditional_branch_count++;
		      	}
		      }
		      if(dyn_cast<CallInst>(I)) // Is call instruction
		      {
			      callcount++;
		      }
		      instcount++;
	      }
	      bbcount++;
      }
      for(Function::arg_iterator A = F.arg_begin(); A != F.arg_end(); ++A) // arguments iterator
      {
	      argscount++;
      }
      outs()<<F.getName() << ",\t" << argscount << ",\t" << callcount << ",\t" << bbcount << ",\t" << instcount<< ",\t"<<addcount<<",\t"<<mulcount<<",\t"<<branchcount<<",\t"<<unconditional_branch_count<<"\n";
      return false;
    }
  };
}

// LLVM uses the address of this static member to identify the pass, so the
// initialization value is unimportant.
char FunctionInfo::ID = 0;
static RegisterPass<FunctionInfo> X("function-info", "5984: Function Information", false, false);
