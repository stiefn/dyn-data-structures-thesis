#include <libdash.h>
#include <iostream>
#include <ctime>
#include <limits>
#include "rmatrandom.h"

struct vprop {
  int comp;
};

struct eprop {
  int weight;
};

typedef dash::Graph<dash::UndirectedGraph, vprop, eprop>  graph_t;
typedef std::vector<std::vector<int>>                     matrix_t;
typedef std::vector<std::vector<std::pair<int, int>>>     matrix_pair_t;
// (supervertex, new component, vertex src, vertex trg, weight)
typedef std::tuple<int, int, int, int, int>               tuple_t;
typedef std::vector<std::vector<tuple_t>>                 matrix_min_pairs_t;

std::vector<int> get_data(
    matrix_t & indices, 
    matrix_t & permutations, 
    graph_t & graph) 
{
  // exchange indices to get
  std::vector<std::size_t> sizes_send(indices.size());
  std::vector<std::size_t> displs_send(indices.size());
  std::vector<int> indices_send;
  int total_send = 0;
  for(int i = 0; i < indices.size(); ++i) {
    sizes_send[i] = indices[i].size();
    displs_send[i] = total_send;
    total_send += indices[i].size();
  }
  indices_send.reserve(total_send);
  for(auto & index_set : indices) {
    indices_send.insert(indices_send.end(), index_set.begin(), 
        index_set.end());
  }
  std::vector<std::size_t> sizes_recv(indices.size());
  std::vector<std::size_t> displs_recv(indices.size());
  dart_alltoall(sizes_send.data(), sizes_recv.data(), sizeof(std::size_t), 
      DART_TYPE_BYTE, graph.team().dart_id());
  int total_recv = 0;
  for(int i = 0; i < sizes_recv.size(); ++i) {
    displs_recv[i] = total_recv;
    total_recv += sizes_recv[i];
  }
  std::vector<int> indices_recv(total_recv);
  if(total_send > 0 || total_recv > 0) {
    dart_alltoallv(indices_send.data(), sizes_send.data(), displs_send.data(),
        DART_TYPE_INT, indices_recv.data(), sizes_recv.data(), 
        displs_recv.data(), graph.team().dart_id());
  }

  // exchange data
  for(auto & index : indices_recv) {
    // TODO: optimize cache performance
    index = graph[graph.vertices().begin() + index].attributes().comp;
  }
  if(total_send > 0 || total_recv > 0) {
    dart_alltoallv(indices_recv.data(), sizes_recv.data(), displs_recv.data(),
        DART_TYPE_INT, indices_send.data(), sizes_send.data(), 
        displs_send.data(), graph.team().dart_id());
  }

  // restore original order
  // TODO: use more sophisticated ordering mechanism
  std::vector<int> output(indices_send.size());
  int index = 0;
  for(int i = 0; i < permutations.size(); ++i) {
    for(int j = 0; j < permutations[i].size(); ++j) {
      output[permutations[i][j]] = indices_send[index];
      ++index;
    }
  }
  return output;
}

void set_data_min(matrix_min_pairs_t & data_pairs, graph_t & graph) {
  std::vector<std::size_t> sizes_send(data_pairs.size());
  std::vector<std::size_t> displs_send(data_pairs.size());
  std::vector<tuple_t> pairs_send;
  int total_send = 0;
  for(int i = 0; i < data_pairs.size(); ++i) {
    sizes_send[i] = data_pairs[i].size() * sizeof(tuple_t);
    displs_send[i] = total_send * sizeof(tuple_t);
    total_send += data_pairs[i].size();
  }
  pairs_send.reserve(total_send);
  for(auto & pair_set : data_pairs) {
    pairs_send.insert(pairs_send.end(), pair_set.begin(), pair_set.end());
  }
  std::vector<std::size_t> sizes_recv(data_pairs.size());
  std::vector<std::size_t> displs_recv(data_pairs.size());
  dart_alltoall(sizes_send.data(), sizes_recv.data(), sizeof(std::size_t), 
      DART_TYPE_BYTE, graph.team().dart_id());
  int total_recv = 0;
  for(int i = 0; i < sizes_recv.size(); ++i) {
    displs_recv[i] = total_recv;
    total_recv += sizes_recv[i];
  }
  std::vector<tuple_t> pairs_recv(total_recv / 
      sizeof(tuple_t));
  if(total_send > 0 || total_recv > 0) {
  dart_alltoallv(pairs_send.data(), sizes_send.data(), displs_send.data(),
      DART_TYPE_BYTE, pairs_recv.data(), sizes_recv.data(), 
      displs_recv.data(), graph.team().dart_id());
  }
  std::unordered_map<int, tuple_t> mapping;
  for(auto & pair : pairs_recv) {
    auto it = mapping.find(std::get<0>(pair));
    if(it != mapping.end()) {
      auto weight = std::get<4>(it->second);
      if(weight > std::get<4>(pair)) {
        it->second = pair;
      }
    } else {
      mapping[std::get<0>(pair)] = pair;
    }
  }
  for(auto & pair : mapping) {
    vprop prop { std::get<1>(pair.second) };
    graph[graph.vertices().begin() + pair.first].set_attributes(prop);
    // save edges here (src = get<2>(pair.second), trg = get<3>(pair.second)
  }
}

