#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/IRBuilder.h"
#include <math.h>
#include <iostream>
#include <stack>

using namespace llvm;
namespace{
    class LocalOpts: public FunctionPass {
        public:
        static char ID;
        LocalOpts() : FunctionPass(ID) {}
        ~LocalOpts() {}

        //Initialization of pass
        bool doInitialization(Module &M) override {
            outs() << "Local Optimization Pass\n";
            return false;
        }

        //function to find the logarithm to base2
        int find_exponential(int64_t n)
        {
            int ct = 0;
            int st = 0;
            while(n)
            {
                if(n & 1)
                {
                    st++;
                }
                n = n >> 1;
                ct++;
            }
            if(st ==  1)
            {
                return ct - 1;
            }
            else
            {
                return -1;
            }
            
        }

        bool runOnFunction(Function &F) override {
            using namespace std;
            Value *z, *op1, *op2, *op3, *o, *temp;
            ConstantInt *x, *y;
            stack<Instruction*>delList;
            int algebraic_identities_count = 0;
            int constant_folding_count = 0;
            int strength_reduction_count = 0;
            //loop through each basic block
            for(BasicBlock &BB : F)
            {
                //loop through each instruction
                for(Instruction &II : BB)
                {
                    Instruction *I = &II;
                    if(dyn_cast<BinaryOperator>(I)) // check if instruction is a binary operator instruction
                    {
                        z = ConstantInt::get(I->getOperand(0)->getType(),0); //Create zero constant
                        o = ConstantInt::get(I->getOperand(0)->getType(),1); //create 1 constant
                        op1 = I->getOperand(0); //extract 1st operand
                        op2 = I->getOperand(1); //extract 2nd operand
                        x = dyn_cast<ConstantInt>(op1); //Check if any operands are constant
                        y = dyn_cast<ConstantInt>(op2);
                        IRBuilder<> buildInst(I); // to generate new instruction
                        if(I->getOpcode() == Instruction::Sub)
                        {
                            if(op1 == op2)
                            {
                                I->replaceAllUsesWith(z); // replace the instruction
                                delList.push(I); // push the instruction into a stack for later removal
                                algebraic_identities_count++;
                            }
                            else if((y != NULL) && (y->isZero()))
                            {
                                I->replaceAllUsesWith(op1);
                                delList.push(I);
                                algebraic_identities_count++;
                            }
                            else if((x != NULL) && (y != NULL)) // both the operands are constant
                            {
                                auto temp_val = x->getSExtValue() - y->getSExtValue(); 
                                temp = ConstantInt::get(I->getOperand(0)->getType(),temp_val);
                                I->replaceAllUsesWith(temp);
                                delList.push(I);
                                constant_folding_count++;
                            }
                        }
                        else if(I->getOpcode() == Instruction:: Add)
                        {
                            if((x != NULL) && (x->isZero()))
                            {
                                I->replaceAllUsesWith(op2);
                                delList.push(I);
                                algebraic_identities_count++;
                            }
                            else if((y != NULL) && (y->isZero()))
                            {
                                I->replaceAllUsesWith(op1);
                                delList.push(I);
                                algebraic_identities_count++;
                            }
                            else if((x != NULL) && (y != NULL))
                            {
                                auto temp_val = x->getSExtValue() + y->getSExtValue();
                                temp = ConstantInt::get(I->getOperand(0)->getType(),temp_val);
                                I->replaceAllUsesWith(temp);
                                delList.push(I);
                                constant_folding_count++; 
                            }
                        }
                        else if(I->getOpcode() == Instruction::Mul)
                        {
                            if((x != NULL) && (x->isZero()))
                            {
                                I->replaceAllUsesWith(z);
                                delList.push(I);
                                algebraic_identities_count++;
                            }
                            else if((y != NULL) && (y->isZero()))
                            {
                                I->replaceAllUsesWith(z);
                                delList.push(I);
                                algebraic_identities_count++;
                            }
                            else if((x != NULL) && (x->isOne()))
                            {
                                I->replaceAllUsesWith(op2);
                                delList.push(I);
                                algebraic_identities_count++;
                            }
                            else if((y != NULL) && (y->isOne()))
                            {
                                I->replaceAllUsesWith(op1);
                                delList.push(I);
                                algebraic_identities_count++;
                            }
                            else if((x != NULL) /*&& (x->equalsInt(2))*/)
                            {
                                auto exp = find_exponential(y->getSExtValue());
                                if(exp != -1)
                                {
                                    Value *e = ConstantInt::get(I->getOperand(0)->getType(),exp);
                                    op3 = buildInst.CreateShl(op2,e);
                                    for(auto& it : I->uses())
                                    {
                                        User* user = it.getUser();
                                        user->setOperand(it.getOperandNo(),op3);
                                    }
                                    delList.push(I);
                                    strength_reduction_count++;
                                }
                            }
                            else if((y != NULL) /*&& (y->equalsInt(2))*/)
                            {
                                auto exp = find_exponential(y->getSExtValue());
                                if(exp != -1)
                                {
                                    Value *e = ConstantInt::get(I->getOperand(0)->getType(),exp);
                                    op3 = buildInst.CreateShl(op1,e);
                                    for(auto& it : I->uses())
                                    {
                                        User* user = it.getUser();
                                        user->setOperand(it.getOperandNo(),op3);
                                    }
                                    delList.push(I);
                                    strength_reduction_count++;
                                }
                            }
                            else if((x != NULL) && (y != NULL))
                            {
                                auto temp_val = x->getSExtValue() * y->getSExtValue();
                                temp = ConstantInt::get(I->getOperand(0)->getType(),temp_val);
                                I->replaceAllUsesWith(temp);
                                delList.push(I);
                                constant_folding_count++;
                            }
                        }
                        else if((I->getOpcode() == Instruction:: SDiv) || (I->getOpcode() == Instruction:: UDiv))
                        {
                            if((I->getOperand(0)) == (I->getOperand(1)))
                            {
                                I->replaceAllUsesWith(o);
                                delList.push(I);
                                algebraic_identities_count++;
                            }
                            else if((x != NULL) && (x->isZero()))
                            {
                                I->replaceAllUsesWith(z);
                                delList.push(I);
                                algebraic_identities_count++;
                            }
                            else if((y != NULL) && (y->isOne()))
                            {
                                I->replaceAllUsesWith(op1);
                                delList.push(I);
                                algebraic_identities_count++;
                            }
                            else if((y != NULL) /*&& (y->equalsInt(2))*/)
                            {
                                auto exp = find_exponential(y->getSExtValue());
                                if(exp != -1)
                                {
                                    Value *e = ConstantInt::get(I->getOperand(0)->getType(),exp);
                                    op3 = buildInst.CreateLShr(op1,e);
                                    for(auto& it : I->uses())
                                    {
                                        User* user = it.getUser();
                                        user->setOperand(it.getOperandNo(),op3);
                                    }
                                    delList.push(I);
                                    strength_reduction_count++;
                                }
                            }
                            else if((x != NULL) && (y != NULL))
                            {
                                auto temp_val = x->getSExtValue() / y->getSExtValue();
                                temp = ConstantInt::get(I->getOperand(0)->getType(),temp_val);
                                I->replaceAllUsesWith(temp);
                                delList.push(I);
                                constant_folding_count++;
                            }
                        }                      
                    }
                }
            }
            while(!delList.empty()) // delete the instructions that are no more required.
            {
                delList.top()->eraseFromParent();
                delList.pop();
            }
            outs() << "Transformations Applied:\n";
            outs() << "Algebraic Indentities: "<< algebraic_identities_count <<"\n";
            outs() << "Constant Folding: "<< constant_folding_count <<"\n";
            outs() << "Strength Reduction: "<< strength_reduction_count <<"\n";
            return true;
        }
    };
}

char LocalOpts::ID = 0;
static RegisterPass<LocalOpts>X("local-opts","Local Optimization Pass", false, false);