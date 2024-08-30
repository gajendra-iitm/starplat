# Triangle Counting Algorithm

Given DSL code
```c++
function Compute_TC(Graph g) {
  long triangle_count = 0;
  forall(v in g.nodes()) {
    forall(u in g.neighbors(v).filter(u < v)) {
      forall(w in g.neighbors(v).filter(w > v)) {
        if (g.is_an_edge(u, w)) {
          triangle_count += 1;
        }
      }
    }
  }
  return triangle_count;
}
```
python code for it
```python
def Compute_TC(g):
    triangle_count = 0
    
    for v in g.nodes():
        for u in list(filter(lambda u: u<v, g.neighbors(v))):
            for w in list(filter(lambda w: w>v, g.neighbors(v))):
                if g.is_an_edge(u,w):
                    triangle_count += 1
                    
    return triangle_count
```
generated translated code

```c++
function Compute_TC(Graph g) {
  long triangle_count = 0;
  forall(v in g.nodes()) {
  forall(u in g.neighbors(v).filter(u < v)) {
  forall(w in g.neighbors(v).filter(w > v)) {
    if (g.is_an_edge(u, w)) {
  triangle_count + 1;
    }
  }
  }
  }
  return triangle_count;
}

}
```
formatted it properly below


```c++
function Compute_TC(Graph g) {
  long triangle_count = 0;
  forall(v in g.nodes()) {
    forall(u in g.neighbors(v).filter(u < v)) {
      forall(w in g.neighbors(v).filter(w > v)) {
        if (g.is_an_edge(u, w)) {
          triangle_count += 1;
        }
      }
    }
  }
  return triangle_count;
}
```




