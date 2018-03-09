#include <math.h>
#include <libdash.h>
#include <tuple>
#include <random>

class xorshift {

public:

  typedef unsigned long result_type;
  
  template<typename SeedGenerator>
  xorshift(SeedGenerator & s) 
   : x(s()),
     y(s()),
     z(s())
  { }

  result_type operator()() {
      result_type t;
      x ^= x << 16;
      x ^= x >> 5;
      x ^= x << 1;

     t = x;
     x = y;
     y = z;
     z = t ^ x ^ y;

     return z;
  }

  // std::uniform distribution too slow
  double dist() {
    return static_cast<double>((*this)()) / max();
  }

  result_type min() {
    return 0;
  }

  result_type max() {
    return std::numeric_limits<result_type>::max() - 1;
  }

private:

  result_type x, y, z;

};


template<typename GraphType>
class RMATRandomGenerator {

public:
  typedef GraphType                                     graph_type;
  typedef RMATRandomGenerator<graph_type>               self_t;
  typedef typename graph_type::vertex_size_type         vertex_size_type;
  typedef typename graph_type::edge_size_type           edge_size_type;
  typedef std::uniform_int_distribution<int>            dist_int_type;
  typedef std::pair<vertex_size_type, vertex_size_type> value_type;
  typedef std::pair<value_type, int>                    data_type;

private:

public:

  /**
   * Begin iterator
   */
  template<typename VertexMapperFunction>
  RMATRandomGenerator(
      vertex_size_type n, 
      edge_size_type m, 
      int n_units, 
      dash::team_unit_t myid, 
      VertexMapperFunction owner, 
      double a, 
      double b, 
      double c, 
      double d
  ) {
    std::uniform_int_distribution<int> edist(0, 10);
    std::random_device rd;
    xorshift gen(rd);

    if(n == 0) {
      _done = true;
    } else {
      // generate 50% of the edges with RMAT
      m /= 2;
      edge_size_type m_unit_random = m / n_units;
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
          std::tie(u, v) = generate_rmat_edge(gen, n, SCALE, a, b, c, d);

          if (owner(u, n, n_units, myid) == myid) {
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
        _values.push_back(std::make_pair(em_i->first, edist(gen)));
      }

      // generate 50% random edges
      // source edge has to belong to this unit
      int start = 0;
      int end = 0;
      if(myid == 0) {
        end = owner.size(myid) - 1;
      } else {
        for(int i = 0; i < myid; ++i) {
          dash::team_unit_t unit { i }; 
          start += owner.size(unit);
        }
        end = start + owner.size(myid) - 1;
      }
      dist_int_type dist_u(start, end);
      dist_int_type dist_v(0, n - 1);
      for(int i = 0; i < m_unit_random; ++i) {
        vertex_size_type u = dist_u(gen);
        vertex_size_type v = dist_v(gen);
        _values.push_back(std::make_pair(std::make_pair(u, v), edist(gen)));
      }

      _current = _values.back();
      _values.pop_back();
    }
  }

  /**
   * End iterator
   */
  RMATRandomGenerator()
    : _done(true)
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

  const data_type & operator*() const {
    return _current;
  }

  const data_type * operator->() const {
    return &_current;
  }

  

  bool operator!=(const self_t & other) {
    return !(_done && other._done);
  }

private:

  int owner(vertex_size_type v) {
    return v / _blocksize;
  }

  value_type generate_rmat_edge(
      xorshift & gen,
      vertex_size_type n,
      unsigned int SCALE,
      double a,
      double b,
      double c,
      double d) {
    vertex_size_type u = 0;
    vertex_size_type v = 0;
    vertex_size_type step = n/2;

    for (unsigned int j = 0; j < SCALE; ++j) {
      double p = gen.dist();

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

      a *= 0.9 + 0.2 * gen.dist();
      b *= 0.9 + 0.2 * gen.dist();
      c *= 0.9 + 0.2 * gen.dist();
      d *= 0.9 + 0.2 * gen.dist();

      double S = a + b + c + d;

      a /= S; b /= S; c /= S;
      d = 1. - a - b - c;
    }

    return std::make_pair(u, v);
  }

  int                     _myid = dash::myid();
  int                     _size = dash::size();
  std::vector<data_type> _values;
  data_type              _current;
  int                     _blocksize;
  bool                    _done = false;

};
