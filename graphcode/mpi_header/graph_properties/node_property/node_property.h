#ifndef MPI_NODE_PROPERTY_HEADER
#define MPI_NODE_PROPERTY_HEADER

#include "../property.h"
#include "../../rma_datatype/rma_datatype.h"
#include <unordered_set>
#include <vector>
#include <set>
#include <boost/functional/hash.hpp>

class Graph;

template <typename T>
class NodeProperty : public Property
{
private:
private:
    int length;

    bool attached_to_graph;
    int propertyId;
    Graph *graph;
    boost::mpi::communicator world;
    T *history_data;
    std::vector<bool> already_locked_processors;
    std::vector<bool> already_locked_processors_shared;
    std::vector<std::vector<std::pair<int32_t, T>>> reduction_queue;

    // TODO(Rohan): Experiment with better choices of data structures for the following buffer variables
    bool atomic_add_buffer_ready;
    bool write_buffer_ready;
    std::vector<std::vector<T>> atomic_add_buffer;
    std::vector<std::vector<T>> write_buffer;
    std::set<std::pair<int, int>> atomic_add_change_log;
    std::set<std::pair<int, int>> write_change_log;

    // std::unordered_set<std::pair<int,int>, boost::hash<std::pair<int,int>>> atomic_add_change_log;
    // std::unordered_set<std::pair<int,int>, boost::hash<std::pair<int,int>>> write_change_log;
public:
    Rma_Datatype<T> propList;
    void syncAtomicAddsAndWrites();
    void leaveAllSharedLocks();
    NodeProperty() : Property(true)
    {
        if constexpr (std::is_same_v<T, int>)
            propList.mpi_datatype = MPI_INT32_T;
        else if (std::is_same_v<T, long>)
            propList.mpi_datatype = MPI_INT64_T;
        else if (std::is_same_v<T, float>)
            propList.mpi_datatype = MPI_FLOAT;
        else if (std::is_same_v<T, double>)
            propList.mpi_datatype = MPI_DOUBLE;
        else if (std::is_same_v<T, bool>)
            propList.mpi_datatype = MPI_CXX_BOOL;
        else
        {
            std::cerr << "Invalid NodeProperty type";
            exit(-1);
        }
        attached_to_graph = false;
    }

    void initialize_reduction_queue() override;

    void add_to_reduction_queue(int reduction_node, std::pair<int32_t, T> pair);

    std::vector<std::vector<int32_t>> perform_reduction(MPI_Op op) override;

    void assign_reduction_values(std::vector<std::vector<int32_t>> &modified_ids) override;

    void attachToGraph(Graph *graph, T initial_value);
    void attachToGraph(Graph *graph, T *initial_values);

    int32_t getPropertyId() override;

    T getValue(int node_id, bool check_concurrency = true);

    void setValue(int node_id, T value, bool check_concurrency = true);

    void rememberHistory();

    T getHistoryValue(int node_id);

    void operator=(const NodeProperty<T> &property);

    void deleteHistory();

    bool aggregateValue(Aggregate_Op op);

    void getExclusiveLock(int node_id);

    void releaseExclusiveLock(int node_id);

    T compareAndSwap(int node_id, T compare, T swap);

    void atomicAdd(int node_id, T value);
};

#endif
