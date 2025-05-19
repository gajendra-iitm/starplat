"""
#############################################################################
#                                                                           #
#     GNN Training Framework for Graph Node Classification                  #
#                                                                           #
#############################################################################

This module implements a flexible framework for training and evaluating different Graph Neural
Network architectures on various node classification benchmarks. It supports GraphSAGE, GIN, 
and GCN with memory-efficient neighbor sampling, and includes comprehensive evaluation metrics
like accuracy, precision, recall, and F1 score.

---------------------------------------------------------------------------
| AUTHOR INFORMATION:                                                      |
---------------------------------------------------------------------------
Author: Malladi Tejasvi (CS23M036), M.Tech CSE, IIT Madras.
Date created: September 16, 2024.

---------------------------------------------------------------------------
| SUPPORTED MODELS:                                                        |
---------------------------------------------------------------------------
- GraphSAGE (PyG implementation and original paper implementation with concatenation)
- Inductive GCN (using mean aggregation)
- Graph Isomorphism Network (GIN)

---------------------------------------------------------------------------
| USAGE:                                                                   |
---------------------------------------------------------------------------
    python TorchG_GS_Tester.py -ds PubMed -a graphsage_pyg -n False

---------------------------------------------------------------------------
| PACKAGE VERSION REQUIREMENTS:                                            |
---------------------------------------------------------------------------
Python: 3.8
torch==2.1.2 
torch_geometric==2.3.1

---------------------------------------------------------------------------
| CONFIGURATION NOTES:                                                     |
---------------------------------------------------------------------------
The 'max_threads' variable should be set to the number of OpenMP threads 
needed. Set it to 1 for sequential behavior.
"""


import torch
from torch_geometric.transforms import NormalizeFeatures
from torch_geometric.loader import NeighborLoader
from torch_geometric.nn import SAGEConv
from torch_geometric.datasets import Planetoid
from ogb.nodeproppred import PygNodePropPredDataset
import torch.nn.functional as F
import torch_geometric.datasets as tg_datasets
from torch_geometric.datasets import Flickr
from torch_geometric.utils import index_to_mask
from torch_geometric.utils import add_self_loops
from torch_geometric.nn import GINConv
import time
import argparse
import os

seed = 76  # You can set this to any integer
torch.manual_seed(seed)  # Set the seed for CPU operations

"""
Set the appropriate number of threads needed here. Set it to 1 for sequential behaviour.
"""

max_threads = 16#os.cpu_count()

os.environ["OMP_NUM_THREADS"] = str(max_threads)
os.environ["MKL_NUM_THREADS"] = str(max_threads)
torch.set_num_threads(max_threads)

# print(f"Total Threads Used : {max_threads}")
# print(f"PyTorch Threads: {torch.get_num_threads()}")

parser = argparse.ArgumentParser()
parser.add_argument("-s", "--split", type=str, default="public",help="Type of splitting.",choices=["public","full","random"])
parser.add_argument("-a", "--algo", type=str, default="graphsage_pyg",help="graphsage_pyg, graphsage_org, gcn, gin algorithms",choices=["graphsage_pyg","graphsage_org","gcn","gin"])
parser.add_argument("-n", "--norm", type=bool, default=False,help="Whether features should be Normalized.",choices=[True,False])
parser.add_argument("-ds", "--dataset", type=str, default="PubMed",help="Name of the required dataset.",choices=["PubMed","Cora","CiteSeer","Reddit","Yelp","Nell","Amazon-Computers","Amazon-Photo","Flickr","products","arxiv","papers100M"])
parser.add_argument("-hs", "--hidden", type=int, default=128,help="Assuming one hidden layer, number of hidden neurons in the layer.")
args = parser.parse_args()

l2_param = 0.009

is_ogbn = False

if args.dataset == "products" or args.dataset == "arxiv" or args.dataset == "papers100M":
    
    is_ogbn = True
    
    dataset = PygNodePropPredDataset(name='ogbn-'+args.dataset,root='Datasets/ogbn-'+args.dataset)
    data = dataset[0]
    
    split_idx = dataset.get_idx_split()


    if 'num_nodes_dict' in data:
        data.num_nodes = sum(data.num_nodes_dict.values())
        
    
    split_idx = dataset.get_idx_split()

    train_mask, val_mask, test_mask = split_idx["train"], split_idx["valid"], split_idx["test"]
    
    train_mask = index_to_mask(split_idx["train"], size=data.num_nodes)
    val_mask = index_to_mask(split_idx["valid"], size=data.num_nodes)
    test_mask = index_to_mask(split_idx["test"], size=data.num_nodes)

    data.train_mask = train_mask
    data.val_mask = val_mask
    data.test_mask = test_mask


