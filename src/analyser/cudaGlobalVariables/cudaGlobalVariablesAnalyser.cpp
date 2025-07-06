#include "./cudaGlobalVariablesAnalyser.h"
#include "../../symbolutil/SymbolTable.h"
#include "../../ast/ASTHelper.cpp"
#include "../analyserUtil.cpp"
#include <unordered_map>



void cudaGlobalVariablesAnalyser::analyseExpression(Expression *expr)
{
    if (expr == nullptr)
        return;

    switch (expr->getExpressionFamily())
    {
        case EXPR_RELATIONAL:
        case EXPR_LOGICAL:
        case EXPR_ARITHMETIC:
        {
            // Handle relational, logical, and arithmetic expressions
            analyseExpression(expr->getLeft());
            analyseExpression(expr->getRight());
            break;
        }
        case EXPR_MAPGET:
        {
            // Handle map get expressions
            analyseExpression(expr->getIndexExpr());
            analyseExpression(expr->getMapExpr());
            break;
        }
        case EXPR_UNARY:
        {
            // Handle unary expressions
            analyseExpression(expr->getUnaryExpr());
            break;
        }
        case EXPR_PROCCALL:
        {
            // Handle procedure call expressions
           proc_callExpr* proc_expr = (proc_callExpr *)expr;
            std::string procName(proc_expr->getMethodId()->getIdentifier());
            if (this->funcHeaders.find(procName) != this->funcHeaders.end())
            {
                this->funcProcInvocations[this->currFuncName].insert(proc_expr); 
            }
            if(procName == "getGraphAtIndex"){
                if(this->currLoop != nullptr){
                    this->currLoop->setIsGraphListUsed();
                }
            }
            break;
        }
        case EXPR_ID:
        {
            // Handle identifier expressions
            Identifier *id = expr->getId();
            if (id != nullptr)
            {
                std::string idName(id->getIdentifier());
                if(this->inParallelLoop){
                    if (this->isUsedInLoop.find(idName) != this->isUsedInLoop.end())
                    {
                        this->funcLoopVars[this->currFuncName].insert(idName); // Add to current function's loop variables
                        this->currLoop->addLoopVariable(idName); // Add to current loop variables
                        this->currLoop->addLoopVariableSize(idName, this->sizeVar[idName]);
                        this->isUsedInLoop[idName] = true; // Initialize as not used
                        if(id->getSymbolInfo() != nullptr && id->getSymbolInfo()->getId() != nullptr)
                        {
                            id->getSymbolInfo()->getId()->setUsedInKernel(); // Mark as used in kernel
                        }
                    }
                }

            }
            break;
        }
        default:
        {
            // Handle other expression types if needed
            break;
        }
    }
}

