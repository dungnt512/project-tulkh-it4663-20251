#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using db = double;
using ld = long double;

const int inf = 0x3f3f3f3f;
const ll linf = 0x3f3f3f3f3f3f3f3f;


mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

ll rnd(ll l, ll r) { return uniform_int_distribution<ll>(l, r)(rng); }
ld rnd_db(ld l, ld r) { return uniform_real_distribution<ld>(l, r)(rng); }


template<class U, class V>
bool minimize(U &a, V b) { return b < a ? a = b, 1 : 0; };
template<class U, class V>
bool maximize(U &a, V b) { return a < b ? a = b, 1 : 0; };

const int K = 1e2;
const int N = 1e3 + K + 5;

struct Node {
  int i = 0;
  Node *L = NULL, *R = NULL;
  Node(int i) : i(i) {}
  Node(Node *L = NULL, Node *R = NULL) : L(L), R(R) {}
};

struct Route {
  int cnt = 1;
  int weight = 0;
  Node *st = NULL, *en = NULL;
  Route() {
    st = new Node();
    st->R = en = new Node(st);
  }
  Route(Node *st, Node *en) : st(st), en(en) {}

  friend ostream& operator << (ostream &os, Route &rt) {
    os << rt.cnt << "\n";
    auto it = rt.st;
    while (it != rt.en) {
      os << it->i << " ";
      it = it->R;
    }
    return os;
  }
};

int n, k, dist[N][N], id[N];

Node *nodes[N];
Route *rt[K];
// multiset<int> st;

struct Sol {
  int obj;
  vector<int> weight;
  vector<vector<int>> route;
  Sol(bool ok = 1) {
    route.resize(k);
    weight.resize(k);
    obj = 0;
    if (ok) {
      for (int i = 0; i < k; i++) {
        auto &r = rt[i];
        weight[i] = r->weight;
        maximize(obj, r->weight);
        for (auto it = r->st; it != r->en; it = it->R) {
          route[i].push_back(it->i);
        }
      }
    }
  }

  bool operator < (Sol &o) {
    return obj < o.obj;
  }
} best_sol;

ostream& operator << (ostream &os, Sol &best_sol) {
  os << k << "\n";
  for (auto &r : best_sol.route) {
    os << r.size() << "\n";
    for (auto &u : r) cout << u << " ";
    os << "\n";
  }
  return os;
}


void prt() {
  // cout << k << "\n";
  // for (int i = 0; i < k; i++) cout << *rt[i] << "\n";
  cout << best_sol;
}


int greedy() {
  for (int i = 1; i < n; i++) { 
    if (nodes[i]) delete nodes[i];
    nodes[i] = new Node(i); 
  }
  for (int i = 0; i < k; i++) {
    if (rt[i]) delete rt[i];
    rt[i] = new Route();
  }

  shuffle(id + 1, id + n, rng);
  for (int i = 0; i < k; i++) rt[i]->en->i = n;
  // cerr << "ok\n";
  for (int it = 1; it < n; it++) {
    int i = id[it];
    int mn = inf, l = -1;
    Node *best;
    for (int j = 0; j < k; j++) {
      auto &r = rt[j];
      auto it = r->st;
      // cerr << j << ": ";
      // cerr << it << " " << r->en << "\n";
      while (it != r->en) {
        // cerr << it->i << " ";
        if (minimize(mn, r->weight - dist[it->i][it->R->i] + dist[it->i][i] + dist[i][it->R->i])) {
          l = j;
          best = it;
        }
        it = it->R;
      }
    }
    // cerr << "\n";
    // cerr << nodes[i]->i << " " << l << " " << best->i << ", ";

    if (~l) {
      nodes[i]->R = best->R;
      nodes[i]->L = best;
      best->R->L = nodes[i];
      best->R = nodes[i];
      rt[l]->weight = mn;
      rt[l]->cnt++;
    }
    // cerr << nodes[i]->i << " " << l << "\n";
  }
  // exit(0);

  int mx = 0;
  for (int i = 0; i < k; i++) maximize(mx, rt[i]->weight);
  return mx;
  // for (int i = 0; i < k; i++) st.insert(rt[i]->weight);
}

struct DSU {
  vector<int> par, sz;
  int n;
  DSU(int n): n(n) {
    par.assign(n, -1);
    sz.assign(n, 1);
  }
  int fnd(int x) {
    return (par[x] < 0 ? x: par[x] = fnd(par[x]));
  }
  bool unite(int x, int y) {
    if ((x = fnd(x)) == (y = fnd(y))) return 0;
    if (sz[x] < sz[y]) swap(x, y);
    par[y] = x;
    sz[x] += sz[y];
    return 1;
  }
};

pair<int, vector<pair<int, int>>> prim() {
  int tot = 0;
  int m = n + k - 1;
  vector<int> used(m);
  vector<pair<int, int>> min_e(m, {inf, -1});
  vector<pair<int, int>> edges;
  min_e[0].first = 0;
  for (int i = 0; i < m; i++) {
    int v = -1;
    for (int j = 0; j < m; j++) {
      if (!used[j] && (!~v || min_e[j].first < min_e[v].first)) {
        v = j;
      }
    }

    if (min_e[v].first == inf) {
      cout << "No MST\n";
      exit(0);
    }
    used[v] = 1;
    tot += min_e[v].first;
    if (~min_e[v].second) {
      edges.push_back({v, min_e[v].second});
    }

    for (int u = 0; u < m; u++) {
      if (v != u && dist[v][u] < min_e[u].first) {
        min_e[u] = {dist[v][u], v};
      }
    }
  }

  return {tot, edges};
}

