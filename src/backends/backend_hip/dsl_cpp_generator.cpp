/**
 * Implementation of Code Generator for AMD HIP Backend.
 * Implements dsl_cpp_generator.h
 * 
 * @author cs22m056
*/

#include <cctype>
#include <sstream>
#include <stdexcept>

#include "../../ast/ASTHelper.cpp"
#include "dsl_cpp_generator.h"
#include "getUsedVars.cpp"

// #define HIT_CHECK std::std::cout << "Hit at line " << __LINE__ << " of function " << __func__ << " in file " << __FILE__ << "\n";

string forAllSrcVariable = "";
string forAllNbrVariable = ""; 

namespace sphip {

    DslCppGenerator::DslCppGenerator(const std::string& fileName, const int threadsPerBlock) : 
        fileName(StripName(fileName)), 
        HEADER_GUARD("GENHIP_" + ToUpper(StripName(fileName)) + "_H"),
        threadsPerBlock(threadsPerBlock) {

        generateCsr = false;

        if(threadsPerBlock > 1024)
            throw std::runtime_error("Threads per block should be less than 1024");
    }   

    bool DslCppGenerator::Generate() {

        if(!OpenOutputFile()) return false;

        GenerateIncludeFiles();

        list<Function*> funcList = frontEndContext.getFuncList();
        for (Function* func : funcList) {
            SetCurrentFunction(func);
            GenerateFunction(func);
        }

        if(generateInitIndex)
            GenerateInitIndex();
        if(generateInitKernel)
            GenerateInitKernel();

        GenerateEndOfFile(); 
        CloseOutputFile();  

        return true;
    }

    void DslCppGenerator::GenerateIncludeFiles() {

        // TODO: Add meta comments for both header and main files

        header.pushStringWithNewLine("#ifndef " + HEADER_GUARD);
        header.pushStringWithNewLine("#define " + HEADER_GUARD);
        header.NewLine();

        header.addHeaderFile("iostream", true);
        header.addHeaderFile("climits", true);

        header.addHeaderFile("hip/hip_runtime.h", true);
        header.addHeaderFile("hip/hip_cooperative_groups.h", true); // TODO
        header.addHeaderFile("../graph.hpp", false);
        header.NewLine();

        main.addHeaderFile(this->fileName + ".h", false);
        main.NewLine();
        main.NewLine();
    }

    void DslCppGenerator::GenerateEndOfFile() {

        header.NewLine();
        header.pushStringWithNewLine("#endif");
    }

    void DslCppGenerator::GenerateFunction(Function* func) {

        GenerateFunctionHeader(func, false);
        GenerateFunctionHeader(func, true);

        main.pushStringWithNewLine("{");

        GenerateFunctionBody(func);
        main.NewLine();

        // GenerateTimerStart();
        // GenerateHipMallocParams(func->getParamList()); //TODO: impl
        GenerateBlock(func->getBlockStatement(), false);
        
        // GenerateTimerStop();

        GenerateHipMemcpyParams(func->getParamList());

        main.pushStringWithNewLine("}");
    }

    void DslCppGenerator::GenerateFunctionHeader(Function* func, bool isMainFile) {

        dslCodePad &targetFile = isMainFile ? main:header;

        // TODO: Add function to increment indentation

        if(func->containsReturn()){
            targetFile.pushString("auto");
        }
        else{
            targetFile.pushString("void");
        }
        targetFile.AddSpace();
        std::cout << func->getIdentifier()->getIdentifier() << std::endl;
        targetFile.pushString(func->getIdentifier()->getIdentifier());
        targetFile.pushString("(");
        targetFile.NewLine();
        // TODO: Add indent by "\t"

        /* Adding function parameters*/

        list<formalParam*> parameterList = func->getParamList();

        int arg_currNo = 0;
        int argumentTotal = func->getParamList().size();
        int maximum_arginline = 4;

        for(auto itr = parameterList.begin(); itr != parameterList.end(); itr++) {
            arg_currNo++;
            argumentTotal--;
            Type *type = (*itr)->getType();
            targetFile.pushString(ConvertToCppType(type, false, true)); // TODO: add function in header and impl
            targetFile.AddSpace();

            char* parName = (*itr)->getIdentifier()->getIdentifier();

            targetFile.pushString(" ");

            if (!isMainFile) {
                if (type->isGraphType()) {
                    generateCsr = true;
                }
            }
            targetFile.pushString(/*createParamName(*/ parName);
            if (argumentTotal > 0) targetFile.pushString(",");

            if (arg_currNo == maximum_arginline) {
                targetFile.NewLine();
                arg_currNo = 0;
            }
        }

        //TODO: Remove indent
        targetFile.pushString(")");
        // if(!isMainFile)
        //     targetFile.pushString(";");

        if (!isMainFile) {
            targetFile.pushString(";");
            targetFile.NewLine();
            targetFile.NewLine();

            for (auto itr = parameterList.begin(); itr != parameterList.end(); itr++) {
                Type* type = (*itr)->getType();
                char* parName = (*itr)->getIdentifier()->getIdentifier();

                if (type->isPrimitiveType()) {
                    char strBuffer[1024];

                    std::sprintf(strBuffer, "__device__ %s %s ;", ConvertToCppType(type).c_str(), parName);
                    // targetFile.pushString(strBuffer);
                    // targetFile.NewLine();
                }
            }
        }

        targetFile.NewLine();
    }

    void DslCppGenerator::GenerateFunctionBody(Function* func) {

        list<formalParam*> parameterList = func->getParamList();
        std::string graphId;

        for(auto itr = parameterList.begin(); itr != parameterList.end(); itr++) {

            Type *type = (*itr)->getType();
            const std::string parameterName = (*itr)->getIdentifier()->getIdentifier();

            if(type->isGraphType()) {
             
                generateCsr = true;
                graphId = parameterName;
            }
        }

        if(generateCsr) {

            GenerateCsrArrays(graphId, func);
            main.NewLine();
        }

        CheckAndGenerateVariables(func, "d");
        CheckAndGenerateHipMalloc(func);
        CheckAndGenerateMemcpy(func);
        // GenerateLaunchConfiguration();
    }

    void DslCppGenerator::SetCurrentFunction(Function* func) {

        this->function = func;
    }

    Function* DslCppGenerator::GetCurrentFunction() {
        return this->function;
    }

    void DslCppGenerator::GenerateCsrArrays(const std::string &graphId, Function *func) {

        main.pushStringWithNewLine("int V = " + graphId + ".num_nodes();");
        main.pushStringWithNewLine("int E = " + graphId + ".num_edges();");

        main.NewLine();

        if(func->getIsWeightUsed())
            main.pushStringWithNewLine("int *edgeLens = " + graphId + ".getEdgeLen();");

        main.NewLine();

        CheckAndGenerateVariables(func, "h");
        CheckAndGenerateMalloc(func);

        if(func->getIsMetaUsed() || func->getIsRevMetaUsed()) {

            main.pushStringWithNewLine(
                "for(int i = 0; i <= V; i++) {"
            );

            if(func->getIsMetaUsed())
                main.pushStringWithNewLine(
                    "hOffsetArray[i] = " + graphId + ".indexofNodes[i];"
                );

            if(func->getIsRevMetaUsed())
                main.pushStringWithNewLine(
                    "hRevOffsetArray[i] = " + graphId + ".rev_indexofNodes[i];"
                );

            main.pushStringWithNewLine("}");
        }

        if(
            func->getIsDataUsed() ||
            func->getIsSrcUsed()  ||
            func->getIsWeightUsed()
        ) {

            main.pushStringWithNewLine(
                "for(int i = 0; i < E; i++) {"
            );

            if(func->getIsDataUsed()) 
                main.pushStringWithNewLine(
                    "hEdgelist[i] = " + graphId + ".edgeList[i];"
                );

            if(func->getIsSrcUsed()) 
                main.pushStringWithNewLine(
                    "hSrcList[i] = " + graphId + ".srcList[i];"
                );

            if(func->getIsWeightUsed()) 
                main.pushStringWithNewLine(
                    "hWeight[i] = edgeLens[i];"
                );

            main.pushStringWithNewLine("}");
        }

        main.NewLine();
    }

    void DslCppGenerator::GenerateStatement(statement* stmt, bool isMainFile) {
        dslCodePad &targetFile = isMainFile ? main : header;
        switch (stmt->getTypeofNode()) {
            

        case NODE_BLOCKSTMT:
            GenerateBlock(static_cast<blockStatement*>(stmt), false, isMainFile);
            break;

        case NODE_DECL:
            GenerateVariableDeclaration(static_cast<declaration*>(stmt), isMainFile);
            break;

        case NODE_ASSIGN:
            {
                assignment *asst = static_cast<assignment*>(stmt);
                std::cout<<"ASSIGNMENT EXPR "<<asst->isDeviceAssignment()<<endl;
                if(asst->isDeviceAssignment())
                    GenerateDeviceAssignment(asst, isMainFile);
                else
                    GenerateAtomicOrNormalAssignment(asst, isMainFile);

                break;
            }

        case NODE_IFSTMT:
            GenerateIfStmt(static_cast<ifStmt*>(stmt), isMainFile);
            break;

        case NODE_FORALLSTMT:
            GenerateForAll(static_cast<forallStmt*>(stmt), isMainFile);
            break;

        case NODE_FIXEDPTSTMT:
            GenerateFixedPoint(static_cast<fixedPointStmt*>(stmt), isMainFile);
            break;

        case NODE_REDUCTIONCALLSTMT:
            GenerateReductionStmt(static_cast<reductionCallStmt*>(stmt), isMainFile);
            break;

        case NODE_RETURN:
        {
            returnStmt* returnStmtNode = (returnStmt*)stmt;
            main.pushstr_space("return");
            GenerateExpression(returnStmtNode->getReturnExpression(), isMainFile);
            main.pushstr_newL(";");
            break;
        }

        case NODE_ITRBFS:
            GenerateItrBfs(static_cast<iterateBFS*>(stmt), isMainFile);
            break;

        case NODE_WHILESTMT:
            GenerateWhileStmt((whileStmt*)stmt, isMainFile);
            break;

        // case NODE_ITRBFS2:
        //     GenerateItrBfs2(static_cast<iterateBFS2*>(stmt), isMainFile);
        //     break;

        case NODE_ITRRBFS:
            std::cout << "DOES IT HIT ITRTBFS\n";
            // GenerateItrRevBfs(static_cast<iterateReverseBFS*>(stmt), isMainFile);
            break;

        // case NODE_ITERBFSREV:
        //     GenerateItrBfsRev(static_cast<iterateBFSReverse*>(stmt), isMainFile);
        //     break;

        case NODE_PROCCALLSTMT:
            GenerateProcCallStmt(static_cast<proc_callStmt*>(stmt), isMainFile);
            break;

        case NODE_UNARYSTMT:
            {
                unary_stmt* unaryStmt = (unary_stmt*)stmt;
                GenerateExpression(unaryStmt->getUnaryExpr(), isMainFile);
                break;
            }

        case NODE_DOWHILESTMT:
            GenerateDoWhileStmt((dowhileStmt*)stmt, isMainFile);
            break;
        
        default:
            string temp = "Generation function not implemented for this node! and type is " + (int)stmt->getTypeofNode();
            throw std::runtime_error(temp);
            break;
        }
    }

    void DslCppGenerator::GenerateBlock(
        blockStatement* blockStmt,
        bool includeBrace,
        bool isMainFile
    ) {
        dslCodePad &targetFile = isMainFile ? main : header;

        //TODO: Used variables thingy
        
        list<statement*> stmtList = blockStmt->returnStatements();

        if(includeBrace)
            targetFile.pushStringWithNewLine("{");
        for(auto itr = stmtList.begin(); itr != stmtList.end(); itr++)
            GenerateStatement(*itr, isMainFile);
        usedVariables usedVars = GetDeclaredPropertyVarsOfBlock(blockStmt);
        list<Identifier*> vars = usedVars.getVariables();

        // HIP FREE
        char strBuffer[1024];

        if (vars.size() > 0) {
            targetFile.NewLine();
            targetFile.pushstr_newL("//hipFree up!! all propVars in this BLOCK!");
        }
        for (Identifier* iden : vars) {
            std::sprintf(strBuffer, "hipFree(d%s);", CapitalizeFirstLetter(iden->getIdentifier()));
            targetFile.pushstr_newL(strBuffer);
        }
        targetFile.NewLine();

        if(includeBrace)
            targetFile.pushStringWithNewLine("}");

        main.NewLine();
    }