void cudaGlobalVariablesAnalyser::analyseStatement(statement *stmt)
{
    if(stmt == nullptr)
        return;
    switch (stmt->getTypeofNode())
    {
        case NODE_ASSIGN:
        {
            // Handle assignment statements
            assignment *assignStmt = (assignment *)stmt;

            // Check if the assignment is to a variable that is used in a parallel loop
            if(assignStmt->lhs_isIdentifier())
            {
                // If the assignment is to an identifier
                std::string assignedId(assignStmt->getId()->getIdentifier());
                if(this->funcModifiedParamList[this->currFuncName].find(assignedId) != this->funcModifiedParamList[this->currFuncName].end())
                {
                    this->funcModifiedParamList[this->currFuncName][assignedId] = true; // Mark as modified
                }
            }else if(assignStmt->lhs_isIndexAccess()){
                
                std::string assignedId(assignStmt->getIndexAccess()->getMapExpr()->getId()->getIdentifier());
                if(this->funcModifiedParamList[this->currFuncName].find(assignedId) != this->funcModifiedParamList[this->currFuncName].end())
                {
                    this->funcModifiedParamList[this->currFuncName][assignedId] = true; // Mark as modified
                }
                auto expr = assignStmt->getIndexAccess();
                this->analyseExpression(expr);


            }
            if(assignStmt->getExpr() != nullptr)
            {
                analyseExpression(assignStmt->getExpr());
            }

            // If we are in a parallel loop, we need to check if the variable is used in the loop
            if(this->inParallelLoop){
                if(assignStmt->lhs_isIdentifier())
                {
                    // If the assignment is to an identifier
                    std::string assignedId(assignStmt->getId()->getIdentifier());
                    if(this->isUsedInLoop.find(assignedId) != this->isUsedInLoop.end())
                    {
                        this->funcLoopVars[this->currFuncName].insert(assignedId);
                        this->isUsedInLoop[assignedId] = true; // Mark as used in loop
                        assignStmt->getId()->getSymbolInfo()->getId()->setUsedInKernel(); // Mark as used in kernel
                        this->currLoop->addLoopVariable(assignedId);
                        this->currLoop->addLoopVariableSize(assignedId, this->sizeVar[assignedId]);
                    }
                }else if(assignStmt->lhs_isIndexAccess()){
                    auto expr = assignStmt->getIndexAccess();
                    this->analyseExpression(expr);
                }
                if(assignStmt->getExpr() != nullptr)
                {
                    analyseExpression(assignStmt->getExpr());
                }
            }
            
            break;
        }

        case NODE_DECL:
        {
            // Handle declaration statements
            declaration *declStmt = (declaration *)stmt;
            if(this->inParallelLoop)
            {
                std::string declId(declStmt->getdeclId()->getIdentifier());
                
                if(this->isUsedInLoop.find(declId) != this->isUsedInLoop.end())
                {
                    this->funcLoopVars[this->currFuncName].insert(declId);
                    this->isUsedInLoop[declId] = true;
                    declStmt->getdeclId()->getSymbolInfo()->getId()->setUsedInKernel(); // Mark as used in kernel
                    this->currLoop->addLoopVariable(declId);
                    this->currLoop->addLoopVariableSize(declId, this->sizeVar[declId]);
                }
                
                if(declStmt->getExpressionAssigned() != nullptr)
                {
                    analyseExpression(declStmt->getExpressionAssigned());
                }

            }else{
                // If not in parallel loop, we can still check if the declaration is used
                std::string declId(declStmt->getdeclId()->getIdentifier());
                if(this->isUsedInLoop.find(declId) == this->isUsedInLoop.end())
                {
                    this->isUsedInLoop[declId] = false;
                }
                if(declStmt->getExpressionAssigned() != nullptr)
                {
                    auto assignedExpr = declStmt->getExpressionAssigned();
                    if(assignedExpr->getExpressionFamily() == EXPR_INTCONSTANT)
                    {
                        this->sizeVar[declId] = "sizeof(int)";
                    }else if(assignedExpr->getExpressionFamily() == EXPR_ALLOCATE)
                    {
                        
                        std::string calculation = "sizeof(";
                        allocaExpr *allocatedExpr = (allocaExpr *)assignedExpr;
                        if(allocatedExpr->getType() != nullptr)
                        {
                            auto getType = allocatedExpr->getType()->gettypeId();
                            if(getType == TYPE_INT){
                                calculation += "int";
                            }else if(getType == TYPE_FLOAT){
                                calculation += "float";
                            }else if(getType == TYPE_DOUBLE){
                                calculation += "double";
                            }else if(getType == TYPE_LONG){
                                calculation += "long";
                            }else if(getType == TYPE_BOOL){
                                calculation += "bool";
                            }else if(getType == TYPE_NODE){
                                calculation += "Node";
                            }
                            calculation += ")*";

                            auto getArg = allocatedExpr->getFirstArg();
                            auto argExpr = getArg->getExpr();
                            if(argExpr != nullptr)
                            {
                                if(argExpr->getExpressionFamily() == EXPR_ID)
                                {
                                    std::string argId(argExpr->getId()->getIdentifier());
                                    calculation += argId;
                                }
                            };
                            this->sizeVar[declId] = calculation;
                        }
                    }else if(declStmt->getType()->isIntegerType()){
                        this->sizeVar[declId] = "sizeof(int)";
                    }
                }
            }

            
            
            break;
        }

        case NODE_IFSTMT:
        {
            // Handle if statements
            analyseExpression(((ifStmt *)stmt)->getCondition());
            analyseStatement(((ifStmt *)stmt)->getIfBody());
            analyseStatement(((ifStmt *)stmt)->getElseBody());
            break;
        }

        case NODE_WHILESTMT:
        {
            // Handle while statements
            analyseExpression(((whileStmt *)stmt)->getCondition());
            auto body = ((whileStmt *)stmt)->getBody();
            analyseStatement(((whileStmt *)stmt)->getBody());
            break;
        }

        case NODE_DOWHILESTMT:
        {
            // Handle do-while statements
            analyseStatement(((dowhileStmt *)stmt)->getBody());
            break;
        }



        case NODE_BLOCKSTMT:
        {
            // Handle block statements
            blockStatement *blockStmt = (blockStatement *)stmt;
            for (auto &innerStmt : blockStmt->returnStatements())
            {
                this->analyseStatement(innerStmt);
            }
            break;
        }

        case NODE_LOOPSTMT:
        {
            // Handle loop statements
            // You can add specific handling for loop statements if needed
            loopStmt* prevLoop = this->currLoop;
            bool isParallel = false;
            auto loopStatement = (loopStmt *)stmt;
            

            if(((loopStmt *)stmt)->isLoop())
            {
                if(this->currLoop == NULL){
                   this->currLoop = loopStatement;
                }
                if(!this->inParallelLoop)
                {
                    this->currLoop = loopStatement;
                    this->inParallelLoop = true;
                    isParallel = true;
                }
            }
            if (loopStatement->getBody() != nullptr)
            {
                analyseStatement(loopStatement->getBody());
            }
            if(isParallel)
            {
                this->inParallelLoop = false;
                this->currLoop = prevLoop; // Restore previous loop context

            }
            break;
        }

        case NODE_REDUCTIONCALLSTMT:
        {
            if(((reductionCallStmt*)stmt)->isLeftIdentifier()){
                if(this->inParallelLoop){
                    if(((reductionCallStmt*)stmt)->getLeftId() != nullptr){
                        std::string assignedId(((reductionCallStmt*)stmt)->getLeftId()->getIdentifier());
                        if(this->isUsedInLoop.find(assignedId) != this->isUsedInLoop.end())
                        {
                            this->isUsedInLoop[assignedId] = true; // Mark as used in loop
                            this->currLoop->addLoopVariable(assignedId);
                            this->currLoop->addLoopVariableSize(assignedId, this->sizeVar[assignedId]);
                            ((reductionCallStmt*)stmt)->getLeftId()->getSymbolInfo()->getId()->setUsedInKernel(); // Mark as used in kernel

                        }

                    }
                }              
            }

            break;
        }

        case NODE_FORALLSTMT:
        {
            // Handle forall statements
            bool isParallel   = false;
            if(((forallStmt *)stmt)->isForall()){
                if(!this->inParallelLoop){
                    this->inParallelLoop = true;
                    isParallel = true;
                }
            }
            // analyseExpression(((forallStmt *)stmt)->getSourceExpr());
            // analyseExpression(((forallStmt *)stmt)->getfilterExpr());
            // analyseExpression(((forallStmt *)stmt)->getAssocExpr());
            analyseStatement(((forallStmt *)stmt)->getBody());
            // analyseStatement(((forallStmt *)stmt)->getReductionStatement());
            if(isParallel){
                this->inParallelLoop = false;
            }
            break;
        }

        case NODE_PROCCALLSTMT:
        {
            proc_callStmt *procCall = (proc_callStmt *)stmt;
            std::string procCallName(procCall->getProcCallExpr()->getMethodId()->getIdentifier());
            analyseExpression(procCall->getProcCallExpr());
            break;
        }

        default:
        {
            // Handle other statement types if needed
            break;
        }
    }
}