if args.dataset == "Reddit":
    if args.norm:
        dataset = tg_datasets.Reddit(root='Datasets/'+args.dataset+"/",transform=NormalizeFeatures(),force_reload=False)
    else:
        dataset = tg_datasets.Reddit(root='Datasets/'+args.dataset+"/",force_reload=False)

elif args.dataset == "Nell":
    if args.norm:
        dataset = tg_datasets.NELL(root='Datasets/'+args.dataset+"/",transform=NormalizeFeatures(),force_reload=False)
    else:
        dataset = tg_datasets.NELL(root='Datasets/'+args.dataset+"/",force_reload=False)

elif args.dataset == "Yelp":
    if args.norm:
        dataset = tg_datasets.Yelp(root='Datasets/'+args.dataset+"/",transform=NormalizeFeatures(),force_reload=False)
    else:
        dataset = tg_datasets.Yelp(root='Datasets/'+args.dataset+"/",force_reload=False)

elif args.dataset == "Amazon-Computers":
    if args.norm:
        dataset = tg_datasets.Amazon(root='Datasets/'+args.dataset+"/",name="Computers",transform=NormalizeFeatures(),force_reload=False)
    else:
        dataset = tg_datasets.Amazon(root='Datasets/'+args.dataset+"/",name="Computers",force_reload=False)

elif args.dataset == "Amazon-Photo":
    if args.norm:
        dataset = tg_datasets.Amazon(root='Datasets/'+args.dataset+"/",name="Photo",transform=NormalizeFeatures(),force_reload=False)
    else:
        dataset = tg_datasets.Amazon(root='Datasets/'+args.dataset+"/",name="Photo",force_reload=False)

if args.dataset == "Flickr":
    if args.norm:
        dataset = Flickr(root='Datasets/Flickr',transform=NormalizeFeatures(),force_reload=False)
    else:
        dataset = Flickr(root='Datasets/Flickr',force_reload=False)

elif args.dataset == "Cora" or args.dataset == "CiteSeer" or args.dataset == "PubMed":
    l2_param = 0.05
    if args.norm:
        dataset = Planetoid(root='Datasets/'+args.dataset+"/", name=args.dataset,split=args.split, transform=NormalizeFeatures(),force_reload=False)
    else:
        dataset = Planetoid(root='Datasets/'+args.dataset+"/", name=args.dataset,split=args.split,force_reload=False)


if "Amazon" in args.dataset:

    data = dataset[0]

    # Get the number of nodes in the dataset
    num_nodes = data.num_nodes

    # Create random indices for train, validation, and test
    torch.manual_seed(42)  # For reproducibility

    # Split percentages
    train_size = int(0.7 * num_nodes)
    val_size = int(0.1 * num_nodes)
    test_size = num_nodes - train_size - val_size

    # Random permutation of indices
    indices = torch.randperm(num_nodes)

    train_idx = indices[:train_size]
    val_idx = indices[train_size:train_size + val_size]
    test_idx = indices[train_size + val_size:]

    # Create masks for training, validation, and testing
    data.train_mask = index_to_mask(train_idx, size=num_nodes)
    data.val_mask = index_to_mask(val_idx, size=num_nodes)
    data.test_mask = index_to_mask(test_idx, size=num_nodes)


elif not is_ogbn:
    # Get the data object
    data = dataset[0]

    # Train, validation, and test masks
    train_mask = data.train_mask
    val_mask = data.val_mask
    test_mask = data.test_mask

# train_indices = train_mask.nonzero(as_tuple=True)[0]
# val_indices = val_mask.nonzero(as_tuple=True)[0]
# test_indices = test_mask.nonzero(as_tuple=True)[0]