    std::string DslCppGenerator::ConvertToCppType(Type* type, bool isMemcpy, bool isParameter) {
        if (type->isPrimitiveType()) {
            int typeId = type->gettypeId();
            switch (typeId) {
            case TYPE_INT:
                return "int";
            case TYPE_BOOL:
                return "bool";
            case TYPE_LONG:
                return "long";
            case TYPE_FLOAT:
                return "float";
            case TYPE_DOUBLE:
                return "double";
            case TYPE_NODE:
                return "int";
            case TYPE_EDGE:
                return "int";
            default:
                assert(false);
            }
        } else if (type->isPropType()) {
            Type* targetType = type->getInnerTargetType();
            if (targetType->isPrimitiveType()) {
            int typeId = targetType->gettypeId();
            //~ cout << "TYPEID IN CPP" << typeId << "\n";
            switch (typeId) {
                case TYPE_INT:
                return "int*";
                case TYPE_BOOL:
                return "bool*";
                case TYPE_LONG:
                return "long*";
                case TYPE_FLOAT:
                return "float*";
                case TYPE_DOUBLE:
                return "double*";
                default:
                assert(false);
            }
            }
        } else if (type->isNodeEdgeType()) {
            return "int";  // need to be modified.

        } else if (type->isGraphType()) {
            return "graph&";
        } else if (type->isCollectionType()) {
            int typeId = type->gettypeId();

            switch (typeId) {
            case TYPE_SETN:
                return "std::set<int>&";

            case TYPE_CONTAINER:
                {
                char* newS = new char[1024];
                string vecString = "std::vector<";   

                string valType = ConvertToCppType(type->getInnerTargetType());
                string innerString = valType;
                vecString = vecString + innerString;
                vecString = vecString + ">";

                if(isMemcpy){ vecString = innerString + "*";}

                if(isParameter) {vecString = vecString + "&";}

                copy(vecString.begin(), vecString.end(), newS);
                newS[vecString.size()] = '\0';
                return newS; 

                }
            default:
                assert(false);
            }
        }

        return "NA";
        }

    void DslCppGenerator::GenerateVariableDeclaration(declaration* stmt, bool isMainFile) {

        dslCodePad &targetFile = isMainFile ? main : header;

        Type *type = stmt->getType();
        if(type->isPropType()) {

            if(type->getInnerTargetType()->isPrimitiveType()) {

                Type *innerType = type->getInnerTargetType();

                main.pushString(ConvertToCppType(innerType));
                main.AddSpace();
                main.pushString("*");
                main.pushString("d");

                std::string idName = stmt->getdeclId()->getIdentifier();
                idName[0] = std::toupper(idName[0]);

                main.pushString(idName);
                main.pushStringWithNewLine(";");

                GenerateHipMalloc(type, idName);

                if(stmt->getdeclId()->getSymbolInfo()->getId()->require_redecl()) {

                    main.pushString(ConvertToCppType(innerType));
                    main.AddSpace();
                    main.pushString("*");
                    main.pushString("d");
                    main.pushString(idName);
                    main.pushString("Next");
                    main.pushString(";");

                    GenerateHipMalloc(type, idName + "Next");
                }
            }
        } else if(type->isNodeEdgeType()) {
            if(GenerateVariableDeclGetEdge(stmt, isMainFile))  //if decl stmt has been given assigned get_edge then return.
                return;
            targetFile.pushString(ConvertToCppType(type));
            targetFile.AddSpace();
            targetFile.pushString(stmt->getdeclId()->getIdentifier());

            if(stmt->isInitialized()) {

                targetFile.pushString(" = ");
                GenerateExpression(stmt->getExpressionAssigned(), isMainFile); // TODO
                
            }
            targetFile.pushStringWithNewLine(";");
        
        } else if(type->isPrimitiveType()) {
            char strBuffer[1024];
            const char* varType = ConvertToCppType(type).c_str();
            const char* varName = stmt->getdeclId()->getIdentifier();

            bool declInHeader = !isMainFile;  // if variable is declared in header file, to stop generating unnecessary commas and newline

            if (isMainFile == true) {
                std::sprintf(strBuffer, "__device__ %s %s; ", varType, varName);
                // header.pushString(strBuffer);
                declInHeader = true;
            }
            /// REPLICATE ON HOST AND DEVICE
            std::sprintf(strBuffer, "%s %s", varType, varName);
            targetFile.pushString(strBuffer);

            if (stmt->isInitialized()) {
                targetFile.pushString(" = ");
                if (stmt->getExpressionAssigned()->getExpressionFamily() == EXPR_PROCCALL) {
                    proc_callExpr* pExpr = (proc_callExpr*)stmt->getExpressionAssigned();
                    Identifier* methodId = pExpr->getMethodId();
                    CastIfRequired(type, methodId, main);
                }
                GenerateExpression(stmt->getExpressionAssigned(), isMainFile);  // PRINTS RHS? YES
            }

            targetFile.pushString(";  ");

            if(declInHeader) {
                header.pushstr_newL("// DEVICE ASSTMENT in .h");
            }

        }

        else if(type->isCollectionType())
         {
           if(type->gettypeId() == TYPE_UPDATES)
             {
                main.pushString(ConvertToCppType(type));
                main.push(' ');
                main.pushString(stmt->getdeclId()->getIdentifier());
                if(stmt->isInitialized())
                   {
                      main.pushString(" = ");
                      GenerateExpression(stmt->getExpressionAssigned(), true, false);
                     
                   } 
                  main.pushstr_newL(";");
   

                // if(insideBatchBlock)
                //    freeIdStore.back().push_back(stmt->getdeclId());   
             }
            if(type->gettypeId() == TYPE_NODEMAP){
               
                main.pushString(ConvertToCppType(type));
                main.push(' ');
                main.pushString(stmt->getdeclId()->getIdentifier());
                main.pushstr_newL(";");
           }
           if(type->gettypeId() == TYPE_CONTAINER){
             
              main.pushString(ConvertToCppType(type));
              main.push(' ');
              main.pushString(stmt->getdeclId()->getIdentifier());

              if(type->isInitializeType()){
                string temp = "";
                temp = temp + "(" + std::string(type->getId()->getIdentifier()) + ", " + to_string(type->getVal()) + ")";
                main.pushString(temp);
              }

              if(type->getArgList().size() !=0){
                 
                 list<argument*> args = type->getArgList();
                 Expression* expr = args.front()->getExpr();
                 main.pushString("(");
                 GenerateExpression(expr, isMainFile);

              // if(type->getInnerTargetSize() != NULL)
              //    generateNestedContainer(type->getInnerTargetSize());
              if(type->getArgList().size() == 2) {
                 main.pushstr_space(",");
                 GenerateExpression(args.back()->getExpr(), isMainFile);
               }   

                 main.pushString(")");

              }

              main.pushstr_newL(";");

              if(stmt->getdeclId()->getSymbolInfo()->getId()->isLocalMapReq()){

                 main.pushString("std::vector<");
                 main.pushString(ConvertToCppType(type));
                 main.pushstr_space(">");

                 main.pushString(stmt->getdeclId()->getIdentifier());
                 main.pushString("_local");
                 main.pushString("(omp_get_max_threads() , ");
                 main.pushString(ConvertToCppType(type));
                 
                 if(type->getArgList().size() !=0){
                 
                    list<argument*> args = type->getArgList();
                    Expression* expr = args.front()->getExpr();
                    main.pushString("(");
                    GenerateExpression(expr, isMainFile);
  
                // if(type->getInnerTargetSize() != NULL)
                //    generateNestedContainer(type->getInnerTargetSize());
                if(type->getArgList().size() == 2) {
                   main.pushstr_space(",");
                   GenerateExpression(args.back()->getExpr(), isMainFile);
                }   

                 main.pushString(")");
              }
              main.pushString(")");
                  
              main.pushstr_newL(";");

              }

           }

         }
    }

    void DslCppGenerator::GenerateDeviceAssignment(assignment* stmt, bool isMainFile) {
        dslCodePad& targetFile = isMainFile ? main : header;
        bool isDevice = false;
        char strBuffer[300];
        if (stmt->lhs_isIdentifier()) {
            Identifier* id = stmt->getId();

            targetFile.pushString(id->getIdentifier());
        } else if (stmt->lhs_isProp())  // the check for node and edge property to be
                                        // carried out.
        {
            PropAccess* propId = stmt->getPropId();
            std::string varName = propId->getIdentifier2()->getIdentifier();
            if (stmt->isDeviceAssignment()) {
                isDevice = true;                         
                Type* typeB = propId->getIdentifier2()->getSymbolInfo()->getType()->getInnerTargetType();
                
                const char* varType = ConvertToCppType(typeB).c_str();  //DONE: get the type from id
                if(isMainFile) {
                    std::sprintf(strBuffer, "initIndex<%s><<<1,1>>>(V,d%s,%s,(%s)",
                        varType,
                        CapitalizeFirstLetter(propId->getIdentifier2()->getIdentifier()),
                        propId->getIdentifier1()->getIdentifier(),
                        varType);                
                }
                else {
                    std::sprintf(strBuffer, "d%s[%s] = (",
                            CapitalizeFirstLetter(propId->getIdentifier2()->getIdentifier()),
                            propId->getIdentifier1()->getIdentifier());
                }

                targetFile.pushString(strBuffer);
                generateInitIndex = true;
            } else {
                targetFile.pushString(varName);
                targetFile.push('[');
                targetFile.pushString(propId->getIdentifier1()->getIdentifier());
                targetFile.push(']');
            }
        }

        if (!isDevice)
            targetFile.pushString(" = ");

        GenerateExpression(stmt->getExpr(), isMainFile);

        if (isDevice)
            targetFile.pushstr_newL("); //InitIndexDevice");
        else
            targetFile.pushstr_newL("; //InitIndex");

    }

    void DslCppGenerator::GenerateAtomicOrNormalAssignment(assignment* stmt, bool isMainFile) {

        dslCodePad& targetFile = isMainFile ? main : header;
        bool isAtomic = false;
        bool isResult = false;
        // if(stmt->isDeviceAssignment())
        //     targetFile.pushStringWithNewLine("Inside GenerateAtomicOrNormalAssignment, yes --remove");
        // else
        //     targetFile.pushStringWithNewLine("Inside GenerateAtomicOrNormalAssignment, no --remove");
        if (stmt->lhs_isIdentifier()) {
            Identifier* id = stmt->getId();
            Expression* exprAssigned = stmt->getExpr();
            std::cout<<"LHS ID "<<stmt->hasPropCopy()<<" "<<id->getIdentifier()<<endl;
            if (stmt->hasPropCopy())  // prop_copy is of the form (propId = propId)
            {
                char strBuffer[1024];
                Identifier* rhsPropId2 = exprAssigned->getId();
                Type* type = id->getSymbolInfo()->getType();

                std::sprintf(strBuffer, "hipMemcpy(d%s, d%s, sizeof(%s)*V, hipMemcpyDeviceToDevice)", CapitalizeFirstLetter(id->getIdentifier()),
                        CapitalizeFirstLetter(rhsPropId2->getIdentifier()), ConvertToCppType(type->getInnerTargetType()).c_str());
                targetFile.pushString(strBuffer);
                targetFile.pushstr_newL(";");
            } else
            {
                targetFile.pushString(id->getIdentifier());
            }
        }
         else if (stmt->lhs_isProp()){  // the check for node and edge property to be carried out.
            std::cout<<"LHS PROP "<<endl;
            PropAccess* propId = stmt->getPropId();
            if (stmt->getAtomicSignal()) {
                targetFile.pushString("atomicAdd(&");
                isAtomic = true;
            }
            if (stmt->isAccumulateKernel()) {  // NOT needed
                isResult = true;
                std::cout << "\t  RESULT NO BC by 2 ASST" << '\n';
            }
            std::string varName = CapitalizeFirstLetter(std::string(propId->getIdentifier2()->getIdentifier()));
            varName = "d" + varName;
            targetFile.pushString(varName);
            targetFile.push('[');
            targetFile.pushString(propId->getIdentifier1()->getIdentifier());
            targetFile.push(']');
        }
        else if(stmt->lhs_isIndexAccess()){
    
            cout<<"entered here for index access assignment type"<<"\n";
            Expression* indexAccessExpr = stmt->getIndexAccess();
            GenerateExpression(indexAccessExpr, isMainFile);

        }

        if (isAtomic)
            targetFile.pushString(", ");
        else if (!stmt->hasPropCopy())
            targetFile.pushString(" = ");

        if (!stmt->hasPropCopy())
            GenerateExpression(stmt->getExpr(), isMainFile, isAtomic);
        
        if (isAtomic)
            targetFile.pushstr_newL(");");
        else if (isResult)
            targetFile.pushstr_newL(";");  // No need "/2.0;" for directed graphs
        else if (!stmt->hasPropCopy())
            targetFile.pushstr_newL(";");

    }
    

