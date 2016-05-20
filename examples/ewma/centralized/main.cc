#include <iostream>

#include <map>
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <limits>
#include <algorithm>

using namespace std;

struct Graph {

  struct Edge {
    string dst;
    int w;
    Edge(string d, int v): dst(d), w(v) {}
  };

  struct Node {
    string name;
    int x, y;
    vector<Edge> edges;
    Node(const string& n, int a, int b) : name(n), x(a), y(b) {}

    void addEdge(const string& d, int w) {
      edges.emplace_back(Edge(d, w));
    }

    int distance(Node& n) {
      return (n.x - x)*(n.x - x) + (n.y - y)*(n.y - y);
    }
  };

  Node& find_node(const string& name) {
    auto node = find_if(begin(nodes), end(nodes), [&] (const Graph::Node n) {
      return n.name == name;
    });
    return *node;
  }

  std::vector<Node> nodes;

};

static Graph graph;

static void
initiatilze_graph(void)
{
  Graph::Node n1("1", 1, 3);
  Graph::Node n2("2", 7, 6);
  Graph::Node n3("3", 1, 1);
  Graph::Node n4("4", 1, 6);
  Graph::Node n5("5", 7, 2);
  Graph::Node n6("6", 5, 3);
  Graph::Node n7("7", 4, 1);
  Graph::Node n8("8", 7, 4);
  Graph::Node n9("9", 3, 4);
  Graph::Node n10("10", 4, 4);
  n1.addEdge("3", 4);
  n1.addEdge("9", 5);
  n2.addEdge("8", 4);
  n3.addEdge("1", 5);
  n4.addEdge("9", 8);
  n5.addEdge("8", 4);
  n6.addEdge("10", 2);
  n6.addEdge("8", 5);
  n6.addEdge("7", 5);
  n7.addEdge("6", 5);
  n8.addEdge("2", 4);
  n8.addEdge("5", 4);
  n8.addEdge("6", 5);
  n9.addEdge("10", 1);
  n9.addEdge("4", 8);
  n9.addEdge("1", 5);
  n10.addEdge("6", 2);
  n10.addEdge("9", 1);
  graph.nodes.push_back(n1);
  graph.nodes.push_back(n2);
  graph.nodes.push_back(n3);
  graph.nodes.push_back(n4);
  graph.nodes.push_back(n5);
  graph.nodes.push_back(n6);
  graph.nodes.push_back(n7);
  graph.nodes.push_back(n8);
  graph.nodes.push_back(n9);
  graph.nodes.push_back(n10);
}

static int
energy_required(Graph& g, string r, map<string, int>& e)
{
  std::set<string> marked;
  std::queue<string> q;
  marked.emplace(r);
  q.push(r);
  int total_e = 0;
  while (!q.empty()) {
    string s = q.front();
    q.pop();
    // cout << " Checking " << s << endl;
    auto node = g.find_node(s);
    // cout << " Checking 1 " << node->edges.size() << endl;
    int max_e = 0;
    for_each(begin(node.edges), end(node.edges), [&] (const Graph::Edge e) {
      if (marked.find(e.dst) == marked.end()) {
        marked.emplace(e.dst);
        q.push(e.dst);
        // cout << "\t" << e.w << " vs " << max_e << endl;
        if (max_e < e.w) {
          max_e = e.w;
        }
      }
    });

    e.emplace(s, max_e);

    total_e += max_e;
  }
  return total_e;
}

static void
compute_nodes_to_exclude(Graph& mst, string r, map<string, int>& energy, int max_dist, set<string>& excluded)
{
  auto node_r = mst.find_node(r);

  for (auto n : mst.nodes) {
    if (energy[n.name] == 0 && node_r.distance(n) <= max_dist && node_r.distance(n) > energy[r]) {
      // it is not a transmining node that can be covered by the new transmition range
      // the first and unique edge is its transmition node
      string t = n.edges[0].dst;
      if (excluded.find(t) == excluded.end()){
        excluded.emplace(t);
      }
    }
    // FIXME: I think there is a missing case here
  }
}

