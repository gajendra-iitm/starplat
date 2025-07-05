#ifndef CGV_ANALYSER
#define CGV_ANALYSER
#include "../../ast/ASTNodeTypes.hpp"
#include <unordered_map>

class cudaGlobalVariablesAnalyser
{
private:
    std::set<std::string> funcHeaders;
    std::unordered_map<std::string, bool> isUsedInLoop;
    std::unordered_map<std::string, std::string> sizeVar;
    std::unordered_map<std::string, int> typeVar;
    std::unordered_map<std::string, std::set<std::string>> typeVarName;


    std::unordered_map<std::string, std::unordered_map<std::string, bool>> funcModifiedParamList;
    std::unordered_map<std::string, std::map<std::string,int>> funcModifiedParamsIndex;
    std::unordered_map<std::string, std::set<std::string>> funcLoopVars;
    std::unordered_map<std::string, std::set<proc_callExpr*>> funcProcInvocations;


    loopStmt* currLoop;
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
