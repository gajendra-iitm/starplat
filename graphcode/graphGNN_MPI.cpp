#include "graph.hpp"
#include<mpi.h>
#include <boost/mpi.hpp>


void GCN_aggregate_mpi(GNN &gnn, int node, int layerNumber, boost::mpi::communicator world)
{
    graph &g = gnn.getGraph();
    std::vector<Layers> &layers = gnn.getLayers();
    std::vector<int32_t> y_true = gnn.getLabels();
    
    for (int i = 0; i < layers[layerNumber - 1].num_features; i++)
    {
        layers[layerNumber].aggregatedFeatures[node][i] = 0.0f;
    }

    for (int k = 0; k < g.getNeighbors(node).size(); k++)
    {
        auto edge = g.getNeighbors(node)[k];

            for (int i = 0; i < layers[layerNumber - 1].num_features; i++)
            {
                layers[layerNumber].aggregatedFeatures[node][i] += layers[layerNumber - 1].postActivatedFeatures[edge.destination][i] * edge.weight;
            }
    }
}

void GIN_aggregate_mpi(GNN &gnn, int node, int layerNumber, boost::mpi::communicator world)
{
    graph &g = gnn.getGraph();
    std::vector<Layers> &layers = gnn.getLayers();
    float epsilon = layers[layerNumber].epsilon;

    for (int i = 0; i < layers[layerNumber - 1].num_features; i++)
    {
        layers[layerNumber].aggregatedFeatures[node][i] = (1 + epsilon) * layers[layerNumber - 1].postActivatedFeatures[node][i];
    }

    for (int k = 0; k < g.getNeighbors(node).size(); k++)
    {
        auto edge = g.getNeighbors(node)[k];

        if (world.rank() == g.getProcess(edge.destination))
        {
            for (int i = 0; i < layers[layerNumber - 1].num_features; i++)
            {
                layers[layerNumber].aggregatedFeatures[node][i] += layers[layerNumber - 1].postActivatedFeatures[edge.destination][i];
            }
        }
        else
        {
            std::vector<float> receivedFeatures(layers[layerNumber - 1].num_features);
            boost::mpi::broadcast(world, receivedFeatures, g.getProcess(edge.destination));

            for (int i = 0; i < layers[layerNumber - 1].num_features; i++)
            {
                layers[layerNumber].aggregatedFeatures[node][i] += receivedFeatures[i];
            }
        }
    }
}


void forwardPass_mpi(GNN &gnn, int layerNumber, int aggtype, boost::mpi::communicator world)
{
    graph &g = gnn.getGraph();
    int startNode = g.start_node(world.rank());
    int endNode = g.end_node(world.rank());

    for (int node = startNode; node < endNode; node++)
    {
        if (layerNumber == 0)
        {
            return;
        }
        std::vector<Layers> &layers = gnn.getLayers();

        if (aggtype == 1)
            GCN_aggregate_mpi(gnn, node, layerNumber, world);
        else if (aggtype == 2)
            GIN_aggregate_mpi(gnn, node, layerNumber, world);

        for (int i = 0; i < layers[layerNumber].num_features; i++)
        {
            float sum = 0.0f;
            for (int j = 0; j < layers[layerNumber - 1].num_features; j++)
            {
                sum += layers[layerNumber].aggregatedFeatures[node][j] * layers[layerNumber].weights[j][i];
            }
            layers[layerNumber].preActivatedFeatures[node][i] = sum + layers[layerNumber].bias[i];
            layers[layerNumber].postActivatedFeatures[node][i] = tanh(layers[layerNumber].preActivatedFeatures[node][i]);
        }

        if (layerNumber == layers.size() - 1)
        {
            softmax(layers[layerNumber].postActivatedFeatures[node], layers[layerNumber].num_features, layers[layerNumber].postActivatedFeatures[node]);
        }
    }

    world.barrier();
}

