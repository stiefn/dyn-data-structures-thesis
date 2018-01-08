#include <math.h>
#include <libdash.h>
#include <tuple>
#include <random>

template<typename GraphType>
class RMATRandomGenerator {

public:

  typedef GraphType                                     graph_type;
  typedef RMATRandomGenerator<graph_type>               self_t;
  typedef typename graph_type::vertex_size_type         vertex_size_type;
  typedef typename graph_type::edge_size_type           edge_size_type;
  typedef std::uniform_real_distribution<double>        dist_type;
  typedef std::uniform_int_distribution<int>            dist_int_type;
  typedef std::minstd_rand                              rng_type;
  typedef std::pair<vertex_size_type, vertex_size_type> value_type;

public:

  /**
   * Begin iterator
   */
  RMATRandomGenerator(vertex_size_type n, edge_size_type m, double a, double b,
      double c, double d) 
    : _dist(0.0, 1.0),
      _gen((std::random_device())()),
      _blocksize(n / _size)
  { 
    // generate 50% of the edges with RMAT
    m /= 2;
    edge_size_type m_unit_random = m / _size;
    int SCALE = int(floor(log(double(n))/log(2.)));

    std::map<value_type, bool> edge_map;

    // generate whole graph on each unit, but only use edges belonging to this
    // unit
    edge_size_type generated = 0;
    edge_size_type local_edges = 0;
    do {
      edge_size_type rejected = 0;
      do {
        vertex_size_type u, v;
        std::tie(u, v) = generate_rmat_edge(n, SCALE, a, b, c, d);

        if (owner(u) == _myid) {
          // reject loop edges and multi-edges
          if (u != v 
              && edge_map.find(std::make_pair(u, v)) == edge_map.end()) {
            edge_map[std::make_pair(u, v)] = true;
            ++local_edges;
          } else {
            ++rejected;
          }
        }
        ++generated;
      } while (generated < m);
      // generate more edges based on the amount of edges rejected on each unit
      int rejected_all;
      dart_allreduce(&rejected, &rejected_all, 1, DART_TYPE_INT, DART_OP_SUM,
          dash::Team::All().dart_id());
      generated -= rejected_all;
    } while (generated < m);

    // reserve space for generated rmat edges and for coming random edges
    _values.reserve(local_edges + m_unit_random);
    typename std::map<value_type, bool>::reverse_iterator em_end = 
      edge_map.rend();
    for (typename std::map<value_type, bool>::reverse_iterator em_i = 
        edge_map.rbegin(); em_i != em_end; ++em_i) {
      _values.push_back(em_i->first);
    }

    // generate 50% random edges
    auto n_unit = n / _size;
    // source edge has to belong to this unit
    dist_int_type dist_u(n_unit * _myid, n_unit * (_myid + 1) - 1);
    dist_int_type dist_v(0, n - 1);
    for(int i = 0; i < m_unit_random; ++i) {
      vertex_size_type u = dist_u(_gen);
      vertex_size_type v = dist_v(_gen);
      _values.push_back(std::make_pair(u, v));
    }

    _current = _values.back();
    _values.pop_back();
  }

  /**
   * End iterator
   */
  RMATRandomGenerator() 
    : _dist(0.0, 1.0),
      _gen((std::random_device())()),
      _done(true)
  { }

  self_t & operator++() {
    if(!_values.empty()) {
      _current = _values.back();
      _values.pop_back();
    } else {
      _done = true;
    }

    return *this;
  }

  const value_type & operator*() const {
    return _current;
  }

  const value_type * operator->() const {
    return &_current;
  }

  

  bool operator!=(const self_t & other) {
    return !(_done && other._done);
  }

private:

  int owner(vertex_size_type v) {
    return v / _blocksize;
  }

  value_type generate_rmat_edge(vertex_size_type n, unsigned int SCALE, 
      double a, double b, double c, double d) {
    vertex_size_type u = 0;
    vertex_size_type v = 0;
    vertex_size_type step = n/2;

    for (unsigned int j = 0; j < SCALE; ++j) {
      double p = _dist(_gen);

      if (p < a)
        ;
      else if (p >= a && p < a + b)
        v += step;
      else if (p >= a + b && p < a + b + c)
        u += step;
      else {
        u += step;
        v += step;
      }

      step /= 2;

      a *= 0.9 + 0.2 * _dist(_gen);
      b *= 0.9 + 0.2 * _dist(_gen);
      c *= 0.9 + 0.2 * _dist(_gen);
      d *= 0.9 + 0.2 * _dist(_gen);

      double S = a + b + c + d;

      a /= S; b /= S; c /= S;
      d = 1. - a - b - c;
    }

    return std::make_pair(u, v);
  }

  dist_type               _dist;
  rng_type                _gen;
  int                     _myid = dash::myid();
  int                     _size = dash::size();
  std::vector<value_type> _values;
  value_type              _current;
  int                     _blocksize;
  bool                    _done = false;

};
