#include "bfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <omp.h>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#define ROOT_NODE_ID 0
#define NOT_VISITED_MARKER -1
#define THRESHOLD 1e7

void vertex_set_clear(vertex_set *list) {
    list->count = 0; 
}

void vertex_set_init(vertex_set *list, int count) {
    list->max_vertices = count; 
    list->vertices = (int *)malloc(sizeof(int) * list->max_vertices);
    vertex_set_clear(list);
}

// BFS的helper function
void initialize_frontier(vertex_set *frontier, int num_nodes, int frontier_id) {
    memset(frontier->vertices, 0, sizeof(int) * num_nodes);  // 初始化邊界
    frontier->vertices[ROOT_NODE_ID] = frontier_id;
    frontier->count++;
}

void initialize_distances(int *distances, int num_nodes) {
    for (int i = 0; i < num_nodes; i++) {  // 初始化距離
        distances[i] = NOT_VISITED_MARKER;
    }
    distances[ROOT_NODE_ID] = 0;
}

// BFS步驟函數
void top_down_step(Graph g, vertex_set *frontier, int *distances, int frontier_id);  // 從上到下的步驟
void bottom_up_step(Graph g, vertex_set* frontier, int *distances, int frontier_id);  // 從下到上的步驟

void bfs_generic(Graph graph, solution *sol, void (*step_function)(Graph, vertex_set*, int*, int)) {
    vertex_set list1;
    vertex_set_init(&list1, graph->num_nodes);
    int frontier_id = 1;

    initialize_frontier(&list1, graph->num_nodes, frontier_id);  // 初始化邊界和距離
    initialize_distances(sol->distances, graph->num_nodes);

    while (list1.count != 0) {  // 當邊界不為空時，繼續執行

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif
        list1.count = 0;

        step_function(graph, &list1, sol->distances, frontier_id);  // 執行步驟函數

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", list1->count, end_time - start_time);
#endif

        frontier_id++;
    }
}

void bfs_top_down(Graph graph, solution *sol) {
    bfs_generic(graph, sol, top_down_step);  // 從上到下的BFS
}

void bfs_bottom_up(Graph graph, solution *sol) {
    bfs_generic(graph, sol, bottom_up_step);  // 從下到上的BFS
}

void bfs_hybrid(Graph graph, solution *sol) {
    vertex_set list1;
    vertex_set_init(&list1, graph->num_nodes);
    int frontier_id = 1;

    initialize_frontier(&list1, graph->num_nodes, frontier_id);  // 初始化邊界和距離
    initialize_distances(sol->distances, graph->num_nodes);

    while (list1.count != 0) {  // 當邊界不為空時，繼續執行
        if (list1.count > THRESHOLD) {
            list1.count = 0;
            bottom_up_step(graph, &list1, sol->distances, frontier_id);  // 執行從下到上的步驟
        } else {
            list1.count = 0;
            top_down_step(graph, &list1, sol->distances, frontier_id);  // 執行從上到下的步驟
        }

        frontier_id++;
    }
}

void update_frontier_and_distances(vertex_set *frontier, int *distances, int node, int neighbor_node, int frontier_id, int &next_frontier_cnt) {
    if (frontier->vertices[neighbor_node] == 0) {  // 更新邊界和距離
        distances[neighbor_node] = distances[node] + 1;
        frontier->vertices[neighbor_node] = frontier_id + 1;
        next_frontier_cnt++;
    }
}

// Take one step of "top-down" BFS.  For each vertex on the frontier,
// follow all outgoing edges, and add all neighboring vertices to the
// new_frontier.
void top_down_step(Graph g, vertex_set *frontier, int *distances, int frontier_id) {
    int next_frontier_cnt = 0;

    for (int i = 0; i < g->num_nodes; i++) {  // 從上到下的步驟
        if (frontier->vertices[i] == frontier_id) {
            int start_edge = g->outgoing_starts[i];
            int end_edge = (i == g->num_nodes - 1) ? g->num_edges : g->outgoing_starts[i + 1];

            for (int neighbor = start_edge; neighbor < end_edge; neighbor++) {
                int neighbor_node = g->outgoing_edges[neighbor];
                update_frontier_and_distances(frontier, distances, i, neighbor_node, frontier_id, next_frontier_cnt);
            }
        }
    }

    frontier->count += next_frontier_cnt;
}

void bottom_up_step(Graph g, vertex_set* frontier, int *distances, int frontier_id) {
    int next_frontier_cnt = 0;

    for (int i = 0; i < g->num_nodes; i++) {  // 從下到上的步驟
        if (frontier->vertices[i] == 0) {
            int start_edge = g->incoming_starts[i];
            int end_edge = (i == g->num_nodes - 1) ? g->num_edges : g->incoming_starts[i + 1];

            for (int neighbor = start_edge; neighbor < end_edge; neighbor++) {
                int neighbor_node = g->incoming_edges[neighbor];
                if (frontier->vertices[neighbor_node] == frontier_id) {
                    update_frontier_and_distances(frontier, distances, neighbor_node, i, frontier_id, next_frontier_cnt);
                    break;
                }
            }
        }
    }

    frontier->count += next_frontier_cnt;
}