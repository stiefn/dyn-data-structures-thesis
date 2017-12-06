#include <math.h>
#include <libdash.h>
#include <tuple>
#include <random>

template<typename GraphType>
class RMATGenerator {

  typedef GraphType                                     graph_type;
  typedef RMATGenerator<graph_type>                     self_t;
  typedef typename graph_type::vertex_size_type         vertex_size_type;
  typedef typename graph_type::edge_size_type           edge_size_type;
  typedef std::uniform_real_distribution<double>        dist_type;
  typedef std::minstd_rand                              rng_type;
  typedef std::pair<vertex_size_type, vertex_size_type> value_type;

public:

  /**
   * Begin iterator
   */
  RMATGenerator(vertex_size_type n, edge_size_type m, double a, double b,
      double c, double d) 
    : _dist(0.0, 1.0),
      _gen((std::random_device())()),
      _blocksize(n / _size)
  { 
    int SCALE = int(floor(log(double(n))/log(2.)));

    std::map<value_type, bool> edge_map;

    edge_size_type generated = 0;
    edge_size_type local_edges = 0;
    do {
      edge_size_type tossed = 0;
      do {
        vertex_size_type u, v;
        std::tie(u, v) = generate_edge(n, SCALE, a, b, c, d);

        if (owner(u) == _myid) {
          if (u != v 
              && edge_map.find(std::make_pair(u, v)) == edge_map.end()) {
            edge_map[std::make_pair(u, v)] = true;
            ++local_edges;
          } else {
            ++tossed;
          }
        }
        ++generated;
      } while (generated < m);
      int tossed_all;
      dart_allreduce(&tossed, &tossed_all, 1, DART_TYPE_INT, DART_OP_SUM,
          dash::Team::All().dart_id());
      if(tossed_all > generated) {
        tossed_all = generated;
      }
      generated -= tossed_all;
    } while (generated < m);

    _values.reserve(local_edges);
    typename std::map<value_type, bool>::reverse_iterator em_end = 
      edge_map.rend();
    for (typename std::map<value_type, bool>::reverse_iterator em_i = 
        edge_map.rbegin(); em_i != em_end; ++em_i) {
      _values.push_back(em_i->first);
    }

    _current = _values.back();
    _values.pop_back();
  }

  /**
   * End iterator
   */
  RMATGenerator() 
    : _dist(0.0, 1.0),
      _gen((std::random_device())()),
      _done(true)
  { }

  RMATGenerator & operator++() {
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

  value_type generate_edge(vertex_size_type n, unsigned int SCALE, double a, 
      double b, double c, double d) {
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
