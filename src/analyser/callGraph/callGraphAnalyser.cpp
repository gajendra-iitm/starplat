#include "./callGraphAnalyser.h"
#include "../analyserUtil.cpp"
#include "../../ast/ASTHelper.cpp"
#include "../../symbolutil/SymbolTable.h"
#include <unordered_map>

void callGraphAnalyser::analyse(list<Function *> funcList)
{
    for (auto &func : funcList)
    {
        std::string funcName(func->getIdentifier()->getIdentifier());
        this->isCalled[funcName] = false;
        this->calledBy[funcName] = std::unordered_set<std::string>();
    }

    for (auto &func : funcList)
    {
        this->currFunc = func;
        analyseFunc(func);
    }

    // After analysing all functions, we need to set the isCalled and isInParallelLoop flags for each function
    // This is done to ensure that the flags are set correctly based on the analysis and set the corresponding parameters
    // in the function headers
    for (auto &func : funcList)
    {
        std::string funcName(func->getIdentifier()->getIdentifier());
        if (this->isCalled.find(funcName) != this->isCalled.end())
        {
            if (this->isCalled[funcName])
            {
                func->setIsCalled(true);
            }
            else
            {
                func->setIsCalled(false);
            }
        }
        if (this->isInParallelLoop.find(funcName) != this->isInParallelLoop.end())
        {
            if (this->isInParallelLoop[funcName])
            {
                func->setIsInParallelLoop(true);
            }
            else
            {
                func->setIsInParallelLoop(false);
            }
        }
        else
        {
            func->setIsInParallelLoop(false);
        }
    }

    // We analyse each procedure call expression to determine if it is in a loop call, if it uses graph, graphList or MST variables,
    for (auto &procCallExpr : this->procCallExprs)
    {
        std::string procCallName(procCallExpr->getMethodId()->getIdentifier());
        if (this->isInParallelLoop.find(procCallName) != this->isInParallelLoop.end())
        {
            if (this->isInParallelLoop[procCallName])
            {
                procCallExpr->setIsLoopCall();
            }
        }
        std::list<argument *> argList = procCallExpr->getArgList();
        for (auto &arg : argList)
        {
            if (arg->getExpr() != nullptr)
            {
                if (arg->getExpr()->isIdentifierExpr())
                {
                    Identifier *id = arg->getExpr()->getId();
                    std::string idName(id->getIdentifier());
                    if (this->isGraphVariable.find(idName) != this->isGraphVariable.end())
                    {
                        arg->setGraphArg();
                    }
                    if (this->isMSTVariable.find(idName) != this->isMSTVariable.end())
                    {
                        arg->setMSTArg();
                    }
                    if (this->isGraphListVariable.find(idName) != this->isGraphListVariable.end())
                    {
                        arg->setGraphListArg();
                    }
                }
            }
        }
    }

    // We also need to set the isUsingWeight flag for each function and procedure call expression
    // This is done by checking if the function or procedure call expression is using weight parameter
    // We do this by checking if the function or procedure call expression is called by a function
    // that is using weight parameter
    // We use a queue to traverse the call graph and set the isUsingWeight flag for all functions and procedure call expressions
    // that are reachable from the functions that are using weight parameter
    // We start from the functions that are using weight parameter and traverse the call graph
    // to set the isUsingWeight flag for all functions and procedure call expressions that are reachable
    // from the functions that are using weight parameter and perform a BFS traversal
    unordered_map<std::string, bool> visited;
    queue<std::string> q;
    for (auto &key : this->isUsingWeight)
    {
        std::string procCallName(key.first);
        for (auto &calledByFunc : calledBy[key.first])
        {
            if (visited.find(calledByFunc) == visited.end())
            {
                visited[calledByFunc] = true;
                q.push(calledByFunc);
            }
        }
    }
    unordered_map<std::string, bool> visited2;
    while (!q.empty())
    {
        std::string currFuncName = q.front();
        q.pop();
        if (visited2.find(currFuncName) == visited2.end())
        {
            visited2[currFuncName] = true;
            this->isUsingWeight[currFuncName] = true;
            if (this->isCalled.find(currFuncName) != this->isCalled.end())
            {
                if (this->calledBy.find(currFuncName) != this->calledBy.end())
                {
                    for (auto &calledByFunc : this->calledBy[currFuncName])
                    {
                        if (visited2.find(calledByFunc) == visited2.end())
                        {
                            visited2[calledByFunc] = true;
                            q.push(calledByFunc);
                        }
                    }
                }
            }
        }
    }

    // Now we set the isUsingWeight flag for each function and procedure call expression
    for (auto &x : funcList)
    {
        std::string funcName(x->getIdentifier()->getIdentifier());
        if (this->isUsingWeight.find(funcName) != this->isUsingWeight.end())
        {
            if (this->isUsingWeight[funcName])
            {
                x->setIsUsingWeight();
            }
        }
    }

    // We also need to set the isUsingWeight flag for each procedure call expression
    for (auto &procCallExpr : this->procCallExprs)
    {
        std::string procCallName(procCallExpr->getMethodId()->getIdentifier());
        if (this->isUsingWeight.find(procCallName) != this->isUsingWeight.end())
        {
            if (this->isUsingWeight[procCallName])
            {
                procCallExpr->setIsUsingWeight();
            }
        }
    }
}

