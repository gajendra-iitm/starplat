EXPENDABLES_WINDOWS = bin/MainContextj.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o parser/y.tab.c parser/lex.yy.c bin/deviceVarsAnalyser.o bin/analyserUtil.o bin/dv*.o bin/bv*.o
EXPENDABLES = bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o parser/y.tab.c parser/lex.yy.c  bin/deviceVarsAnalyser.o bin/blockVarsAnalyser.o
ANALYSER = bin/dataRaceAnalyser.o
PROGRAMS = bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/cu_dsl_cpp_generator.o  bin/ac_dsl_cpp_generator.o bin/om_dsl_cpp_generator.o bin/hi_dsl_cpp_generator.o bin/hi_get_used_data.o bin/hi_auxillary_functions.o bin/hip_gen_functions.o bin/mpi_dsl_cpp_generator.o bin/om_dyn_dsl_cpp_generator.o bin/mpi_dsl_cpp_generator_helper.o bin/mpi_dsl_cpp_expression_generator.o bin/mpi_dsl_cpp_statement_generator.o bin/mpi_dsl_dyn_cpp_generator.o bin/sycl_dsl_cpp_generator.o bin/multigpu_dsl_cpp_generator.o  bin/amd_dsl_cpp_generator.o bin/bAnalyzer.o
#bin/ac_dsl_cpp_generator.o 
#bin/om_dsl_cpp_generator.o
A_PROGRAM = bin/attachPropAnalyser.o bin/dataRaceAnalyser.o bin/deviceVarsAnalyser.o bin/blockVarsAnalyser.o bin/analyserUtil.o bin/pushpullAnalyser.o bin/callGraphAnalyser.o bin/cudaGlobalVariablesAnalyser.o

CFLAGS  = -g 
CC = g++ -std=c++11 #-Wall # -Wno-unused-variable #-fsanitize=address

all: backendsdir starplat

#~ openACC: generator_openACC starplat_openACC clean

#~ starplat_openACC: bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o $(A_PROGRAM)
#~ 	$(CC) bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o $(A_PROGRAM) -o starplat



#~ cuda: generator_cuda starplat_cuda clean_windows

#~ starplat_cuda: bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o $(A_PROGRAM)
#~ 	$(CC) bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o $(A_PROGRAM) -o starplat


#amd
bin/amd_dsl_cpp_generator.o: backends/backend_amd/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_amd/dsl_cpp_generator.cpp -o bin/amd_dsl_cpp_generator.o


#multigpu
bin/multigpu_dsl_cpp_generator.o : backends/backend_multigpu/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_multigpu/dsl_cpp_generator.cpp -o bin/multigpu_dsl_cpp_generator.o


#CUDA
bin/cu_dsl_cpp_generator.o: backends/backend_cuda/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_cuda/dsl_cpp_generator.cpp -o bin/cu_dsl_cpp_generator.o

#openACC
bin/ac_dsl_cpp_generator.o: backends/backend_openACC/dsl_cpp_generator.cpp backends/dslCodePad.h backends/backend_openACC/getMetaDataUsed.cpp
	$(CC) $(CFLAGS) -c backends/backend_openACC/dsl_cpp_generator.cpp -o bin/ac_dsl_cpp_generator.o
	
#omp
bin/om_dsl_cpp_generator.o: backends/backend_omp/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_omp/dsl_cpp_generator.cpp -o bin/om_dsl_cpp_generator.o

#hip

bin/hi_get_used_data.o: backends/backend_hip/get_used_data.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_hip/get_used_data.cpp -o bin/hi_get_used_data.o

bin/hi_auxillary_functions.o: backends/backend_hip/auxillary_functions.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_hip/auxillary_functions.cpp -o bin/hi_auxillary_functions.o

bin/hip_gen_functions.o: backends/backend_hip/hip_gen_functions.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_hip/hip_gen_functions.cpp -o bin/hip_gen_functions.o

bin/hi_dsl_cpp_generator.o: backends/backend_hip/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_hip/dsl_cpp_generator.cpp -o bin/hi_dsl_cpp_generator.o


#mpi
bin/mpi_dsl_cpp_generator.o: backends/backend_mpi/dsl_cpp_generator.h backends/backend_mpi/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_mpi/dsl_cpp_generator.cpp -o bin/mpi_dsl_cpp_generator.o

bin/mpi_dsl_cpp_generator_helper.o: backends/backend_mpi/dsl_cpp_generator.h backends/backend_mpi/dsl_cpp_generator_helper.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_mpi/dsl_cpp_generator_helper.cpp -o bin/mpi_dsl_cpp_generator_helper.o

