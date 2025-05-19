/**
 * @file JV_GNN_Main.cpp
 * @brief This is the file to be compiled and executed to train the GNNs.
 * @author Malladi Tejasvi (CS23M036), M.Tech CSE, IIT Madras.
 * @date April 9, 2025

 * Here is where the hyperparameters and dataset are set and this calls the generated parallel code for GNN training.
 * Consider the GNN subclass API while creating the object. 
 * The main function handles:
 * - Command line arguments for dataset selection and thread count
 * - Graph and dataset loading
 * - Model initialization with specified architecture (GCN, GraphSAGE, or GIN)
 * - Training hyperparameter configuration
 * - Model training and saving
 * 

 Make sure that this file is compiled and run from the same directory level as graph.hpp
 */

#ifndef GENCPP_TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <atomic>
#include <omp.h>

#include "JV_GNN.cc"
#define GENCPP_TEST_H

using namespace std;


#endif


int main(int argc, char* argv[]) 
{
    ios::sync_with_stdio(0);
    cin.tie(0);

    int max_threads = omp_get_max_threads();

    string dataset = "PubMed"; //default dataset
    if(argc>1)
        dataset = argv[1];

    if(argc>2)
        max_threads = stoi(argv[2]);

    omp_set_num_threads(max_threads);   
        
    string dataset_dir = "Datasets/"+dataset;
    string graph_file = dataset_dir+"/edgelist.txt";
    char* graph_file_path = strdup(graph_file.c_str());

    graph G(graph_file_path);
    G.parseGraph();

    Dataset data(dataset_dir,G);
    data.printDataStats();

    vector<int> hidden_sizes = {128};
    //vector<int> hidden_sizes = {64,64,64,64};
    
    // vector<int> sample_sizes = {30,25,20,10}; //sample sizes for each layer, starting with the input layer.
    vector<int> sample_sizes = {30,25};

    int input_size = data.input_feature_dim;
    int output_size = data.num_classes;

    string hidden_activation = "tanh";
    string output_activation = "softmax";
    string aggregation_type = "mean";
    
    //Creating the gnn and initializing the weights.
    GCN gnn(hidden_sizes,sample_sizes,input_size,output_size,hidden_activation,output_activation,data);
    
    // GraphSAGE gnn(hidden_sizes,sample_sizes,aggregation_type,input_size,output_size,hidden_activation,output_activation,data);

    // GIN gnn(hidden_sizes,input_size,output_size,hidden_activation,output_activation,data);
    
    gnn.InitializeWeights();

    //Defining the training hyperparmeters and training the gnn.
    double lr = 1e-3;
    int total_epochs = 10;
    double weight_decay = 0.5;
    int batch_size = 128;

    Activation activ; //creating an instance of the activation class

    RMSprop optim(gnn,lr,weight_decay);
    // cout<<"\nv_w size: "<<optim.v_w.size()<<endl;
    gnn.optimiser = &optim;
    
    unordered_map<string, string> algo_detail_map;
    
    algo_detail_map["GraphSAGE"] = "GraphSAGE as per original paper with concat-aggregation";
    algo_detail_map["GCN"] = "Inductive variant of GCN with mean aggregation";
    algo_detail_map["GIN"] = "Graph Isomorphism Network (GIN) with degree based scaling";

    // Print configuration information
    cout << "================================================================================" << endl;
    cout << "                       GRAPH NEURAL NETWORK CONFIGURATION                       " << endl;
    cout << "================================================================================" << endl;
    cout << endl;
    cout << "[MODEL ARCHITECTURE]" << endl;
    cout << "Type:                     " << gnn.algo << endl;
    cout << "Specification:            " << algo_detail_map[gnn.algo] << endl;
    cout << "Hidden dimensions:        ";
    for(int i = 0; i < hidden_sizes.size(); i++) {
        cout << hidden_sizes[i];
        if(i < hidden_sizes.size() - 1) cout << ", ";
    }
    cout << endl;
    cout << "Activation function:      " << hidden_activation << endl;
    cout << endl;

    cout << "[TRAINING PARAMETERS]" << endl;
    cout << "Optimizer:                " << typeid(*gnn.optimiser).name() << endl;
    cout << "Learning rate:            " << lr << endl;
    cout << "Weight decay:             " << weight_decay << endl;
    cout << "Batch size:               " << batch_size << endl;
    cout << "Total epochs:             " << total_epochs << endl;
    cout << "Random seed:              " << 76 << endl;
    cout << endl;

    cout << "[SAMPLING CONFIGURATION]" << endl;
    cout << "Neighborhood sizes:       [";
    for(int i = 0; i < sample_sizes.size(); i++) {
        cout << sample_sizes[i];
        if(i < sample_sizes.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
    cout << endl;

    cout << "[COMPUTE RESOURCES]" << endl;
    cout << "Device:                   " << "OpenMP Parallel" << endl;
    cout << "Number of OMP Threads:    " << omp_get_max_threads() << endl;
    cout << "Environment:              " << "IITM Aqua Cluster" << endl;
    cout << endl;

    cout << "[DATASET]" << endl;
    cout << "Name:                     " << dataset << endl;
    cout << "Nodes:                    " << data.Graph.num_nodes() << endl;
    cout << "Edges:                    " << data.Graph.num_edges() << endl;
    cout << "Features:                 " << data.input_feature_dim << endl;
    cout << "Classes:                  " << data.num_classes << endl;
    // cout << "================================================================================" << endl;
    cout << endl;
    
    
    gnn.printArchitecture();
    // cout<<"\nInitializing Model Parameters with Xavier Initialization and starting Training...\n\n";

    cout<<"Starting Training...\n\n";
    GNN_Train(gnn,total_epochs,batch_size);

    gnn.saveModel(dataset + std::string("_model.bin"));
    gnn.saveModelTxt(dataset + std::string("_model.txt"));

    return 0;
}