void callGraphAnalyser::analyseExpression(Expression *expr)
{
    if (expr == NULL)
    {
        return;
    }

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
    case EXPR_UNARY:
    {
        // Handle unary expressions
        analyseExpression(expr->getUnaryExpr());
        break;
    }
    case EXPR_ID:
    {
        // Handle identifier expressions
        Identifier *id = expr->getId();
        std::string idName(id->getIdentifier());
        if (this->isCopyGraphVariable.find(idName) != this->isCopyGraphVariable.end())
        {
            // If the identifier is a graph variable, mark it
            id->setIsCopyGraph();
        }
        if (this->isMSTVariable.find(idName) != this->isMSTVariable.end())
        {
            // If the identifier is an MST variable, mark it
            std::cout << "The MST variable is used here: " << idName << std::endl;
            currLoop->setIsMSTUsed();
        }
        break;
    }
    case EXPR_PROPID:
    {
        break;
    }
    case EXPR_PROCCALL:
    {
        // Handle procedure call expressions
        std::string procCallName(((proc_callExpr *)expr)->getMethodId()->getIdentifier());
        if (this->isCalled.find(procCallName) != this->isCalled.end())
        {
            // If the procedure call is not already recorded, add it
            this->isCalled[procCallName] = true;
            this->isInParallelLoop[procCallName] = this->inParallelLoop;
        }
        if (this->calledBy.find(procCallName) != this->calledBy.end())
        {
            // If the procedure call is not already recorded in calledBy, add it
            std::string currFuncName(this->currFunc->getIdentifier()->getIdentifier());
            this->calledBy[procCallName].insert(currFuncName);
        }
        if (procCallName == calculateDistanceCall)
        {
            // If the procedure call is calculateDistance, mark it
            std::string currFuncName(this->currFunc->getIdentifier()->getIdentifier());
            this->isUsingWeight[currFuncName] = true;
        }

        if (this->currLoop != nullptr && this->loopVarMap[this->currLoop].find(procCallName) == this->loopVarMap[this->currLoop].end())
        {
            // If the procedure call is not already recorded in the loop variable map, add it
            this->loopVarMap[this->currLoop].insert(procCallName);
        }

        proc_callExpr *procCall = (proc_callExpr *)expr;
        std::list<argument *> argList = procCall->getArgList();
        for (auto &arg : argList)
        {
            analyseExpression(arg->getExpr());
        }
        if (procCall->getId1() != nullptr)
        {
            // If the procedure call has an id1, analyse it
            auto id1 = procCall->getId1();
            std::string id1Name(id1->getIdentifier());
            if (this->isCopyGraphVariable.find(id1Name) != this->isCopyGraphVariable.end())
            {
                // If the id1 is a copy graph variable, mark it
                id1->setIsCopyGraph();
            }
        }
        if (procCall->getId2() != nullptr)
        {

            // If the procedure call has an id2, analyse it
            auto id2 = procCall->getId2();
            std::string id2Name(id2->getIdentifier());
            if (this->isCopyGraphVariable.find(id2Name) != this->isCopyGraphVariable.end())
            {
                // If the id2 is a copy graph variable, mark it
                id2->setIsCopyGraph();
            }
        }
        break;
    }
    case EXPR_BOOLCONSTANT:
    {
        break;
    }
    default:
        break;
    }
}