# The GraphSAGE that comes with PyG doesnt use concat,as specified in the original paper
# Instead it uses separate weight matrix for the root node.
# This class enables concatenation based graphsage
class CustomSAGE(SAGEConv):
    def __init__(self, in_channels, out_channels, **kwargs):
        # Initialize SAGEConv without 'concat' parameter
        super().__init__(in_channels, out_channels, **kwargs)
        
        # Store that we want concatenation behavior
        self.concat = True
        
        # Reinitialize linear layers with correct dimensions for concatenation
        self.lin_l = torch.nn.Linear(2 * in_channels, out_channels)
        self.lin_r = torch.nn.Linear(in_channels, out_channels)
        
    def forward(self, x, edge_index):
        # Neighbor aggregation
        aggregated = self.propagate(edge_index, x=x)
        
        # Align dimensions (handles padding in fixed-size batches)
        x_root = x[:aggregated.size(0)]
        
        # Concatenate root + aggregated features
        combined = torch.cat([x_root, aggregated], dim=-1)
        
        # Transform concatenated features (dim = 2*in_channels → out_channels)
        out = self.lin_l(combined) #+ self.lin_r(x_root)
        
        return out #if not self.normalize else F.normalize(out, p=2., dim=-1)


#As per GraphSAGE paper, mean aggregation for NH and the root gives an Inductive GCN.
class GCNSAGE(SAGEConv):
    def __init__(self, in_channels, out_channels):
        super().__init__(in_channels, out_channels, 
                        aggr='mean',  # Force mean aggregation
                        root_weight=False)  # Disable separate root transform
    
    def forward(self, x, edge_index):
        # Add self-loops to include root in neighborhood
        edge_index, _ = add_self_loops(edge_index, num_nodes=x.size(0))
        
        # Propagate through modified neighborhood
        return super().forward(x, edge_index)

# Define GraphSAGE model
class GNN(torch.nn.Module):
    def __init__(self, in_channels, hidden_channels, out_channels,algo="graphsage"):
        super(GNN, self).__init__()
        # concat_agg = False
        # if algo == "graphsage":
        #     concat_agg = True
        # self.conv1 = SAGEConv(in_channels, hidden_channels,concat=concat_agg)
        # self.conv2 = SAGEConv(hidden_channels, out_channels,concat=concat_agg)
        
        if algo == "graphsage_pyg": # the default one by PyG
            self.conv1 = SAGEConv(in_channels, hidden_channels)
            self.conv2 = SAGEConv(hidden_channels, out_channels)

        elif algo == "graphsage_org":
            self.conv1 = CustomSAGE(in_channels, hidden_channels)
            self.conv2 = CustomSAGE(hidden_channels, out_channels)

        elif algo == "gcn":
            self.conv1 = GCNSAGE(in_channels, hidden_channels)
            self.conv2 = GCNSAGE(hidden_channels, out_channels)

        elif algo == "gin":
            # Create MLPs for GIN layers
            mlp1 = torch.nn.Sequential(
            torch.nn.Linear(in_channels, hidden_channels))
            # torch.nn.BatchNorm1d(hidden_channels),
            # torch.nn.ReLU(),
            # torch.nn.Linear(hidden_channels, hidden_channels)
            # )
            
            mlp2 = torch.nn.Sequential(
            torch.nn.Linear(hidden_channels, out_channels))
            # ,
            # torch.nn.BatchNorm1d(hidden_channels),
            # torch.nn.ReLU(),
            # torch.nn.Linear(hidden_channels, out_channels)
            # )
            
            # Create GIN convolutional layers
            self.conv1 = GINConv(mlp1, train_eps=True)
            self.conv2 = GINConv(mlp2, train_eps=True)
            

    def forward(self, x, edge_index):
        x = self.conv1(x, edge_index)
        x = F.tanh(x)
        x = self.conv2(x, edge_index)
        return F.log_softmax(x, dim=1)

device = torch.device('cpu') #for sequential code

print(f"Device of Execution : {device}")

# Instantiate the model
hidden_size = args.hidden

# if args.algo.lower() == 'gcn' and args.dataset == "Amazon-Computers":
#     hidden_size = 64

model = GNN(dataset.num_node_features, hidden_size, dataset.num_classes, args.algo).to(device)
#model = torch_geometric.compile(model).to(device)
optimizer = torch.optim.RMSprop(model.parameters(), lr=1e-3,weight_decay=l2_param)

NH_Sample_Sizes = [30, 25]  # Number of neighbors to sample at each layer
if args.algo == "gin":
    NH_Sample_Sizes = [-1,-1]