bool check_(int B, int printed = 0) {
  vector<int> used(n, 0), par(n, -1), list;
  vector<pair<int, int>> f(n, {inf, 0});
  // set<int> st;
  // for (int i = 0; i < n; i++) st.insert(i);
  f[0] = {0, 0};
  int m = n - 1;
  if (printed) {
    cout << k << "\n";
  }
  for (int it = 0; it < k; it++) {
    // used[0] = 0;
    for (int i = 0; i < n; i++) {
      if (!used[i]) {
        list.push_back(i);
      }
    }
    for (int c = 0; c < m; c++) {
      int u = -1;
      // for (int i = 0; i < n; i++) {
      for (int i : list) {
        if (!used[i] && (!~u || f[u] > f[i])) {
          u = i;
        }
      }
      if (!~u) break;
      used[u] = 1;
      // for (int v = 0; v < n; v++) {
      for (int v : list) {
        if (!used[v] && minimize(f[v], pair<int, int>{f[u].first + dist[u][v], f[u].second + 1})) {
          par[v] = u;
        }
      }
    }

    int u = 0;
    // for (int i = 0; i < n; i++) {
    // shuffle(begin(list), end(list), rng);
    for (int i : list) {
      used[i] = 0;
      if (f[i].first <= B && (!~u || f[u].second < f[i].second || (f[u].second == f[i].second && f[u].first > f[i].first))) {
        u = i;
      }
    }
    // cerr << "->" << B << " " << u << " " << f[u].first << " " << f[u].second << "\n";

    if (!u) {
      return 0;
    }

    if (printed) {
      vector<int> cur;
      while (u) {
        used[u] = 1;
        cur.push_back(u);
        u = par[u];
      }
      cout << cur.size() + 1 << "\n" << 0 << " ";
      m -= cur.size();
      for (auto it = rbegin(cur); it != rend(cur); it++) cout << *it << " ";
      cout << "\n";
    }
    else {
      while (u) {
        m--;
        used[u] = 1;
        cerr << u << " ";
        u = par[u];
      }
      cerr << "\n";
    }

    if (!m) return 1;
  }
  return !m;
}

vector<int> ords;
vector<vector<int>> adj;

void dfs(int u) {
  ords.push_back(u);

  sort(begin(adj[u]), end(adj[u]), [&](int a, int b) {
    return dist[u][a] < dist[u][b];
  });

  for (auto v : adj[u]) {
    dfs(v);
  }
}

void dijk() {
  vector<int> f(n, inf), used(n), par(n, -1);
  f[0] = 0;
    for (int c = 1; c < n; c++) {
      int u = -1;
      for (int i = 0; i < n; i++) {
        if (!used[i] && (!~u || f[u] > f[i])) {
          u = i;
        }
      }
      if (!~u) break;
      used[u] = 1;
      for (int v = 0; v < n; v++) {
        if (minimize(f[v], f[u] + dist[u][v])) {
          par[v] = u;
        }
      }
    }

  adj.resize(n);
  for (int i = 1; i < n; i++) {
    // cerr << par[i] << " " << i << "\n";
    adj[par[i]].push_back(i);
  }
  dfs(0);
}

bool check(int B, int printed = 0) {
  int cur = 0, p = 0;
  if (printed) {
    vector<vector<int>> rt{{}};
    cerr << rt.size() << "\n";
    for (auto u : ords) {
      if (cur + dist[p][u] > B) {
        if (rt.size() >= k) {
          return 0;
        }
        cur = dist[0][u];
        rt.push_back({0, u});
      }
      else {
        cur += dist[p][u];
        rt.back().push_back(u);
      }
      p = u;
    }
    while (rt.size() < k) rt.push_back({0});
    cout << k << "\n";
    for (auto &v : rt) {
      for (auto &u : v) cout << u << " ";
      cout << "\n";
    }
    return 1;
  }
  else {
    int cnt = 1;
    for (auto u : ords) {
      if (cur + dist[p][u] > B) {
        if (cnt >= k) {
          return 0;
        }
        cnt++;
        cur = dist[0][u];
      }
      else {
        cur += dist[p][u];
      }
      p = u;
    }
    return 1;
  }

  return 1;
}

int main() {
  #define taskname "1"
  for (string iext : {"in", "inp"}) {
    if (fopen((taskname"." + iext).c_str(), "r")) {
      freopen((taskname"." + iext).c_str(), "r", stdin);
      freopen(taskname".out", "w", stdout);
      break;
    }
  }

  cin.tie(0)->sync_with_stdio(0);

  cin >> n >> k;
  k = min(n, k);
  n++;
  iota(id + 1, id + n, 1);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      cin >> dist[i][j];
    }
  }


  // int curr = greedy();
  int curr = inf;
  for (int it = 0; it < 8000; it++) {
    if (minimize(curr, greedy())) {
      best_sol = Sol(1);
    }
  }

  // return 0;

  for (int i = n; i + 1 < n + k; i++) {
    for (int j = 0; j < n; j++) {
      dist[i][j] = dist[0][j];
      dist[j][i] = dist[j][0];
    }
    for (int j = n; j < i; j++) {
      dist[i][j] = dist[j][i] = inf;
    }
  }

  auto [tot, edges] = prim();

  // prt();

  int lo = (tot + k - 1) / k, hi = curr;
  int best = -1;
  // cerr << tot << " " << lo << " " << hi << "\n";
  dijk();
  // return 0;

  while (lo <= hi) {
    int mid = lo + hi >> 1;
    if (check(mid)) {
      best = mid;
      hi = mid - 1;
    }
    else {
      lo = mid + 1;
    }
  }
  cerr << best << "\n";

  if (~best) 
    check(best, 1);
  else
    prt();
  
  return 0;
}