int main(int argc, char* argv[]) {
  dash::init(&argc, &argv);
  int n_vertices = 100000000;
  int n_edges = 400000000;
  RMATRandomGenerator<graph_t> begin(n_vertices, n_edges, 0.57, 0.19, 0.19, 
      0.05);
  RMATRandomGenerator<graph_t> end;
  std::clock_t g_begin_time = clock();
  graph_t g(begin, end, n_vertices);
  std::clock_t g_end_time = clock();
  if(dash::myid() == 0) {
    std::cout << double(g_end_time - g_begin_time) / CLOCKS_PER_SEC << 
      std::endl;
  }

  // set component to global index in iteration space
  int i = 0;
  for(auto it = g.vertices().begin(); it != g.vertices().end(); ++it) {
    if(it.is_local()) {
      vprop prop { it.pos() };
      g[it].set_attributes(prop);
    }
    ++i;
  }


  dash::barrier();
  std::clock_t begin_time = clock();

  /*
  graph_t g(6);

  if(dash::myid() == 0) {
    vprop prop1 { 0 };
    g.add_vertex(prop1);
    vprop prop2 { 1 };
    g.add_vertex(prop2);
    vprop prop3 { 2 };
    g.add_vertex(prop3);
  } else {
    vprop prop1 { 3 };
    g.add_vertex(prop1);
    vprop prop2 { 4 };
    g.add_vertex(prop2);
    vprop prop3 { 5 };
    g.add_vertex(prop3);
  }
  g.commit();
  if(dash::myid() == 0) {
    eprop prop1 { 1 };
    g.add_edge(g.vertices().lbegin(), g.vertices().begin() + 2, prop1);
    eprop prop2 { 3 };
    g.add_edge(g.vertices().lbegin(), g.vertices().begin() + 1, prop2);
    eprop prop3 { 2 };
    g.add_edge(g.vertices().lbegin() + 2, g.vertices().begin() + 3, prop3);
    eprop prop4 { 1 };
    g.add_edge(g.vertices().lbegin() + 1, g.vertices().begin() + 5, prop4);
  } else {
    eprop prop1 { 2 };
    g.add_edge(g.vertices().lbegin() + 2, g.vertices().begin() + 4, prop1);
    eprop prop2 { 4 };
    g.add_edge(g.vertices().lbegin(), g.vertices().begin() + 4, prop2);
  }
  g.commit();
  */

  while(1) {
    int gr = 0;
    std::vector<int> data;
    {
      int i = 0;
      matrix_t indices(g.team().size());
      matrix_t permutations(g.team().size());

      for(auto it = g.vertices().lbegin(); it != g.vertices().lend(); ++it) {
        auto v = g[it];
        for(auto e_it = v.out_edges().lbegin(); e_it != v.out_edges().lend(); 
            ++e_it) {
          auto trg = g[e_it].target();
          auto lpos = trg.lpos();
          indices[lpos.unit].push_back(trg.pos());
          permutations[lpos.unit].push_back(i);
          ++i;
        }
      }
      data = get_data(indices, permutations, g);
    }

    {
      int i = 0;
      matrix_min_pairs_t data_pairs(g.team().size());
      for(auto it = g.vertices().lbegin(); it != g.vertices().lend(); ++it) {
        auto v = g[it];
        auto src_comp = v.attributes().comp;
        int min_weight = std::numeric_limits<int>::max();
        int trg_comp_min = -1;
        int trg_min = -1;
        for(auto e_it = v.out_edges().lbegin(); e_it != v.out_edges().lend(); 
            ++e_it) {
          auto e = g[e_it];
          auto e_weight = e.attributes().weight;
          auto trg_comp = data[i];
          if(src_comp != trg_comp && min_weight > e_weight) {
            min_weight = e_weight;
            trg_comp_min = trg_comp;
            trg_min = e.target().pos();
          }
          ++i;
        }
        if(trg_comp_min >= 0) {
          auto src = g.vertex_gptr(it).pos();
          auto src_comp_it = g.vertices().begin() + src_comp;
          auto trg_comp_it = g.vertices().begin() + trg_comp_min;
          data_pairs[src_comp_it.lpos().unit].push_back(
              std::make_tuple(src_comp_it.pos(), trg_comp_min, src, trg_min, 
              min_weight));
          data_pairs[trg_comp_it.lpos().unit].push_back(
              std::make_tuple(trg_comp_it.pos(), src_comp, src, trg_min, 
              min_weight));
          gr = 1;
        }
      }
      set_data_min(data_pairs, g);
    }

    int gr_all = 0;
    dart_allreduce(&gr, &gr_all, 1, DART_TYPE_INT, DART_OP_SUM, 
        g.team().dart_id());
    if(gr_all == 0) break; 
    while(1) {
      int pj = 0;
      {
        matrix_t indices(g.team().size());
        matrix_t permutations(g.team().size());
        int i = 0;
        for(auto it = g.vertices().lbegin(); it != g.vertices().lend(); ++it) {
          auto v = g[it];
          auto comp = v.attributes().comp;
          auto next_it = g.vertices().begin() + comp;
          auto lpos = next_it.lpos();
          indices[lpos.unit].push_back(next_it.pos());
          permutations[lpos.unit].push_back(i);
          ++i;
        }
        data = get_data(indices, permutations, g);
      }

      int i = 0;
      for(auto it = g.vertices().lbegin(); it != g.vertices().lend(); ++it) {
        auto v = g[it];
        auto comp = v.attributes().comp;
        auto comp_next = data[i];
        if(comp > comp_next) {
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

  std::clock_t end_time = clock();

  if(dash::myid() == 0) {
    std::cout << double(end_time - begin_time) / CLOCKS_PER_SEC << 
      std::endl;
  }

  dash::barrier();
  dash::finalize();
  return 0;
} 