# Use NeighborLoader for memory-efficient training
train_loader = NeighborLoader(
    data, 
    num_neighbors=NH_Sample_Sizes,  # 30 neighbors at the first layer, 25 at the second
    batch_size=128,         # Tune this for memory constraints
    shuffle=True,
    input_nodes=data.train_mask  # Only sample from training nodes
)

val_loader = NeighborLoader(
    data,
    num_neighbors=NH_Sample_Sizes,
    batch_size=128,
    shuffle=False,
    input_nodes=data.val_mask
)

test_loader = NeighborLoader(
    data,
    num_neighbors=NH_Sample_Sizes,
    batch_size=128,
    shuffle=False,
    input_nodes=data.test_mask
)

def train(last_epoch=False):
    model.train()
    total_loss = 0
    correct_preds = 0
    total_examples = 0
    
    # Initialize arrays for confusion matrix components per class only if it's the last epoch
    num_classes = dataset.num_classes
    TP = [0] * num_classes
    FP = [0] * num_classes
    FN = [0] * num_classes
    
    for batch in train_loader:
        if not device == 'cpu':
            batch = batch.to(device)

        optimizer.zero_grad()
        out = model(batch.x, batch.edge_index)
        
        # Get the subset of nodes that are actually training nodes in this batch
        batch_mask = batch.train_mask

        # Calculate loss and predictions only for the training nodes in this batch
        loss = F.cross_entropy(out[batch_mask], batch.y[batch_mask].squeeze())
        pred = out.argmax(dim=1)
        
        # Update statistics
        if args.dataset == "Yelp":
            true_labels = torch.argmax(batch.y[batch_mask], dim=1)
            correct_preds += (pred[batch_mask] == true_labels).sum().item()
        else:
            true_labels = batch.y[batch_mask].squeeze()
            correct_preds += (pred[batch_mask] == true_labels).sum().item()
        
        # Update confusion matrix for each class only if it's the last epoch
        if last_epoch:
            for c in range(num_classes):
                # True positives: predicted class c and true class is c
                TP[c] += ((pred[batch_mask] == c) & (true_labels == c)).sum().item()
                # False positives: predicted class c but true class is not c
                FP[c] += ((pred[batch_mask] == c) & (true_labels != c)).sum().item()
                # False negatives: predicted class is not c but true class is c
                FN[c] += ((pred[batch_mask] != c) & (true_labels == c)).sum().item()
        
        total_examples += batch_mask.sum().item()
        
        loss.backward()
        optimizer.step()
        total_loss += float(loss) * batch_mask.sum().item()

        del batch
        torch.cuda.empty_cache()
    
    # Default values for metrics if not calculating them
    macro_precision = 0.0
    macro_recall = 0.0
    macro_f1 = 0.0
    
    # Calculate precision, recall, and F1 score only if it's the last epoch
    if last_epoch:
        precision = []
        recall = []
        f1_score = []
        
        for c in range(num_classes):
            # Precision: TP / (TP + FP)
            if TP[c] + FP[c] > 0:
                p = TP[c] / (TP[c] + FP[c])
            else:
                p = 0.0
            precision.append(p)
            
            # Recall: TP / (TP + FN)
            if TP[c] + FN[c] > 0:
                r = TP[c] / (TP[c] + FN[c])
            else:
                r = 0.0
            recall.append(r)
            
            # F1 Score: 2 * (precision * recall) / (precision + recall)
            if p + r > 0:
                f1 = 2 * p * r / (p + r)
            else:
                f1 = 0.0
            f1_score.append(f1)
        
        # Calculate macro averages
        macro_precision = sum(precision) / num_classes
        macro_recall = sum(recall) / num_classes
        macro_f1 = sum(f1_score) / num_classes
    
    return total_loss/total_examples, correct_preds*100/total_examples, macro_precision, macro_recall, macro_f1

