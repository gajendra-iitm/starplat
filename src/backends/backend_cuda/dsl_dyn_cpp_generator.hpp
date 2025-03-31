#ifndef CU_DSL_DYN_CPP_GENERATOR
#define CU_DSL_DYN_CPP_GENERATOR

#include "dsl_cpp_generator.h"


namespace spdyncuda{
class dsl_dyn_cpp_generator:public spcuda::dsl_cpp_generator
{   
 private:
 Identifier* batchEnvSizeId;
 Identifier* updatesId;

 public:
  
  dsl_dyn_cpp_generator();
 void generateIncrementalDecremental(Function* incrementalFunc, bool isMainFile );
void addOnAddkernel(onAddBlock* onAddStmt,bool isMainFile,list<Identifier*> vars);
void addOnDeletekernel(onDeleteBlock* onDeleteStmt,bool isMainFile,list<Identifier*> vars);
 void generateInDecHeader(Function* func, bool isMainFile);
 void generateDynamicFunc(Function* func, bool isMainFile);
 void generateDynamicHeader(Function* func, bool isMainFile);
 void generateBlock(blockStatement* blockStmt, bool includeBrace, bool isMainFile);
 void generateFunction(ASTNode* proc);
 void generateForAllSignature(forallStmt* forAll, bool isMainFile);
 void generateForAll(forallStmt* forAll, bool isMainFile);
 void generate_exprPropId(PropAccess* propId, bool isMainFile);
 void generate_exprProcCall(Expression* expr, bool isMainFile);
 void generateStatement(statement* stmt, bool isMainFile);
 void generateBatchBlock(batchBlock* batchStmt, bool isMainFile);
 void generateOnAddBlock(onAddBlock* onaddStmt, bool isMainFile);
 void generateOnDeleteBlock(onDeleteBlock* ondeleteStmt, bool isMainFile);
 bool generate();
 void generation_begin();
 void generation_end();
 void setBatchEnvIds(Identifier* updatesId);
 void setPreprocessEnv();
 bool getPreprocessEnv();
 void resetPreprocessEnv();
 Identifier* getUpdatesId();
 void resetBatchEnv();
 bool openFileforOutput();
 void closeOutputFile();
 void generateFreeInCurrentBatch();
 void generateBatchInitializer();
 void generateBatchFinalizer();
};

}
#endif

