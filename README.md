# LLVM-Optimization-Pass
LLVM Optimization pass for Constant Folding, Strength Reduction and Algebraic Identities

1) To build the FunctionInfo Pass run the following command in the FunctionInfo folder:  

**make**  

Then build the loop.c file in the tests folder.  
Run the following command in the tests folder:  
**clang -O -emit-llvm -c loop.c**  

To run the pass, run the following command:  
**opt -load ../FunctionInfo/FunctionInfo.so -function-info loop.bc -o out**  

2) To build the LocalOpts Pass run the following command in the LocalOpts folder:  

**make**  

Then build the algebraic.c, strength.c and constfold.c file in the tests folder.  
Run the following command in the tests folder:  
**clang -Xclang -disable-O0-optnone -O0 -emit-llvm -c algebraic.c -o algebraic.bc  
opt -mem2reg algebraic.bc -o algebraic-m2r.bc  
clang -Xclang -disable-O0-optnone -O0 -emit-llvm -c strength.c -o strength.bc  
opt -mem2reg strength.bc -o strength-m2r.bc  
clang -Xclang -disable-O0-optnone -O0 -emit-llvm -c constfold.c -o constfold.bc  
opt -mem2reg constfold.bc -o constfold-m2r.bc**  


To run the pass, run the following command:  
**1) opt -load ./LocalOpts/LocalOpts.so -local-opts algebraic-m2r.bc -o out  
2) opt -load ./LocalOpts.so -local-opts strength-m2r.bc -o out**  
**3) opt -load ./LocalOpts.so -local-opts constfold-m2r.bc -o out**  