    void DslCppGenerator::GenerateIfStmt(ifStmt* stmt, bool isMainFile) {
        std::cout<<"Entered IF BLOCK..\n";
        dslCodePad& targetFile = isMainFile ? main : header;
        Expression* condition = stmt->getCondition();

        targetFile.pushString("\nif (");
        GenerateExpression(condition, isMainFile);
        targetFile.pushStringWithNewLine(") {");
        GenerateStatement(stmt->getIfBody(), isMainFile);
        targetFile.pushString("} ");

        if(stmt->getElseBody() != NULL) {

            targetFile.pushStringWithNewLine(" else {");
            GenerateStatement(stmt->getElseBody(), isMainFile);
            targetFile.pushStringWithNewLine("}");
        } else {
            targetFile.NewLine();
        }
    }

    void DslCppGenerator::GenerateDoWhileStmt(dowhileStmt* doWhile, bool isMainFile) {
        dslCodePad& targetFile = isMainFile ? main : header;
        //flag_for_device_var = 1;  //done for PR fix
        targetFile.pushstr_newL("do{");
        //~ targetFile.pushString("{");
        GenerateStatement(doWhile->getBody(), isMainFile);
        //~ targetFile.pushString("}");
        targetFile.pushString("}while(");
        GenerateExpression(doWhile->getCondition(), isMainFile);
        targetFile.pushstr_newL(");");
    }

    void DslCppGenerator::GenerateForAll(forallStmt* stmt, bool isMainFile) {
        static int loopNum = 0;
        dslCodePad& targetFile = isMainFile ? main : header;
        proc_callExpr* extractElemFunc = stmt->getExtractElementFunc();
        PropAccess* sourceField = stmt->getPropSource();
        Identifier* sourceId = stmt->getSource();
        Identifier* collectionId;

        if(sourceField) 
            collectionId = sourceField->getIdentifier1();

        if(sourceId)
            collectionId = sourceId;

        Identifier* iteratorMethodId;

        if(extractElemFunc)
            iteratorMethodId = extractElemFunc->getMethodId();

        statement* body = stmt->getBody();

        char strBuffer[1024];

        if(stmt->isForall()) {

            std::cout << "HIT inside FORALL";

            usedVariables usedVars = GetVarsForAll(stmt);
            list<Identifier*> vars = usedVars.getVariables();

            for (Identifier* iden : vars) {
                std::cout<<"IDENTIFIER "<<iden->getIdentifier()<<"\n";
                Type* type = iden->getSymbolInfo()->getType();

                if (type->isPrimitiveType())
                    GenerateHipMemCpySymbol(iden->getIdentifier(), ConvertToCppType(type), true);
                else if(type->isCollectionType()){
                    std::cout<<"COLLECTION TYPE\n";
                    string memcpy = "";
                    memcpy = memcpy + ConvertToCppType(type, true) + " d_" + iden->getIdentifier() + to_string(loopNum) + ";\n";
                    memcpy = memcpy + "hipMalloc(" + "&d_" + iden->getIdentifier() + to_string(loopNum) + ", sizeof(" + ConvertToCppType(type->getInnerTargetType()) +  ") * " + iden->getIdentifier() + ".size());\n";
                    memcpy = memcpy + "hipMemcpy(" + "d_" + string(iden->getIdentifier()) + to_string(loopNum) + 
                    + ", " + string(iden->getIdentifier()) + ".data(), " + string(iden->getIdentifier()) + ".size() * sizeof(" + ConvertToCppType(type->getInnerTargetType())  
                    + "), hipMemcpyHostToDevice);\n";
                    targetFile.pushString(memcpy);
                }
            }
            

            GenerateLaunchConfiguration(stmt, loopNum, isMainFile);

            targetFile.pushString(GetCurrentFunction()->getIdentifier()->getIdentifier());
            std::string temp = "_kernel" + to_string(loopNum);
            targetFile.pushString(temp);
            targetFile.pushString("<<<numBlocks" + to_string(loopNum) + ", threadsPerBlock" + to_string(loopNum) + ">>>(V" + to_string(loopNum));
            if(stmt->getIsMetaUsed())
                targetFile.pushString(", dOffsetArray");
            if(stmt->getIsDataUsed())
                targetFile.pushString(", dEdgelist");
            if(stmt->getIsSrcUsed())
                targetFile.pushString(", dSrcList");
            if(stmt->getIsWeightUsed())
                targetFile.pushString(", dWeight");
            if(stmt->getIsRevMetaUsed())                                   // if d_rev_meta is used, i.e. nodes_to is called
                targetFile.pushString(",dRevOffsetArray");

            for (Identifier* iden : vars) {
                Type* type = iden->getSymbolInfo()->getType();
                if (type->isPropType()) {
                    std::string parameterName = CapitalizeFirstLetter(std::string(iden->getIdentifier()));
                    parameterName = ", d" + parameterName;
                    targetFile.pushString(/*createParamName(*/ parameterName);
                }
                else if(type->isPrimitiveType()){
                    string parameterName = ", " + std::string(iden->getIdentifier());
                    targetFile.pushString(parameterName);
                }
                else if(type->isCollectionType()){
                    string parameterName = ", d_" + std::string(iden->getIdentifier()) + to_string(loopNum);
                    targetFile.pushString(parameterName);
                }
            }
            targetFile.pushString(")");
            targetFile.pushStringWithNewLine(";");

            targetFile.pushStringWithNewLine("hipDeviceSynchronize();");

            for (Identifier* iden : vars) {
                Type* type = iden->getSymbolInfo()->getType();
                if (type->isPrimitiveType())
                    GenerateHipMemCpySymbol(iden->getIdentifier(), ConvertToCppType(type), false);
                else if(type->isCollectionType()){
                    string memcpy = "";
                    memcpy = memcpy + "hipMemcpy(" + string(iden->getIdentifier()) + ".data(), " + "d_" + string(iden->getIdentifier()) + to_string(loopNum) + ", " + string(iden->getIdentifier()) + ".size() * sizeof(" + ConvertToCppType(type->getInnerTargetType())  
                    + "), hipMemcpyDeviceToHost);\n";
                    memcpy = memcpy + "hipFree(d_" + string(iden->getIdentifier()) + to_string(loopNum) + ");\n";
                    targetFile.pushString(memcpy);
                }
            }

            loopNum++;

            GenerateHipKernel(stmt);

        } else {

            std::cout << "HIT inside FORALL ELSE\n";

            GenerateForAllSignature(stmt, false);  // FOR LINE

            if (stmt->hasFilterExpr()) {
                blockStatement* changedBody = IncludeIfToBlock(stmt);
                stmt->setBody(changedBody);
            }

            if (extractElemFunc != NULL) {
                std::cout<<"CHECK HERE..\n";
                if (NeighbourIteration(iteratorMethodId->getIdentifier())) {  
                    
                    char* wItr = stmt->getIterator()->getIdentifier();  // w iterator
                    std::cout << "src:" << wItr << '\n';
                    char* nbrVar;

                    if (stmt->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS) {
                        list<argument*> argList = extractElemFunc->getArgList();
                        assert(argList.size() == 1);
                        Identifier* nodeNbr = argList.front()->getExpr()->getId();
                        nbrVar = nodeNbr->getIdentifier();
                        
                        std::sprintf(strBuffer, "if(dLevel[%s] == -1) {", wItr);
                        targetFile.pushstr_newL(strBuffer);
                        std::sprintf(strBuffer, "dLevel[%s] = *dHopsFromSource + 1;", wItr);

                        targetFile.pushstr_newL(strBuffer);
                        targetFile.pushstr_newL("*dFinished = false;");
                        targetFile.pushstr_newL("}");

                        std::sprintf(strBuffer, "if(dLevel[%s] == *dHopsFromSource + 1) {", wItr);
                        targetFile.pushstr_newL(strBuffer);

                        GenerateBlock((blockStatement*)stmt->getBody(), false, false);

                        targetFile.pushstr_newL("}");

                        targetFile.pushstr_newL("}");

                    }

                    
                    else if (stmt->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS) {  // ITERATE REV BFS
                        char strBuffer[1024];
                        list<argument*> argList = extractElemFunc->getArgList();
                        assert(argList.size() == 1);
                        Identifier* nodeNbr = argList.front()->getExpr()->getId();
                        nbrVar = nodeNbr->getIdentifier();
                        std::cout << "V?:" << nbrVar << '\n';
                        std::sprintf(strBuffer, "if(dLevel[%s] == *dHopsFromSource) {", wItr);
                        targetFile.pushstr_newL(strBuffer);
                        GenerateBlock((blockStatement*)stmt->getBody(), false, false);
                        targetFile.pushstr_newL("} // end IF  ");
                        targetFile.pushstr_newL("} // end FOR");

                    } else {
                        GenerateStatement(stmt->getBody(), isMainFile);
                        targetFile.pushstr_newL("} //  end FOR NBR ITR. TMP FIX!");
                        std::cout << "FOR BODY END" << '\n';
                    }

                } else {
                    GenerateStatement(stmt->getBody(), false);
                }

                if (stmt->isForall() && stmt->hasFilterExpr()) {
                    Expression* filterExpr = stmt->getfilterExpr();
                    GeneratefixedPoint_filter(filterExpr, false);
                }

            } else {
                if (collectionId->getSymbolInfo()->getType()->gettypeId() == TYPE_SETN) {
                    if (body->getTypeofNode() == NODE_BLOCKSTMT) {
                        targetFile.pushstr_newL("{");
                        //~ targetFile.pushstr_newL("//HERE");
                        std::sprintf(strBuffer, "int %s = *itr;", stmt->getIterator()->getIdentifier());
                        targetFile.pushstr_newL(strBuffer);
                        GenerateBlock((blockStatement*)body, false);  //FOR BODY for
                        targetFile.pushstr_newL("}");
                    } else
                        GenerateStatement(stmt->getBody(), false);

                } else {
                    GenerateBlock((blockStatement*)body, false);  //FOR BODY for
                    targetFile.pushstr_newL("\n}\n");
                }

                if (stmt->isForall() && stmt->hasFilterExpr()) {
                    Expression* filterExpr = stmt->getfilterExpr();
                    GeneratefixedPoint_filter(filterExpr, false);
                }
            }
        }
    }

    void DslCppGenerator::GenerateFixedPoint(fixedPointStmt* stmt, bool isMainFile) {

        std::cout << "Inside GenerateFixedPoint\n";

        dslCodePad& targetFile = isMainFile ? main : header;

        char strBuffer[1024];
        Expression* convergeExpr = stmt->getDependentProp();
        Identifier* fixedPointId = stmt->getFixedPointId();

        assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
                convergeExpr->getExpressionFamily() == EXPR_ID);

        Identifier* dependentId = NULL;
        assert(convergeExpr->getExpressionFamily() == EXPR_UNARY ||
                convergeExpr->getExpressionFamily() == EXPR_ID);
        
        if (convergeExpr->getExpressionFamily() == EXPR_UNARY) {
            if (convergeExpr->getUnaryExpr()->getExpressionFamily() == EXPR_ID) {
                dependentId = convergeExpr->getUnaryExpr()->getId();
            }
        }
        else if(convergeExpr->getExpressionFamily() == EXPR_ID){
            dependentId=convergeExpr->getId();
        }

        const char* modifiedVar = CapitalizeFirstLetter(dependentId->getIdentifier());
        char* fixPointVar = fixedPointId->getIdentifier();

        const std::string fixPointVarType = ConvertToCppType(fixedPointId->getSymbolInfo()->getType());

        targetFile.pushStringWithNewLine("// FIXED POINT variables");
        char modifiedVarNext[80] = "d";

        strcat(modifiedVarNext, modifiedVar);
        strcat(modifiedVarNext, "Next");

        if (convergeExpr->getExpressionFamily() == EXPR_ID)
            dependentId = convergeExpr->getId();
        
