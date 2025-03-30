#ifndef GET_USED_VARS
#define GET_USED_VARS
#include "../../analyser/analyserUtil.cpp"
usedVariables getVarsStatement(statement* stmt);
usedVariables getVarsExpr(Expression *expr);
usedVariables getVarsWhile(whileStmt *stmt);
usedVariables getVarsDoWhile(dowhileStmt *stmt);
usedVariables getVarsAssignment(assignment *stmt);
usedVariables getVarsIf(ifStmt *stmt);
usedVariables getVarsFixedPoint(fixedPointStmt *stmt);
usedVariables getVarsReduction(reductionCallStmt *stmt);
usedVariables getVarsUnary(unary_stmt *stmt);
usedVariables getVarsForAll(forallStmt *stmt);
usedVariables getVarsBlock(blockStatement *blockStmt);
usedVariables getDeclaredPropertyVarsOfBlock(blockStatement *blockStmt);
#endif