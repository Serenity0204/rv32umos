/* ===============================
   Manual Memory Pool
   =============================== */
#include "syscall.h"
#define POOL_SIZE 1024

static unsigned char memory_pool[POOL_SIZE];
static unsigned int pool_offset = 0;

void* pool_alloc(unsigned int size)
{
    if (pool_offset + size > POOL_SIZE)
    {
        return 0; /* out of memory */
    }
    void* ptr = memory_pool + pool_offset;
    pool_offset += size;
    return ptr;
}

/* ===============================
   Graph Structures
   =============================== */

typedef struct Edge Edge;
typedef struct Node Node;

struct Edge
{
    Node* to;
    int weight;
    Edge* next;
};

struct Node
{
    int value;
    Edge* edges;
    int visited;
};

/* ===============================
   Function Pointer Type
   =============================== */

typedef int (*Aggregator)(int current, int node_value, int edge_weight);

/* ===============================
   Graph Construction
   =============================== */

Node* create_node(int value)
{
    Node* n = (Node*)pool_alloc(sizeof(Node));
    if (!n) return 0;

    n->value = value;
    n->edges = 0;
    n->visited = 0;
    return n;
}

void add_edge(Node* from, Node* to, int weight)
{
    Edge* e = (Edge*)pool_alloc(sizeof(Edge));
    if (!e) return;

    e->to = to;
    e->weight = weight;
    e->next = from->edges;
    from->edges = e;
}

/* ===============================
   DFS Traversal with Aggregation
   =============================== */

int dfs(Node* node, Aggregator agg, int acc)
{
    Edge* e;
    int result = acc;

    if (!node || node->visited)
        return acc;

    node->visited = 1;

    e = node->edges;
    while (e)
    {
        result = agg(result, e->to->value, e->weight);
        result = dfs(e->to, agg, result);
        e = e->next;
    }

    return result;
}

/* ===============================
   Example Aggregators
   =============================== */

int sum_aggregator(int current, int node_value, int edge_weight)
{
    return current + node_value * edge_weight;
}

int max_aggregator(int current, int node_value, int edge_weight)
{
    int v = node_value + edge_weight;
    return (v > current) ? v : current;
}

/* ===============================
   Entry Point (No I/O)
   =============================== */

int main(void)
{
    Node* a = create_node(1);
    Node* b = create_node(2);
    Node* c = create_node(3);
    Node* d = create_node(4);

    add_edge(a, b, 5);
    add_edge(a, c, 2);
    add_edge(b, d, 1);
    add_edge(c, d, 7);

    /* Perform DFS with different behaviors */
    int result1 = dfs(a, sum_aggregator, 0);

    /* Reset visited flags manually */
    a->visited = b->visited = c->visited = d->visited = 0;

    int result2 = dfs(a, max_aggregator, 0);

    /* Prevent unused variable optimization */
    return result1 + result2;
}