def validate(last_epoch=False):
    model.eval()
    correct_preds = 0
    total_loss = 0
    total_examples = 0
    
    # Initialize arrays for confusion matrix components per class
    num_classes = dataset.num_classes
    TP = [0] * num_classes
    FP = [0] * num_classes
    FN = [0] * num_classes

    for batch in val_loader:
        if not device == 'cpu':
            batch = batch.to(device)
        
        out = model(batch.x, batch.edge_index)
        
        # Get the subset of nodes that are actually validation nodes in this batch
        batch_mask = batch.val_mask
        pred = out.argmax(dim=1)

        if args.dataset == "Yelp":
            true_labels = torch.argmax(batch.y[batch_mask], dim=1)
            correct_preds += (pred[batch_mask] == true_labels).sum().item()
        else:
            true_labels = batch.y[batch_mask].squeeze()
            correct_preds += (pred[batch_mask] == true_labels).sum().item()
        
        # Update confusion matrix for each class only if it's the last epoch
        if last_epoch:
            for c in range(num_classes):
                # True positives: predicted class c and true class is c
                TP[c] += ((pred[batch_mask] == c) & (true_labels == c)).sum().item()
                # False positives: predicted class c but true class is not c
                FP[c] += ((pred[batch_mask] == c) & (true_labels != c)).sum().item()
                # False negatives: predicted class is not c but true class is c
                FN[c] += ((pred[batch_mask] != c) & (true_labels == c)).sum().item()
            
        total_examples += int(batch_mask.sum())
        total_loss += float(F.cross_entropy(out[batch_mask], batch.y[batch_mask].squeeze())) * batch_mask.sum()

        del batch
        torch.cuda.empty_cache()
    
    # Default values if not last_epoch
    macro_precision = 0.0
    macro_recall = 0.0
    macro_f1 = 0.0
    
    # Calculate precision, recall, and F1 score only if last_epoch
    if last_epoch:
        precision = []
        recall = []
        f1_score = []
        
        for c in range(num_classes):
            # Precision: TP / (TP + FP)
            if TP[c] + FP[c] > 0:
                p = TP[c] / (TP[c] + FP[c])
            else:
                p = 0.0
            precision.append(p)
            
            # Recall: TP / (TP + FN)
            if TP[c] + FN[c] > 0:
                r = TP[c] / (TP[c] + FN[c])
            else:
                r = 0.0
            recall.append(r)
            
            # F1 Score: 2 * (precision * recall) / (precision + recall)
            if p + r > 0:
                f1 = 2 * p * r / (p + r)
            else:
                f1 = 0.0
            f1_score.append(f1)
        
        # Calculate macro averages
        macro_precision = sum(precision) / num_classes
        macro_recall = sum(recall) / num_classes
        macro_f1 = sum(f1_score) / num_classes

    return total_loss/total_examples, correct_preds*100/total_examples, macro_precision, macro_recall, macro_f1

def test():
    model.eval()
    correct_preds = 0
    total_loss = 0
    total_examples = 0
    
    # Initialize arrays for confusion matrix components per class
    num_classes = dataset.num_classes
    TP = [0] * num_classes
    FP = [0] * num_classes
    FN = [0] * num_classes

    for batch in test_loader:
        if not device == 'cpu':
            batch = batch.to(device)

        out = model(batch.x, batch.edge_index)
        
        # Get the subset of nodes that are actually test nodes in this batch
        batch_mask = batch.test_mask
        pred = out.argmax(dim=1)

        if args.dataset == "Yelp":
            true_labels = torch.argmax(batch.y[batch_mask], dim=1)
            correct_preds += (pred[batch_mask] == true_labels).sum().item()
        else:
            true_labels = batch.y[batch_mask].squeeze()
            correct_preds += (pred[batch_mask] == true_labels).sum().item()
        
        # Update confusion matrix for each class
        for c in range(num_classes):
            # True positives: predicted class c and true class is c
            TP[c] += ((pred[batch_mask] == c) & (true_labels == c)).sum().item()
            # False positives: predicted class c but true class is not c
            FP[c] += ((pred[batch_mask] == c) & (true_labels != c)).sum().item()
            # False negatives: predicted class is not c but true class is c
            FN[c] += ((pred[batch_mask] != c) & (true_labels == c)).sum().item()
            
        total_examples += int(batch_mask.sum())
        total_loss += float(F.cross_entropy(out[batch_mask], batch.y[batch_mask].squeeze())) * batch_mask.sum()

        del batch
        torch.cuda.empty_cache()

    # Calculate precision, recall, and F1 score for each class
    precision = []
    recall = []
    f1_score = []
    
    for c in range(num_classes):
        # Precision: TP / (TP + FP)
        if TP[c] + FP[c] > 0:
            p = TP[c] / (TP[c] + FP[c])
        else:
            p = 0.0
        precision.append(p)
        
        # Recall: TP / (TP + FN)
        if TP[c] + FN[c] > 0:
            r = TP[c] / (TP[c] + FN[c])
        else:
            r = 0.0
        recall.append(r)
        
        # F1 Score: 2 * (precision * recall) / (precision + recall)
        if p + r > 0:
            f1 = 2 * p * r / (p + r)
        else:
            f1 = 0.0
        f1_score.append(f1)
    
    # Calculate macro averages
    macro_precision = sum(precision) / num_classes
    macro_recall = sum(recall) / num_classes
    macro_f1 = sum(f1_score) / num_classes

    return total_loss/total_examples, correct_preds*100/total_examples,macro_precision,macro_recall,macro_f1