static int
gain(Graph& mst, string r, string i, map<string, int>& energy, int* d)
{
  /* compute delta */
  auto node_r = mst.find_node(r);
  auto node_i = mst.find_node(i);
  int max_dist = numeric_limits<int>::min();
  for (auto e: node_i.edges) {
    if (energy[e.dst] == 0) {
      auto node_dst = mst.find_node(e.dst);
      int tmp_e = node_r.distance(node_dst);
      if (tmp_e > max_dist) {
        max_dist = tmp_e;
      }
    }
  }
  int delta = max_dist - energy[r];
  /* compute gain */
  int g = -delta;
  set<string> excluded;
  compute_nodes_to_exclude(mst, r, energy, max_dist, excluded);
  for (auto t: excluded) {
    g+=energy[t];
  }
  *d = max_dist;
  return g;
}


static pair<string, int>
best_gain(Graph& mst, string r, map<string, int>& energy, int * d)
{
  int bb = numeric_limits<int>::min();
  string s;
  for (auto n: mst.nodes) {
    if (r !=  n.name && energy[n.name] != 0) {
      int x;
      int g = gain(mst, r, n.name, energy, &x);
      if (g > bb) {
        bb = g;
        s = n.name;
        *d = x;
      }
      // cout << " gain(" << r << ", " << n.name << ") = " << g << endl;
    }
  }
  return make_pair(s, bb);
}

static int
ewma(Graph& mst, string r, map<string, int>& energy)
{
  set<string> C;
  set<string> F;
  set<string> E;
  C.emplace(r);

   map<string, int> energy2 = energy;

  while (C.size() < mst.nodes.size()) {
    /* compute C - F - E */
    set<string> tmp;
    set_difference(C.begin(), C.end(), F.begin(), F.end(), inserter(tmp, tmp.end()));
    set<string> x;
    set_difference(tmp.begin(), tmp.end(), E.begin(), E.end(), inserter(x, x.end()));

    string max;
    string q;
    int max_gain = numeric_limits<int>::min();
    int ddd = 0;
    for (auto n : x) {
      int distance;
      auto p = best_gain(mst, n, energy, &distance);
      cout << p.first << " " << p.second << endl;
      if (p.second > max_gain) {
        max= p.first;
        max_gain = p.second;
        ddd = distance;
        q = n;
      }
    }
    // cout << " HERE : " << max_gain << " q: " << q << " max: " << max_gain << " distance: " << ddd << endl;
    if (max_gain <= 0) {
      cout << "uffffffffff" << endl;
      max_gain = numeric_limits<int>::max();
      for (auto n : x) {
        if (energy[n] > 0 && energy[n] < max_gain) {
          q = n;
          max_gain = energy[q];
          ddd = energy[q];
        }
      }
    }
    // TODO: a case where max_gain is below 0
    F.emplace(q);
    cout << " LA Q " << q << " la ddd " << ddd << endl;
    energy2[q] = ddd;
    auto node_q = mst.find_node(q);
    for (auto n : mst.nodes) {
      if (node_q.distance(n) <= ddd) {
        C.emplace(n.name);
      }
    }
    set<string> excluded;
    compute_nodes_to_exclude(mst, r, energy, ddd, excluded);
    for (auto n: excluded) {
      E.emplace(n);
      energy2[n] = 0;
      energy[n] = 0;
    }

    // energy[q] = ddd;

    cout << "F:";
    for (auto s: F) {
      cout << s << " ";
    }
    cout << endl;
    cout << "C:";
    for (auto s: C) {
      cout << s << " ";
    }
    cout << endl;
    cout << "E:";
    for (auto s: E) {
      cout << s << " ";
    }
    cout << endl;
  }
  int final_e = 0;
  for (auto n : energy2) {
    cout << " " << n.first << " => " << n.second << endl;
    final_e += n.second;
  }
  return final_e;
}


int
main(int argc, char const *argv[]) {
  initiatilze_graph();
  map<string, int> energy;
  cout << "e(mwst) = " << energy_required(graph, "10", energy) << endl;
  cout << "Initial energy requirement per node:" << endl;
  for (auto e: energy) {
    cout << "\t" << e.first << ": " << e.second << endl;
  }

  cout << "we can reduce the consumption to " << ewma(graph, "10", energy) << endl;

  return 0;
}
