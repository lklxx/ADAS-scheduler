template <typename T, typename CalculateCost, typename GenerateNeighbor>
T simulated_annealing(T initial_sol, CalculateCost calculate_cost, GenerateNeighbor generate_neighbor) {
  float cost = calculate_cost(initial_sol);
  T neighbor = generate_neighbor(initial_sol);
  return neighbor;
}
