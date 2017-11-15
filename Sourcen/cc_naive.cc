#include <libdash.h>
#include <iostream>

struct vprop {
  int comp;
};

typedef dash::Graph<dash::DirectedGraph, vprop> graph_t;

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

  while(1) {
    int gr = 0;
    for(auto it = g.out_edges().lbegin(); it != g.out_edges().lend(); ++it) {
      auto e = g[it];
      auto src = g[e.source()];
      auto trg = g[e.target()];
      auto src_comp = src.attributes().comp;
      auto trg_comp = trg.attributes().comp;
      if(src_comp < trg_comp) {
        vprop prop { src_comp };
        g[g.vertices().begin() + trg_comp].set_attributes(prop);
        gr = 1;
      }
    }
    dash::barrier();
    int gr_all = 0;
    dart_allreduce(&gr, &gr_all, 1, DART_TYPE_INT, DART_OP_SUM, 
        g.team().dart_id());
    if(gr_all == 0) break; 
    for(auto it = g.vertices().lbegin(); it != g.vertices().lend(); ++it) {
      auto v = g[it];
      auto comp = v.attributes().comp;
      auto comp_next = g[g.vertices().begin() + comp].attributes().comp;
      while(comp != comp_next) {
        vprop prop { comp_next };
        v.set_attributes(prop);
        comp = comp_next;
        comp_next = g[g.vertices().begin() + comp].attributes().comp;
      }
    }
    dash::barrier();
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