bin/mpi_dsl_cpp_expression_generator.o: backends/backend_mpi/dsl_cpp_generator.h backends/backend_mpi/dsl_cpp_expression_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_mpi/dsl_cpp_expression_generator.cpp -o bin/mpi_dsl_cpp_expression_generator.o

bin/mpi_dsl_cpp_statement_generator.o: backends/backend_mpi/dsl_cpp_generator.h backends/backend_mpi/dsl_cpp_statement_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_mpi/dsl_cpp_statement_generator.cpp -o bin/mpi_dsl_cpp_statement_generator.o

bin/mpi_dsl_dyn_cpp_generator.o : backends/backend_mpi/dsl_cpp_generator.h backends/backend_mpi/dsl_cpp_statement_generator.cpp backends/backend_mpi/dsl_dyn_cpp_generator.h backends/backend_mpi/dsl_dyn_cpp_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_mpi/dsl_dyn_cpp_generator.cpp -o bin/mpi_dsl_dyn_cpp_generator.o

bin/bAnalyzer.o : backends/backend_mpi/bAnalyzer/bAnalyzer.h backends/backend_mpi/bAnalyzer/bAnalyzer.cc 
	$(CC) $(CFLAGS) -c backends/backend_mpi/bAnalyzer/bAnalyzer.cc -o bin/bAnalyzer.o

#sycl
bin/sycl_dsl_cpp_generator.o: backends/backend_sycl/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_sycl/dsl_cpp_generator.cpp -o bin/sycl_dsl_cpp_generator.o

#dyn OMP
bin/om_dyn_dsl_cpp_generator.o: backends/backend_omp/dsl_dyn_cpp_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_omp/dsl_dyn_cpp_generator.cpp -o bin/om_dyn_dsl_cpp_generator.o


backendsdir:
	mkdir -p bin
	mkdir -p ../graphcode/generated_omp/
	mkdir -p ../graphcode/generated_mpi/
	mkdir -p ../graphcode/generated_cuda/
	mkdir -p ../graphcode/generated_openacc/
	mkdir -p ../graphcode/generated_hip/
	mkdir -p ../graphcode/generated_sycl/
	mkdir -p ../graphcode/generated_amd/
	mkdir -p ../graphcode/generated_multigpu/


starplat: $(PROGRAMS) $(A_PROGRAM)
	$(CC)  $(PROGRAMS) $(A_PROGRAM) -o StarPlat

#~ bin/SymbolTableBuilder.o: symbolutil/SymbolTableBuilder.cpp
#~ 	$(CC) -c symbolutil/SymbolTableBuilder.cpp -o bin/SymbolTableBuilder.o

bin/MainContext.o: maincontext/MainContext.cpp maincontext/MainContext.hpp
	$(CC) $(CFLAGS) -c maincontext/MainContext.cpp -o bin/MainContext.o

bin/ASTHelper.o: ast/ASTHelper.cpp ast/ASTNode.hpp 
	$(CC) $(CFLAGS) -c ast/ASTHelper.cpp -o bin/ASTHelper.o

bin/SymbolTableBuilder.o: symbolutil/SymbolTableBuilder.cpp symbolutil/SymbolTableBuilder.h
	$(CC) $(CFLAGS) -c symbolutil/SymbolTableBuilder.cpp -o bin/SymbolTableBuilder.o

#~ bin/dsl_cpp_generator.o: backends/backend_omp/dsl_cpp_generator.cpp backends/dslCodePad.h
#~ 	$(CC) -c backends/backend_omp/dsl_cpp_generator.cpp -o bin/dsl_cpp_generator.o


bin/SymbolTableNew.o: symbolutil/SymbolTableNew.cpp  symbolutil/SymbolTable.h 
	$(CC) $(CFLAGS) -c symbolutil/SymbolTableNew.cpp -o bin/SymbolTableNew.o


bin/attachPropAnalyser.o: analyser/attachProp/attachPropAnalyser.cpp analyser/attachProp/attachPropAnalyser.h
	$(CC) $(CFLAGS) -c analyser/attachProp/attachPropAnalyser.cpp -o bin/attachPropAnalyser.o

bin/dataRaceAnalyser.o: analyser/dataRace/dataRaceAnalyser.cpp analyser/dataRace/dataRaceAnalyser.h
	$(CC) $(CFLAGS) -c analyser/dataRace/dataRaceAnalyser.cpp -o bin/dataRaceAnalyser.o                  

bin/pushpullAnalyser.o: analyser/pushpull/pushpullAnalyser.cpp analyser/pushpull/pushpullAnalyser.h
	$(CC) $(CFLAGS) -c analyser/pushpull/pushpullAnalyser.cpp -o bin/pushpullAnalyser.o                  