void cudaGlobalVariablesAnalyser::analyseFunc(Function *func)
{
    if (func == nullptr)
        return;
    blockStatement *blockStmt = func->getBlockStatement();
    if (blockStmt != nullptr)
    {
        for (auto &stmt : blockStmt->returnStatements())
        {
            this->analyseStatement(stmt);
        }
    }

}

void cudaGlobalVariablesAnalyser::analyse(std::list<Function*> funcList)
{

    this->currLoop = nullptr;

    printf("Starting analysis of CUDA global variables...\n");

    // Store function headers and initialize modified parameters
    for(auto &func: funcList){
        std::string funcName(func->getIdentifier()->getIdentifier());
        this->funcHeaders.insert(funcName);
        auto paramList = func->getParamList();
        int index = 0;
        for(auto &x: paramList){
            Identifier *paramId = x->getIdentifier();
            std::string paramName(paramId->getIdentifier());
            if(paramId != nullptr){
                this->funcModifiedParamsIndex[funcName][paramId->getIdentifier()] = index; // Initialize index for modified params
                index+=1;
            }
        }        
    }

    for (auto &func : funcList)
    {  
        std::string funcName(func->getIdentifier()->getIdentifier());
        for(auto &x: func->getParamList()){
            Identifier *paramId = x->getIdentifier();
            if (paramId != nullptr)
            {
               std::string paramName(paramId->getIdentifier());
               this->funcModifiedParamList[funcName][paramName] = false; // Initialize as not modified
            }
        }
        this->currFuncName = funcName;
        this->analyseFunc(func);
        this->isUsedInLoop = std::unordered_map<std::string, bool>();
    }

    // for(auto &x: this->funcModifiedParamList){
    //     std::string funcName(x.first);
    //     for(auto &args: x.second){
    //         std::string paramName(args.first);
    //         bool isModified = args.second;
    //         if(isModified){
    //             auto index = this->funcModifiedParamsIndex[funcName][paramName];
    //         }
    //     }
    // }

    for(auto &x: this->funcProcInvocations){
        std::string funcName(x.first);
        for(auto &x: x.second){
            std::vector<std::string> argNames;
            std::string procName(x->getMethodId()->getIdentifier());
            auto paramList = x->getArgList();
            for(auto &arg: paramList){
                if(arg->getExpr() != nullptr && arg->getExpr()->getExpressionFamily() == EXPR_ID){
                    std::string argName(arg->getExpr()->getId()->getIdentifier());
                    argNames.push_back(argName);
                }else{
                    argNames.push_back("Unknown Argument");
                }
            }
            std::vector<std::string> modifiedVars;
            for(auto &args: this->funcModifiedParamList[procName]){
                std::string paramName(args.first);
                if(args.second){
                    auto index = this->funcModifiedParamsIndex[procName][paramName];
                    auto argName = argNames[index];
                    if(this->funcLoopVars[funcName].find(argName) != this->funcLoopVars[funcName].end())
                    {
                        modifiedVars.push_back(argName);
                    }
                }
            }

            unordered_map<std::string, std::string> modifiedVarsSizeMap;
            for (const auto &var : modifiedVars)
            {
                if (this->sizeVar.find(var) != this->sizeVar.end())
                {
                    modifiedVarsSizeMap[var] = this->sizeVar[var]; // Store the size of the modified variable
                }
            }


            x->setModifiedVarsSizeMap(modifiedVarsSizeMap); // Set modified variables for the procedure call

        }
    }


    // After analysis, print the results
    printf("CUDA global variables analysis completed.\n");
    printf("Global variables used in parallel loops:\n");


    for (const auto &entry : this->isUsedInLoop)
    {
        if (entry.second)
        {
            printf("Variable '%s' is used in a parallel loop.\n", entry.first.c_str());
        }
    }

}