        if (dependentId != NULL) {
            if (dependentId->getSymbolInfo()->getType()->isPropType()) {
                if (dependentId->getSymbolInfo()->getType()->isPropNodeType()) {
                    targetFile.pushStringWithNewLine("//BEGIN FIXED POINT");
                    std::sprintf(strBuffer, "initKernel<%s> <<<numBlocks,threadsPerBlock>>>(V, %s, false);", fixPointVarType.c_str(), modifiedVarNext);
                    targetFile.pushstr_newL(strBuffer);
                    generateInitKernel = true;

                    targetFile.pushStringWithNewLine("int k=0; // #fixpt-Iterations");
                    std::sprintf(strBuffer, "while(!%s) {", fixPointVar);
                    targetFile.pushstr_newL(strBuffer);

                    std::cout<< "Size::" << graphId.size() << '\n';
                    std::sprintf(strBuffer, "%s = %s", fixPointVar, "true");
                    targetFile.pushString(strBuffer);
                    targetFile.pushStringWithNewLine(";");

                    GenerateHipMemCpySymbol(fixPointVar, fixPointVarType, true);

                    if (stmt->getBody()->getTypeofNode() != NODE_BLOCKSTMT)
                        GenerateStatement(stmt->getBody(), isMainFile);
                    else
                        GenerateBlock((blockStatement*)stmt->getBody(), false, isMainFile);

                    GenerateHipMemCpySymbol(fixPointVar, fixPointVarType, false);
                    
                    std::sprintf(strBuffer, "hipMemcpy(d%s, %s, sizeof(%s)*V, hipMemcpyDeviceToDevice)", modifiedVar,
                            modifiedVarNext, fixPointVarType.c_str());
                    targetFile.pushString(strBuffer);
                    targetFile.pushstr_newL(";");

                    std::sprintf(strBuffer, "initKernel<%s> <<<numBlocks,threadsPerBlock>>>(V, %s, false);", fixPointVarType.c_str(), modifiedVarNext);
                    targetFile.pushstr_newL(strBuffer);
                    generateInitKernel = true;

                    targetFile.pushStringWithNewLine("k++;");

                    Expression* initializer = dependentId->getSymbolInfo()->getId()->get_assignedExpr();
                    assert(initializer->isBooleanLiteral());

                    targetFile.pushStringWithNewLine("} // END FIXED POINT");
                    targetFile.NewLine();
                }
            }
            else{
                std::cout<<"NOT YET IMPLEMENTED\n";
            }
        }

    }

    void DslCppGenerator::GeneratefixedPoint_filter(Expression* stmt,
                                               bool isMainFile) {

        Expression* lhs = stmt->getLeft();
        if (lhs->isIdentifierExpr()) {
            //~ Identifier* filterId = lhs->getId();
            //~ TableEntry* tableEntry = filterId->getSymbolInfo();
            /*
            if(tableEntry->getId()->get_fp_association())
            {
                targetFile.pushstr_newL("#pragma omp for");
                std::sprintf(strBuffer,"for (%s %s = 0; %s < %s.%s(); %s ++)
            ","int","v","v",graphId[0]->getIdentifier(),"num_nodes","v");
                targetFile.pushstr_newL(strBuffer);
                targetFile.pushstr_space("{");
                std::sprintf(strBuffer,"%s[%s] =  %s_nxt[%s]
            ;",filterId->getIdentifier(),"v",filterId->getIdentifier(),"v");
                targetFile.pushstr_newL(strBuffer);
                Expression* initializer =
            filterId->getSymbolInfo()->getId()->get_assignedExpr();
                assert(initializer->isBooleanLiteral());
                std::sprintf(strBuffer,"%s_nxt[%s] = %s
            ;",filterId->getIdentifier(),"v",initializer->getBooleanConstant()?"true":"false");
                targetFile.pushstr_newL(strBuffer);
                targetFile.pushstr_newL("}");
                targetFile.pushstr_newL("}");

            }
            */
        }
    }

    void DslCppGenerator::GenerateWhileStmt(whileStmt* whilestmt, bool isMainFile) {
        Expression* conditionExpr = whilestmt->getCondition();
        main.pushString("\n");
        main.pushString("while (");
        GenerateExpression(conditionExpr, isMainFile);
        main.pushString(" ){\n");
        GenerateStatement(whilestmt->getBody(), isMainFile);
        main.pushString("\n}\n");
    }

    void DslCppGenerator::GenerateReductionStmt(reductionCallStmt* stmt, bool isMainFile) {
        if (stmt->is_reducCall()) {
            // GenerateReductionCallStmt(stmt, isMainFile);
        } else {
            GenerateReductionOpStmt(stmt, isMainFile);
        }

    }

    void DslCppGenerator::GenerateReductionOpStmt(reductionCallStmt* stmt,
                                                bool isMainFile) {
        dslCodePad& targetFile = isMainFile ? main : header;
        char strBuffer[1024];

        if (stmt->isLeftIdentifier()) {

            Identifier* id = stmt->getLeftId();  
            const char* typVar = ConvertToCppType(id->getSymbolInfo()->getType()).c_str();
            if (strcmp("long", typVar) == 0) {
            std::sprintf(strBuffer, "atomicAdd((unsigned long long*)& %s, (unsigned long long)", id->getIdentifier());
            } else {
            std::sprintf(strBuffer, "atomicAdd(& %s, (%s)", id->getIdentifier(), typVar);
            }
            targetFile.pushString(strBuffer);
            std::string operatorString = GetOperatorString(stmt->reduction_op());
            if(operatorString == "-")
                targetFile.pushString("-");
            GenerateExpression(stmt->getRightSide(), isMainFile);
            targetFile.pushstr_newL(");");

        } else {
            targetFile.pushString("atomicAdd(&");
            GenerateExpressionPropId(stmt->getPropAccess(), isMainFile);
            targetFile.pushString(", (int)");
            std::string operatorString = GetOperatorString(stmt->reduction_op());
            if(operatorString == "-")
                targetFile.pushString("-");
            GenerateExpression(stmt->getRightSide(), isMainFile);
            targetFile.pushstr_newL(");");
        }
    }

    // void DslCppGenerator::GenerateItrBfs2(iterateBFS2* stmt, bool isMainFile) {

    //     std::cout << "Inside GenerateItrBfs2\n"; 

    //     char strBuffer[1024];
    //     bool generateRevBfs = stmt->getRBFS() && stmt->getRBFS()->getBody();

    //     statement* body = stmt->getBody();
    //     assert(body->getTypeofNode() == NODE_BLOCKSTMT);
    //     blockStatement* block = (blockStatement*)body;
    //     list<statement*> statementList = block->returnStatements();

    //     main.NewLine();
    //     main.pushstr_newL("//EXTRA vars for ITBFS AND REVBFS");  //NOT IN DSL so hardcode is fine
    //     main.pushstr_newL("bool finished2;");
    //     if(generateRevBfs)
    //         main.pushstr_newL("int hopsFromSource2=0;");

    //     main.pushstr_newL("bool* dFinished2;");
    //     main.pushstr_newL("hipMalloc(&dFinished2,sizeof(bool) *(1));");
    //     if(generateRevBfs) {
    //         main.pushstr_newL("int* dHopsFromSource2;");
    //         main.pushstr_newL("hipMalloc(&dHopsFromSource2, sizeof(int));");
    //         main.pushstr_newL("hipMemset(dHopsFromSource2,0,sizeof(int));");
    //         main.pushstr_newL("int* dLevel2;           hipMalloc(&dLevel2,sizeof(int) *(V));");
        
    //         main.NewLine();
    //         main.pushstr_newL("//EXTRA vars INITIALIZATION");

    //         GenerateInitkernelStr("int", "dLevel2", "-1");

    //         std::sprintf(strBuffer, "initIndex<int><<<1,1>>>(V, dLevel2, %s, 0);", stmt->getRootNode()->getIdentifier());
    //         main.pushstr_newL(strBuffer);
    //         main.NewLine();
    //         generateInitIndex = true;

    //     }
    //     main.pushStringWithNewLine("bool *dVisitBfs2;");
    //     GenerateHipMallocStr("dVisitBfs2", "bool", "V");
    //     GenerateInitkernelStr("bool", "dVisitBfs2", "false");
    //     main.pushstr_newL("do {");

    //     AddHipBFS2IterationLoop(stmt, generateRevBfs);  // ADDS BODY OF ITERBFS + KERNEL LAUNCH

    //     main.NewLine();

    //     GenerateHipMemcpyStr("&finished2", "dFinished2", "bool", "1", false);

    //     main.pushstr_newL("}while(!finished2);"); 
        
    //     iterateReverseBFS* revBfs = stmt->getRBFS();

    //     if(revBfs == NULL)
    //         return;

    //     blockStatement* revBlock = (blockStatement*)stmt->getRBFS()->getBody();

    //     if(revBlock == NULL)
    //         return;

    //     list<statement*> revStmtList = revBlock->returnStatements();
        
    //     AddHipRevBFS2IterationLoop(stmt);

    //     main.pushstr_newL("//BACKWARD PASS");
    //     main.pushstr_newL("while(hopsFromSource2 > 1) {");

    //     main.NewLine();
    //     main.pushstr_newL("//KERNEL Launch");
    //     main.pushString("back_pass2<<<numBlocks,threadsPerBlock>>>(V, dRevOffsetArray, dEdgelist, dWeight, dLevel2, dHopsFromSource2, dFinished2");

    //     GeneratePropParams(GetCurrentFunction()->getParamList(), false, true);  // true: typeneed false:inMainfile

    //     usedVariables usedVars = getVarsBFS(stmt);
    //     list<Identifier*> vars = usedVars.getVariables();

    //     for (Identifier* iden : vars) {
    //         Type* type = iden->getSymbolInfo()->getType();
    //         if (type->isPropType()) {
    //             std::string parameterName = CapitalizeFirstLetter(std::string(iden->getIdentifier()));
    //             parameterName = ", d" + parameterName;
    //             main.pushString(/*createParamName(*/ parameterName);
    //         }
    //     }

    //     main.pushstr_newL("); ///DONE from varList");  ///TODO get all propnodes from function body and params
        
    //     AddHipRevBFS2IterKernel(revStmtList, stmt);  // KERNEL BODY

    //     main.pushstr_newL("hopsFromSource2--;");
    //     GenerateHipMemcpyStr("dHopsFromSource2", "&hopsFromSource2", "int", "1", true);

    //     main.pushstr_newL("}");
    // }

    void DslCppGenerator::GenerateItrBfs(iterateBFS* stmt, bool isMainFile) {

        std::cout << "Inside GenerateItrBfs\n";
        char strBuffer[1024];
        bool generateRevBfs = stmt->getRBFS() && stmt->getRBFS()->getBody();

        statement* body = stmt->getBody();
        assert(body->getTypeofNode() == NODE_BLOCKSTMT);
        blockStatement* block = (blockStatement*)body;
        list<statement*> statementList = block->returnStatements();

        main.NewLine();
        main.pushstr_newL("//EXTRA vars for ITBFS AND REVBFS");  //NOT IN DSL so hardcode is fine
        main.pushstr_newL("bool finished;");
        if(generateRevBfs)
            main.pushstr_newL("int hopsFromSource=0;");

        main.pushstr_newL("bool* dFinished;");
        main.pushstr_newL("hipMalloc(&dFinished,sizeof(bool) *(1));");
        if(generateRevBfs) {
            main.pushstr_newL("int* dHopsFromSource;");
            main.pushstr_newL("hipMalloc(&dHopsFromSource, sizeof(int));");
            main.pushstr_newL("hipMemset(dHopsFromSource,0,sizeof(int));");
            main.pushstr_newL("int* dLevel;           hipMalloc(&dLevel,sizeof(int) *(V));");

            main.NewLine();
            main.pushstr_newL("//EXTRA vars INITIALIZATION");

            GenerateInitkernelStr("int", "dLevel", "-1");

            std::sprintf(strBuffer, "initIndex<int><<<1,1>>>(V, dLevel, %s, 0);", stmt->getRootNode()->getIdentifier());
            main.pushstr_newL(strBuffer);
            main.NewLine();
            generateInitIndex = true;

        }
        main.pushStringWithNewLine("bool *dVisitBfs;");
        GenerateHipMallocStr("dVisitBfs", "bool", "V");
        GenerateInitkernelStr("bool", "dVisitBfs", "false");
        main.pushstr_newL("do {");

        AddHipBFSIterationLoop(stmt, generateRevBfs);  // ADDS BODY OF ITERBFS + KERNEL LAUNCH

        main.NewLine();

        GenerateHipMemcpyStr("&finished", "dFinished", "bool", "1", false);

        main.pushstr_newL("}while(!finished);"); 
        
        iterateReverseBFS* revBfs = stmt->getRBFS();

        if(revBfs == NULL)
            return;

        blockStatement* revBlock = (blockStatement*)stmt->getRBFS()->getBody();

        if(revBlock == NULL)
            return;

        list<statement*> revStmtList = revBlock->returnStatements();
        
        AddHipRevBFSIterationLoop(stmt);

        main.pushstr_newL("//BACKWARD PASS");
        main.pushstr_newL("while(hopsFromSource > 1) {");

        main.NewLine();
        main.pushstr_newL("//KERNEL Launch");
        main.pushString("back_pass<<<numBlocks,threadsPerBlock>>>(V, dOffsetArray, dEdgelist, dWeight, dLevel, dHopsFromSource, dFinished");

        GeneratePropParams(GetCurrentFunction()->getParamList(), false, true);  // true: typeneed false:inMainfile

        usedVariables usedVars = getVarsBFS(stmt);
        list<Identifier*> vars = usedVars.getVariables();

        for (Identifier* iden : vars) {
            Type* type = iden->getSymbolInfo()->getType();
            if (type->isPropType()) {
                std::string parameterName = CapitalizeFirstLetter(std::string(iden->getIdentifier()));
                parameterName = ", d" + parameterName;
                main.pushString(/*createParamName(*/ parameterName);
            }
        }

        main.pushstr_newL("); ///DONE from varList");  ///TODO get all propnodes from function body and params
        
        AddHipRevBFSIterKernel(revStmtList, stmt);  // KERNEL BODY

        main.pushstr_newL("hopsFromSource--;");
        GenerateHipMemcpyStr("dHopsFromSource", "&hopsFromSource", "int", "1", true);

        main.pushstr_newL("}");
    }

    void DslCppGenerator::GenerateItrRevBfs(iterateReverseBFS* stmt, bool isMainFile) {

        std::cout << "Inside GenerateItrRevBfs\n";

        statement* body = stmt->getBody();
        assert(body->getTypeofNode() == NODE_BLOCKSTMT);
        blockStatement* block = (blockStatement*)body;
        list<statement*> statementList = block->returnStatements();


        for (statement* stmt : statementList) {
            GenerateStatement(stmt, isMainFile);  //false. All these stmts should be inside kernel
        }

    }

    // void DslCppGenerator::GenerateItrBfsRev(iterateBFSReverse* stmt, bool isMainFile) {
    //     std::std::cout << "Inside reverse bfs\n";
    //     header.pushstr_newL("It works, bfs");
    // }

    void DslCppGenerator::GenerateProcCallStmt(proc_callStmt* stmt, bool isMainFile) {

        proc_callExpr *proc = stmt->getProcCallExpr();
        string methodId = proc->getMethodId()->getIdentifier();
        string nodeCall = "attachNodeProperty";
        string edgeCall = "attachEdgeProperty";

        std::cout<<"HELLOO\n";
        if(methodId.compare(nodeCall) == 0) {

            list<argument*> argList = proc->getArgList();

            for(auto itr =  argList.begin(); itr != argList.end(); itr++) {

                GenerateInitKernelCall((*itr)->getAssignExpr(), isMainFile);
            }
            generateInitKernel = true;

        } else if(methodId.compare(edgeCall) == 0) {

            // HIT_CHECK

        } else {

            // HIT_CHECK
            GenerateExpressionProcCallExpression(proc, isMainFile);
            main.pushstr_newL(";");
            main.NewLine();

        }
        
    }

    void DslCppGenerator::generate_exprIndex(Expression* expr, bool isLocal, bool isMainFile){

        dslCodePad & targetFile = (isMainFile ? main : header);
        char strBuffer[1024];
        Expression* mapExpr = expr->getMapExpr();
        Expression* indexExpr = expr->getIndexExpr();

        Identifier* mapExprId = mapExpr->getId();

        if(indexExpr->isIdentifierExpr()){

            cout<<"entered here for indentifier gen for indexexpr"<<"\n";
            Identifier* indexExprId = indexExpr->getId();  

            if(isLocal)
                sprintf(strBuffer , "%s_local[omp_get_thread_num()][%s]", mapExprId->getIdentifier() , indexExprId->getIdentifier());
            else
                sprintf(strBuffer , "%s[%s]", mapExprId->getIdentifier() , indexExprId->getIdentifier());

            cout<<"string gen "<<strBuffer<<"\n";
            targetFile.pushString(strBuffer);
        }
        else if(indexExpr->isPropIdExpr()){

            cout<<"entered here for index "<<"\n";
            if(isLocal){
                sprintf(strBuffer, "%s_local[omp_get_thread_num()][", mapExprId->getIdentifier());
                targetFile.pushString(strBuffer);
                // generate_exprPropId(indexExpr->getPropId(), true);
                targetFile.pushString("]");
            }
            else {
                sprintf(strBuffer, "%s[", mapExprId->getIdentifier());
                targetFile.pushString(strBuffer);
                // generate_exprPropId(indexExpr->getPropId(), true);
                targetFile.pushString("]");
            }

        }
        else{
            sprintf(strBuffer, "%s[", mapExprId->getIdentifier());
            targetFile.pushString(strBuffer);
            GenerateExpression(indexExpr, isMainFile);
            targetFile.pushString("]");
        }

    }

    void DslCppGenerator::GenerateExpression(
        Expression *expr, bool isMainFile, bool isToUpper, bool isAtomic
    ) {
        std::cout<<"EXPR "<<expr->isProcCallExpr()<<endl;
        if(expr->isLiteral()) 
            GenerateExpressionLiteral(expr, isMainFile);

        else if(expr->isInfinity())
            GenerateExpressionInfinity(expr, isMainFile);

        else if(expr->isIdentifierExpr())
            GenerateExpressionIdentifier(expr->getId(), isMainFile);

        else if(expr->isPropIdExpr())
            GenerateExpressionPropId(expr->getPropId(), isMainFile);

        else if(expr->isArithmetic() || expr->isLogical())
            GenerateExpressionArithmeticOrLogical(expr, isMainFile, isAtomic);

        else if(expr->isRelational())
            GenerateExpressionRelational(expr, isMainFile);

        else if(expr->isProcCallExpr())
            GenerateExpressionProcCallExpression(static_cast<proc_callExpr*>(expr), isMainFile);

        else if(expr->isUnary())
            GenerateExpressionUnary(expr, isMainFile);

        else if(expr->isIndexExpr())
       {
        std::cout<<"INDEX EXPR\n";

          if(expr->getTypeofExpr() == TYPE_BOOL){
            if(expr->getMapExpr()->getId()->getSymbolInfo()!= NULL){
              if(expr->getMapExpr()->getId()->getSymbolInfo()->getType()->gettypeId() == TYPE_CONTAINER)
                 main.pushString("!");

          } }

         generate_exprIndex(expr, false, isMainFile);

          if(expr->getTypeofExpr() == TYPE_BOOL){
            if(expr->getMapExpr()->getId()->getSymbolInfo()!= NULL){
              if(expr->getMapExpr()->getId()->getSymbolInfo()->getType()->gettypeId() == TYPE_CONTAINER)
                 main.pushString(".empty()");

          } }


       }

        else    
            assert(false);
    }

    void DslCppGenerator::GenerateExpressionLiteral(Expression* expr, bool isMainFile) {

        int expressionType = expr->getExpressionFamily();
        std::ostringstream oss;

        switch(expressionType) {

            case EXPR_INTCONSTANT:
                oss << expr->getIntegerConstant();
                break;

            case EXPR_FLOATCONSTANT:
                oss << expr->getFloatConstant();
                break;

            case EXPR_BOOLCONSTANT:
                oss << (expr->getBooleanConstant() ? "true" : "false");
                break;

            default:
                assert(false);
        }

        (isMainFile ? main : header).pushString(oss.str());
    }

    void DslCppGenerator::GenerateExpressionInfinity(Expression* expr, bool isMainFile) {

        std::string value = expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN";

        if(expr->getTypeofExpr()) {

            switch(expr->getTypeofExpr()) {

                case TYPE_INT:
                    value = expr->isPositiveInfinity() ? "INT_MAX" : "INT_MIN";
                    break;

                case TYPE_LONG:
                    value = expr->isPositiveInfinity() ? "LLONG_MAX" : "LLONG_MIN";
                    break;

                case TYPE_FLOAT:
                    value = expr->isPositiveInfinity() ? "FLT_MAX" : "FLT_MIN";
                    break;

                case TYPE_DOUBLE:
                    value = expr->isPositiveInfinity() ? "DBL_MAX" : "DBL_MIN";
                    break;
            }
        }

        (isMainFile ? main : header).pushString(value);
    }

    void DslCppGenerator::GenerateExpressionIdentifier(Identifier* id, bool isMainFile, bool isNotToUpper) {

        (isMainFile ? main : header).pushString(id->getIdentifier());
    }

    void DslCppGenerator::GenerateExpressionPropId(PropAccess* propId, bool isMainFile) {

        std::string value;

        Identifier *id1 = propId->getIdentifier1();
        Identifier *id2 = propId->getIdentifier2();
        ASTNode *propParent = propId->getParent();
        bool isRelatedToReduction = false;
        
        if(propParent)
            isRelatedToReduction = propParent->getTypeofNode() == NODE_REDUCTIONCALLSTMT;

        if(
            id2->getSymbolInfo() &&
            id2->getSymbolInfo()->getId()->get_fp_association() &&
            isRelatedToReduction
        ) 
            value = string("d") + id2->getIdentifier() + "Next[" + id1->getIdentifier() + "]";
        else {

            if(isMainFile)
                value = id2->getIdentifier() + string("[") + id1->getIdentifier() + "]";
            else
                value = string("d") + CapitalizeFirstLetter(id2->getIdentifier()) + "[" + id1->getIdentifier() + "]";
        }

        (isMainFile ? main : header).pushString(value);
    }

    void DslCppGenerator::GenerateExpressionArithmeticOrLogical(
        Expression* expr, bool isMainFile, bool isAtomic
    ) {

        dslCodePad &target = isMainFile ? main : header;

        if(expr->hasEnclosedBrackets())
            target.pushString("(");

        if(!isAtomic)
            GenerateExpression(expr->getLeft(), isMainFile);

        target.AddSpace();

        const std::string operatorString = GetOperatorString(expr->getOperatorType());

        if(!isAtomic) {
            target.pushString(operatorString);
            target.AddSpace();
        }

        GenerateExpression(expr->getRight(), isMainFile);

        if(expr->hasEnclosedBrackets())
            target.pushString(")");
    }

    void DslCppGenerator::GenerateExpressionRelational(Expression* expr, bool isMainFile) {

        dslCodePad & target = isMainFile ? main : header;

        if(expr->hasEnclosedBrackets())
            target.pushString("(");

        GenerateExpression(expr->getLeft(), isMainFile);
        target.AddSpace();

        const string op = GetOperatorString(expr->getOperatorType());
        target.pushString(op);
        target.AddSpace();
        GenerateExpression(expr->getRight(), isMainFile);

        if(expr->hasEnclosedBrackets())
            target.pushString(")");
    }

    void DslCppGenerator::generateArgList(list<argument*> argList, bool addBraces, bool isMainFile){

        dslCodePad & targetFile = (isMainFile ? main : header);
        char strBuffer[1024]; 

        if(addBraces)
            targetFile.pushString("(") ;

        int argListSize = argList.size();
        int commaCounts = 0;
        list<argument*>::iterator itr;
        for(itr=argList.begin();itr!=argList.end();itr++)
        {
            commaCounts++;
            argument* arg = *itr;
            Expression* expr = arg->getExpr();//->getId();
            //sprintf(strBuffer, "%s", id->getIdentifier());
        // targetFile.pushString(strBuffer);
            GenerateExpression(expr, isMainFile);
            if(commaCounts < argListSize)
            targetFile.pushString(",");

        }
        
        if(addBraces)
            targetFile.pushString(")");
    }


    string DslCppGenerator::getProcName(proc_callExpr* proc){

        string methodId(proc->getMethodId()->getIdentifier());

        if(methodId == "push") {

            string modifiedId = "push_back";
            return modifiedId;

        }
        else
        return methodId;

    }

    void DslCppGenerator::GenerateExpressionProcCallExpression(
        proc_callExpr* expr, bool isMainFile
    ) {

        dslCodePad & targetFile = isMainFile ? main : header;

        string methodId(expr->getMethodId()->getIdentifier());

        if (methodId == "get_edge")
            targetFile.pushString("edge");
        
        else if (methodId == "count_outNbrs"){

            string strBuffer;
            list<argument*> argList = expr->getArgList();
            assert(argList.size() == 1);

            Identifier* nodeId = argList.front()->getExpr()->getId();

            strBuffer = string("(dOffsetArray[") + nodeId->getIdentifier() + " + 1] -" +
                         "dOffsetArray[" + nodeId->getIdentifier() + "]";

            targetFile.pushString(strBuffer);

        } else if (methodId == "is_an_edge") {

            //TODO: Implement
            std::cout << "DANGER HIT";

        } else if (methodId == "num_nodes") {
            targetFile.pushString("g.num_nodes()");
        } else {
            
            // std::cout << "DANGER HIT";
            proc_callExpr* proc = expr;
                char strBuffer[1024];
                list<argument*> argList=proc->getArgList();
                Identifier* objectId = proc->getId1();
                Expression* indexExpr = proc->getIndexExpr();

                if(objectId!=NULL) 
                {
                    cout << "isnide here 1"<<endl;
                    Identifier* id2 = proc->getId2();
                    if(id2 != NULL)
                    {

                        sprintf(strBuffer,"%s.%s.%s",objectId->getIdentifier(), id2->getIdentifier(), getProcName(proc).c_str());
                    }
                    else
                    {
                        sprintf(strBuffer,"%s.%s",objectId->getIdentifier(), getProcName(proc).c_str());  
                
                    }  
                }
                else if(indexExpr != NULL)
                {
                    cout << "isnide here 2"<<endl;
                    cout<<"ENTERED HERE FOR INDEXEXPR GENERATION DYNAMIC"<<"\n";
                    Expression* mapExpr = indexExpr->getMapExpr();
                    Identifier* mapExprId = mapExpr->getId();

                    generate_exprIndex(indexExpr, true, isMainFile);

                    sprintf(strBuffer,".%s", getProcName(proc).data());
                } 
                else {
                cout << "isnide here 3"<<endl;
                sprintf(strBuffer,"%s", getProcName(proc).data());
            
                }


                targetFile.pushString(strBuffer);

            if(methodId == "insert"){
                
                targetFile.pushString("(");

                if(indexExpr != NULL){
                    Expression* mapExpr = indexExpr->getMapExpr();
                    Identifier* mapExprId = mapExpr->getId();

                    generate_exprIndex(indexExpr, true, isMainFile);
                }
                else if(objectId != NULL){
                    Identifier* id2 = proc->getId2();
                    if(id2 != NULL)
                        sprintf(strBuffer,"%s.%s",objectId->getIdentifier(), id2->getIdentifier());               
                    else
                        sprintf(strBuffer,"%s",objectId->getIdentifier()); 

                    targetFile.pushString(strBuffer);      
                }

                targetFile.pushString(".end()");
                targetFile.pushString(",");
                generateArgList(argList,false, isMainFile);
                targetFile.pushString(".begin(),");
                generateArgList(argList, false, isMainFile);
                targetFile.pushString(".end())");
                        
            }  
            else  
            {
                cout << "isnide here 4"<<endl;
                generateArgList(argList, true, isMainFile);   
            } 
        }
    }

    void DslCppGenerator::GenerateExpressionUnary(
        Expression* expr, bool isMainFile
    ) {

        dslCodePad& targetFile = isMainFile ? main : header;

        if (expr->hasEnclosedBrackets()) {
            targetFile.pushString("(");
        }

        if (expr->getOperatorType() == OPERATOR_NOT) {

            const string operatorString = GetOperatorString(expr->getOperatorType());
            targetFile.pushString(operatorString);
            GenerateExpression(expr->getUnaryExpr(), isMainFile);
        }

        if (
            expr->getOperatorType() == OPERATOR_INC ||
            expr->getOperatorType() == OPERATOR_DEC
        ) {
            if(!isMainFile) {
                if(expr->getOperatorType() == OPERATOR_INC) {
                    targetFile.pushString("atomicAdd(&");
                    GenerateExpression(expr->getUnaryExpr(), isMainFile);
                    targetFile.pushString(", 1)");
                }
                else if(expr->getOperatorType() == OPERATOR_DEC) {
                    targetFile.pushString("atomicAdd(&");
                    GenerateExpression(expr->getUnaryExpr(), isMainFile);
                    targetFile.pushString(", -1)");
                }
            }
            else {
                GenerateExpression(expr->getUnaryExpr(), isMainFile);
                const string operatorString = GetOperatorString(expr->getOperatorType());
                targetFile.pushString(operatorString);
            }
        }

        if (expr->hasEnclosedBrackets()) {
            targetFile.pushString(")");
        }
        targetFile.pushStringWithNewLine(";");


        // PropAccess* propId = stmt->getPropId();
        //         if (stmt->getAtomicSignal()) {
        //             targetFile.pushString("atomicAdd(&");
        //             isAtomic = true;
        //         }
        //         if (stmt->isAccumulateKernel()) {  // NOT needed
        //             isResult = true;
        //             std::std::cout << "\t  RESULT NO BC by 2 ASST" << '\n';
        //         }
        //         std::string varName = CapitalizeFirstLetter(std::string(propId->getIdentifier2()->getIdentifier()));
        //         varName = "d" + varName;
        //         targetFile.pushString(varName);
        //         targetFile.push('[');
        //         targetFile.pushString(propId->getIdentifier1()->getIdentifier());
        //         targetFile.push(']');
    }


    void DslCppGenerator::GenerateMallocStr(
        const std::string &hVar, 
        const std::string &typeStr, 
        const std::string &sizeOfType
    ) {

        main.pushStringWithNewLine(
            hVar + " = (" + typeStr + "*) malloc(sizeof(" + typeStr + ")" +
            " * (" + sizeOfType + "));"
        );
    }

    /**
     * Simple functions to reduce cognitive complexity
    */

   /**
     * TODO
    */
    void DslCppGenerator::CheckAndGenerateVariables(Function *function, const std::string &loc) {

        if(function->getIsMetaUsed()) 
            main.pushStringWithNewLine("int *" + loc +"OffsetArray;");

        if(function->getIsDataUsed())
            main.pushStringWithNewLine("int *" + loc +"Edgelist;");

        if(function->getIsSrcUsed())
            main.pushStringWithNewLine("int *" + loc +"SrcList;");

        if(function->getIsWeightUsed())
            main.pushStringWithNewLine("int *" + loc +"Weight;");

        if(function->getIsRevMetaUsed())
            main.pushStringWithNewLine("int *" + loc +"RevOffsetArray;");

        main.NewLine();
    }

    /**
     * TODO
    */
    void DslCppGenerator::CheckAndGenerateHipMalloc(Function *function) {

        if(function->getIsMetaUsed()) 
            GenerateHipMallocStr("dOffsetArray", "int", "V + 1");

        if(function->getIsDataUsed())
            GenerateHipMallocStr("dEdgelist", "int", "E");

        if(function->getIsSrcUsed())
            GenerateHipMallocStr("dSrcList", "int", "E");

        if(function->getIsWeightUsed())
            GenerateHipMallocStr("dWeight", "int", "E");

        if(function->getIsRevMetaUsed())
            GenerateHipMallocStr("dRevOffsetArray", "int", "V + 1");

        main.NewLine();
    }

    /**
     * TODO
    */
    void DslCppGenerator::CheckAndGenerateMalloc(Function *function) {

        if(function->getIsMetaUsed()) 
            GenerateMallocStr("hOffsetArray", "int", "V + 1");

        if(function->getIsDataUsed())
            GenerateMallocStr("hEdgelist", "int", "E");

        if(function->getIsSrcUsed())
            GenerateMallocStr("hSrcList", "int", "E");

        if(function->getIsWeightUsed())
            GenerateMallocStr("hWeight", "int", "E");

        if(function->getIsRevMetaUsed())
            GenerateMallocStr("hRevOffsetArray", "int", "V + 1");

        main.NewLine();
    }

    /**
     * TODO
    */
    void DslCppGenerator::CheckAndGenerateMemcpy(Function *function) {

        if(function->getIsMetaUsed())
            GenerateHipMemcpyStr("dOffsetArray", "hOffsetArray", "int", "V + 1");

        if(function->getIsDataUsed())
            GenerateHipMemcpyStr("dEdgelist", "hEdgelist", "int", "E");

        if(function->getIsSrcUsed())
            GenerateHipMemcpyStr("dSrcList", "hSrcList", "int", "E");

        if(function->getIsWeightUsed())
            GenerateHipMemcpyStr("dWeight", "hWeight", "int", "E");

        if(function->getIsRevMetaUsed())
            GenerateHipMemcpyStr("dRevOffsetArray", "hRevOffsetArray", "int", "V + 1");

        main.NewLine();
    }

    void DslCppGenerator::GenerateHipKernel(forallStmt *stmt) {
        static int kernelNum = 0;
        Identifier* iterator = stmt->getIterator();
        const char *loopVar = iterator->getIdentifier();
        char strBuffer[1024];
        usedVariables usedVars = GetVarsForAll(stmt);
        list<Identifier*> vars = usedVars.getVariables();
        forAllSrcVariable = loopVar;
        
        header.pushString("__global__ void ");
        header.pushString(GetCurrentFunction()->getIdentifier()->getIdentifier());
        std::string temp = "_kernel" + to_string(kernelNum);
        header.pushString(temp);
        kernelNum++;

        header.pushString("(int V");
        if(stmt->getIsMetaUsed())
            header.pushString(", int* dOffsetArray");
        if(stmt->getIsDataUsed())
            header.pushString(", int* dEdgelist");
        if(stmt->getIsSrcUsed())
            header.pushString(", int* dSrcList");
        if(stmt->getIsWeightUsed())
            header.pushString(", int* dWeight");
        if(stmt->getIsRevMetaUsed())
            header.pushString(", int *dRevOffsetArray");

        for (Identifier* iden : vars) {
            Type* type = iden->getSymbolInfo()->getType();
            if (type->isPropType()) {
                char strBuffer[1024];
                std::sprintf(strBuffer, ", %s d%s", ConvertToCppType(type).c_str(), CapitalizeFirstLetter(iden->getIdentifier()));
                header.pushString(/*createParamName(*/ strBuffer);
                if(iden->getSymbolInfo()->getId()->get_fp_association()) { 
                    std::sprintf(strBuffer, ", %s d%sNext", ConvertToCppType(type).c_str(), CapitalizeFirstLetter(iden->getIdentifier()));
                    header.pushString(/*createParamName(*/ strBuffer);
                }
            }
            else if(type->isCollectionType()){
                string parameter = "";
                parameter = parameter + ", " + ConvertToCppType(type, true).c_str() + " " + iden->getIdentifier();
                header.pushString(parameter);
            }
            else if(type->isPrimitiveType()){
                string parameter = "";
                parameter = parameter + ", " + ConvertToCppType(type, false, true).c_str() + " " + iden->getIdentifier();
                header.pushString(parameter);
            }
        }

        header.pushStringWithNewLine("){ // BEGIN KER FUN via ADDKERNEL");

        std::sprintf(strBuffer, "unsigned %s = blockIdx.x * blockDim.x + threadIdx.x;", loopVar);
        header.pushstr_newL(strBuffer);

        std::sprintf(strBuffer, "if(%s >= V) return;", loopVar);
        header.pushstr_newL(strBuffer);

        if (stmt->hasFilterExpr()) {
            blockStatement* changedBody = IncludeIfToBlock(stmt);
            std::cout << "============CHANGED BODY  TYPE==============" << (changedBody->getTypeofNode() == NODE_BLOCKSTMT);
            stmt->setBody(changedBody);
            // std::cout<<"FORALL BODY
            // TYPE"<<(forAll->getBody()->getTypeofNode()==NODE_BLOCKSTMT);
        }

        statement* body = stmt->getBody();
        assert(body->getTypeofNode() == NODE_BLOCKSTMT);
        blockStatement* block = (blockStatement*)body;
        list<statement*> statementList = block->returnStatements();


        for (statement* stmt : statementList) {
            GenerateStatement(stmt, false);  //false. All these stmts should be inside kernel
                                            //~ if (stmt->getTypeofNode() == NODE_FORALLSTMT) {
        }

        header.pushStringWithNewLine("} // end KER FUNC");
        header.NewLine();
        forAllSrcVariable = "";
    }

    blockStatement* DslCppGenerator::IncludeIfToBlock(forallStmt* stmt) {
        Expression* filterExpr = stmt->getfilterExpr();
        Expression* modifiedFilterExpr = filterExpr;
        if (filterExpr->getExpressionFamily() == EXPR_RELATIONAL) {
            Expression* expr1 = filterExpr->getLeft();
            Expression* expr2 = filterExpr->getRight();
            if (expr1->isIdentifierExpr()) {
            /*if it is a nodeproperty, the filter is on the nodes that are iterated on
            One more check can be applied to check if the iterating type is a neigbor
            iteration or allgraph iterations.
            */
                if (expr1->getId()->getSymbolInfo() != NULL) {
                    if (expr1->getId()->getSymbolInfo()->getType()->isPropNodeType()) {
                        Identifier* iterNode = stmt->getIterator();
                        Identifier* nodeProp = expr1->getId();
                        PropAccess* propIdNode = (PropAccess*)Util::createPropIdNode(iterNode, nodeProp);
                        Expression* propIdExpr = Expression::nodeForPropAccess(propIdNode);
                        modifiedFilterExpr = (Expression*)Util::createNodeForRelationalExpr(propIdExpr, expr2, filterExpr->getOperatorType());
                    }
                }
            }
        }
        ifStmt* ifNode = (ifStmt*)Util::createNodeForIfStmt(modifiedFilterExpr,
                                                            stmt->getBody(), NULL);
        blockStatement* newBlock = new blockStatement();
        newBlock->setTypeofNode(NODE_BLOCKSTMT);
        newBlock->addStmtToBlock(ifNode);
        return newBlock;
    }

    void DslCppGenerator::GenerateForAllSignature(forallStmt* stmt, bool isMainFile) {
        dslCodePad& targetFile = isMainFile ? main : header;

        char strBuffer[1024];
        Identifier* iterator = stmt->getIterator();
        if (stmt->isSourceProcCall()) {
            proc_callExpr* extractElemFunc = stmt->getExtractElementFunc();
            Identifier* iteratorMethodId = extractElemFunc->getMethodId();
            if (AllGraphIteration(iteratorMethodId->getIdentifier())) {

            } else if (NeighbourIteration(iteratorMethodId->getIdentifier())) {
                char* methodId = iteratorMethodId->getIdentifier();
                string s(methodId);
                if (s.compare("neighbors") == 0) {
                    list<argument*> argList = extractElemFunc->getArgList();
                    assert(argList.size() == 1);
                    std::sprintf(strBuffer, "for (%s %s = %s[%s]; %s < %s[%s+1]; %s++) { // FOR NBR ITR ", "int", "edge", "dOffsetArray", "v", "edge", "dOffsetArray", "v", "edge");
                    isForwardBfsLoop = true;
                    targetFile.pushstr_newL(strBuffer);
                    //~ targetFile.pushString("{");
                    std::sprintf(strBuffer, "%s %s = %s[%s];", "int", iterator->getIdentifier(), "dEdgelist", "edge");  //needs to move the addition of
                    targetFile.pushstr_newL(strBuffer);
                    forAllNbrVariable = iterator->getIdentifier();
                }
                if (s.compare("nodes_to") == 0)  //for pageRank
                {
                    list<argument*> argList = extractElemFunc->getArgList();
                    assert(argList.size() == 1);
                    Identifier* nodeNbr = argList.front()->getExpr()->getId();
                    std::sprintf(strBuffer, "for (%s %s = %s[%s]; %s < %s[%s+1]; %s++)", "int", "edge", "dRevOffsetArray", nodeNbr->getIdentifier(), "edge", "dRevOffsetArray", nodeNbr->getIdentifier(), "edge");
                    isForwardBfsLoop = false;
                    targetFile.pushstr_newL(strBuffer);
                    targetFile.pushstr_newL("{");
                    std::sprintf(strBuffer, "%s %s = %s[%s] ;", "int", iterator->getIdentifier(), "dSrcList", "edge");  //needs to move the addition of
                    targetFile.pushstr_newL(strBuffer);
                }  //statement to  a different method.
            }
        }
        else if (stmt->isSourceField()) {
            std::cout<<"Source Field\n";
        } 
        else if(stmt->isSourceExpr()){
  
            Expression* expr = stmt->getSourceExpr();
            Expression* mapExpr = expr->getMapExpr();
            Identifier* mapId = mapExpr->getId();

            cout<<"ENTERED................................................."<<"\n";

            if(mapId->getSymbolInfo()->getType()->gettypeId() == TYPE_CONTAINER){
                main.pushString("for(int i = 0 ; i < ");
                GenerateExpression(expr, isMainFile);
                main.pushstr_newL(".size() ; i++)");
                main.pushString("{\n");
                sprintf(strBuffer, "int %s = ", iterator->getIdentifier());
                main.pushString(strBuffer);
                GenerateExpression(expr, isMainFile);
                main.pushstr_newL("[i];\n");
            } 
            
            // cout<<"REACHED HERE AFTER COMPLETING "<<"\n";
            
        }
        else {
            Identifier* sourceId = stmt->getSource();
            if (sourceId != NULL) {
                std::cout<<"CHECK\n";
                if (sourceId->getSymbolInfo()->getType()->gettypeId() == TYPE_SETN) {  //FOR SET
                    main.pushStringWithNewLine("std::set<int>::iterator itr;");
                    std::sprintf(strBuffer, "for(itr=%s.begin();itr!=%s.end();itr++) ", sourceId->getIdentifier(), sourceId->getIdentifier());
                    main.pushstr_newL(strBuffer);
                }
                else if(sourceId->getSymbolInfo()->getType()->gettypeId() == TYPE_CONTAINER){
                    sprintf(strBuffer,"\nfor(int i = 0 ; i < %s.size() ; i++)",sourceId->getIdentifier());
                    main.pushstr_newL(strBuffer); 
                    main.pushString("{\n");
                    sprintf(strBuffer, "int %s = %s[i];\n", iterator->getIdentifier(), sourceId->getIdentifier());
                    main.pushstr_newL(strBuffer); 

                }
            }
        }
    }

    bool DslCppGenerator::AllGraphIteration(char* methodId) {
        string methodString(methodId);

        return (methodString == "nodes" || methodString == "edges");
    }

    bool DslCppGenerator::NeighbourIteration(char* methodId) {
        string methodString(methodId);
        return (methodString == "neighbors" || methodString == "nodes_to");
    }

    void DslCppGenerator::GenerateInitkernelStr(const char* inVarType, const char* inVarName, const char* initVal) {
        char strBuffer[1024];
        std::sprintf(strBuffer, "initKernel<%s> <<<numBlocks,threadsPerBlock>>>(V, %s, %s);", inVarType, inVarName, initVal);
        main.pushstr_newL(strBuffer);
        generateInitKernel = true;
    }

    // void DslCppGenerator::AddHipBFS2IterationLoop(iterateBFS2* bfsAbstraction, bool generateRevBfs) {
        
    //     main.pushstr_newL("finished2 = true;");  // these vars are BFS specific
    //     GenerateHipMemcpyStr("dFinished2", "&finished2", "bool", "1");

    //     main.NewLine();
    //     main.pushstr_newL("//Kernel LAUNCH");
    //     main.pushString("fwd_pass2<<<numBlocks,threadsPerBlock>>>(V, dOffsetArray, dEdgelist, dRevOffsetArray, dSrcList, dWeight");
    //     if(generateRevBfs)
    //         main.pushString(", dLevel2, dHopsFromSource2");
    //     main.pushString(", dFinished2, dVisitBfs2");

    //     GeneratePropParams(GetCurrentFunction()->getParamList(), false, true);  // true: typeneed false:inMainfile

    //     usedVariables usedVars = getVarsBFS(bfsAbstraction);
    //     list<Identifier*> vars = usedVars.getVariables();

    //     for (Identifier* iden : vars) {
    //         Type* type = iden->getSymbolInfo()->getType();
    //         if (type->isPropType()) {
    //             std::string parameterName = CapitalizeFirstLetter(std::string(iden->getIdentifier()));
    //             parameterName = ", d" + parameterName;
    //             main.pushString(/*createParamName(*/ parameterName);
    //         }
    //     }

    //     main.pushstr_newL("); ///DONE from varList");

    //     AddHipBFS2IterKernel(bfsAbstraction, generateRevBfs);  // KERNEL BODY!!!

    //     main.pushstr_newL("hipDeviceSynchronize();");
    //     if(generateRevBfs) {
    //         main.pushstr_newL("++hopsFromSource2; // updating the level to process in the next iteration");
    //     }
        
    // }

    void DslCppGenerator::AddHipBFSIterationLoop(iterateBFS* bfsAbstraction, bool generateRevBfs) {
        
        main.pushstr_newL("finished = true;");  // these vars are BFS specific
        GenerateHipMemcpyStr("dFinished", "&finished", "bool", "1");

        main.NewLine();
        main.pushstr_newL("//Kernel LAUNCH");
        main.pushString("fwd_pass<<<numBlocks,threadsPerBlock>>>(V, dOffsetArray, dEdgelist, dRevOffsetArray, dSrcList, dWeight");
        if(generateRevBfs)
            main.pushString(", dLevel, dHopsFromSource");
        main.pushString(", dFinished, dVisitBfs");

        GeneratePropParams(GetCurrentFunction()->getParamList(), false, true);  // true: typeneed false:inMainfile

        usedVariables usedVars = getVarsBFS(bfsAbstraction);
        list<Identifier*> vars = usedVars.getVariables();

        for (Identifier* iden : vars) {
            Type* type = iden->getSymbolInfo()->getType();
            if (type->isPropType()) {
                std::string parameterName = CapitalizeFirstLetter(std::string(iden->getIdentifier()));
                parameterName = ", d" + parameterName;
                main.pushString(/*createParamName(*/ parameterName);
            }
        }

        main.pushstr_newL("); ///DONE from varList");

        AddHipBFSIterKernel(bfsAbstraction, generateRevBfs);  // KERNEL BODY!!!
        main.pushstr_newL("hipDeviceSynchronize();");
        if(generateRevBfs) {
            main.pushstr_newL("++hopsFromSource; // updating the level to process in the next iteration");
        }
    }

    void DslCppGenerator::AddHipRevBFSIterationLoop(iterateBFS* stmt) {
        main.NewLine();
        main.pushstr_newL("hopsFromSource--;");
        GenerateHipMemcpyStr("dHopsFromSource", "&hopsFromSource", "int", "1");
        main.NewLine();
    }

    // void DslCppGenerator::AddHipRevBFS2IterationLoop(iterateBFS2* stmt) {
    //     main.NewLine();
    //     main.pushstr_newL("hopsFromSource2--;");
    //     GenerateHipMemcpyStr("dHopsFromSource2", "&hopsFromSource2", "int", "1");
    //     main.NewLine();
    // }

    void DslCppGenerator::AddHipBFSIterKernel(iterateBFS* bfsAbstraction, bool generateRevBfs) {
        const char* loopVar = "v";
        char strBuffer[1024];

        statement* body = bfsAbstraction->getBody();
        assert(body->getTypeofNode() == NODE_BLOCKSTMT);
        blockStatement* block = (blockStatement*)body;
        list<statement*> statementList = block->returnStatements();

        header.pushString("__global__ void fwd_pass(int n, int* dOffsetArray, int* dEdgelist, int* dRevOffsetArray, int* SrcList, int* dWeight");
        if(generateRevBfs)
            header.pushString(", int* dLevel, int* dHopsFromSource");
        header.pushString(", bool* dFinished, bool* dVisitBfs");

        GeneratePropParams(GetCurrentFunction()->getParamList(), true, false);  // true: typeneed false:inMainfile

        usedVariables usedVars = getVarsBFS(bfsAbstraction);
        list<Identifier*> vars = usedVars.getVariables();

        for (Identifier* iden : vars) {
            Type* type = iden->getSymbolInfo()->getType();
            if (type->isPropType()) {
                
                std::string parameterName = CapitalizeFirstLetter(std::string(iden->getIdentifier()));
                parameterName = ", " + ConvertToCppType(type) + " d" + parameterName;
                header.pushString(/*createParamName(*/ parameterName);
            }
        }

        header.pushstr_newL(") {");

        std::sprintf(strBuffer, "unsigned %s = blockIdx.x * blockDim.x + threadIdx.x;", loopVar);
        header.pushstr_newL(strBuffer);

        std::sprintf(strBuffer, "if(%s >= n) return;", loopVar);
        header.pushstr_newL(strBuffer);
        if(generateRevBfs) {
            std::sprintf(strBuffer, "if(dLevel[%s] == *dHopsFromSource) {", loopVar);
            header.pushstr_newL(strBuffer);
        }

        for (statement* stmt : statementList) {
            GenerateStatement(stmt, false);  //false. All these stmts should be inside kernel
        }

        if(generateRevBfs)
            header.pushstr_newL("} // end if d lvl");

        header.pushStringWithNewLine("dVisitBfs[v] = true;");
        if(isForwardBfsLoop) {
            header.pushStringWithNewLine("for (int edge = dOffsetArray[v]; edge < dOffsetArray[v+1]; edge++) { // FOR NBR ITR ");
            header.pushStringWithNewLine("int w = dEdgelist[edge];");
            header.pushStringWithNewLine("if (dVisitBfs[w] == false) {");
            header.pushStringWithNewLine("*dFinished = false;");
            header.pushStringWithNewLine("dVisitBfs[w] = true;");
            header.pushStringWithNewLine("}");
            header.pushStringWithNewLine("}");
        }
        else {
            header.pushStringWithNewLine("for (int edge = dRevOffsetArray[v]; edge < dRevOffsetArray[v+1]; edge++) { // FOR NBR REV ITR ");
            header.pushStringWithNewLine("int w = dSrcList[edge];");
            header.pushStringWithNewLine("if (dVisitBfs[w] == false) {");
            header.pushStringWithNewLine("*dFinished = false;");
            header.pushStringWithNewLine("dVisitBfs[w] = true;");
            header.pushStringWithNewLine("}");
            header.pushStringWithNewLine("}");
        }
        header.pushstr_newL("} // kernel end");
        header.NewLine();
    }

    // void DslCppGenerator::AddHipBFS2IterKernel(iterateBFS2* bfsAbstraction, bool generateRevBfs) {
    //     const char* loopVar = "v";
    //     char strBuffer[1024];

    //     statement* body = bfsAbstraction->getBody();
    //     assert(body->getTypeofNode() == NODE_BLOCKSTMT);
    //     blockStatement* block = (blockStatement*)body;
    //     list<statement*> statementList = block->returnStatements();

    //     header.pushString("__global__ void fwd_pass2(int n, int* dOffsetArray, int* dEdgelist, int* dRevOffsetArray, int* dSrcList, int* dWeight");
    //     if(generateRevBfs)
    //         header.pushString(", int* dLevel2, int* dHopsFromSource2");
    //     header.pushString(", bool* dFinished2, bool* dVisitBfs2");

    //     GeneratePropParams(GetCurrentFunction()->getParamList(), true, false);  // true: typeneed false:inMainfile

    //     usedVariables usedVars = getVarsBFS(bfsAbstraction);
    //     list<Identifier*> vars = usedVars.getVariables();

    //     for (Identifier* iden : vars) {
    //         Type* type = iden->getSymbolInfo()->getType();
    //         if (type->isPropType()) {
    //             std::string parameterName = CapitalizeFirstLetter(std::string(iden->getIdentifier()));
    //             parameterName = ", " + ConvertToCppType(type) + " d" + parameterName;
    //             header.pushString(/*createParamName(*/ parameterName);
    //         }
    //     }

    //     header.pushstr_newL(") {");

    //     std::sprintf(strBuffer, "unsigned %s = blockIdx.x * blockDim.x + threadIdx.x;", loopVar);
    //     header.pushstr_newL(strBuffer);

    //     std::sprintf(strBuffer, "if(%s >= n) return;", loopVar);
    //     header.pushstr_newL(strBuffer);
    //     if(generateRevBfs) {
    //         std::sprintf(strBuffer, "if(dLevel2[%s] == *dHopsFromSource2) {", loopVar);
    //         header.pushstr_newL(strBuffer);
    //     }
    //     header.pushStringWithNewLine("//Before for loop");
    //     for (statement* stmt : statementList) {
    //         header.pushStringWithNewLine("//Generating a statement");
    //         GenerateStatement(stmt, false);  //false. All these stmts should be inside kernel
    //     }
    //     header.pushStringWithNewLine("//After for loop");
    //     if(generateRevBfs)
    //         header.pushstr_newL("} // end if d lvl");

    //     header.pushStringWithNewLine("dVisitBfs2[v] = true;");
    //     if(isForwardBfsLoop) {
    //         header.pushStringWithNewLine("for (int edge = dOffsetArray[v]; edge < dOffsetArray[v+1]; edge++) { // FOR NBR ITR ");
    //         header.pushStringWithNewLine("int w = dEdgelist[edge];");
    //         header.pushStringWithNewLine("if (dVisitBfs2[w] == false) {");
    //         header.pushStringWithNewLine("*dFinished2 = false;");
    //         header.pushStringWithNewLine("dVisitBfs[w] = true;");
    //         header.pushStringWithNewLine("}");
    //         header.pushStringWithNewLine("}");
    //     }
    //     else {
    //         header.pushStringWithNewLine("for (int edge = dRevOffsetArray[v]; edge < dRevOffsetArray[v+1]; edge++) { // FOR NBR REV ITR ");
    //         header.pushStringWithNewLine("int w = dSrcList[edge];");
    //         header.pushStringWithNewLine("if (dVisitBfs2[w] == false) {");
    //         header.pushStringWithNewLine("*dFinished2 = false;");
    //         header.pushStringWithNewLine("dVisitBfs2[w] = true;");
    //         header.pushStringWithNewLine("}");
    //         header.pushStringWithNewLine("}");
    //     }
    //     header.pushstr_newL("} // kernel end");
    //     header.NewLine();
    // }

    void DslCppGenerator::AddHipRevBFSIterKernel(list<statement*>& statementList, iterateBFS* bfsAbstraction) {
        const char* loopVar = "v";
        char strBuffer[1024];

        std::sprintf(strBuffer, "__global__ void back_pass(int n, int* dOffsetArray, int* dEdgelist, int* dWeight, int* dLevel, int* dHopsFromSource, bool* dFinished");
        header.pushString(strBuffer);

        GeneratePropParams(GetCurrentFunction()->getParamList(), true, false);  // true: typeneed false:inMainfile

        usedVariables usedVars = getVarsBFS(bfsAbstraction);
        list<Identifier*> vars = usedVars.getVariables();

        for (Identifier* iden : vars) {
            Type* type = iden->getSymbolInfo()->getType();
            if (type->isPropType()) {
                std::string parameterName = CapitalizeFirstLetter(std::string(iden->getIdentifier()));
                parameterName = ", " + ConvertToCppType(type) + " d" + parameterName;
                header.pushString(/*createParamName(*/ parameterName);
            }
        }

        header.pushstr_newL(") {");

        std::sprintf(strBuffer, "unsigned %s = blockIdx.x * blockDim.x + threadIdx.x;", loopVar);
        header.pushstr_newL(strBuffer);

        std::sprintf(strBuffer, "if(%s >= n) return;", loopVar);
        header.pushstr_newL(strBuffer);
        header.pushstr_newL("auto grid = cooperative_groups::this_grid();");

        std::sprintf(strBuffer, "if(dLevel[%s] == *dHopsFromSource-1) {", loopVar);
        header.pushstr_newL(strBuffer);

        for (statement* stmt : statementList) {
            GenerateStatement(stmt, false);  //false. All these stmts should be inside kernel
        }

        header.pushstr_newL("} // end if d lvl");
        header.pushstr_newL("} // kernel end");
        header.NewLine();
    }

    // void DslCppGenerator::AddHipRevBFS2IterKernel(list<statement*>& statementList, iterateBFS2* bfsAbstraction) {
    //     const char* loopVar = "v";
    //     char strBuffer[1024];

    //     std::sprintf(strBuffer, "__global__ void back_pass2(int n, int* dRevOffsetArray, int* dEdgelist, int* dWeight, int* dLevel2, int* dHopsFromSource2, bool* dFinished2");
    //     header.pushString(strBuffer);

    //     GeneratePropParams(GetCurrentFunction()->getParamList(), true, false);  // true: typeneed false:inMainfile

    //     usedVariables usedVars = getVarsBFS(bfsAbstraction);
    //     list<Identifier*> vars = usedVars.getVariables();

    //     for (Identifier* iden : vars) {
    //         Type* type = iden->getSymbolInfo()->getType();
    //         if (type->isPropType()) {
    //             std::string parameterName = CapitalizeFirstLetter(std::string(iden->getIdentifier()));
    //             parameterName = ", " + ConvertToCppType(type) + " d" + parameterName;
    //             header.pushString(/*createParamName(*/ parameterName);
    //         }
    //     }

    //     header.pushstr_newL(") {");

    //     std::sprintf(strBuffer, "unsigned %s = blockIdx.x * blockDim.x + threadIdx.x;", loopVar);
    //     header.pushstr_newL(strBuffer);

    //     std::sprintf(strBuffer, "if(%s >= n) return;", loopVar);
    //     header.pushstr_newL(strBuffer);
    //     header.pushstr_newL("auto grid = cooperative_groups::this_grid();");

    //     std::sprintf(strBuffer, "if(dLevel2[%s] == *dHopsFromSource2-1) {", loopVar);
    //     header.pushstr_newL(strBuffer);

    //     for (statement* stmt : statementList) {
    //         GenerateStatement(stmt, false);  //false. All these stmts should be inside kernel
    //     }

    //     header.pushstr_newL("} // end if d lvl");
    //     header.pushstr_newL("} // kernel end");
    //     header.NewLine();
    // }

    void DslCppGenerator::GeneratePropParams(list<formalParam*> paramList, bool isNeedType = true, bool isMainFile = true) {
        list<formalParam*>::iterator itr;
        dslCodePad& targetFile = isMainFile ? main : header;
        //~ Assumes that there is at least one param already. so prefix with  "," is okay
        char strBuffer[1024];
        for (itr = paramList.begin(); itr != paramList.end(); itr++) {
            Type* type = (*itr)->getType();
            if (type->isPropType()) {
                if (type->getInnerTargetType()->isPrimitiveType()) {
                    const char* temp = "d";
                    char* temp1 = CapitalizeFirstLetter((*itr)->getIdentifier()->getIdentifier());
                    char* temp2 = (char*)malloc(1 + strlen(temp) + strlen(temp1));
                    strcpy(temp2, temp);
                    strcat(temp2, temp1);

                    if (isNeedType)
                        std::sprintf(strBuffer, ",%s* %s", ConvertToCppType(type->getInnerTargetType()).c_str(), temp2);
                    else
                        std::sprintf(strBuffer, ",%s", temp2);
                    targetFile.pushString(strBuffer);
                }
            }
        }
    }

    void DslCppGenerator::CastIfRequired(Type* type, Identifier* methodID,
                                       dslCodePad& main) {
    /* This needs to be made generalized, extended for all predefined function,
        made available by the DSL*/
        string predefinedFunc("num_nodes");
        if (predefinedFunc.compare(methodID->getIdentifier()) == 0) {
            if (type->gettypeId() != TYPE_INT) {
                char strBuffer[1024];
                std::sprintf(strBuffer, "(%s)", ConvertToCppType(type).c_str());
                main.pushString(strBuffer);
            }
        }
    }

    void DslCppGenerator::GenerateInitIndex() {
        header.pushStringWithNewLine("template <typename T>");
        header.pushStringWithNewLine("__global__ void initIndex(int V, T* init_array, int s, T init_value) {  // intializes an index 1D array with init val");
        header.pushStringWithNewLine("if (s < V) {  // bound check");
        header.pushStringWithNewLine("init_array[s] = init_value;");
        header.pushStringWithNewLine("}");
        header.pushStringWithNewLine("}");
    }

    void DslCppGenerator::GenerateInitKernel() {

        header.pushStringWithNewLine("template <typename T>");
        header.pushStringWithNewLine("__global__ void initKernel(unsigned V, T* init_array, T init_value) {  // intializes one 1D array with init val");
        header.pushStringWithNewLine("unsigned id = threadIdx.x + blockDim.x * blockIdx.x;");
        header.pushStringWithNewLine("if (id < V) {");
        header.pushStringWithNewLine("init_array[id] = init_value;");
        header.pushStringWithNewLine("}");
        header.pushStringWithNewLine("}");
    }

    bool DslCppGenerator::GenerateVariableDeclGetEdge(declaration *declStmt, bool isMainFile){
        dslCodePad &targetFile = isMainFile ? main : header;
        char strBuffer[1024];
        if(declStmt->isInitialized() && declStmt->getExpressionAssigned()->isProcCallExpr()){
            Expression* expr = declStmt->getExpressionAssigned();
            proc_callExpr *proc = (proc_callExpr *)expr;
            string methodId(proc->getMethodId()->getIdentifier());
            if(methodId == "get_edge"){

                list<argument *> argList = proc->getArgList();
                assert(argList.size() == 2);
                char* srcId = argList.front()->getExpr()->getId()->getIdentifier();
                char *destId = argList.back()->getExpr()->getId()->getIdentifier(); 

                targetFile.pushstr_newL("");
                targetFile.pushstr_newL("// Traversing for get_edge");

                const char *varType = ConvertToCppType(declStmt->getType()).c_str();
                const char *varName = declStmt->getdeclId()->getIdentifier();

                std::sprintf(strBuffer, "%s %s = 0;", varType, varName);
                targetFile.pushstr_newL(strBuffer);

                if(isMainFile){
                    std::sprintf(strBuffer, "for(int edge=hOffsetArray[%s]; edge<hOffsetArray[%s+1]; edge++){", srcId, srcId);
                    targetFile.pushstr_newL(strBuffer);
                    std::sprintf(strBuffer, "if(hEdgelist[edge] == %s){", destId);
                    targetFile.pushstr_newL(strBuffer);
                }
                else{
                    std::sprintf(strBuffer, "for(int edge=dOffsetArray[%s]; edge<dOffsetArray[%s+1]; edge++){", srcId, srcId);
                    targetFile.pushstr_newL(strBuffer);
                    std::sprintf(strBuffer, "if(dEdgelist[edge] == %s){", destId);
                    targetFile.pushstr_newL(strBuffer);
                }
                std::sprintf(strBuffer, "%s = edge;", varName);
                targetFile.pushstr_newL(strBuffer);
                targetFile.pushstr_newL("}");
                targetFile.pushstr_newL("}");
                targetFile.pushstr_newL("");
                return true;
            }
        }
        return false;
    }
}