void backPropagation_mpi(GNN &gnn, int layerNumber, int rank, int numProcesses)
{
    graph &g = gnn.getGraph();
    std::vector<Layers> &layers = gnn.getLayers();
    if (layerNumber == 0)
        return;

    std::vector<int32_t> y_true = gnn.getLabels();
    double **y_pred = layers[layers.size() - 1].preActivatedFeatures;
    
    double **global_grad_pre_act_output;
    if (rank == 0)
    {
        global_grad_pre_act_output = new double*[g.num_nodes()];
        for (int i = 0; i < g.num_nodes(); ++i)
            global_grad_pre_act_output[i] = new double[layers[layerNumber].num_features];
    }

    if (layerNumber == layers.size() - 1)
    {
        for (int i = rank; i < y_true.size(); i += numProcesses)
        {
            int c = y_true[i];  label of the node
            for (int j = 0; j < gnn.numClasses(); j++)
            {
                layers[layerNumber].grad_pre_act_output[i][j] = y_pred[i][j] - (c == j ? 1 : 0);
            }
        }
    }
    else
    {
        for (int nod = rank; nod < g.num_nodes(); nod += numProcesses)
        {
            for (int i = 0; i < layers[layerNumber].num_features; i++)
            {
                layers[layerNumber].grad_pre_act_output[nod][i] = 0;
                for (int j = 0; j < layers[layerNumber + 1].num_features; j++)
                {
                    layers[layerNumber].grad_pre_act_output[nod][i] +=
                        layers[layerNumber + 1].grad_pre_act_output[nod][j] * layers[layerNumber + 1].weights[i][j];
                }
            }
        }
    }

    for (int nod = 0; nod < g.num_nodes(); ++nod)
    {
        MPI_Reduce(layers[layerNumber].grad_pre_act_output[nod], global_grad_pre_act_output[nod], layers[layerNumber].num_features, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    if (rank == 0)
    {
        for (int nod = 0; nod < g.num_nodes(); nod++)
        {
            for (int i = 0; i < layers[layerNumber].num_features; i++)
            {
                if (gnn.initType() == 1) 
                    global_grad_pre_act_output[nod][i] *= derivative_tanh(layers[layerNumber].preActivatedFeatures[nod][i]);
                else if (gnn.initType() == 2) 
                    global_grad_pre_act_output[nod][i] *= derivative_relu(layers[layerNumber].preActivatedFeatures[nod][i]);
            }
        }
    }

    for (int nod = 0; nod < g.num_nodes(); ++nod)
    {
        MPI_Bcast(global_grad_pre_act_output[nod], layers[layerNumber].num_features, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    
}

void adamOptimizer_mpi(GNN &gnn, int epochNumber, double lr, double beta1, double beta2, double epsilon, double weight_decay, int rank, int numProcesses)
{
    int t = epochNumber;
    std::vector<Layers> &layers = gnn.getLayers();

    double **global_grad_weights;
    double *global_grad_bias;

    for (int layer = 1; layer < layers.size(); layer++)
    {
        if (rank == 0)
        {
            global_grad_weights = new double*[layers[layer - 1].num_features];
            for (int i = 0; i < layers[layer - 1].num_features; i++)
                global_grad_weights[i] = new double[layers[layer].num_features];

            global_grad_bias = new double[layers[layer].num_features];
        }

        for (int i = rank; i < layers[layer - 1].num_features; i += numProcesses)
        {
            for (int j = 0; j < layers[layer].num_features; j++)
            {
                layers[layer].m_weights[i][j] = beta1 * layers[layer].m_weights[i][j] + (1 - beta1) * layers[layer].grad_weights[i][j];
                layers[layer].v_weights[i][j] = beta2 * layers[layer].v_weights[i][j] + (1 - beta2) * layers[layer].grad_weights[i][j] * layers[layer].grad_weights[i][j];
            }
        }

        for (int i = 0; i < layers[layer - 1].num_features; i++)
        {
            MPI_Reduce(layers[layer].grad_weights[i], global_grad_weights[i], layers[layer].num_features, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        }

        for (int j = rank; j < layers[layer].num_features; j += numProcesses)
        {
            layers[layer].m_biases[j] = beta1 * layers[layer].m_biases[j] + (1 - beta1) * layers[layer].grad_bias[j];
            layers[layer].v_biases[j] = beta2 * layers[layer].v_biases[j] + (1 - beta2) * layers[layer].grad_bias[j] * layers[layer].grad_bias[j];
        }

        MPI_Reduce(layers[layer].grad_bias, global_grad_bias, layers[layer].num_features, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0)
        {
            for (int i = 0; i < layers[layer - 1].num_features; i++)
            {
                for (int j = 0; j < layers[layer].num_features; j++)
                {
                    double m_hat = layers[layer].m_weights[i][j] / (1 - pow(beta1, t));
                    double v_hat = layers[layer].v_weights[i][j] / (1 - pow(beta2, t));

                    layers[layer].weights[i][j] -= lr * (m_hat / (sqrt(v_hat) + epsilon) + weight_decay * layers[layer].weights[i][j]);
                }
            }

            for (int j = 0; j < layers[layer].num_features; j++)
            {
                double m_hat = layers[layer].m_biases[j] / (1 - pow(beta1, t));
                double v_hat = layers[layer].v_biases[j] / (1 - pow(beta2, t));

                layers[layer].bias[j] -= lr * (m_hat / (sqrt(v_hat) + epsilon));
            }
        }

        for (int i = 0; i < layers[layer - 1].num_features; i++)
        {
            MPI_Bcast(layers[layer].weights[i], layers[layer].num_features, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        MPI_Bcast(layers[layer].bias, layers[layer].num_features, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        
    }
}

