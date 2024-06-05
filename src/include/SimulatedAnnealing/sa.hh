template <typename T, typename CalculateCost, typename GenerateNeighbor>
T simulated_annealing(T initial_sol, CalculateCost calculate_cost, GenerateNeighbor generate_neighbor) {
  float cost1 = calculate_cost(initial_sol);
  T neighbor = generate_neighbor(initial_sol);
  float cost2 = calculate_cost(neighbor);
  return cost1 < cost2 ? initial_sol : neighbor;
}
