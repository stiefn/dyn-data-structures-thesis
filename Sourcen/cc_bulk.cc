#include <libdash.h>
#include <iostream>

struct vprop {
  int comp;
};

#define UNITS 2

typedef dash::Graph<dash::DirectedGraph, vprop> graph_t;
typedef std::array<std::vector<int>, UNITS> matrix_t;
typedef std::array<std::vector<std::pair<int, int>>, UNITS> matrix_pair_t;

std::vector<int> get_data(
    matrix_t & indices, 
    matrix_t & permutations, 
    int amount) 
{ }

void set_data(matrix_pair_t & data_pairs) 
{ }


int main(int argc, char* argv[]) {
  dash::init(&argc, &argv);
  graph_t g(10);
  int num_vertices = 5;
  for(int i = 0; i < num_vertices; ++i) {
    // initially set component to self
    vprop prop { i + (num_vertices * dash::myid())};
    g.add_vertex(prop);
  }
  g.barrier();
  if(dash::myid() == 0) {
    g.add_edge(g.vertices().lbegin(), g.vertices().begin() + 4);
    g.add_edge(g.vertices().lbegin(), g.vertices().begin() + 2);
    g.add_edge(g.vertices().lbegin() + 1, g.vertices().begin() + 2);
    g.add_edge(g.vertices().lbegin() + 1, g.vertices().begin() + 4);
    g.add_edge(g.vertices().lbegin() + 2, g.vertices().begin() + 3);
    g.add_edge(g.vertices().lbegin() + 2, g.vertices().begin() + 4);
    g.add_edge(g.vertices().lbegin() + 3, g.vertices().begin() + 1);
    g.add_edge(g.vertices().lbegin() + 4, g.vertices().begin() + 2);
    g.add_edge(g.vertices().lbegin() + 4, g.vertices().begin());
  }
  if(dash::myid() == 1) {
    g.add_edge(g.vertices().lbegin(), g.vertices().begin() + 6);
    g.add_edge(g.vertices().lbegin(), g.vertices().begin() + 7);
    g.add_edge(g.vertices().lbegin() + 1, g.vertices().begin() + 9);
    g.add_edge(g.vertices().lbegin() + 2, g.vertices().begin() + 5);
    g.add_edge(g.vertices().lbegin() + 2, g.vertices().begin() + 9);
    g.add_edge(g.vertices().lbegin() + 3, g.vertices().begin() + 6);
    g.add_edge(g.vertices().lbegin() + 4, g.vertices().begin() + 8);
  }
  g.barrier();

  dash::

  while(1) {
    int gr = 0;
    std::vector<int> data;
    {
      matrix_t indices;
      matrix_t permutations;
      int i = 0;
      for(auto it = g.out_edges().lbegin(); it != g.out_edges().lend(); ++it) {
        auto lpos = g[it].target().lpos();
        indices[lpos.unit].push_back(lpos.index);
        permutations[lpos.unit].push_back(i);
        ++i;
      }
      data = get_data(indices, permutations, i);
    }

    {
      matrix_pair_t data_pairs;
      int i = 0;
      for(auto it = g.out_edges().lbegin(); it != g.out_edges().lend(); ++it) {
        auto trg = g[e.target()];
        auto src_comp = src.attributes().comp;
        auto trg_comp = data[i];
        if(src_comp < trg_comp) {
          auto trg_comp_it = g.vertices().begin() + trg_comp;
          auto lpos = trg_comp_it.lpos();
           data_pairs[lpos.unit].push_back(
               std::make_pair(lpos.index, src_comp)
          );
          gr = 1;
        }
        ++i;
      }
      set_data(data_pairs);
    }
    int gr_all = 0;
    dart_allreduce(&gr, &gr_all, 1, DART_TYPE_INT, DART_OP_SUM, 
        g.team().dart_id());
    if(gr_all == 0) break; 
    while(1) {
      int pj = 0;
      {
        matrix_t indices;
        matrix_t permutations;
        int i = 0;
        for(auto it = g.vertices().lbegin(); it != g.vertices().lend(); ++it) {
          auto v = g[it];
          auto comp = v.attributes().comp;
          auto next_it = g.vertices().begin() + comp;
          auto lpos = next_it.lpos();
          indices[lpos.unit].push_back(lpos.index);
          permutations[lpos.unit].push_back(i);
          ++i;
        }
        data = get_data(indices, permutations, i);
      }

      int i = 0;
      for(auto it = g.vertices().lbegin(); it != g.vertices().lend(); ++it) {
        auto v = g[it];
        auto comp = v.attributes().comp;
        auto comp_next = data[i];
        if(comp != comp_next) {
          vprop prop { comp_next };
          v.set_attributes(prop);
          pj = 1;
        }
        ++i;
      }
      int pj_all = 0;
      dart_allreduce(&pj, &pj_all, 1, DART_TYPE_INT, DART_OP_SUM, 
          g.team().dart_id());
      if(pj_all == 0) break; 
    }
  }

  if(dash::myid() == 1) {
    for(auto it = g.vertices().begin(); it != g.vertices().end(); ++it) {
      std::cout << it.pos() << " : " << g[it].attributes().comp 
        << std::endl;
    }
  }
  dash::barrier();
  return 0;
} 

