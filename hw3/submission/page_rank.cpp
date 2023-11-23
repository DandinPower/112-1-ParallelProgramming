#include "page_rank.h"

#include <omp.h>

#include <cmath>
#include <utility>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

// pageRank --
//
// g:           graph to process (see common/graph.h)
// solution:    array of per-vertex vertex scores (length of array is
// num_nodes(g)) damping:     page-rank algorithm's damping parameter
// convergence: page-rank algorithm's convergence threshold
//
void initialize(Graph g, double *solution, int *outgoing_sizes, int number_of_nodes, double inverse_number_of_nodes) {
#pragma omp parallel for
  // 初始化解決方案和出站大小
  for (int i = 0; i < number_of_nodes; ++i)
  {
    solution[i] = inverse_number_of_nodes;
    outgoing_sizes[i] = outgoing_size(g, i);
  }
}

double compute_sum_and_update_oldscores(double *solution, double *score_old, int *outgoing_sizes, int number_of_nodes, double damping, double inverse_number_of_nodes) {
  double sum = 0;
#pragma omp parallel for reduction(+ : sum)
  // 計算總和並更新舊分數
  for (int i = 0; i < number_of_nodes; ++i)
  {
    score_old[i] = solution[i];
    if (outgoing_sizes[i] == 0)
    {
      sum += solution[i];
    }
  }
  sum *= (damping * inverse_number_of_nodes);
  return sum;
}

double compute_new_scores(Graph g, double *solution, double *score_old, int *outgoing_sizes, int number_of_nodes, double damping, double sum) {
  double global_diff = 0;
#pragma omp parallel for schedule(static, 1) reduction(+ : global_diff)
  // 計算新分數
  for (int i = 0; i < number_of_nodes; ++i)
  {
    double incoming_sum = 0;
    for (const Vertex *j = incoming_begin(g, i); j != incoming_end(g, i); ++j)
    {
      incoming_sum += score_old[*j] / outgoing_sizes[*j];
    }
    solution[i] = damping * incoming_sum + sum;
    global_diff += std::abs(solution[i] - score_old[i]);
  }
  return global_diff;
}

bool check_convergence(double global_diff, double convergence) {
  // 檢查收斂
  return (global_diff < convergence);
}

void pageRank(Graph g, double *solution, double damping, double convergence)
{
  const int number_of_nodes = num_nodes(g);
  const double inverse_number_of_nodes = 1.0 / number_of_nodes;
  const double random_jump_probability = (1 - damping) * inverse_number_of_nodes;

  double *score_old = new double[number_of_nodes];
  int *outgoing_sizes = new int[number_of_nodes];

  // 初始化
  initialize(g, solution, outgoing_sizes, number_of_nodes, inverse_number_of_nodes);

  double sum, global_diff;
  bool converged = false;
  while (!converged)
  {
    // 計算總和並更新舊分數
    sum = compute_sum_and_update_oldscores(solution, score_old, outgoing_sizes, number_of_nodes, damping, inverse_number_of_nodes);
    sum += random_jump_probability;

    // 計算新分數
    global_diff = compute_new_scores(g, solution, score_old, outgoing_sizes, number_of_nodes, damping, sum);

    // 檢查收斂
    converged = check_convergence(global_diff, convergence);
  }

  delete[] outgoing_sizes;
  delete[] score_old;
}