tot_epochs = 10

algo_detail_map = {}

algo_detail_map['graphsage_org'] = "GraphSAGE as per original paper with concat aggregation"
algo_detail_map['graphsage_pyg'] = "PyG's default GraphSAGE, with separate weight matrix for root node"
algo_detail_map['gcn'] = "Inductive variant of GCN with mean aggregation"
algo_detail_map['gin'] = "Graph Isomorphism Network (GIN) with MLPs for neighborhood aggregation"

# ============================== EXPERIMENT CONFIGURATION ==============================
print("\n" + "="*80)
print("                       GRAPH NEURAL NETWORK CONFIGURATION                       ")
print("="*80)

print("\n[MODEL ARCHITECTURE]")
print(f"Type:                     {args.algo}")
print(f"Specification:            {algo_detail_map[args.algo.lower()]}")
print(f"Hidden dimension:         {hidden_size}")

print("\n[TRAINING PARAMETERS]")
print(f"Optimizer:                {optimizer.__class__.__name__}")
print(f"Learning rate:            {1e-3}")
print(f"Weight decay:             {l2_param}")
print(f"Batch size:               {128}")
print(f"Total epochs:             {tot_epochs}")
print(f"Random seed:              {seed}")


print("\n[SAMPLING CONFIGURATION]")
print(f"Neighborhood sizes:       {NH_Sample_Sizes}")

print("\n[COMPUTE RESOURCES]")
print(f"Device:                   {device}")
print(f"Environment:              IITM Aqua Cluster")
print(f"OMP threads:              {os.environ['OMP_NUM_THREADS']}")
print(f"PyTorch threads:          {torch.get_num_threads()}")

print("\n[DATASET]")
print(f"Name:                     {args.dataset}")
print(f"Feature normalization:    {args.norm}")
print(f"PyG Split Config:         {args.split}")
print(f"Dataset split:            Train 72% | Validation 8% | Test 20% (Validation is 10% of train)")
print("="*80 + "\n")


# Training loop remains the same
for epoch in range(1, tot_epochs+1):
    last_epoch = False
    if epoch == tot_epochs:
        last_epoch = True
    start = time.time()
    train_loss, train_accuracy, macro_precision, macro_recall, macro_f1 = train(last_epoch)
    end = time.time()
    duration = end - start

    val_loss, val_accuracy, macro_precision,macro_recall,macro_f1 = validate(last_epoch)
    
    if last_epoch:
        print(f'Epoch : {epoch:03d} Train Acc : {train_accuracy:.3f}% Train Loss: {train_loss:.3f} Train Macro Precision: {macro_precision:.3f}, Recall: {macro_recall:.3f}, F1: {macro_f1:.3f} Val Acc: {val_accuracy:.3f}% Val Loss: {val_loss:.3f} Val Macro Precision: {macro_precision:.3f}, Val Macro Recall: {macro_recall:.3f}, Val Macro F1: {macro_f1:.3f} Duration: {duration:.3f}s')
    else:
        print(f'Epoch : {epoch:03d} Train Acc : {train_accuracy:.3f}% Train Loss: {train_loss:.3f} Val Acc: {val_accuracy:.3f}% Val Loss: {val_loss:.3f}  Duration: {duration:.3f}s')

# Evaluate on test set
test_loss, test_acc, macro_precision, macro_recall, macro_f1 = test()
print(f'Test Accuracy: {test_acc:.3f}% Test Loss: {test_loss:.3f} Test Macro Precision: {macro_precision:.3f}, Recall: {macro_recall:.3f}, F1: {macro_f1:.3f}')