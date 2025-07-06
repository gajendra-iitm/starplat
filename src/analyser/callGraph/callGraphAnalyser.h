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
    // Flag to check if the current function is in a parallel loop
    bool inParallelLoop = false;

    // Check if a function is called by another function
    std::unordered_map<string, bool> isCalled;

    // Map to store the functions that call a particular function
    std::unordered_map<string, std::unordered_set<string>> calledBy;

    // Map to store the functions that are called in a parallel loop
    std::unordered_map<string, bool> isInParallelLoop;

    std::unordered_map<string, bool> hasAParallelLoop;

    // Map to store the functions that are graph type
    std::unordered_map<string, bool> isGraphVariable;

    // Map to store the functions that are MST type
    std::unordered_map<string, bool> isMSTVariable;

    // Map to store the functions that are graph list type
    std::unordered_set<string> isGraphListVariable;

    // Map to store the functions that are copy graph type
    std::unordered_set<string> isCopyGraphVariable;

    // Store the functions that calculates distance and hence uses weight parameter
    std::unordered_map<string, bool> isUsingWeight;

    // Store the procedure call expressions
    std::vector<proc_callExpr *> procCallExprs;

    // Store the procedure invoked for a current loop
    std::map<loopStmt *, std::set<std::string>> loopVarMap;

    // current function under analysis
    Function *currFunc = nullptr;

    // Currrent loop under analysis
    loopStmt *currLoop = nullptr;
    bool isLoopNull = false;

    void analyseExpression(Expression *expr);
    void analyseStatement(statement *stmt);
    void analyseFunc(Function *func);

public:
    callGraphAnalyser() {}

    void analyse(list<Function *> funcList);
};

#endif // CG_ANALYSER