#include <chrono>
#include <random>

bool move_to(float delta_c, double t) {
  if (delta_c <= 0) {
    return true;
  }

  static std::random_device dev;
  static std::knuth_b rand_engine(dev());
  std::uniform_real_distribution<double> rand(0.0, 1.0);
  double prob = 1 / exp(delta_c / t);
  return rand(rand_engine) <= prob;
}

template <typename T, typename CalculateCost, typename GenerateNeighbor>
T simulated_annealing(T initial_sol, CalculateCost calculate_cost,
                      GenerateNeighbor generate_neighbor, int exec_time) {
  T curr_sol, best_sol;
  float curr_cost, best_cost;
  curr_sol = best_sol = initial_sol;
  curr_cost = best_cost = calculate_cost(curr_sol);
  std::cout << "best_cost: " << best_cost << std::endl;

  auto start = std::chrono::steady_clock::now();
  while (true) {
    auto curr = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = curr - start;
    if (duration.count() >= exec_time) {
      break;
    }

    T new_sol = generate_neighbor(curr_sol);
    float new_cost = calculate_cost(new_sol);

    if (new_cost < best_cost) {
      best_sol = new_sol;
      best_cost = new_cost;
      std::cout << "best_cost: " << best_cost << std::endl;
    }

    if (move_to(new_cost - curr_cost, exec_time - duration.count())) {
      curr_sol = new_sol;
      curr_cost = new_cost;
    }
  }

  return best_sol;
}
