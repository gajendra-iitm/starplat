#include "dsl_dyn_cpp_generator.hpp"
#include "../../ast/ASTHelper.cpp"
// #include "getUsedVars.cpp"
// #include "dsl_cpp_generator.cpp"

namespace spdyncuda{

// void dsl_dyn_cpp_generator::setPreprocessEnv()
// {
//   // insidePreprocessEnv = true;
// }

// void dsl_dyn_cpp_generator::resetPreprocessEnv()
// {
//   // insidePreprocessEnv = false;
// }

// bool dsl_dyn_cpp_generator::getPreprocessEnv()
// {
//   // return insidePreprocessEnv;
// }
  dsl_dyn_cpp_generator::  dsl_dyn_cpp_generator()
  {
    std::cout<<"I AM SUPPOSED TO EXIST\n";
    batchEnvSizeId = NULL;
    updatesId = NULL;
    isDynamic = true;
  }
void dsl_dyn_cpp_generator::setBatchEnvIds(Identifier* updatesIdSent)
{
   updatesId = updatesIdSent;

}

void dsl_dyn_cpp_generator::resetBatchEnv()
{
   updatesId = NULL;

}



Identifier* dsl_dyn_cpp_generator::getUpdatesId()
{
    return updatesId;
}



void dsl_dyn_cpp_generator::generateOnDeleteBlock(onDeleteBlock* onDeleteStmt, bool isMainFile)
{

  // dslCodePad& targetFile = isMainFile ? main : header;
  //  setPreprocessEnv();
   char strBuffer[1024];
   Identifier* updatesId = onDeleteStmt->getUpdateId();
   proc_callExpr* updatesFunc = onDeleteStmt->getUpdateFunc();
   string methodId = (updatesFunc->getMethodId()->getIdentifier());

   if(methodId == "currentBatch")
   {
     sprintf(strBuffer,"for(int batchIndex = updateIndex ; batchIndex < (updateIndex+batchSize) && batchIndex < %s.size() ; batchIndex++){",updatesId->getIdentifier());
     main.pushstr_newL(strBuffer);
     sprintf(strBuffer,"if(%s[batchIndex].type == 'd')",updatesId->getIdentifier());
     main.pushstr_newL(strBuffer);
     main.pushstr_newL("{");
     sprintf(strBuffer, "update %s = %s[batchIndex] ;",onDeleteStmt->getIteratorId()->getIdentifier(), updatesId->getIdentifier());
     main.pushstr_newL(strBuffer);
     generateBlock(onDeleteStmt->getStatements(),true,true);
     main.NewLine();
     main.pushstr_newL("}");
     main.pushstr_newL("}");
   }
   
  //  resetPreprocessEnv();


}

void dsl_dyn_cpp_generator::generateOnAddBlock(onAddBlock* onAddStmt,bool isMainFile)
{
  // dslCodePad& targetFile = isMainFile ? main : header;
    // setPreprocessEnv();

   char strBuffer[1024];
  Identifier* updatesId = onAddStmt->getUpdateId();
   proc_callExpr* updatesFunc = onAddStmt->getUpdateFunc();
   string methodId = (updatesFunc->getMethodId()->getIdentifier());

   if(methodId == "currentBatch")
   {
      usedVariables usedVars = getVarsBlock(onAddStmt->getStatements());
  list<Identifier*> vars = usedVars.getVariables();
      addOnAddkernel(onAddStmt,isMainFile,vars);
     std::cout<<"After Kernel\n";
     sprintf(strBuffer,"OnAdd_kernel<<<updateBlocks,updateThreads>>>(d_%s,batchElements",updatesId->getIdentifier());
     main.pushString(strBuffer);
     MetaDataUsed *m = onAddStmt->getMetaDataUsed(); 
     if(m->isMetaUsed)
        main.pushString(", d_meta");
    if(m->isDataUsed)
      main.pushString(", d_data");
  if(m->isSrcUsed)
    main.pushString(", d_src");
  if(m->isWeightUsed)
    main.pushString(", d_weight");
  if(m->isRevMetaUsed)
    main.pushString(", d_rev_meta");
  //  std::cout<<"OnADD KERNEL After Normal HEADERS\n";
  for (Identifier* iden : vars) {
    if(iden->getSymbolInfo()==NULL) continue;
    Type* type = iden->getSymbolInfo()->getType();
    if (type->isPropType()) {
      sprintf(strBuffer, ", d_%s", iden->getIdentifier());
      main.pushString(/*createParamName(*/ strBuffer);
      if(iden->getSymbolInfo()->getId()->get_fp_association()) { // If id has a fp association then _next must also be added as a parameter
        sprintf(strBuffer, ", d_%s_next",iden->getIdentifier());
        main.pushString(/*createParamName(*/ strBuffer);
      }
    } else {
      std::cout<<"vars not property:"<<iden->getIdentifier()<<"\n";
    }
  }
    sprintf(strBuffer,");");
     main.pushstr_newL(strBuffer);
    
   }

  //  resetPreprocessEnv();


}
/*
void dsl_dyn_cpp_generator::generateFreeInCurrentBatch()
{
  //dslCodePad& targetFile = isMainFile ? main : header;
  char strBuffer[1024];
  for(int i=0 ; i < freeIdStore.back().size() ; i++)
     { 
       Identifier* id = freeIdStore.back()[i];
       if(id->getSymbolInfo()->getType()->gettypeId() == TYPE_PROPNODE ||
          id->getSymbolInfo()->getType()->gettypeId() == TYPE_PROPEDGE)
         {
          sprintf(strBuffer, "free(%s) ;", id->getIdentifier());
          main.pushstr_newL(strBuffer);
         }
       if(id->getSymbolInfo()->getType()->gettypeId() == TYPE_UPDATES)
          {
            sprintf(strBuffer, "%s.clear();", id->getIdentifier());
            main.pushstr_newL(strBuffer);
          }   

     }

}
*/

void dsl_dyn_cpp_generator::generateBatchBlock(batchBlock* batchStmt, bool isMainFile)
{
  dslCodePad& targetFile = isMainFile ? main : header;
  
  // list<formalParam*> paramList = currentFunc->getParamList();
  // list<formalParam*>::iterator itr;
  std::cout<<"164 BATCH BLOCK\n";
  Identifier* updateId = batchStmt->getUpdateId();
  std::cout<<"166 BATCH BLOCK\n";
  char * updateIdentifier = updateId->getIdentifier();
  std::cout<<"168 BATCH BLOCK\n";
  // auto updateTypeId = updatesId->getSymbolInfo()->getType();
  // std::cout<<"170 BATCH BLOCK\n";
  // auto targetGraph = updateTypeId->getTargetGraph();
  isInsideBatch = true;
  char strBuffer[1024];
  // insideBatchBlock = true;

  /*for(itr=paramList.begin();itr!=paramList.end();itr++)
  { 
      Type* type=(*itr)->getType();
      if(type->gettypeId() == TYPE_UPDATES)  
        {
          updateId = (*itr)->getIdentifier();
          break;
        }
         
  }*/
  
  if(updateId != NULL)
    {
      // setBatchEnvIds(updateId); //TODO: need to set batchsize's Id as well.
      std::cout<<"186 generateBatch Block\n";
      // Identifier*targetGraph = updateId->getSymbolInfo()->getType()->getTargetGraph();
      std::cout<<"188 generateBatch Block\n";
      // generateCSRArrays(targetGraph->getIdentifier(),currentFunc);
      // generateBatchInitializer();

      std::cout<<"STARTING NOW\n";
      main.pushString("int _batchSize = ");
      generateExpr(batchStmt->getBatchSizeExpr(),true,false);
      main.pushstr_newL(";");
      sprintf(strBuffer,"update *d_%s;",updateIdentifier);
      main.pushstr_newL(strBuffer);
      sprintf(strBuffer,"cudaMalloc(&d_%s,sizeof(update)*_batchSize);",updateIdentifier);
      main.pushstr_newL(strBuffer);
      sprintf(strBuffer,"int batchElements = 0;");
      main.pushstr_newL(strBuffer);
      generateStartTimer();
     /* sprintf(strBuffer,"int updateSize = %s.size() ;", updateId->getIdentifier());
      main.pushstr_newL(strBuffer);*/
      sprintf(strBuffer,"for( int updateIndex = 0 ; updateIndex < %s.size() ; updateIndex += _batchSize){",updateId->getIdentifier());
      main.pushstr_newL(strBuffer);
      sprintf(strBuffer,"if((updateIndex + _batchSize) > %s.size())",updateId->getIdentifier());
      main.pushstr_newL(strBuffer);
      main.pushstr_newL("{");
      sprintf(strBuffer,"batchElements = %s.size() - updateIndex ;",updateId->getIdentifier());
      main.pushstr_newL(strBuffer);
      main.pushstr_newL("}");
      main.pushstr_newL("else");
      sprintf(strBuffer,"batchElements = _batchSize ;");
      main.pushstr_newL(strBuffer);
      snprintf(strBuffer,sizeof(strBuffer),"cudaMemcpy(d_%s,&%s[updateIndex],batchElements*sizeof(update),cudaMemcpyHostToDevice);",updateIdentifier,updateIdentifier);
      main.pushstr_newL(strBuffer);  
  
      sprintf(strBuffer, "unsigned updateThreads = (batchElements < THREADS_PER_BLOCK)? batchElements: THREADS_PER_BLOCK;");
      main.pushstr_newL(strBuffer);
      sprintf(strBuffer,"unsigned updateBlocks = (batchElements+updateThreads-1)/updateThreads;");
      main.pushstr_newL(strBuffer);
      main.NewLine();
      generateBlock(batchStmt->getStatements(),false,true);
      main.NewLine();
     // generateFreeInCurrentBatch();
     //freeIdStore.pop_back();
      main.pushstr_newL("}");
      generateStopTimer();
      // generateBatchFinalizer();
      // resetBatchEnv();

    }

  

    // insideBatchBlock = false;

  isInsideBatch = false;
}

void dsl_dyn_cpp_generator::generateBlock(blockStatement* blockStmt, bool includeBrace, bool isMainFile){
    dslCodePad& targetFile = isMainFile ? main : header;

  // usedVariables usedVars = getDeclaredPropertyVarsOfBlock(blockStmt);
  // list<Identifier*> vars = usedVars.getVariables();
  // std::cout << "\t==VARSIZE:" << vars.size() << '\n';

  //~ for(Identifier* iden: vars) {
  //~ Type* type = iden->getSymbolInfo()->getType();
  //~ char strBuffer[1024];
  //~ printf("\t===%s d_%s\n", convertToCppType(type), iden->getIdentifier());
  //~ main.pushstr_newL(strBuffer);
  //~ }

  list<statement*> stmtList = blockStmt->returnStatements();
  list<statement*>::iterator itr;
  if (includeBrace) {
    targetFile.pushstr_newL("{");
  }
  int sc = 0;
  for (itr = stmtList.begin(); itr != stmtList.end(); itr++) {
    statement* stmt = *itr;
    std::cout<<"Generating individual statements"<<sc++<<"\n";
    generateStatement(stmt, isMainFile);
  }

  // CUDA FREE
  // char strBuffer[1024];

  // if (vars.size() > 0) {
  //   targetFile.NewLine();
  //   targetFile.pushstr_newL("//cudaFree up!! all propVars in this BLOCK!");
  // }
  // for (Identifier* iden : vars) {
  //   sprintf(strBuffer, "cudaFree(d_%s);", iden->getIdentifier());
  //   targetFile.pushstr_newL(strBuffer);
  // }
  targetFile.NewLine();

  if (includeBrace) {
    targetFile.pushstr_newL("}");
  }
}


void dsl_dyn_cpp_generator::generateStatement(statement* stmt, bool isMainFile )
{ 
    std::cout<<"DYN GENERATING STATEMENT TYPE:"<<stmt->getTypeofNode()<<"\n";
    if(stmt->getTypeofNode() == NODE_BATCHBLOCKSTMT)
       {
        std::cout<<"TRYING TO GENERATE BATCG STATEMENT TYPE:"<<stmt->getTypeofNode()<<"\n";
         generateBatchBlock((batchBlock*)stmt, isMainFile);

       }
    else if(stmt->getTypeofNode() == NODE_ONADDBLOCK)
      {
         std::cout<<"TRYING TO GENERATE ONADD STATEMENT TYPE:"<<stmt->getTypeofNode()<<"\n";
        generateOnAddBlock((onAddBlock*)stmt, isMainFile);

      }
    else if(stmt->getTypeofNode() == NODE_ONDELETEBLOCK)
      {
         std::cout<<"TRYING TO GENERATE ONDELETE STATEMENT TYPE:"<<stmt->getTypeofNode()<<"\n";
        generateOnDeleteBlock((onDeleteBlock*)stmt, isMainFile);
        
      }  
    else {
      std::cout<<"TRYING TO GENERATE STATIC STATEMENT TYPE:"<<stmt->getTypeofNode()<<"\n";
      dsl_cpp_generator::generateStatement(stmt,isMainFile);
      std::cout<<"FINISHED TRYING TO GENERATE STATIC STATEMENT TYPE:"<<stmt->getTypeofNode()<<"\n";
    }
         
}


void dsl_dyn_cpp_generator::generate_exprProcCall(Expression* expr, bool isMainFile)
{
  dslCodePad& targetFile = isMainFile ? main : header;
  proc_callExpr* proc=(proc_callExpr*)expr;
  string methodId(proc->getMethodId()->getIdentifier());
  //  printf("HERE PRESENT %s\n",proc->getMethodId()->getIdentifier());
  if(methodId == "updateCSRAdd" || methodId == "updateCSRDel") 
     {
      
       char strBuffer[1024];
       list<argument*> argList = proc->getArgList();
       assert(argList.size() == 1);
       Identifier* updateId = argList.front()->getExpr()->getId();
       Identifier* objectId=proc->getId1();
       assert(updateId->getSymbolInfo()->getType()->gettypeId() == TYPE_UPDATES);
       if(methodId == "updateCSRAdd")
          sprintf(strBuffer,"%s.%s(%s, %s, %s);",objectId->getIdentifier(),"updateCSR_Add",updateId->getIdentifier(),"updateIndex","batchElements");
       else
          sprintf(strBuffer,"%s.%s(%s, %s, %s);",objectId->getIdentifier(),"updateCSR_Del",updateId->getIdentifier(),"updateIndex","batchElements");
       main.pushstr_newL(strBuffer);

     }
   else if(methodId == "currentBatch")
      {
          // TODO
      }   
   else if(methodId == "Incremental" || methodId == "Decremental") 
        {
          /* for dynamic algos */

           char strBuffer[1024];
           if(methodId == "Incremental")
               sprintf(strBuffer,"%s_add",fileName);
           if(methodId == "Decremental")
              sprintf(strBuffer, "%s_del", fileName);
           main.pushString(strBuffer);
           generateArgList(proc->getArgList(), true,true);   // uncomment it later  

        } 
   else
    { 

      dsl_cpp_generator::generate_exprProcCall(expr,isMainFile) ;
    }

}


// void dsl_dyn_cpp_generator::generate_exprPropId(PropAccess* propId, bool isMainFile) //This needs to be made more generic.
// { 
//   dslCodePad& targetFile = isMainFile ? main : header;
//   char strBuffer[1024];
//   Identifier* id1=propId->getIdentifier1();
//   Identifier* id2=propId->getIdentifier2();
//   Expression* indexexpr = propId->getPropExpr();
//   ASTNode* propParent=propId->getParent();
//   //bool relatedToReduction = propParent!=NULL?propParent->getTypeofNode()==NODE_REDUCTIONCALLSTMT:false;
//   if(id2 != NULL){
//    if(id2->getSymbolInfo()!=NULL&&id2->getSymbolInfo()->getId()->get_fp_association())
//     {
//        bool relatedToUpdation = propParent!=NULL?((propParent->getTypeofNode() == NODE_REDUCTIONCALLSTMT|| propParent->getTypeofNode() == NODE_ASSIGN)):false;
//        if(relatedToUpdation)
//            {
//              sprintf(strBuffer,"%s_nxt[%s]",id2->getIdentifier(),id1->getIdentifier());
//           // printf("Inside this checked !\n");
//            }
//        else
//           {
//             sprintf(strBuffer,"%s[%s]",id2->getIdentifier(),id1->getIdentifier());
//           }    
      
//     }
//     else
//     {
//       if(id2->getSymbolInfo()==NULL)
//         {
//           if(id1->getUpdateAssociation()!=NULL)
//             {
//                sprintf(strBuffer, "%s[%s].%s" , getUpdatesId()->getIdentifier(),"batchIndex",id2->getIdentifier());
//             }
//           else
//             sprintf(strBuffer, "%s.%s",id1->getIdentifier(),id2->getIdentifier());
//         }
//       else 
//        {   
//         if(curFuncType == INCREMENTAL_FUNC || curFuncType == DECREMENTAL_FUNC || curFuncType == DYNAMIC_FUNC)
//            { 
//               if(id2->getSymbolInfo()->getType()->gettypeId()==TYPE_PROPEDGE)
//                  sprintf(strBuffer,"%s[%s.id]",id2->getIdentifier(),id1->getIdentifier());
//               else
//                  sprintf(strBuffer,"%s[%s]",id2->getIdentifier(),id1->getIdentifier());  
//            }
//         else 
//           sprintf(strBuffer,"%s[%s]",id2->getIdentifier(),id1->getIdentifier());

//        }
//     }

// }

// else if(indexexpr != NULL){

//      /* semantic checks needs to be added for maps containing property as values */
//      generateExpr(indexexpr, isMainFile);

//      if(id1->getSymbolInfo() != NULL) 
//      {
//        int typeId = id1->getSymbolInfo()->getType()->gettypeId();
//        if(typeId == TYPE_INT || typeId == TYPE_NODE)

//           sprintf(strBuffer,"[%s]",id1->getIdentifier());

//        else if(typeId == TYPE_EDGE){

//           sprintf(strBuffer,"[%s.id]",id1->getIdentifier());

//        }    
//      }
//      else
//         sprintf(strBuffer,"[%s]",id1->getIdentifier());   
//  }
     
//  main.pushString(strBuffer);
// }


// void dsl_dyn_cpp_generator::generateForAllSignature(forallStmt* forAll, bool isMainFile)
// {
  
//   dslCodePad& targetFile = isMainFile ? main : header;

//   map<string, bool> dynamicLinkFunc = frontEndContext.getDynamicLinkFuncs();
//   if(curFuncType == STATIC_FUNC)
//     {
//       dsl_cpp_generator::generateForAllSignature(forAll,false );
//     } 
//   else
//    { 

//     Identifier* funcId = currentFunc->getIdentifier();  
//    string funcIdString = "";
//    if(funcId != NULL)
//       funcIdString.assign(funcId->getIdentifier());

//    if(curFuncType == DECREMENTAL_FUNC || curFuncType == INCREMENTAL_FUNC 
//       ||curFuncType == DYNAMIC_FUNC || (dynamicLinkFunc.find(funcIdString) != dynamicLinkFunc.end())) {

//    char strBuffer[1024];
//    Identifier* iterator=forAll->getIterator();
//    if(forAll->isSourceProcCall())
//     {
//     Identifier* sourceGraph=forAll->getSourceGraph();
//     proc_callExpr* extractElemFunc=forAll->getExtractElementFunc();
//     Identifier* iteratorMethodId=extractElemFunc->getMethodId();
//     if(allGraphIteration(iteratorMethodId->getIdentifier()))
//     {
//       char* graphId=sourceGraph->getIdentifier();
//       char* methodId=iteratorMethodId->getIdentifier();
//       string s(methodId);
//       if(s.compare("nodes")==0)
//       {
//         cout<<"INSIDE NODES VALUE"<<"\n";
//        sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++) ","int",iterator->getIdentifier(),iterator->getIdentifier(),graphId,"num_nodes",iterator->getIdentifier());
//       }
//       else
//       sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++) ","int",iterator->getIdentifier(),iterator->getIdentifier(),graphId,"num_edges",iterator->getIdentifier());

//       main.pushstr_newL(strBuffer);

//      }
//     else if(neighbourIteration(iteratorMethodId->getIdentifier()))
//     { 
       
//        char* graphId=sourceGraph->getIdentifier();
//        char* methodId=iteratorMethodId->getIdentifier();
//        string s(methodId);
//        if(s.compare("neighbors")==0)
//        {
//        list<argument*>  argList=extractElemFunc->getArgList();
//        assert(argList.size()==1);
//        Identifier* nodeNbr=argList.front()->getExpr()->getId();
//        sprintf(strBuffer,"for (edge %s_edge : %s.getNeighbors(%s)) ",nodeNbr->getIdentifier(),graphId,nodeNbr->getIdentifier());
//        main.pushstr_newL(strBuffer);
//        main.pushString("{");
//        sprintf(strBuffer,"%s %s = %s_edge.destination ;","int",iterator->getIdentifier(),nodeNbr->getIdentifier()); //needs to move the addition of
//        main.pushstr_newL(strBuffer);
//        }
//        if(s.compare("nodes_to")==0)
//        {
//         list<argument*>  argList=extractElemFunc->getArgList();
//        assert(argList.size()==1);
//        Identifier* nodeNbr=argList.front()->getExpr()->getId();
//        sprintf(strBuffer,"for (edge %s_inedge : %s.getInNeighbors(%s)) ",nodeNbr->getIdentifier(),graphId,nodeNbr->getIdentifier());
//        main.pushstr_newL(strBuffer);
//        main.pushString("{");
//        sprintf(strBuffer,"%s %s = %s_inedge.destination ;","int",iterator->getIdentifier(),nodeNbr->getIdentifier()); //needs to move the addition of
//        main.pushstr_newL(strBuffer);
//        }
//         if(s.compare("inOutNbrs")==0)
//        {
//         list<argument*>  argList=extractElemFunc->getArgList();
//        assert(argList.size()==1);
//        Identifier* nodeNbr=argList.front()->getExpr()->getId();
//        sprintf(strBuffer,"for (edge %s_edges: %s.getInOutNbrs(%s)) ",nodeNbr->getIdentifier(),graphId,nodeNbr->getIdentifier());
//        main.pushstr_newL(strBuffer);
//        main.pushString("{");
//        sprintf(strBuffer,"%s %s = %s_edges.destination ;","int",iterator->getIdentifier(),nodeNbr->getIdentifier()); //needs to move the addition of
//        main.pushstr_newL(strBuffer);
//        }                                                                                                //statement to a different method.                                                                                                    //statement to a different method.

//     }
//   }
//   else if(forAll->isSourceField())
//   {
//     /*PropAccess* sourceField=forAll->getPropSource();
//     Identifier* dsCandidate = sourceField->getIdentifier1();
//     Identifier* extractId=sourceField->getIdentifier2();
    
//     if(dsCandidate->getSymbolInfo()->getType()->gettypeId()==TYPE_SETN)
//     {

//       main.pushstr_newL("std::set<int>::iterator itr;");
//       sprintf(strBuffer,"for(itr=%s.begin();itr!=%s.end();itr++)",dsCandidate->getIdentifier(),dsCandidate->getIdentifier());
//       main.pushstr_newL(strBuffer);

//     }
        
//     /*
//     if(elementsIteration(extractId->getIdentifier()))
//       {
//         Identifier* collectionName=forAll->getPropSource()->getIdentifier1();
//         int typeId=collectionName->getSymbolInfo()->getType()->gettypeId();
//         if(typeId==TYPE_SETN)
//         {
//           main.pushstr_newL("std::set<int>::iterator itr;");
//           sprintf(strBuffer,"for(itr=%s.begin();itr!=%s.end();itr++)",collectionName->getIdentifier(),collectionName->getIdentifier());
//           main.pushstr_newL(strBuffer);
//         }

//       }*/

//   }

//    else if(forAll->isSourceExpr()){
  
//   Expression* expr = forAll->getSourceExpr();
//   Expression* mapExpr = expr->getMapExpr();
//   Identifier* mapId = mapExpr->getId();

//   cout<<"ENTERED................................................."<<"\n";

//   if(mapId->getSymbolInfo()->getType()->gettypeId() == TYPE_CONTAINER){
//      main.pushString("for(int i = 0 ; i < ");
//      generateExpr(expr, isMainFile);
//      main.pushstr_newL(".size() ; i++)");
//      main.pushString("{ ");
//      sprintf(strBuffer, "int %s = ", iterator->getIdentifier());
//      main.pushString(strBuffer);
//      generateExpr(expr, isMainFile);
//      main.pushstr_newL("[i] ;");
//   } 
  
//  // cout<<"REACHED HERE AFTER COMPLETING "<<"\n";
  
//   }
//   else
//   {
//     Identifier* sourceId = forAll->getSource();
//     if(sourceId !=NULL)
//        {
//           if(sourceId->getSymbolInfo()->getType()->gettypeId()==TYPE_SETN)
//           {

//            main.pushstr_newL("std::set<int>::iterator itr;");
//            sprintf(strBuffer,"for(itr=%s.begin();itr!=%s.end();itr++)",sourceId->getIdentifier(),sourceId->getIdentifier());
//            main.pushstr_newL(strBuffer);

//           }  
//           else if(sourceId->getSymbolInfo()->getType()->gettypeId()==TYPE_UPDATES)
//               {
//                  sprintf(strBuffer,"for(int i = 0 ; i < %s.size() ; i++)",sourceId->getIdentifier());
//                  main.pushstr_newL(strBuffer);

//               }

//                else if(sourceId->getSymbolInfo()->getType()->gettypeId() == TYPE_CONTAINER){

//                  sprintf(strBuffer,"for(int i = 0 ; i < %s.size() ; i++)",sourceId->getIdentifier());
//                  main.pushstr_newL(strBuffer); 
//                  main.pushString("{ ");
//                  sprintf(strBuffer, "int %s = %s[i] ;", iterator->getIdentifier(), sourceId->getIdentifier());
//                  main.pushstr_newL(strBuffer); 

//           }  
//        }
//    }
// }
// else {

//  dsl_cpp_generator::generateForAllSignature(forAll, true);

// } 
//  }


// }

// void dsl_dyn_cpp_generator::generateForAll(forallStmt* forAll, bool isMainFile )
// { 

//    dslCodePad& targetFile = isMainFile ? main : header;
//     proc_callExpr* extractElemFunc = forAll->getExtractElementFunc();
//   PropAccess* sourceField = forAll->getPropSource();
//   Identifier* sourceId = forAll->getSource();
//   // Identifier* extractId;
//   Identifier* collectionId;
//   if (sourceField != NULL) {
//     collectionId = sourceField->getIdentifier1();
//   }
//   if (sourceId != NULL) {
//     collectionId = sourceId;
//   }
//   Identifier* iteratorMethodId;
//   if (extractElemFunc != NULL)
//     iteratorMethodId = extractElemFunc->getMethodId();
//   statement* body = forAll->getBody();
//   char strBuffer[1024];
//   if (forAll->isForall()) {  // IS FORALL

//     /*
//     if (forAll->hasFilterExpr()) {
//       Expression* filterExpr = forAll->getfilterExpr();
//       Expression* lhs = filterExpr->getLeft();
//       Identifier* filterId = lhs->isIdentifierExpr() ? lhs->getId() : NULL;
//       TableEntry* tableEntry = filterId != NULL ? filterId->getSymbolInfo() : NULL;


//       if (tableEntry != NULL && tableEntry->getId()->get_fp_association()) {
//         generateForAll_header(forAll, false);
//       } else {
//         generateForAll_header(forAll, false);
//       }

//     } else {
//       //~ std::cout << "\t\tFFOOOOOORRRRR" << '\n';
//       generateForAll_header(forAll, isMainFile);
//     }
//     */
//     printf("Entered here for forall \n");

//     if (!isOptimized) {
//       std::cout<< "============EARLIER NOT OPT=============" << '\n';
//       usedVariables usedVars = getVarsForAll(forAll);
//       list<Identifier*> vars = usedVars.getVariables();
      
      

//       for (Identifier* iden : vars) {

//         if(iden == NULL || iden->getIdentifier() == NULL){
//             cout << "iden is NULL ---------------";
//         }
//         std::cout<< "varName:" << iden->getIdentifier() << '\n';
      
//        // Type* type = iden->getSymbolInfo()->getType();

//        // if (type->isPrimitiveType())
//          // generateCudaMemCpySymbol(iden->getIdentifier(), convertToCppType(type), true);
//         /*else if(type->isPropType())
//           {
//             Type* innerType = type->getInnerTargetType();
//             string dIden = "d_" + string(iden->getIdentifier());
//             generateCudaMemCpyStr(dIden.c_str(), iden->getIdentifier(), convertToCppType(innerType), "V", true);
//           }*/
//       }
//     }
//     /*memcpy to symbol*/

//     //main.pushString(getCurrentFunc()->getIdentifier()->getIdentifier());
//     main.pushString("_kernel");
//     main.pushString("<<<");
//     main.pushString("numBlocks, threadsPerBlock");
//     main.pushString(">>>");
//     main.push('(');
//     main.pushString("V,E,d_meta,d_data,d_src,d_weight,d_rev_meta,d_modified_next");
//     //  if(currentFunc->getParamList().size()!=0)
//     // main.pushString(",");
//     if (!isOptimized) {
//       std::cout<< "NOT OPTIMESED ---------------" << '\n';
//       usedVariables usedVars = getVarsForAll(forAll);
//       list<Identifier*> vars = usedVars.getVariables();
//       for (Identifier* iden : vars) {
//         Type* type = iden->getSymbolInfo()->getType();
//         if (type->isPropType()) {
//           main.pushString(",");
//           main.pushString("d_");
//           main.pushString(/*createParamName(*/ iden->getIdentifier());
//         }
//       }
//     } else {
//       std::cout<< "INN OPTIMESED ---------------" << '\n';
//       for (Identifier* iden : forAll->getUsedVariables()) {
//         std::cout<< "_" << '\n';
//         Type* type = iden->getSymbolInfo()->getType();
//         if (type->isPropType()) {
//           main.pushString(",");
//           main.pushString("d_");
//           main.pushString(/*createParamName(*/ iden->getIdentifier());
//         }
//       }
//     }
//     main.pushString(")");
//     main.push(';');
//     main.NewLine();

//     main.pushString("cudaDeviceSynchronize();");
//     main.NewLine();

//     if (!isOptimized) {
//       usedVariables usedVars = getVarsForAll(forAll);
//       list<Identifier*> vars = usedVars.getVariables();
//       for (Identifier* iden : vars) {
//         Type* type = iden->getSymbolInfo()->getType();
//         if (type->isPrimitiveType())
//           generateCudaMemCpySymbol(iden->getIdentifier(), convertToCppType(type), false);
//         /*else if(type->isPropType())
//           {
//             Type* innerType = type->getInnerTargetType();
//             string dIden = "d_" + string(iden->getIdentifier());
//             generateCudaMemCpyStr(iden->getIdentifier(),dIden.c_str(), convertToCppType(innerType), "V", false);
//           }*/
//       }
//       /*memcpy from symbol*/
//     }

//     main.NewLine();
//     // main.pushString("cudaMemcpyFromSymbol(&diff_check, diff, sizeof(float));");
//     main.NewLine();
//     // main.pushString("diff = diff_check;");
//     main.NewLine();

//     //~ main.pushString("// cudaDeviceSynchronize(); //SSSP");
//     //~ main.NewLine();
//     //~ main.NewLine();

//     //~ if (forAll->hasFilterExpr()) {
//     //~ blockStatement* changedBody = includeIfToBlock(forAll);
//     //~ cout << "============CHANGED BODY  TYPE==============" << (changedBody->getTypeofNode() == NODE_BLOCKSTMT);
//     //~ forAll->setBody(changedBody);
//     //~ // cout<<"FORALL BODY
//     //~ // TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
//     //~ }

//     addCudaKernel(forAll);

//   }

//   else {  // IS FOR

//     generateForAllSignature(forAll, false);  // FOR LINE

//     if (forAll->hasFilterExpr()) {
//       blockStatement* changedBody = includeIfToBlock(forAll);
//       cout << "============CHANGED BODY  TYPE==============" << (changedBody->getTypeofNode() == NODE_BLOCKSTMT);
//       forAll->setBody(changedBody);
//       // cout<<"FORALL BODY
//       // TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
//     }

//     if (extractElemFunc != NULL) {
//       forallStack.push_back(make_pair(forAll->getIterator(),forAll->getExtractElementFunc())); 

//       if (neighbourIteration(iteratorMethodId->getIdentifier())) {  // todo forall neigbour iterion
//         cout << "\t ITERATE Neighbour \n";

//         //~ char* tmpStr = forAll->getSource()->getIdentifier();
//         char* wItr = forAll->getIterator()->getIdentifier();  // w iterator
//         std::cout << "src:" << wItr << '\n';
//         //~ char* gVar = forAll->getSourceGraph()->getIdentifier();     //g variable
//         //~ std::cout<< "G:" << gVar << '\n';
//         char* nbrVar;

//         if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS) {
//           list<argument*> argList = extractElemFunc->getArgList();
//           assert(argList.size() == 1);
//           Identifier* nodeNbr = argList.front()->getExpr()->getId();
//           //~ targetFile.pushstr_newL("FOR begin | nbr iterate"); // ITERATE BFS
//           nbrVar = nodeNbr->getIdentifier();
//           //~ std::cout<< "nbr?:" <<  nbrVar<< '\n';

//           //~ sprintf(strBuffer, "for(unsigned i = d_meta[%s], end = d_meta[%s+1]; i < end; ++i)", nbrVar, nbrVar);
//           //~ targetFile.pushstr_newL(strBuffer);

//           //~ // HAS ALL THE STMTS IN FOR
//           //~ targetFile.pushstr_newL("{"); // uncomment after fixing NBR FOR brackets } issues.

//           //~ sprintf(strBuffer, "unsigned %s = d_data[i];", wItr);
//           //~ targetFile.pushstr_newL(strBuffer);

//           sprintf(strBuffer, "if(d_level[%s] == -1) {", wItr);
//           targetFile.pushstr_newL(strBuffer);
//           sprintf(strBuffer, "d_level[%s] = *d_hops_from_source + 1;", wItr);

//           targetFile.pushstr_newL(strBuffer);
//           targetFile.pushstr_newL("*d_finished = false;");
//           targetFile.pushstr_newL("}");

//           sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source + 1) {", wItr);
//           targetFile.pushstr_newL(strBuffer);

//           generateBlock((blockStatement*)forAll->getBody(), false, false);

//           targetFile.pushstr_newL("}");

//           targetFile.pushstr_newL("}");

//           // HAS ALL THE STMTS IN FOR
//           //~ targetFile.pushstr_newL("{");
//           //~ generateStatement(forAll->getBody(), false); //false. All these stmts should be inside kernel
//           //~ targetFile.pushstr_newL("}");

//           //~ sprintf(strBuffer, "if(bfsDist[%s]==bfsDist[%s]+1)",
//           //~ forAll->getIterator()->getIdentifier(),
//           //~ nodeNbr->getIdentifier());
//           //~ targetFile.pushstr_newL(strBuffer);
//           //~ targetFile.pushstr_newL("{");
//         }

//         /* This can be merged with above condition through or operator but
//          separating both now, for any possible individual construct updation.*/

//         else if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS) {  // ITERATE REV BFS
//           char strBuffer[1024];
//           list<argument*> argList = extractElemFunc->getArgList();
//           assert(argList.size() == 1);
//           Identifier* nodeNbr = argList.front()->getExpr()->getId();
//           nbrVar = nodeNbr->getIdentifier();
//           std::cout << "V?:" << nbrVar << '\n';
//           //~ sprintf(strBuffer, "for(int i = d_meta[%s], end = d_meta[%s+1]; i < end; ++i)", nbrVar, nbrVar);
//           //~ targetFile.pushstr_newL(strBuffer);

//           // HAS ALL THE STMTS IN FOR
//           //~ targetFile.pushstr_newL("{"); // uncomment after fixing NBR FOR brackets } issues.
//           //~ sprintf(strBuffer, "int %s = d_data[i];", wItr);
//           //~ targetFile.pushstr_newL(strBuffer);
//           sprintf(strBuffer, "if(d_level[%s] == *d_hops_from_source) {", wItr);
//           targetFile.pushstr_newL(strBuffer);
//           generateBlock((blockStatement*)forAll->getBody(), false, false);
//           targetFile.pushstr_newL("} // end IF  ");
//           targetFile.pushstr_newL("} // end FOR");
//           targetFile.pushstr_newL("grid.sync(); // Device-level syncs across all grids. Ref:https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#grid-synchronization-cg ");

//           //~ targetFile.pushstr_newL("FOR begin | nbr iterate");
//           //~ if(forAll->isForall() && forAll->hasFilterExpr()){
//           //~ std::cout<< "HAS FILTER?" << '\n';
//           //~ }
//           //~ Identifier* nodeNbr = argList.front()->getExpr()->getId();
//           //~ sprintf(strBuffer, "if(bfsDist[%s]==bfsDist[%s]+1)",
//           //~ forAll->getIterator()->getIdentifier(),
//           //~ nodeNbr->getIdentifier());
//           //~ targetFile.pushstr_newL(strBuffer);
//           //~ targetFile.pushstr_newL("{");
//         } 
        
//         else {
//           //~ std::cout<< "FOR BODY BEGIN" << '\n';
//           //~ targetFile.pushstr_newL("{ // FOR BEGIN ITR BEGIN");
//           generateStatement(forAll->getBody(), isMainFile);
//           targetFile.pushstr_newL("} //  end FOR NBR ITR. TMP FIX!");
//           std::cout << "FOR BODY END" << '\n';
//         }

//         //~ if (forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS ||
//         //~ forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS)
//         //~ targetFile.pushstr_newL("}");
//         //~ targetFile.pushstr_newL("}");

//       } 
      
//       else {
//         printf("FOR NORML");
//         generateStatement(forAll->getBody(), false);
//       }

//       if (forAll->isForall() && forAll->hasFilterExpr()) {
//         Expression* filterExpr = forAll->getfilterExpr();
//         generatefixedpt_filter(filterExpr, false);
//       }

//     } else {
//       if (collectionId->getSymbolInfo()->getType()->gettypeId() == TYPE_SETN) {  //FOR SET
//         if (body->getTypeofNode() == NODE_BLOCKSTMT) {
//           targetFile.pushstr_newL("{");  // uncomment after fixing NBR FOR brackets } issues.
//           //~ targetFile.pushstr_newL("//HERE");
//           printf("FOR");
//           sprintf(strBuffer, "int %s = *itr;", forAll->getIterator()->getIdentifier());
//           targetFile.pushstr_newL(strBuffer);
//           generateBlock((blockStatement*)body, false);  //FOR BODY for
//           targetFile.pushstr_newL("}");
//         } else
//           generateStatement(forAll->getBody(), false);

//       }
//        else if(collectionId->getSymbolInfo()->getType()->gettypeId()==TYPE_UPDATES)
//        {

//         if(body->getTypeofNode()==NODE_BLOCKSTMT)
//         main.pushstr_newL("{");
//         sprintf(strBuffer,"update %s = %s[i];",forAll->getIterator()->getIdentifier(),collectionId->getIdentifier()); 
//         main.pushstr_newL(strBuffer);
//         if(body->getTypeofNode()==NODE_BLOCKSTMT)
//         {
//           generateBlock((blockStatement*)body,false);
//           main.pushstr_newL("}");
//         }
//         else
//          generateStatement(forAll->getBody(), true);  //check once

//        }

//        else if(collectionId->getSymbolInfo()->getType()->gettypeId()==TYPE_CONTAINER){

//         if(body->getTypeofNode()==NODE_BLOCKSTMT){
//            generateBlock((blockStatement*)body,false);
//         }
//         else
//          generateStatement(forAll->getBody(),isMainFile);

//         main.pushstr_newL("}");
//      }
      
      
//        else {
//         //~ cout << iteratorMethodId->getIdentifier() << "\n";
//         generateStatement(forAll->getBody(), false);
//       }
//       if(forAll->isForall() && (forAll->hasFilterExpr() || forAll->hasFilterExprAssoc()))
//      { 
         
//        checkAndGenerateFixedPtFilter(forAll, isMainFile);
      
//      }
    
//     }

//     if(forAll->isForall()) {
//     parallelConstruct.pop_back();

//  if(forAll->getMapLocal().size() > 0) {

//     /* write logic here */

//     set<Identifier*>  containerId = forAll->getMapLocal();
//     auto it = containerId.begin();
//     Identifier* id = *it;
//     int start = 0;
//     generateForMergeContainer(id->getSymbolInfo()->getType(), start,isMainFile);
//     char val = 'k' + start + 1;
//     sprintf(strBuffer, "for(int %c = 0 ; %c < omp_get_max_threads() ; %c++)", val, val, val);
//     main.pushstr_newL(strBuffer);
//     generateInserts(id->getSymbolInfo()->getType(), id, isMainFile);

//  }   

// } 
// }


   
    
// } 

void dsl_dyn_cpp_generator::generateInDecHeader(Function* inDecFunc, bool isMainFile)
{
  dslCodePad& targetFile = isMainFile ? main : header;
  char temp[1024];
  //dslCodePad& targetFile = main;
  if(inDecFunc->getFuncType()==INCREMENTAL_FUNC)
   {
      // sprintf(temp,"%s_add",fileName);
      sprintf(temp,"Incremental");
   }
  else
   {
    //  sprintf(temp,"%s_del",fileName);
     sprintf(temp,"Decremental");
   }

  if(inDecFunc->containsReturn()) 
     targetFile.pushString("auto ");
  else
    targetFile.pushString("void ");
        
  targetFile.pushString(temp);
  targetFile.push('(');

  // generateParamList(inDecFunc->getParamList(), targetFile);
  list<formalParam*> paramList = inDecFunc->getParamList();
    int maximum_arginline = 4;
  int arg_currNo = 0;
  int argumentTotal = paramList.size();
  list<formalParam*>::iterator itr;
  for (itr = paramList.begin(); itr != paramList.end(); itr++) {
    arg_currNo++;
    argumentTotal--;

    Type* type = (*itr)->getType();
    //  if(!type->isGraphType()) {
    targetFile.pushString(convertToCppType(type));
    //  }
    targetFile.pushString(" ");

    if (type->isPropType()) {
      targetFile.pushString("d_");
    }
    //  if(!type->isGraphType()) {
    targetFile.pushString(/*createParamName(*/ (*itr)->getIdentifier()->getIdentifier());
    //  }

    

     if(type->isGraphType()){
        MetaDataUsed metadata = inDecFunc->getMetaDataUsed();

        /// TODO HOW TO FIGURE OUT THE CONTENTS??
        metadata.isDataUsed = true;
        metadata.isMetaUsed = true;
        metadata.isSrcUsed = true;
        metadata.isRevMetaUsed = true;
        metadata.isWeightUsed = true;

        targetFile.pushString(", int V,int E");
        if(metadata.isMetaUsed){
          targetFile.pushString(",int* d_meta");
        } 
        if(metadata.isDataUsed){
          targetFile.pushString(",int* d_data");
        }
        if(metadata.isRevMetaUsed){
          targetFile.pushString(",int* d_rev_meta");
        }

        if(metadata.isSrcUsed){
          targetFile.pushString(",int* d_src");
        }
        if(metadata.isWeightUsed){
          targetFile.pushString(",int* d_weight");
        }
     }
    if (argumentTotal > 0)
      targetFile.pushString(", ");

    if (arg_currNo == maximum_arginline) {
      targetFile.NewLine();
      arg_currNo = 0;
    }
  }
  

   targetFile.pushString(")");
   targetFile.NewLine();

    return; 

}



void dsl_dyn_cpp_generator::generateDynamicHeader(Function* dynFunc, bool isMainFile)
{

  dslCodePad& targetFile = isMainFile ? main : header;
  char temp[1024];
  //dslCodePad& targetFile = main;
 
 sprintf(temp,"%s",dynFunc->getIdentifier()->getIdentifier());
  
 if(dynFunc->containsReturn()) 
     targetFile.pushString("auto ");
  else
    targetFile.pushString("void ");
        
  targetFile.pushString(temp);
  targetFile.push('(');

  generateParamList(dynFunc->getParamList(), targetFile);
           list<formalParam*> paramList = dynFunc->getParamList();
      list<formalParam*>::iterator itr;
      for (itr = paramList.begin(); itr != paramList.end(); itr++) {
                  Type* type = (*itr)->getType();
      char* parName = (*itr)->getIdentifier()->getIdentifier();

      if (type->isPrimitiveType()) {
        char strBuffer[1024];

        sprintf(strBuffer, "__device__ %s %s ;", convertToCppType(type), parName);
        header.pushString(strBuffer);
        header.NewLine();
      }
        }
   targetFile.pushString(")");
   targetFile.NewLine();

    return; 


}

void dsl_dyn_cpp_generator::generateIncrementalDecremental(Function* incFunc, bool isMainFile)
{
  printf("inside incremental block-------------------");

   dslCodePad& targetFile = isMainFile ? main : header;
   char strBuffer[1024];
   curFuncType = incFunc->getFuncType();
   currentFunc = incFunc;
         list<formalParam*> paramList = incFunc->getParamList();
      list<formalParam*>::iterator itr;
      for (itr = paramList.begin(); itr != paramList.end(); itr++) {
                  Type* type = (*itr)->getType();
      char* parName = (*itr)->getIdentifier()->getIdentifier();

      if (type->isPrimitiveType()) {
        char strBuffer[1024];

        sprintf(strBuffer, "__device__ %s %s ;", convertToCppType(type), parName);
        header.pushString(strBuffer);
        header.NewLine();
      }
        }
   generateInDecHeader(incFunc, isMainFile);
   main.pushstr_newL("{");
    generateLaunchConfig("nodes");
    generateBlock(incFunc->getBlockStatement(),false,true);
   main.NewLine();
   main.pushstr_newL("}");
   




   incFuncCount(incFunc->getFuncType());

   return;

}

// void dsl_dyn_cpp_generator::generateDecremental(Function* decFunc, bool isMainFile)
// {

//    dslCodePad& targetFile = isMainFile ? main : header;
//    char strBuffer[1024];
//    curFuncType = decFunc->getFuncType();
//    currentFunc = decFunc;
//    generateInDecHeader(decFunc, true);

//    main.pushstr_newL("{");

//    if(decFunc->getInitialLockDecl())
//        {
//          vector<Identifier*> graphVar = graphId[curFuncType][curFuncCount()]; 
//          sprintf(strBuffer,"omp_lock_t* lock = (omp_lock_t*)malloc(%s.num_nodes()*sizeof(omp_lock_t));",graphVar[0]->getIdentifier());
//          main.pushstr_newL(strBuffer);
//          main.NewLine();
//          sprintf(strBuffer,"for(%s %s = %s; %s<%s.%s(); %s++)","int","v","0","v",graphVar[0]->getIdentifier(),"num_nodes","v");
//          main.pushstr_newL(strBuffer);
//          sprintf(strBuffer,"omp_init_lock(&lock[%s]);","v");
//          main.space();
//          main.space();
//          main.pushstr_newL(strBuffer);
//          main.NewLine();

//        }

//    generateBlock(decFunc->getBlockStatement(),false);
//    main.NewLine();
//    main.pushstr_newL("}");
//    incFuncCount(decFunc->getFuncType());
//    return;

// }

void dsl_dyn_cpp_generator::generateDynamicFunc(Function* dynFunc, bool isMainFile)
{

   
   char strBuffer[1024];
   curFuncType = dynFunc->getFuncType();
   currentFunc = dynFunc;
   generateDynamicHeader(dynFunc, true);

   main.pushstr_newL("{");
  generateBatchInitializer();
  //  if(dynFunc->getInitialLockDecl())
  //      {
  //        vector<Identifier*> graphVar = graphId[curFuncType][curFuncCount()]; 
  //        sprintf(strBuffer,"omp_lock_t* lock = (omp_lock_t*)malloc(%s.num_nodes()*sizeof(omp_lock_t));",graphVar[0]->getIdentifier());
  //        main.pushstr_newL(strBuffer);
  //        main.NewLine();
  //        sprintf(strBuffer,"for(%s %s = %s; %s<%s.%s(); %s++)","int","v","0","v",graphVar[0]->getIdentifier(),"num_nodes","v");
  //        main.pushstr_newL(strBuffer);
  //        sprintf(strBuffer,"omp_init_lock(&lock[%s]);","v");
  //        main.space();
  //        main.space();
  //        main.pushstr_newL(strBuffer);
  //        main.NewLine();

  //      }
  //  generatePriorDeclarations(dynFunc, isMainFile);
   generateBlock(dynFunc->getBlockStatement(),false,true);
   main.NewLine();
   generateBatchFinalizer();
   main.pushstr_newL("}");
   incFuncCount(dynFunc->getFuncType());
   return;


}

void dsl_dyn_cpp_generator::generateFunction(ASTNode* func)
{

  //dslCodePad& targetFile = isMainFile ? main : header;
  Function* function = (Function*)func;
  if(function->getFuncType() == STATIC_FUNC)
     {
       generateFunc(function);
       
     }
   else if(function->getFuncType() == INCREMENTAL_FUNC )
   {
        // function->functionId = Identifier::createIdNode(fileName);
        generateIncrementalDecremental(function, true);
   }
   else if(function->getFuncType() == DECREMENTAL_FUNC)
       {
         generateIncrementalDecremental(function, true);
           
       }
   else if(function->getFuncType() == DYNAMIC_FUNC)
        {      
          generateDynamicFunc(function, true);
           
        }
   else if(function->getFuncType() == GEN_FUNC)
         {
            generateFunc(function);
         }         
}

bool dsl_dyn_cpp_generator::openFileforOutput()
{  

  char temp[1024];
  printf("fileName %s\n",fileName); 

  sprintf(temp, "%s/%s_dyn.h", "../graphcode/generated_cuda", fileName);
  headerFile = fopen(temp, "w");
  if (headerFile == NULL) return false;
  header.setOutputFile(headerFile);

  sprintf(temp,"%s/%s_dyn.cu","../graphcode/generated_cuda",fileName);
  bodyFile=fopen(temp,"w"); 
  if(bodyFile==NULL)
     return false;
  main.setOutputFile(bodyFile);     
  
  return true;

}

void dsl_dyn_cpp_generator::closeOutputFile() {
  if (headerFile != NULL) {
    header.outputToFile();
    fclose(headerFile);
  }
  headerFile = NULL;

  if (bodyFile != NULL) {
    main.outputToFile();
    fclose(bodyFile);
  }

  bodyFile = NULL;
}

void dsl_dyn_cpp_generator::generation_begin()
{ 
  char temp[1024];
  header.pushstr_newL("// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after");
  main.pushstr_newL("// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after");
  header.pushString("#ifndef GENCPP_");
  header.pushUpper(fileName);
  header.pushstr_newL("_H");
  header.pushString("#define GENCPP_");
  header.pushUpper(fileName);
  header.pushstr_newL("_H");
  header.pushString("#include ");
  addIncludeToFile("stdio.h", header, true);
  header.pushString("#include ");
  addIncludeToFile("stdlib.h", header, true);
  header.pushString("#include ");
  addIncludeToFile("limits.h", header, true);
  header.pushString("#include ");
  addIncludeToFile("cuda.h", header, true);
  header.pushString("#include ");
  addIncludeToFile("../graph.hpp", header, false);
  header.pushString("#include ");
  addIncludeToFile("../libcuda.cuh", header, false);

  header.pushstr_newL("#include <cooperative_groups.h>");
  //header.pushstr_newL("graph &g = NULL;");  //temporary fix - to fix the PageRank graph g instance

  header.NewLine();

  main.pushString("#include ");
  sprintf(temp, "%s.h", fileName);
  addIncludeToFile(temp, main, false);
  main.NewLine();

}

bool dsl_dyn_cpp_generator::generate()
{  
  // cout<<"FRONTEND VALUES"<<frontEndContext.getFuncList().front()->getBlockStatement()->returnStatements().size();    //openFileforOutput();
   if(!openFileforOutput())
      return false;
   generation_begin(); 
   
   list<Function*> funcList=frontEndContext.getFuncList();
   for(Function* func:funcList)
   {
      dsl_cpp_generator::setCurrentFunc(func);
       generateFunction(func);

   }
   

   closeOutputFile();

   return true;

}

void dsl_dyn_cpp_generator::addOnAddkernel(onAddBlock* onAddStmt,bool isMainFile,list<Identifier*> vars){
  const char* updateVar = onAddStmt->getIteratorId()->getIdentifier();
  const char* batchVar = onAddStmt->getUpdateId()->getIdentifier();
  char strBuffer[1024];

  Function* currentFunc = getCurrentFunc();
  if(currentFunc==NULL) std::cout<<"NULL CURRENT FUNC\n";
  // usedVariables usedVars = getVarsBlock(onAddStmt->getStatements());

 
  header.pushString("__global__ void ");
  // header.pushString(getCurrentFunc()->getIdentifier()->getIdentifier());
  header.pushString("OnAdd_kernel");

  header.pushString("(update* d_");
  header.pushString(batchVar);
  header.pushString(", int batchelements");
   std::cout<<"OnADD KERNEL METADATA HEADERS\n";
   if(onAddStmt->getMetaDataUsed()==NULL)  std::cout<<"METADATA NULL\n";
   MetaDataUsed *m = onAddStmt->getMetaDataUsed();
  if(m->isMetaUsed)
    header.pushString(", int* d_meta");
  if(m->isDataUsed)
    header.pushString(", int* d_data");
  if(m->isSrcUsed)
    header.pushString(", int* d_src");
  if(m->isWeightUsed)
    header.pushString(", int* d_weight");
  if(m->isRevMetaUsed)
    header.pushString(", int *d_rev_meta");
   std::cout<<"OnADD KERNEL After Normal HEADERS\n";
  for (Identifier* iden : vars) {
    std::cout<<"INSIDE LOOP 0 "<<iden->getIdentifier()<<"\n";
    if(iden->getSymbolInfo()==NULL) continue;
    Type* type = iden->getSymbolInfo()->getType();
    std::cout<<"INSIDE LOOP 1\n";
    if (type->isPropType()) {
      std::cout<<"INSIDE LOOP 2\n";
      char strBuffer[1024];
      sprintf(strBuffer, ",%s d_%s", convertToCppType(type), iden->getIdentifier());
      std::cout<<"INSIDE LOOP 3\n";
      header.pushString(/*createParamName(*/ strBuffer);
      if(iden->getSymbolInfo()->getId()->get_fp_association()) { // If id has a fp association then _next must also be added as a parameter
          std::cout<<"INSIDE LOOP 4\n";
        sprintf(strBuffer, ",%s d_%s_next", convertToCppType(type), iden->getIdentifier());
        header.pushString(/*createParamName(*/ strBuffer);
      }
    } else {

      std::cout<<"vars not property:"<<iden->getIdentifier()<<"\n";
    }
  }




  header.pushstr_newL("){ // BEGIN KER FUN via ADDKERNEL");
  header.pushstr_newL("unsigned tid = blockIdx.x * blockDim.x + threadIdx.x;");
  header.pushstr_newL("if(tid >= batchelements) return;");
  sprintf(strBuffer,"update %s = d_%s[tid];",updateVar,batchVar );
  header.pushstr_newL(strBuffer);
  sprintf(strBuffer,"if( %s.type!='a') return;",updateVar);
  header.pushstr_newL(strBuffer);
  for (statement* stmt : (onAddStmt->getStatements())->returnStatements()) {
    generateStatement(stmt, false);  //false. All these stmts should be inside kernel
                                     //~ if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
  }

  header.pushstr_newL("} // end KER FUNC");
}
void dsl_dyn_cpp_generator::generateBatchInitializer(){
  const char* graph ; graph = "g"; // TODO 
  char strBuffer[1024];
  sprintf(strBuffer," int V = %s.num_nodes();",graph);
  main.pushstr_newL(strBuffer);

  sprintf(strBuffer," int E = %s.num_edges();",graph);
  main.pushstr_newL(strBuffer);
  
  sprintf(strBuffer,"int* edgeLen = %s.getEdgeLen();",graph);
  main.pushstr_newL(strBuffer);


  
  main.pushstr_newL("int *h_meta;");
  main.pushstr_newL("int *h_data;");
  main.pushstr_newL("int *h_src;");
  main.pushstr_newL("int *h_weight;");
  main.pushstr_newL("int *h_rev_meta;");
  
    
  main.pushstr_newL("h_meta = (int *)malloc( (V+1)*sizeof(int));");
  main.pushstr_newL("h_data = (int *)malloc( (E)*sizeof(int));");
  main.pushstr_newL("h_src = (int *)malloc( (E)*sizeof(int));");
  main.pushstr_newL("h_weight = (int *)malloc( (E)*sizeof(int));");
  main.pushstr_newL("h_rev_meta = (int *)malloc( (V+1)*sizeof(int));");


  main.pushstr_newL("for(int i=0; i<= V; i++) {");
  main.pushstr_newL("   int temp;");
  sprintf(strBuffer,"  temp = %s.indexofNodes[i];",graph);
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("  h_meta[i] = temp;");
  sprintf(strBuffer," temp = %s.rev_indexofNodes[i];",graph);
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("  h_rev_meta[i] = temp;");
  main.pushstr_newL("}");


  main.pushstr_newL(" for(int i=0; i< E; i++) {");
  main.pushstr_newL(" int temp;");
  sprintf(strBuffer,"temp = %s.edgeList[i];",graph);
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("  h_data[i] = temp;");
  sprintf(strBuffer,"  temp = %s.srcList[i];",graph);
  main.pushstr_newL(strBuffer);
  main.pushstr_newL("  h_src[i] = temp;");
  main.pushstr_newL("  temp = edgeLen[i];");
  main.pushstr_newL("h_weight[i] = temp;");
  main.pushstr_newL("}");

  main.pushstr_newL("int *d_meta;");
  main.pushstr_newL("int *d_data;");
  main.pushstr_newL("int *d_src;");
  main.pushstr_newL("int *d_weight;");
  main.pushstr_newL("int *d_rev_meta;");

  main.pushstr_newL("cudaMalloc(&d_meta,sizeof(int)*(V+1));");
  main.pushstr_newL("cudaMalloc(&d_data,sizeof(int)*(E));");
  main.pushstr_newL("cudaMalloc(&d_src,sizeof(int)*(E));");
  main.pushstr_newL("cudaMalloc(&d_weight,sizeof(int)*(E));");
  main.pushstr_newL("cudaMalloc(&d_rev_meta,sizeof(int)*(V+1));");


  main.pushstr_newL("cudaMemcpy(d_meta,h_meta,sizeof(int)*(V+1),cudaMemcpyHostToDevice);");
  main.pushstr_newL("cudaMemcpy(d_data,h_data,sizeof(int)*(E),cudaMemcpyHostToDevice);");
  main.pushstr_newL("cudaMemcpy(d_src,h_src,sizeof(int)*(E),cudaMemcpyHostToDevice);");
  main.pushstr_newL("cudaMemcpy(d_weight,h_weight,sizeof(int)*(E),cudaMemcpyHostToDevice);");
  main.pushstr_newL("cudaMemcpy(d_rev_meta,h_rev_meta,sizeof(int)*(V+1),cudaMemcpyHostToDevice);");

  main.pushstr_newL("unsigned threadsPerBlock   = (V < THREADS_PER_BLOCK)? V: THREADS_PER_BLOCK;");
  main.pushstr_newL("unsigned numBlocks    = (V+threadsPerBlock-1)/threadsPerBlock;");
}
void dsl_dyn_cpp_generator::generateBatchFinalizer(){
  main.pushstr_newL("cudaMemcpy(h_meta,d_meta,sizeof(int)*(V+1),cudaMemcpyDeviceToHost);");
  main.pushstr_newL("cudaMemcpy(h_data,d_data,sizeof(int)*(E),cudaMemcpyDeviceToHost);");
  main.pushstr_newL("cudaMemcpy(h_src,d_src,sizeof(int)*(E),cudaMemcpyDeviceToHost);");
  main.pushstr_newL("cudaMemcpy(h_weight,d_weight,sizeof(int)*(E),cudaMemcpyDeviceToHost);");
  main.pushstr_newL("cudaMemcpy(h_rev_meta,d_rev_meta,sizeof(int)*(V+1),cudaMemcpyDeviceToHost);");
  
  main.pushstr_newL("cudaFree(d_meta);");
  main.pushstr_newL("cudaFree(d_data);");
  main.pushstr_newL("cudaFree(d_src);");
  main.pushstr_newL("cudaFree(d_weight);");
  main.pushstr_newL("cudaFree(d_rev_meta);");
}

}
