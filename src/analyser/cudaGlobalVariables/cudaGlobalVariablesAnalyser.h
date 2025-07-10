#ifndef CGV_ANALYSER
#define CGV_ANALYSER
#include "../../ast/ASTNodeTypes.hpp"
#include <unordered_map>


// Analyser for CUDA global variables
// This analyser checks for global variables used in CUDA code, especially in parallel loops.
// Though kinda redundant, it is useful for identifying global variables that are modified or used in parallel loops as a compiler pass
// versus in codegen recursion, taking out logic of dependency analysis outside of codegen
class cudaGlobalVariablesAnalyser
{
private:
    // store the function names being analysed
    std::set<std::string> funcHeaders;

    // store the global variables used in parallel loops
    std::unordered_map<std::string, bool> isUsedInLoop;

    // store the size of the variables
    std::unordered_map<std::string, std::string> sizeVar;

    // store list of modified parameters for each function
    std::unordered_map<std::string, std::unordered_map<std::string, bool>> funcModifiedParamList;
    
    // Store the index of modified parameters for each function
    std::unordered_map<std::string, std::map<std::string,int>> funcModifiedParamsIndex;
    
    // Store the loops variables associated with each function
    std::unordered_map<std::string, std::set<std::string>> funcLoopVars;

    // Store the procedure calls in functions
    std::unordered_map<std::string, std::set<proc_callExpr*>> funcProcInvocations;

    // Current loop being analysed
    loopStmt* currLoop;

    // Store the function name currently being analysed
    std::string currFuncName;

    bool inParallelLoop = false;

    void analyseStatement(statement *stmt);
    void analyseFunc(Function *func);
    void analyseExpression(Expression *expr);


public:
    cudaGlobalVariablesAnalyser() {}

    // Function to analyse the global variables in CUDA code
    void analyse(std::list<Function*> funcList);;
};


#endif
