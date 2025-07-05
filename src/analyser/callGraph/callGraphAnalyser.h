#ifndef CG_ANALYSER
#define CG_ANALYSER

#include "../../ast/ASTNodeTypes.hpp"
#include <unordered_map>
#include <unordered_set>




// Compile Time Call Graph Analyser
// Currently employed by CUDA backend to check for nested function call procedures  
// If a function is current called by another function need to change header from Graph to d_meta, h_meta in the function defintion
class callGraphAnalyser
{

private:
    bool inParallelLoop = false;
    
    std::unordered_map<string, bool> isCalled;
    std::unordered_map<string, std::unordered_set<string>> calledBy;
    std::unordered_map<string, bool> isInParallelLoop;
    std::unordered_map<string, bool> hasAParallelLoop;
    std::unordered_map<string, bool> isGraphVariable;
    std::unordered_map<string, bool> isMSTVariable;
    std::unordered_set<string> isGraphListVariable;
    std::unordered_set<string> isCopyGraphVariable;
    std::unordered_map<string, bool> isUsingWeight;
    std::vector<proc_callExpr*> procCallExprs;

    std::map<loopStmt*, std::set<std::string>> loopVarMap;

    Function* currFunc = nullptr;
    loopStmt* currLoop = nullptr;
    bool isLoopNull = false;

    

    void analyseExpression(Expression *expr);
    void analyseStatement(statement *stmt);
    void analyseFunc(Function * func);

public:
    callGraphAnalyser() {}



    void analyse(list<Function *> funcList);
};

#endif // CG_ANALYSER