bin/callGraphAnalyser.o: analyser/callGraph/callGraphAnalyser.cpp analyser/callGraph/callGraphAnalyser.h
	$(CC) $(CFLAGS) -c analyser/callGraph/callGraphAnalyser.cpp -o bin/callGraphAnalyser.o                  

bin/cudaGlobalVariablesAnalyser.o: analyser/cudaGlobalVariables/cudaGlobalVariablesAnalyser.cpp analyser/cudaGlobalVariables/cudaGlobalVariablesAnalyser.h
	$(CC) $(CFLAGS) -c analyser/cudaGlobalVariables/cudaGlobalVariablesAnalyser.cpp -o bin/cudaGlobalVariablesAnalyser.o

bin/deviceVarsAnalyser.o: bin/dv1.o bin/dv2.o bin/dv3.o bin/dv4.o bin/dv5.o
	ld -r bin/dv1.o bin/dv2.o bin/dv3.o bin/dv4.o bin/dv5.o -o bin/deviceVarsAnalyser.o 

bin/dv1.o: analyser/deviceVars/deviceVarsAnalyser.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/deviceVars/deviceVarsAnalyser.cpp -o bin/dv1.o

bin/dv2.o: analyser/deviceVars/deviceVarsInit.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/deviceVars/deviceVarsInit.cpp -o bin/dv2.o

bin/dv3.o: analyser/deviceVars/deviceVarsPrint.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/deviceVars/deviceVarsPrint.cpp -o bin/dv3.o

bin/dv4.o: analyser/deviceVars/deviceVarsTransfer.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC)  -c analyser/deviceVars/deviceVarsTransfer.cpp -o bin/dv4.o

bin/dv5.o: analyser/deviceVars/getUsedVars.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/deviceVars/getUsedVars.cpp -o bin/dv5.o

bin/blockVarsAnalyser.o: bin/bv1.o bin/bv2.o bin/bv3.o bin/bv4.o bin/bv5.o bin/bv6.o.o
	ld -r bin/bv1.o bin/bv2.o bin/bv3.o bin/bv4.o bin/bv5.o bin/bv6.o.o -o bin/blockVarsAnalyser.o

bin/bv1.o: analyser/blockVars/blockVarsAnalyser.cpp analyser/blockVars/blockVarsAnalyser.h
	$(CC) $(CFLAGS) -c analyser/blockVars/blockVarsAnalyser.cpp -o bin/bv1.o

bin/bv2.o: analyser/blockVars/blockVarsInit.cpp analyser/blockVars/blockVarsAnalyser.h
	$(CC) $(CFLAGS) -c analyser/blockVars/blockVarsInit.cpp -o bin/bv2.o

bin/bv3.o: analyser/blockVars/getUsedVars.cpp analyser/blockVars/blockVarsAnalyser.h
	$(CC) $(CFLAGS) -c analyser/blockVars/getUsedVars.cpp -o bin/bv3.o

bin/bv4.o: analyser/blockVars/NodeBlockData.cpp analyser/blockVars/blockVarsAnalyser.h
	$(CC) $(CFLAGS) -c analyser/blockVars/NodeBlockData.cpp -o bin/bv4.o

bin/bv5.o: analyser/blockVars/setVarsInParallel.cpp analyser/blockVars/blockVarsAnalyser.h
	$(CC) $(CFLAGS) -c analyser/blockVars/setVarsInParallel.cpp -o bin/bv5.o

bin/bv6.o.o: analyser/blockVars/analyserUtil.cpp analyser/blockVars/analyserUtil.h
	$(CC) $(CFLAGS) -c analyser/blockVars/analyserUtil.cpp -o bin/bv6.o.o

bin/analyserUtil.o: analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/analyserUtil.cpp -o bin/analyserUtil.o

#bin/dsl_cpp_generator.o: backends/backend_mpi/mpi_cpp_generator.cpp backends/dslCodePad.h
#	$(CC) -c backends/backend_mpi/mpi_cpp_generator.cpp -o bin/dsl_cpp_generator.o

bin/y.tab.o: parser/lexer.l parser/lrparser.y
	$(MAKE) -C parser

bin/lex.yy.o: parser/lexer.l parser/lrparser.y
	$(MAKE) -C parser

clean_windows:
	rm -f $(EXPENDABLES_WINDOWS)

clean:
#~ 	rm -f $(EXPENDABLES)
	rm -rf bin StarPlat
	rm -rf ../graphcode/generated_omp ../graphcode/generated_cuda ../graphcode/generated_mpi ../graphcode/generated_hip ../graphcode/generated_sycl ../graphcode/generated_openacc ../graphcode/generated_amd ../graphcode/generated_multigpu
