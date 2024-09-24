#ifndef LIVE_VARS_ANALYZER_H
#define LIVE_VARS_ANALYZER_H

#include <unordered_map>
#include <set>
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

    static std::set<std::string> getAllUses(ASTNode *stmt)
    {
        if (stmt == nullptr)
        {
            cout << "Error: null ASTNode*" << endl;
        }

        std::set<std::string> uses;

        switch (stmt->getTypeofNode())
        {
        case NODE_BLOCKSTMT:
        {
            list<statement *> stmtlist = ((blockStatement *)stmt)->returnStatements();
            for (auto s = stmtlist.begin(); s != stmtlist.end(); s++)
            {
                auto uses1 = getAllUses(*s);
                uses.insert(uses1.begin(), uses1.end());
            }
            break;
        }
        case NODE_DOWHILESTMT:
        {
            auto uses1 = getAllUses(((dowhileStmt *)stmt)->getBody());
            auto uses2 = getAllUses(((dowhileStmt *)stmt)->getCondition());

            uses.insert(uses1.begin(), uses1.end());
            uses.insert(uses2.begin(), uses2.end());

            break;
        }
        case NODE_FIXEDPTSTMT:
        {
            auto uses1 = getAllUses(((fixedPointStmt *)stmt)->getFixedPointId());
            auto uses2 = getAllUses(((fixedPointStmt *)stmt)->getBody());

            uses.insert(uses1.begin(), uses1.end());
            uses.insert(uses2.begin(), uses2.end());

            break;
        }
        case NODE_FORALLSTMT:
        {
            auto uses1 = getAllUses(((forallStmt *)stmt)->getIterator());
            auto uses2 = getAllUses(((forallStmt *)stmt)->getBody());

            uses.insert(uses1.begin(), uses1.end());
            uses.insert(uses2.begin(), uses2.end());

            break;
        }
        case NODE_IFSTMT:
        {
            auto uses1 = getAllUses(((ifStmt *)stmt)->getCondition());
            auto uses2 = getAllUses(((ifStmt *)stmt)->getIfBody());

            uses.insert(uses1.begin(), uses1.end());
            uses.insert(uses2.begin(), uses2.end());

            if (((ifStmt *)stmt)->getElseBody())
            {
                auto uses3 = getAllUses(((ifStmt *)stmt)->getElseBody());
                uses.insert(uses3.begin(), uses3.end());
            }

            break;
        }
        case NODE_ITRBFS:
        {
            auto uses1 = getAllUses(((iterateBFS *)stmt)->getIteratorNode());
            uses.insert(uses1.begin(), uses1.end());

            Expression *filter = ((iterateBFS *)stmt)->getBFSFilter();
            if (filter != nullptr)
            {
                auto uses2 = getAllUses(filter);
                uses.insert(uses2.begin(), uses2.end());
            }

            auto uses3 = getAllUses(((iterateBFS *)stmt)->getBody());
            uses.insert(uses3.begin(), uses3.end());

            iterateReverseBFS *reverse = ((iterateBFS *)stmt)->getRBFS();
            if (reverse != nullptr)
            {
                auto uses4 = getAllUses(reverse);
                uses.insert(uses4.begin(), uses4.end());
            }

            break;
        }
        case NODE_ITRRBFS:
        {
            Expression *filter = ((iterateReverseBFS *)stmt)->getBFSFilter();
            if (filter != nullptr)
            {
                auto uses1 = getAllUses(filter);
                uses.insert(uses1.begin(), uses1.end());
            }

            auto uses2 = getAllUses(((iterateReverseBFS *)stmt)->getBody());
            uses.insert(uses2.begin(), uses2.end());

            break;
        }
        case NODE_WHILESTMT:
        {
            auto uses1 = getAllUses(((whileStmt *)stmt)->getCondition());
            auto uses2 = getAllUses(((whileStmt *)stmt)->getBody());

            uses.insert(uses1.begin(), uses1.end());
            uses.insert(uses2.begin(), uses2.end());

            break;
        }
        default:
        {
            liveVarsNode *node = stmt->getLiveVarsNode();
            if (node == nullptr)
            {
                cout << "Error: LiveVarsNode* null" << endl;
                break;
            }
            for (TableEntry *t : node->getUse())
                uses.insert(t->getId()->getIdentifier());

            break;
        }
        }

        return uses;
    }

    static std::set<std::string> getAllDefs(ASTNode *stmt)
    {
        if (stmt == nullptr)
        {
            cout << "Error: null ASTNode*" << endl;
        }

        std::set<std::string> defs;

        switch (stmt->getTypeofNode())
        {
        case NODE_BLOCKSTMT:
        {
            list<statement *> stmtlist = ((blockStatement *)stmt)->returnStatements();
            for (auto s = stmtlist.begin(); s != stmtlist.end(); s++)
            {
                auto defs1 = getAllDefs(*s);
                defs.insert(defs1.begin(), defs1.end());
            }
            break;
        }
        case NODE_DOWHILESTMT:
        {
            auto defs1 = getAllDefs(((dowhileStmt *)stmt)->getBody());
            auto defs2 = getAllDefs(((dowhileStmt *)stmt)->getCondition());

            defs.insert(defs1.begin(), defs1.end());
            defs.insert(defs2.begin(), defs2.end());

            break;
        }
        case NODE_FIXEDPTSTMT:
        {
            auto defs1 = getAllDefs(((fixedPointStmt *)stmt)->getFixedPointId());
            auto defs2 = getAllDefs(((fixedPointStmt *)stmt)->getBody());

            defs.insert(defs1.begin(), defs1.end());
            defs.insert(defs2.begin(), defs2.end());

            break;
        }
        case NODE_FORALLSTMT:
        {
            auto defs1 = getAllDefs(((forallStmt *)stmt)->getIterator());
            auto defs2 = getAllDefs(((forallStmt *)stmt)->getBody());

            defs.insert(defs1.begin(), defs1.end());
            defs.insert(defs2.begin(), defs2.end());

            break;
        }
        case NODE_IFSTMT:
        {
            auto defs1 = getAllDefs(((ifStmt *)stmt)->getCondition());
            auto defs2 = getAllDefs(((ifStmt *)stmt)->getIfBody());

            defs.insert(defs1.begin(), defs1.end());
            defs.insert(defs2.begin(), defs2.end());

            if (((ifStmt *)stmt)->getElseBody())
            {
                auto defs3 = getAllDefs(((ifStmt *)stmt)->getElseBody());
                defs.insert(defs3.begin(), defs3.end());
            }

            break;
        }
        case NODE_ITRBFS:
        {
            auto defs1 = getAllDefs(((iterateBFS *)stmt)->getIteratorNode());
            defs.insert(defs1.begin(), defs1.end());

            Expression *filter = ((iterateBFS *)stmt)->getBFSFilter();
            if (filter != nullptr)
            {
                auto defs2 = getAllDefs(filter);
                defs.insert(defs2.begin(), defs2.end());
            }

            auto defs3 = getAllDefs(((iterateBFS *)stmt)->getBody());
            defs.insert(defs3.begin(), defs3.end());

            iterateReverseBFS *reverse = ((iterateBFS *)stmt)->getRBFS();
            if (reverse != nullptr)
            {
                auto defs4 = getAllDefs(reverse);
                defs.insert(defs4.begin(), defs4.end());
            }

            break;
        }
        case NODE_ITRRBFS:
        {
            Expression *filter = ((iterateReverseBFS *)stmt)->getBFSFilter();
            if (filter != nullptr)
            {
                auto defs1 = getAllDefs(filter);
                defs.insert(defs1.begin(), defs1.end());
            }

            auto defs2 = getAllDefs(((iterateReverseBFS *)stmt)->getBody());
            defs.insert(defs2.begin(), defs2.end());

            break;
        }
        case NODE_WHILESTMT:
        {
            auto defs1 = getAllDefs(((whileStmt *)stmt)->getCondition());
            auto defs2 = getAllDefs(((whileStmt *)stmt)->getBody());

            defs.insert(defs1.begin(), defs1.end());
            defs.insert(defs2.begin(), defs2.end());

            break;
        }
        default:
        {
            liveVarsNode *node = stmt->getLiveVarsNode();
            if (node == nullptr)
            {
                cout << "Error: LiveVarsNode* null" << endl;
                break;
            }
            for (TableEntry *t : node->getDef())
                defs.insert(t->getId()->getIdentifier());

            break;
        }
        }

        return defs;
    }
};

#endif