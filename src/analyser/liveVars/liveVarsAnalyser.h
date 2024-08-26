#ifndef LIVE_VARS_ANALYZER_H
#define LIVE_VARS_ANALYZER_H

#include <unordered_map>
#include "../../ast/ASTHelper.cpp"
#include "../analyserUtil.h"
#include "liveVarsNode.h"

class liveVarsAnalyser
{
private:
    std::unordered_map<ASTNode *, liveVarsNode *> nodeMap;
    std::list<ASTNode *> nodeList;
    std::list<liveVarsNode *> currReturnNodes;

public:
    liveVarsAnalyser();

    void analyse(list<Function *>);
    void analysecfg(liveVarsNode *);
    bool analyseNode(liveVarsNode *);

    void initFunc(Function *);
    liveVarsNode *initStatement(statement *, liveVarsNode *);
    liveVarsNode *initAssignment(assignment *, liveVarsNode *);
    liveVarsNode *initBlockStatement(blockStatement *, liveVarsNode *);
    liveVarsNode *initDeclaration(declaration *, liveVarsNode *);
    liveVarsNode *initDoWhile(dowhileStmt *, liveVarsNode *);
    liveVarsNode *initFixedPoint(fixedPointStmt *, liveVarsNode *);
    liveVarsNode *initForAll(forallStmt *, liveVarsNode *);
    liveVarsNode *initIfStmt(ifStmt *, liveVarsNode *);
    liveVarsNode *initIterateBFS(iterateBFS *, liveVarsNode *);
    liveVarsNode *initIterateRBFS(iterateReverseBFS *, liveVarsNode *);
    liveVarsNode *initProcCall(proc_callStmt *, liveVarsNode *);
    liveVarsNode *initReduction(reductionCallStmt *, liveVarsNode *);
    liveVarsNode *initReturn(returnStmt *, liveVarsNode *);
    liveVarsNode *initUnary(unary_stmt *, liveVarsNode *);
    liveVarsNode *initWhile(whileStmt *, liveVarsNode *);

    // Returns the used variables in each statment
    usedVariables getVarsStatement(statement *);
    usedVariables getVarsBlock(blockStatement *);
    usedVariables getVarsForAll(forallStmt *);
    usedVariables getVarsUnary(unary_stmt *);
    usedVariables getVarsDeclaration(declaration *);
    usedVariables getVarsWhile(whileStmt *);
    usedVariables getVarsDoWhile(dowhileStmt *);
    usedVariables getVarsAssignment(assignment *);
    usedVariables getVarsIf(ifStmt *);
    usedVariables getVarsExpr(Expression *);
    usedVariables getVarsReduction(reductionCallStmt *);
    usedVariables getVarsFixedPoint(fixedPointStmt *);
    usedVariables getVarsBFS(iterateBFS *);
    usedVariables getVarsRBFS(iterateReverseBFS *);
    usedVariables getVarsProcCall(proc_callStmt *);
    usedVariables getVarsExprProcCall(proc_callExpr *);

    static void printLiveVarsNode(ASTNode *stmt)
    {
        if (stmt == nullptr)
        {
            std::cout << "Error: null ASTNode*" << std::endl;
        }
        switch (stmt->getTypeofNode())
        {
        case NODE_BLOCKSTMT:
        {
            list<statement *> stmtlist = ((blockStatement *)stmt)->returnStatements();
            for (auto s = stmtlist.begin(); s != stmtlist.end(); s++)
                printLiveVarsNode(*s);
            break;
        }
        case NODE_DOWHILESTMT:
        {
            printLiveVarsNode(((dowhileStmt *)stmt)->getBody());
            printLiveVarsNode(((dowhileStmt *)stmt)->getCondition());
            break;
        }
        case NODE_FIXEDPTSTMT:
        {
            printLiveVarsNode(((fixedPointStmt *)stmt)->getFixedPointId());
            printLiveVarsNode(((fixedPointStmt *)stmt)->getBody());
            break;
        }
        case NODE_FORALLSTMT:
        {
            printLiveVarsNode(((forallStmt *)stmt)->getIterator());
            printLiveVarsNode(((forallStmt *)stmt)->getBody());
            break;
        }
        case NODE_IFSTMT:
        {
            printLiveVarsNode(((ifStmt *)stmt)->getCondition());
            printLiveVarsNode(((ifStmt *)stmt)->getIfBody());
            if (((ifStmt *)stmt)->getElseBody())
                printLiveVarsNode(((ifStmt *)stmt)->getElseBody());
            break;
        }
        case NODE_ITRBFS:
        {
            printLiveVarsNode(((iterateBFS *)stmt)->getIteratorNode());
            Expression *filter = ((iterateBFS *)stmt)->getBFSFilter();
            if (filter != nullptr)
                printLiveVarsNode(filter);
            printLiveVarsNode(((iterateBFS *)stmt)->getBody());
            iterateReverseBFS *reverse = ((iterateBFS *)stmt)->getRBFS();
            if (reverse != nullptr)
                printLiveVarsNode(reverse);
            break;
        }
        case NODE_ITRRBFS:
        {
            Expression *filter = ((iterateReverseBFS *)stmt)->getBFSFilter();
            if (filter != nullptr)
                printLiveVarsNode(filter);
            printLiveVarsNode(((iterateReverseBFS *)stmt)->getBody());
            break;
        }
        case NODE_WHILESTMT:
        {
            printLiveVarsNode(((whileStmt *)stmt)->getCondition());
            printLiveVarsNode(((whileStmt *)stmt)->getBody());
            break;
        }
        default:
        {
            liveVarsNode *node = stmt->getLiveVarsNode();
            if (node == nullptr)
            {
                std::cout << "Error: LiveVarsNode* null" << std::endl;
                break;
            }
            std::cout << "Use : ";
            for (TableEntry *t : node->getUse())
                std::cout << t->getId()->getIdentifier() << ", ";
            std::cout << std::endl;
            std::cout << "Def : ";
            for (TableEntry *t : node->getDef())
                std::cout << t->getId()->getIdentifier() << ", ";
            std::cout << std::endl;
            std::cout << "In  : ";
            for (TableEntry *t : node->getIn())
                std::cout << t->getId()->getIdentifier() << ", ";
            std::cout << std::endl;
            std::cout << "Out : ";
            for (TableEntry *t : node->getOut())
                std::cout << t->getId()->getIdentifier() << ", ";
            std::cout << std::endl
                      << std::endl;
            break;
        }
        }
        return;
    }
};

#endif