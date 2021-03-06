#include <libdash.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include "rmatrandom_edge.h"

struct vprop {
  int comp;
  short unit;
};

struct eprop {
  int weight;
  bool is_min;
};



typedef dash::Graph<dash::UndirectedGraph, vprop, eprop>       graph_t;

int main(int argc, char* argv[]) {
  if(argc == 3) {
    dash::init(&argc, &argv);
    int n_vertices = 1000 * atoi(argv[1]);
    int n_edges = 4000 * atoi(argv[2]);
    auto & team = dash::Team::All();
    dash::LogarithmicVertexMapper<graph_t> mapper(n_vertices, team.size(), 
        1.5, 0.8);
    for(int i = 0; i < 5; ++i) {
      RMATRandomGenerator<graph_t> begin(n_vertices, n_edges, team.size(), 
          team.myid(), mapper, 0.25, 0.25, 0.25, 0.25);
      RMATRandomGenerator<graph_t> end(0, 0, team.size(), team.myid(), mapper, 
          0, 0, 0, 0);

      std::clock_t g_begin_time = clock();
      graph_t g(begin, end, n_vertices, team, mapper);
      std::clock_t g_end_time = clock();
      if(dash::myid() == 0) {
	      std::cout << "[round " << i + 1 << "] construction: " << 
          double(g_end_time - g_begin_time) / CLOCKS_PER_SEC << std::endl;
      }

      dash::barrier();
      std::clock_t begin_time = clock();

      //dash::util::TraceStore::on();
      //dash::util::TraceStore::clear();

      dash::minimum_spanning_tree(g);

      //dash::barrier();
      //dash::util::TraceStore::off();
      //dash::util::TraceStore::write(std::cout);

      std::clock_t end_time = clock();
      if(dash::myid() == 0) {
	      std::cout << "[round " << i + 1 << "] algorithm: " << 
          double(end_time - begin_time) / CLOCKS_PER_SEC << std::endl;
      }
    }
    dash::finalize();
  }
  return 0;
} 