void callGraphAnalyser::analyseStatement(statement *stmt)
{
    if (stmt == nullptr)
        return;

    switch (stmt->getTypeofNode())
    {
    case NODE_ASSIGN:
    {
        // Handle assignment statements
        assignment *assignStmt = (assignment *)stmt;
        if (assignStmt->getExpr() != nullptr)
        {
            analyseExpression(assignStmt->getExpr());
        }
        break;
    }

    case NODE_PROCCALLSTMT:
    {
        // Handle procedure call statements
        proc_callStmt *procCall = (proc_callStmt *)stmt;
        std::string procCallName(procCall->getProcCallExpr()->getMethodId()->getIdentifier());
        if (this->isCalled.find(procCallName) != this->isCalled.end())
        {
            // If the procedure call is not already recorded, add it
            this->isCalled[procCallName] = true;
            this->isInParallelLoop[procCallName] = this->inParallelLoop;
        }
        this->procCallExprs.push_back(procCall->getProcCallExpr());
        // Analyse the arguments of the procedure call
        analyseExpression(procCall->getProcCallExpr());

        break;
    }

    case NODE_DECL:
    {
        // Handle declaration statements
        declaration *declStmt = (declaration *)stmt;
        if (declStmt->getExpressionAssigned() != nullptr)
        {
            analyseExpression(declStmt->getExpressionAssigned());
        }
        // If the declaration is of a graph type, mark it
        if (declStmt->getType()->isGraphType())
        {
            auto id = declStmt->getdeclId();
            if (id != nullptr)
            {
                std::string idName(id->getIdentifier());
                if (this->isGraphVariable.find(idName) == this->isGraphVariable.end())
                {
                    this->isGraphVariable[idName] = true;
                }
            }
        }
        // Check if the declaration is initialized or not
        if (declStmt->isInitialized())
        {
            Expression *expr = declStmt->getExpressionAssigned();
            // If the expression is a procedure call expression, check if it is a MST, graphList or copy graph call
            if (expr->isProcCallExpr())
            {
                auto id = declStmt->getdeclId();
                std::string idName(id->getIdentifier());
                auto procCallExpr = (proc_callExpr *)expr;
                std::string methodString(procCallExpr->getMethodId()->getIdentifier());
                if (methodString == getMSTCall)
                {
                    this->isMSTVariable[idName] = true;
                }
                else if (methodString == copyGraphCall)
                {
                    if (this->isMSTVariable.find(procCallExpr->getId1()->getIdentifier()) != this->isMSTVariable.end())
                    {
                        if (this->currLoop != nullptr)
                        {
                            this->currLoop->setIsMSTUsed();
                        }
                        this->isCopyGraphVariable.insert(idName);
                    }
                }
                else if (methodString == makeGraphCopyCall)
                {
                    this->isGraphListVariable.insert(idName);
                }
                else if (methodString == getGraphAtIndexCall)
                {
                    if (this->currLoop != nullptr)
                    {
                        this->currLoop->setIsGraphListUsed();
                    }
                    this->isCopyGraphVariable.insert(idName);
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

    case NODE_FIXEDPTSTMT:
    {
        // Handle fixed point statements
        analyseExpression(((fixedPointStmt *)stmt)->getDependentProp());
        analyseStatement(((fixedPointStmt *)stmt)->getBody());
        break;
    }

    case NODE_FORALLSTMT:
    {
        // Handle forall statements
        bool isParallel = false;
        if (((forallStmt *)stmt)->isForall())
        {
            if (!this->inParallelLoop)
            {
                this->inParallelLoop = true;
                isParallel = true;
            }
        }
        analyseExpression(((forallStmt *)stmt)->getSourceExpr());
        analyseExpression(((forallStmt *)stmt)->getfilterExpr());
        analyseExpression(((forallStmt *)stmt)->getAssocExpr());
        analyseStatement(((forallStmt *)stmt)->getBody());
        analyseStatement(((forallStmt *)stmt)->getReductionStatement());
        if (isParallel)
        {
            this->inParallelLoop = false;
        }
        break;
    }
    case NODE_BLOCKSTMT:
    {
        // Handle block statements
        blockStatement *blockStmt = (blockStatement *)stmt;
        for (auto &innerStmt : blockStmt->returnStatements())
        {
            analyseStatement(innerStmt);
        }
        break;
    }
    case NODE_LOOPSTMT:
    {
        // Handle loop statements
        // You can add specific handling for loop statements if needed
        bool isLoopNull = false;
        bool isParallel = false;

        this->currFunc->setHasParallelLoop();
        this->hasAParallelLoop[this->currFunc->getIdentifier()->getIdentifier()] = true;
        if (((loopStmt *)stmt)->isLoop())
        {
            if (!this->inParallelLoop)
            {
                this->inParallelLoop = true;
                isParallel = true;
            }
        }
        auto loopStatement = (loopStmt *)stmt;

        if (this->currLoop == nullptr)
        {
            isLoopNull = true;
            this->currLoop = loopStatement;
        }

        if (loopStatement->getBody() != nullptr)
        {
            analyseStatement(loopStatement->getBody());
        }
        if (isParallel)
        {
            this->inParallelLoop = false;
        }
        if (isLoopNull)
        {
            this->currLoop = nullptr; // Reset currLoop to null
        }
        break;
    }
    default:
    {
        // Handle other statement types if needed
        break;
    }
    }
}

void callGraphAnalyser::analyseFunc(Function *func)
{
    if (func == nullptr)
        return;

    printf("========================================\n");
    printf("Analysing the Function: %s\n", func->getIdentifier()->getIdentifier());
    this->inParallelLoop = false;
    // Analyse the function body
    blockStatement *blockStmt = func->getBlockStatement();

    list<formalParam *> funcArgs = func->getParamList();

    for (auto &x : funcArgs)
    {
        if (x->getType()->isGraphType())
        {
        std:
            string idName(x->getIdentifier()->getIdentifier());
            if (this->isGraphVariable.find(idName) == this->isGraphVariable.end())
            {
                this->isGraphVariable[idName] = true;
            }
        }
    }

    if (blockStmt != nullptr)
    {
        for (auto &stmt : blockStmt->returnStatements())
        {
            this->analyseStatement(stmt);
        }
    }

    for (auto &x : this->loopVarMap)
    {
        for (auto &y : x.second)
        {
            if (this->isUsingWeight.find(y) != this->isUsingWeight.end())
            {
                x.first->setIsWeightUsed();
            }
        }
    }

    for (auto &x : procCallExprs)
    {
        std::string procCallName(x->getMethodId()->getIdentifier());
        if (this->hasAParallelLoop.find(procCallName) != this->hasAParallelLoop.end())
        {
            if (this->hasAParallelLoop[procCallName])
            {
                x->setHasAParallelLoop();
            }
        }
    }

    printf("========================================\n");
}