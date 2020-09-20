# libbtavl
AVL Binary Tree C library  

I am not using recursion, because:  
1) It is not optimized in all compilers;  
2) It is not naturally optimized (must be flaged on at compiler);  
3) Call stack size;  
4) _Call_ opcode cost in contrast iterative (_jump_) code;  
5) People are cowards saying that it is a "beatuful code". No, it is not. It breaks **ALL** first eight Zen Python' rules.
