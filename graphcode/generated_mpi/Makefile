# Example usage: make bc_dslV2.out

# Define boost directories
# INCDIR = /lfs/usrhome/oth/rnintern/scratch/MPI_Comparison/boost/install_dir/include
# LIBDIR = /lfs/usrhome/oth/rnintern/scratch/MPI_Comparison/boost/install_dir/lib

# Set Boost Include and Boost library directory (Assuming boost is installed one directory above StarPlat root directory)
INCDIR = ../../../boost_1_85_0/include
LIBDIR = ../../../boost_1_85_0/lib


# Define compiler and flags
OMPI_CXX=g++ # Tells OpenMPI which compiler to use if there's more than one compiler (Comment out if not required)
CXX = mpicxx
CXXFLAGS = -g -std=c++17 -I$(INCDIR)
LDFLAGS = -L$(LIBDIR)

# Boost libraries
LIBS = $(LIBDIR)/libboost_mpi.a $(LIBDIR)/libboost_serialization.a
# LIBS = $(LIBDIR)/libboost_mpi-mt.a $(LIBDIR)/libboost_serialization-mt.a

MPI_SRC = ../mpi_header/graph_mpi.cc ../mpi_header/updates.cc \
       ../mpi_header/graph_properties/node_property/node_property.cc \
       ../mpi_header/graph_properties/edge_property/edge_property.cc \
       ../mpi_header/rma_datatype/rma_datatype.cc

all: bc_dslV2.out

bc_dslV2.out: bc_dslV2_test.cc bc_dslV2.cc bc_dslV2.h $(MPI_SRC)
	$(CXX) $(CXXFLAGS) bc_dslV2_test.cc bc_dslV2.cc $(MPI_SRC) $(LDFLAGS) $(LIBS) -o output/bc_dslV2.out

clean:
	rm -f -r output/*.out
