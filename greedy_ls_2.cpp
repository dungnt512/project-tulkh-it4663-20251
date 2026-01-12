#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using db = double;
using ld = long db;

const int inf = 0x3f3f3f3f;
const ll linf = 0x3f3f3f3f3f3f3f3f;


// mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
// mt19937_64 rng(998244353LL * 1000000007LL);
uint64_t seed;
mt19937_64 rng;

ll rnd(ll l, ll r) { return uniform_int_distribution<ll>(l, r)(rng); }
ld rnd_db(ld l, ld r) { return uniform_real_distribution<ld>(l, r)(rng); }


template<class U, class V>
bool minimize(U &a, V b) { return b < a ? a = b, 1 : 0; };
template<class U, class V>
bool maximize(U &a, V b) { return a < b ? a = b, 1 : 0; };

const int N = 1e3 + 5;
const int K = 1e2;

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

struct Sol {
  int obj, sum = 0;
  vector<Route*> rt;
  vector<Node*> node;
  // multiset<int> st;
  Sol() {}
  Sol(int n, int k) {
    rt.resize(k);
    for (int i = 0; i < k; i++) rt[i] = new Route();
    node.resize(n);
    for (int i = 1; i < n; i++) node[i] = new Node(i);
  }

  Sol(const Sol&) = delete;
  Sol& operator=(const Sol&) = delete;

  Sol(Sol&&) noexcept = default;
  Sol& operator=(Sol&&) noexcept = default;

  ~Sol() { clear(); }

  void clear() {
    for (auto r : rt) {
      if (!r) continue;
      delete r->en;
      delete r->st;
      delete r;
    }
    for (int i = 1; i < (int)node.size(); i++) delete node[i];
    rt.clear();
    node.clear();
  }

  void init_st() {
    sum = 0;
    obj = -inf;
    for (int i = 0; i < k; i++) {
      // st.insert(rt[i]->weight);
      sum += rt[i]->weight;
      maximize(obj, rt[i]->weight);
    }
    // obj = st.empty() ? inf : *rbegin(st);
  }

  bool operator < (Sol &o) {
    if (obj != o.obj) return obj < o.obj;
    return sum < o.sum;
  }

  friend ostream& operator << (ostream &os, Sol &o) {
    os << k << "\n";
    for (int i = 0; i < k; i++) os << *(o.rt[i]) << "\n";
    return os;
  }
};

Sol sol;

Sol greedy() {
  Sol sol(n, k);
  for (int i = 0; i < k; i++) {
    sol.rt[i]->en->i = n;
  }
  shuffle(id + 1, id + n, rng);
  for (int it = 1; it < n; it++) {
    int i = id[it];
    int l = -1;
    pair<int, int> mn = {inf, inf};
    Node *best;
    for (int j = 0; j < k; j++) {
      auto &r = sol.rt[j];
      auto it = r->st;
      // cerr << j << ": ";
      // cerr << it << " " << r->en << "\n";
      while (it != r->en) {
        // cerr << it->i << " ";
        int t = - dist[it->i][it->R->i] + dist[it->i][i] + dist[i][it->R->i];
        // if (minimize(mn, r->weight + t)) {
        if (minimize(mn, pair<int, int>{r->weight + t, t})) {
          l = j;
          best = it;
        }
        it = it->R;
      }
    }
    // cerr << "\n";
    // cerr << nodes[i]->i << " " << l << " " << best->i << ", ";

    if (~l) {
      auto &n = sol.node[i];
      n->R = best->R;
      n->L = best;
      best->R->L = n;
      best->R = n;
      auto &r = sol.rt[l];
      // r->weight = mn;
      r->weight = mn.first;
      r->cnt++;
    }
    // cerr << nodes[i]->i << " " << l << "\n";
  }
  // exit(0);
  // for (int i = 0; i < k; i++) st.insert(rt[i]->weight);
  sol.init_st();
  return move(sol);
}

Sol greedy_2() {
  Sol sol(n, k);
  for (int i = 0; i < k; i++) {
    sol.rt[i]->en->i = n;
  }
  shuffle(id + 1, id + n, rng);
  for (int it = 1; it < n; it++) {
    int i = id[it];
    int l = -1;
    pair<int, int> mn = {inf, inf};
    Node *best;
    for (int j = 0; j < k; j++) {
      auto &r = sol.rt[j];
      auto it = r->en->L;
      // cerr << j << ": ";
      // cerr << it << " " << r->en << "\n";
      // while (it != r->en) {
        // cerr << it->i << " ";
        int t = - dist[it->i][it->R->i] + dist[it->i][i] + dist[i][it->R->i];
        // if (minimize(mn, r->weight + t)) {
        if (minimize(mn, pair<int, int>{r->weight + t, t})) {
          l = j;
          best = it;
        }
        // it = it->R;
      // }
    }
    // cerr << "\n";
    // cerr << nodes[i]->i << " " << l << " " << best->i << ", ";

    if (~l) {
      auto &n = sol.node[i];
      n->R = best->R;
      n->L = best;
      best->R->L = n;
      best->R = n;
      auto &r = sol.rt[l];
      // r->weight = mn;
      r->weight = mn.first;
      r->cnt++;
    }
    // cerr << nodes[i]->i << " " << l << "\n";
  }
  // exit(0);
  // for (int i = 0; i < k; i++) st.insert(rt[i]->weight);
  sol.init_st();
  return move(sol);
}

struct Obj {
  int mx, sum = 0;
  bool operator < (Obj &o) {
    if (mx != o.mx) return mx < o.mx;
    return sum < o.sum;
  }
};

struct Solution {
  int ub;
  Obj O;
  vector<int> w;
  vector<vector<int>> rt;
  set<pair<int, int>> st;
  Solution() {}
  Solution(int n, int k, int ub) : ub(ub) {
    w.resize(k);
    rt.resize(k, vector<int>(2));
  }
  Solution(Sol &o, int ub) : ub(ub) {
    O.sum = o.sum;
    O.mx = o.obj;
    int k = o.rt.size();
    rt.resize(k);
    w.resize(k);
    for (int i = 0; i < k; i++) {
      auto &r = o.rt[i];
      // obj += r->weight > ub;
      w[i] = r->weight;
      st.insert({r->weight, i});
      auto it = r->st;
      while (it != r->en) {
        rt[i].push_back(it->i);
        it = it->R;
      }
      rt[i].push_back(n);
    }
  }

  bool operator < (Solution &o) {
    // if (obj != o.obj) return obj < o.obj;
    // return sum < o.sum;
    return O < o.O;
  }

  friend ostream& operator << (ostream &os, Solution &o) {
    os << k << "\n";
    int mx = 0;
    for (int i = 0; i < k; i++) {
      // for (auto u : o.rt[i]) os << u << " ";
      os << o.rt[i].size() - 1 << "\n";
      int cur = 0;
      for (int j = 0; j + 1 < o.rt[i].size(); j++) {
        os << o.rt[i][j] << " ";
        cur += dist[o.rt[i][j]][o.rt[i][j + 1]];
      }
      maximize(mx, cur);
      os << "\n";
    }
    // cerr << "->" << mx << "\n";
    return os;
  }

  Obj _oneMove(int x, int y, int u, int v) {
    if (!okv(x, y) || !oki(u, v)) return O;
    if (x == u && (v == y || v == y + 1)) return O;

    int mx = -inf;
    int sum = O.sum;
    if (x == u) {
      int &t = w[x];
      auto &r = rt[x];
      int _t = t - dist[r[y - 1]][r[y]] - dist[r[y]][r[y + 1]] + dist[r[y - 1]][r[y + 1]] - dist[r[v - 1]][r[v]] + dist[r[v - 1]][r[y]] + dist[r[y]][r[v]];
      maximize(mx, _t);
      sum += _t - t;
      auto it = rbegin(st);
      for ( ; it != rend(st); it++) {
        if (it->second != x) {
          maximize(mx, it->first);
          break;
        }
      }
      return {mx, sum};
    }

    int &z = w[x], &t = w[u];
    auto &a = rt[x];
    auto &b = rt[u];
    int _z = z - dist[a[y - 1]][a[y]] - dist[a[y]][a[y + 1]] + dist[a[y - 1]][a[y + 1]];
    int _t = t - dist[b[v - 1]][b[v]] + dist[b[v - 1]][a[y]] + dist[a[y]][b[v]];
    maximize(mx, _z);
    maximize(mx, _t);
    sum += _t + _z - t - z;
    auto it = rbegin(st);
    for ( ; it != rend(st); it++) {
      if (it->second != x && it->second != u) {
        maximize(mx, it->first);
        break;
      }
    }
    return {mx, sum};
  }

  Obj oneMove(int x, int y, int u, int v) {
    if (!okv(x, y) || !oki(u, v)) return O;
    if (x == u && (v == y || v == y + 1)) return O;

    if (x == u) {
      int &t = w[x];
      auto &r = rt[x];
      O.sum -= t;
      st.erase({t, x});
      t = t - dist[r[y - 1]][r[y]] - dist[r[y]][r[y + 1]] + dist[r[y - 1]][r[y + 1]] - dist[r[v - 1]][r[v]] + dist[r[v - 1]][r[y]] + dist[r[y]][r[v]];
      O.sum += t;
      st.insert({t, x});
      if (y < v) {
        r.insert(begin(r) + v, r[y]);
        r.erase(begin(r) + y);
      }
      else {
        r.insert(begin(r) + v, r[y]);
        r.erase(begin(r) + y + 1);
      }
      O.mx = rbegin(st)->first;
      return O;
    }

    int &z = w[x], &t = w[u];
    auto &a = rt[x];
    auto &b = rt[u];
    O.sum -= z + t;
    st.erase({z, x});
    st.erase({t, u});
    z = z - dist[a[y - 1]][a[y]] - dist[a[y]][a[y + 1]] + dist[a[y - 1]][a[y + 1]];
    t = t - dist[b[v - 1]][b[v]] + dist[b[v - 1]][a[y]] + dist[a[y]][b[v]];

    b.insert(begin(b) + v, a[y]);
    a.erase(begin(a) + y);

    O.sum += z + t;
    st.insert({z, x});
    st.insert({t, u});

    O.mx = rbegin(st)->first;

    return O;
  }

  bool sym = true; 

  inline void rm(int x) { O.sum -= w[x]; st.erase({w[x], x}); }
  inline void add(int x) { O.sum += w[x]; st.insert({w[x], x}); }

  inline int mx2(int a, int b = -1) {
    for (auto it = rbegin(st); it != rend(st); it++) {
      int id = it->second;
      if (id != a && id != b) return it->first;
    }
    return -inf;
  }

  inline bool okv(int x, int y) const {
    return 0 <= x && x < (int)rt.size() && 1 <= y && y <= (int)rt[x].size() - 2;
  }
  inline bool oki(int x, int y) const {
    return 0 <= x && x < (int)rt.size() && 1 <= y && y <= (int)rt[x].size() - 1;
  }

  inline int seg(const vector<int> &r, int l, int rr) const {
    int s = 0;
    for (int i = l; i < rr; i++) s += dist[r[i]][r[i + 1]];
    return s;
  }
  inline int segR(const vector<int> &r, int l, int rr) const {
    int s = 0;
    for (int i = l; i < rr; i++) s += dist[r[i + 1]][r[i]];
    return s;
  }

  Obj _swp2(int x, int y, int u, int v) {
    if (!okv(x, y) || !okv(u, v)) return O;

    int mx = -inf;
    int sum = O.sum;

    if (x == u) {
      if (y == v) return O;
      auto &r = rt[x];
      int t = w[x];

      int i = y, j = v;
      if (i > j) swap(i, j);

      int A = r[i], B = r[j];
      int nt = t;

      if (j == i + 1) {
        int pre = r[i - 1], post = r[j + 1];
        nt = t - dist[pre][A] - dist[A][B] - dist[B][post] + dist[pre][B] + dist[B][A] + dist[A][post];
      } else {
        int pi = r[i - 1], ni = r[i + 1];
        int pj = r[j - 1], nj = r[j + 1];
        nt = t - dist[pi][A] - dist[A][ni] - dist[pj][B] - dist[B][nj] + dist[pi][B] + dist[B][ni] + dist[pj][A] + dist[A][nj];
      }

      maximize(mx, nt);
      sum += nt - t;
      maximize(mx, mx2(x));
      return {mx, sum};
    }

    auto &a = rt[x];
    auto &b = rt[u];
    int za = w[x], tb = w[u];

    int A = a[y], B = b[v];

    int nza = za - dist[a[y - 1]][A] - dist[A][a[y + 1]] + dist[a[y - 1]][B] + dist[B][a[y + 1]];
    int ntb = tb - dist[b[v - 1]][B] - dist[B][b[v + 1]] + dist[b[v - 1]][A] + dist[A][b[v + 1]];

    maximize(mx, nza);
    maximize(mx, ntb);
    sum += (nza - za) + (ntb - tb);
    maximize(mx, mx2(x, u));

    return {mx, sum};
  }

  Obj swp2(int x, int y, int u, int v) {
    if (!okv(x, y) || !okv(u, v)) return O;
    if (x == u) {
      if (y == v) return O;
      auto &r = rt[x];

      int i = y, j = v;
      if (i > j) swap(i, j);

      rm(x);

      int t = w[x];
      int A = r[i], B = r[j];

      int nt = t;
      if (j == i + 1) {
        int pre = r[i - 1], post = r[j + 1];
        nt = t - dist[pre][A] - dist[A][B] - dist[B][post] + dist[pre][B] + dist[B][A] + dist[A][post];
      } else {
        int pi = r[i - 1], ni = r[i + 1];
        int pj = r[j - 1], nj = r[j + 1];
        nt = t - dist[pi][A] - dist[A][ni] - dist[pj][B] - dist[B][nj] + dist[pi][B] + dist[B][ni] + dist[pj][A] + dist[A][nj];
      }

      w[x] = nt;
      swap(r[i], r[j]);

      add(x);
      O.mx = rbegin(st)->first;
      return O;
    }

    auto &a = rt[x];
    auto &b = rt[u];

    rm(x); rm(u);

    int za = w[x], tb = w[u];
    int A = a[y], B = b[v];

    int nza = za - dist[a[y - 1]][A] - dist[A][a[y + 1]] + dist[a[y - 1]][B] + dist[B][a[y + 1]];

    int ntb = tb - dist[b[v - 1]][B] - dist[B][b[v + 1]] + dist[b[v - 1]][A] + dist[A][b[v + 1]];

    w[x] = nza;
    w[u] = ntb;
    swap(a[y], b[v]);

    add(x); add(u);
    O.mx = rbegin(st)->first;
    return O;
  }

  Obj _opt2(int x, int i, int j) {
    if (!okv(x, i) || !okv(x, j)) return O;
    if (i == j) return O;
    if (i > j) swap(i, j);

    auto &r = rt[x];
    int t = w[x];

    int nt = t - dist[r[i - 1]][r[i]] - dist[r[j]][r[j + 1]] + dist[r[i - 1]][r[j]] + dist[r[i]][r[j + 1]];

    if (!sym) nt += segR(r, i, j) - seg(r, i, j);

    int sum = O.sum + (nt - t);
    int mx = max(nt, mx2(x));
    return {mx, sum};
  }

  Obj opt2(int x, int i, int j) {
    if (!okv(x, i) || !okv(x, j)) return O;
    if (i == j) return O;
    if (i > j) swap(i, j);

    auto &r = rt[x];
    rm(x);

    int t = w[x];
    int nt = t - dist[r[i - 1]][r[i]] - dist[r[j]][r[j + 1]] + dist[r[i - 1]][r[j]] + dist[r[i]][r[j + 1]];

    if (!sym) nt += segR(r, i, j) - seg(r, i, j);

    reverse(begin(r) + i, begin(r) + j + 1);
    w[x] = nt;

    add(x);
    O.mx = rbegin(st)->first;
    return O;
  }

  Obj _opt2s(int x, int cutA, int u, int cutB) {
    if (x == u) return O;
    if (!(0 <= x && x < (int)rt.size() && 0 <= u && u < (int)rt.size())) return O;

    auto &A = rt[x];
    auto &B = rt[u];

    if (!(0 <= cutA && cutA <= (int)A.size() - 2)) return O;
    if (!(0 <= cutB && cutB <= (int)B.size() - 2)) return O;

    int wa = w[x], wb = w[u];

    int prefA = seg(A, 0, cutA);
    int sufA  = seg(A, cutA + 1, (int)A.size() - 1);

    int prefB = seg(B, 0, cutB);
    int sufB  = seg(B, cutB + 1, (int)B.size() - 1);

    int nA = prefA + dist[A[cutA]][B[cutB + 1]] + sufB;
    int nB = prefB + dist[B[cutB]][A[cutA + 1]] + sufA;

    int sum = O.sum + (nA - wa) + (nB - wb);
    int mx = max({nA, nB, mx2(x, u)});
    return {mx, sum};
  }

  Obj opt2s(int x, int cutA, int u, int cutB) {
    if (x == u) return O;
    if (!(0 <= x && x < (int)rt.size() && 0 <= u && u < (int)rt.size())) return O;

    auto &A = rt[x];
    auto &B = rt[u];

    if (!(0 <= cutA && cutA <= (int)A.size() - 2)) return O;
    if (!(0 <= cutB && cutB <= (int)B.size() - 2)) return O;

    rm(x); rm(u);

    int prefA = seg(A, 0, cutA);
    int sufA  = seg(A, cutA + 1, (int)A.size() - 1);

    int prefB = seg(B, 0, cutB);
    int sufB  = seg(B, cutB + 1, (int)B.size() - 1);

    int nA = prefA + dist[A[cutA]][B[cutB + 1]] + sufB;
    int nB = prefB + dist[B[cutB]][A[cutA + 1]] + sufA;

    vector<int> nR, nS;
    nR.reserve(A.size() + B.size());
    nS.reserve(A.size() + B.size());

    nR.insert(end(nR), begin(A), begin(A) + cutA + 1);
    nR.insert(end(nR), begin(B) + cutB + 1, end(B));

    nS.insert(end(nS), begin(B), begin(B) + cutB + 1);
    nS.insert(end(nS), begin(A) + cutA + 1, end(A));

    A.swap(nR);
    B.swap(nS);

    w[x] = nA;
    w[u] = nB;

    add(x); add(u);
    O.mx = rbegin(st)->first;
    return O;
  }

  Obj _or(int x, int y, int len, int u, int v) {
    if (!(2 <= len && len <= 4)) return O;
    if (!okv(x, y) || !oki(u, v)) return O;

    auto &A = rt[x];
    if (y + len - 1 > (int)A.size() - 2) return O;

    int mx = -inf;
    int sum = O.sum;

    if (x == u) {
      if (v >= y && v <= y + len) return O;

      int t = w[x];

      int pre = A[y - 1];
      int fst = A[y];
      int lst = A[y + len - 1];
      int post = A[y + len];

      int a = A[v - 1], b = A[v];

      int nt = t - dist[pre][fst] - dist[lst][post] + dist[pre][post] - dist[a][b] + dist[a][fst] + dist[lst][b];

      maximize(mx, nt);
      sum += nt - t;
      maximize(mx, mx2(x));
      return {mx, sum};
    }

    auto &B = rt[u];
    int za = w[x], tb = w[u];

    int pre = A[y - 1];
    int fst = A[y];
    int lst = A[y + len - 1];
    int post = A[y + len];

    int inner = seg(A, y, y + len - 1);

    int nza = za - dist[pre][fst] - inner - dist[lst][post] + dist[pre][post];

    int a = B[v - 1], b = B[v];
    int ntb = tb - dist[a][b] + dist[a][fst] + inner + dist[lst][b];

    maximize(mx, nza);
    maximize(mx, ntb);
    sum += (nza - za) + (ntb - tb);
    maximize(mx, mx2(x, u));
    return {mx, sum};
  }

  Obj orOpt(int x, int y, int len, int u, int v) {
    if (!(2 <= len && len <= 4)) return O;
    if (!okv(x, y) || !oki(u, v)) return O;

    auto &A = rt[x];
    if (y + len - 1 > (int)A.size() - 2) return O;

    if (x == u) {
      if (v >= y && v <= y + len) return O;

      rm(x);

      int t = w[x];

      int pre = A[y - 1];
      int fst = A[y];
      int lst = A[y + len - 1];
      int post = A[y + len];

      int a = A[v - 1], b = A[v];

      int nt = t - dist[pre][fst] - dist[lst][post] + dist[pre][post] - dist[a][b] + dist[a][fst] + dist[lst][b];

      vector<int> s(begin(A) + y, begin(A) + y + len);

      if (y < v) {
        A.insert(begin(A) + v, begin(s), end(s));
        A.erase(begin(A) + y, begin(A) + y + len);
      } else {
        A.insert(begin(A) + v, begin(s), end(s));
        A.erase(begin(A) + y + len, begin(A) + y + 2 * len);
      }

      w[x] = nt;

      add(x);
      O.mx = rbegin(st)->first;
      return O;
    }


    auto &B = rt[u];

    rm(x); rm(u);

    int za = w[x], tb = w[u];

    int pre = A[y - 1];
    int fst = A[y];
    int lst = A[y + len - 1];
    int post = A[y + len];

    int inner = seg(A, y, y + len - 1);

    int nza = za - dist[pre][fst] - inner - dist[lst][post] + dist[pre][post];

    int a = B[v - 1], b = B[v];
    int ntb = tb - dist[a][b] + dist[a][fst] + inner + dist[lst][b];

    vector<int> s(begin(A) + y, begin(A) + y + len);
    A.erase(begin(A) + y, begin(A) + y + len);
    B.insert(begin(B) + v, begin(s), end(s));

    w[x] = nza;
    w[u] = ntb;

    add(x); add(u);
    O.mx = rbegin(st)->first;
    return O;
  }

  Obj _swp3(int x, int y, int u, int v) {
    if (!okv(x, y) || !okv(x, y + 1) || !okv(u, v)) return O;

    int mx = -inf;
    int sum = O.sum;

    if (x == u) {
      auto &r = rt[x];
      if (v == y || v == y + 1) return O;

      vector<int> tmp = r;
      int p = tmp[y], q = tmp[y + 1];

      tmp.erase(begin(tmp) + y, begin(tmp) + y + 2);
      int vv = v;
      if (vv > y) vv -= 2;

      int s = tmp[vv];
      tmp.erase(begin(tmp) + vv);

      tmp.insert(begin(tmp) + y, s);
      tmp.insert(begin(tmp) + vv, {p, q});

      int nt = seg(tmp, 0, (int)tmp.size() - 1);
      if (nt == w[x]) return O;

      sum += nt - w[x];
      mx = max(nt, mx2(x));
      return {mx, sum};
    }

    auto &A = rt[x];
    auto &B = rt[u];

    int wa = w[x], wb = w[u];

    int a0 = A[y - 1], p = A[y], q = A[y + 1], a3 = A[y + 2];
    int b0 = B[v - 1], s = B[v], b2 = B[v + 1];

    int nA = wa - dist[a0][p] - dist[p][q] - dist[q][a3] + dist[a0][s] + dist[s][a3];
    int nB = wb - dist[b0][s] - dist[s][b2] + dist[b0][p] + dist[p][q] + dist[q][b2];

    maximize(mx, nA);
    maximize(mx, nB);
    sum += (nA - wa) + (nB - wb);
    maximize(mx, mx2(x, u));
    return {mx, sum};
  }

  Obj swp3(int x, int y, int u, int v) {
    if (!okv(x, y) || !okv(x, y + 1) || !okv(u, v)) return O;

    if (x == u) {
      auto &r = rt[x];
      if (v == y || v == y + 1) return O;

      rm(x);

      vector<int> tmp = r;
      int p = tmp[y], q = tmp[y + 1];

      tmp.erase(begin(tmp) + y, begin(tmp) + y + 2);
      int vv = v;
      if (vv > y) vv -= 2;

      int s = tmp[vv];
      tmp.erase(begin(tmp) + vv);

      tmp.insert(begin(tmp) + y, s);
      tmp.insert(begin(tmp) + vv, {p, q});

      r.swap(tmp);
      w[x] = seg(r, 0, (int)r.size() - 1);

      add(x);
      O.mx = rbegin(st)->first;
      return O;
    }

    auto &A = rt[x];
    auto &B = rt[u];

    rm(x); rm(u);

    int wa = w[x], wb = w[u];

    int a0 = A[y - 1], p = A[y], q = A[y + 1], a3 = A[y + 2];
    int b0 = B[v - 1], s = B[v], b2 = B[v + 1];

    int nA = wa - dist[a0][p] - dist[p][q] - dist[q][a3] + dist[a0][s] + dist[s][a3];
    int nB = wb - dist[b0][s] - dist[s][b2] + dist[b0][p] + dist[p][q] + dist[q][b2];

    A.erase(begin(A) + y, begin(A) + y + 2);
    A.insert(begin(A) + y, s);

    B.erase(begin(B) + v);
    B.insert(begin(B) + v, {p, q});

    w[x] = nA;
    w[u] = nB;

    add(x); add(u);
    O.mx = rbegin(st)->first;
    return O;
  }

  Obj _xchg(int x, int l1, int r1, int u, int l2, int r2) {
    if (x == u) return O;
    if (!(0 <= x && x < (int)rt.size() && 0 <= u && u < (int)rt.size())) return O;

    auto &A = rt[x];
    auto &B = rt[u];

    if (!(1 <= l1 && l1 <= r1 && r1 <= (int)A.size() - 2)) return O;
    if (!(1 <= l2 && l2 <= r2 && r2 <= (int)B.size() - 2)) return O;

    int wa = w[x], wb = w[u];

    int inA = seg(A, l1, r1);
    int inB = seg(B, l2, r2);

    int nA = wa - dist[A[l1 - 1]][A[l1]] - inA - dist[A[r1]][A[r1 + 1]] + dist[A[l1 - 1]][B[l2]] + inB + dist[B[r2]][A[r1 + 1]];
    int nB = wb - dist[B[l2 - 1]][B[l2]] - inB - dist[B[r2]][B[r2 + 1]] + dist[B[l2 - 1]][A[l1]] + inA + dist[A[r1]][B[r2 + 1]];

    int sum = O.sum + (nA - wa) + (nB - wb);
    int mx = max({nA, nB, mx2(x, u)});
    return {mx, sum};
  }

  Obj xchg(int x, int l1, int r1, int u, int l2, int r2) {
    if (x == u) return O;
    if (!(0 <= x && x < (int)rt.size() && 0 <= u && u < (int)rt.size())) return O;

    auto &A = rt[x];
    auto &B = rt[u];

    if (!(1 <= l1 && l1 <= r1 && r1 <= (int)A.size() - 2)) return O;
    if (!(1 <= l2 && l2 <= r2 && r2 <= (int)B.size() - 2)) return O;

    rm(x); rm(u);

    int wa = w[x], wb = w[u];

    int inA = seg(A, l1, r1);
    int inB = seg(B, l2, r2);

    int nA = wa
      - dist[A[l1 - 1]][A[l1]] - inA - dist[A[r1]][A[r1 + 1]]
      + dist[A[l1 - 1]][B[l2]] + inB + dist[B[r2]][A[r1 + 1]];

    int nB = wb
      - dist[B[l2 - 1]][B[l2]] - inB - dist[B[r2]][B[r2 + 1]]
      + dist[B[l2 - 1]][A[l1]] + inA + dist[A[r1]][B[r2 + 1]];

    vector<int> sA(begin(A) + l1, begin(A) + r1 + 1);
    vector<int> sB(begin(B) + l2, begin(B) + r2 + 1);

    A.erase(begin(A) + l1, begin(A) + r1 + 1);
    B.erase(begin(B) + l2, begin(B) + r2 + 1);

    A.insert(begin(A) + l1, begin(sB), end(sB));
    B.insert(begin(B) + l2, begin(sA), end(sA));

    w[x] = nA;
    w[u] = nB;

    add(x); add(u);
    O.mx = rbegin(st)->first;
    return O;
  }

  Obj _opt3(int x, int i, int j, int kk) {
    if (!(0 <= x && x < (int)rt.size())) return O;
    auto &r = rt[x];
    int n = (int)r.size();
    if (!(1 <= i && i < j && j < kk && kk <= n - 1)) return O;

    auto calc = [&](bool rb, bool rc, bool sw) -> int {
      vector<int> tmp;
      tmp.reserve(n);

      auto add_rng = [&](int l, int rr, bool rev) {
        if (!rev) for (int t = l; t <= rr; t++) tmp.push_back(r[t]);
        else for (int t = rr; t >= l; t--) tmp.push_back(r[t]);
      };

      for (int t = 0; t <= i - 1; t++) tmp.push_back(r[t]);

      if (!sw) {
        add_rng(i, j - 1, rb);
        add_rng(j, kk - 1, rc);
      } else {
        add_rng(j, kk - 1, rc);
        add_rng(i, j - 1, rb);
      }

      for (int t = kk; t <= n - 1; t++) tmp.push_back(r[t]);

      return seg(tmp, 0, (int)tmp.size() - 1);
    };

    int best = w[x];

    best = min(best, calc(1, 0, 0));
    best = min(best, calc(0, 1, 0));
    best = min(best, calc(1, 1, 0));
    best = min(best, calc(0, 0, 1));
    best = min(best, calc(1, 0, 1));
    best = min(best, calc(0, 1, 1));
    best = min(best, calc(1, 1, 1));

    if (best == w[x]) return O;

    int sum = O.sum + (best - w[x]);
    int mx = max(best, mx2(x));
    return {mx, sum};
  }

  Obj opt3(int x, int i, int j, int kk) {
    if (!(0 <= x && x < (int)rt.size())) return O;
    auto &r = rt[x];
    int n = (int)r.size();
    if (!(1 <= i && i < j && j < kk && kk <= n - 1)) return O;

    auto build = [&](bool rb, bool rc, bool sw) -> pair<vector<int>, int> {
      vector<int> tmp;
      tmp.reserve(n);

      auto add_rng = [&](int l, int rr, bool rev) {
        if (!rev) for (int t = l; t <= rr; t++) tmp.push_back(r[t]);
        else for (int t = rr; t >= l; t--) tmp.push_back(r[t]);
      };

      for (int t = 0; t <= i - 1; t++) tmp.push_back(r[t]);

      if (!sw) {
        add_rng(i, j - 1, rb);
        add_rng(j, kk - 1, rc);
      } else {
        add_rng(j, kk - 1, rc);
        add_rng(i, j - 1, rb);
      }

      for (int t = kk; t <= n - 1; t++) tmp.push_back(r[t]);

      int c = seg(tmp, 0, (int)tmp.size() - 1);
      return {tmp, c};
    };

    rm(x);

    int bestC = w[x];
    vector<int> bestR = r;

    auto relax = [&](bool rb, bool rc, bool sw) {
      auto [tmp, c] = build(rb, rc, sw);
      if (c < bestC) { bestC = c; bestR.swap(tmp); }
    };

    relax(1, 0, 0);
    relax(0, 1, 0);
    relax(1, 1, 0);
    relax(0, 0, 1);
    relax(1, 0, 1);
    relax(0, 1, 1);
    relax(1, 1, 1);

    r.swap(bestR);
    w[x] = bestC;

    add(x);
    O.mx = rbegin(st)->first;
    return O;
  }

  Obj _flp(int x){
    auto &r = rt[x];
    int m = r.size();
    if (m < 4) return O;    

    int t = w[x];
    int f = r[1];
    int l = r[m - 2];

    int nt = t - dist[0][f] + dist[0][l];
    if (!sym) nt = seg(r, 0, 0) + segR(r, 1, m - 2); 

    int sum = O.sum + (nt - t);
    int mx = max(nt, mx2(x));
    return {mx, sum};
  }

  Obj flp(int x){
    auto &r = rt[x];
    int m = r.size();
    if (m < 4) return O;

    rm(x);

    int t = w[x];
    int f = r[1];
    int l = r[m - 2];

    int nt = t - dist[0][f] + dist[0][l];
    if (!sym) nt = seg(r, 0, 0) + segR(r, 1, m - 2);

    reverse(begin(r) + 1, end(r) - 1);
    w[x] = nt;

    add(x);
    O.mx = rbegin(st)->first;
    return O;
  }

  int px() {
    int kk = (int)rt.size();
    if (kk == 1) return 0;
    if (rnd_db(0, 1) < 0.95) return rbegin(st)->second;
    return (int)rnd(0, kk - 1);
    // return rbegin(st)->second;
  }

  int pu(int x) {
    int kk = (int)rt.size();
    if (kk <= 1) return x;
    if (rnd_db(0, 1) < 0.50) {
      // for (auto it = rbegin(st); it != rend(st); it++) if (it->second != x) return it->second;
      for (auto it = begin(st); it != end(st); it++) if (it->second != x) return it->second;
    }
    int u = x;
    while (u == x) u = (int)rnd(0, kk - 1);
    return u;
  }

  int py(int x) {
    int m = (int)rt[x].size();
    if (m <= 2) return -1;
    if (m == 3) return 1;
    return (int)rnd(1, m - 2);
  }

  int pv(int x) {
    int m = (int)rt[x].size();
    if (m <= 1) return -1;
    return (int)rnd(1, m - 1);
  }

  // void shk(int rep = 12) {
  //   for (int t = 0; t < rep; t++) {
  //     int x = px(), u = pu(x);
  //     int coin = (int)rnd(0, 99);

  //     if (coin < 55) {
  //       int y = py(x);
  //       if (y == -1) continue;
  //       int v = pv(u);
  //       if (v == -1) continue;
  //       if (x == u && (v == y || v == y + 1)) continue;
  //       oneMove(x, y, u, v);
  //     } else {
  //       int y = py(x), v = py(u);
  //       if (y == -1 || v == -1) continue;
  //       swp2(x, y, u, v);
  //     }
  //   }
  // }

  // void ls(db tl, int tries = 64, int stall = 6000) {
  //   using clk = chrono::steady_clock;
  //   auto t0 = clk::now();
  //   auto out = [&]() {
  //     return chrono::duration<db>(clk::now() - t0).count() >= tl;
  //   };

  //   struct Mv { int tp, p[8]; Obj v; };

  //   int no = 0;

  //   while (!out()) {
  //     Mv best;
  //     best.tp = -1;
  //     best.v = O;

  //     for (int t = 0; t < tries && !out(); t++) {
  //       int x = px(), u = pu(x);
  //       int coin = (int)rnd(0, 99);

  //       if (coin < 28) {
  //         int y = py(x);
  //         if (y == -1) continue;
  //         int uu = (rnd_db(0, 1) < 0.90 ? u : x);
  //         int v = pv(uu);
  //         if (v == -1) continue;
  //         if (uu == x && (v == y || v == y + 1)) continue;

  //         Obj c = _oneMove(x, y, uu, v);
  //         if (c < best.v) { best.tp = 0; best.p[0]=x; best.p[1]=y; best.p[2]=uu; best.p[3]=v; best.v=c; }
  //         continue;
  //       }

  //       if (coin < 48) {
  //         int len = (int)rnd(2, 4);
  //         auto &A = rt[x];
  //         if ((int)A.size() - 2 < len) continue;
  //         int y = (int)rnd(1, (int)A.size() - 2 - len + 1);

  //         int uu = (rnd_db(0, 1) < 0.10 ? x : u);
  //         int v = pv(uu);
  //         if (v == -1) continue;
  //         if (uu == x && (v >= y && v <= y + len)) continue;

  //         Obj c = _or(x, y, len, uu, v);
  //         if (c < best.v) { best.tp = 1; best.p[0]=x; best.p[1]=y; best.p[2]=len; best.p[3]=uu; best.p[4]=v; best.v=c; }
  //         continue;
  //       }

  //       if (coin < 63) {
  //         auto &A = rt[x];
  //         int m = (int)A.size();
  //         if (m < 5) continue;
  //         int i = (int)rnd(1, m - 3);
  //         int j = (int)rnd(i + 1, m - 2);

  //         Obj c = _opt2(x, i, j);
  //         if (c < best.v) { best.tp = 2; best.p[0]=x; best.p[1]=i; best.p[2]=j; best.v=c; }
  //         continue;
  //       }

  //       if (coin < 75) {
  //         int y = py(x), v = py(u);
  //         if (y == -1 || v == -1) continue;

  //         Obj c = _swp2(x, y, u, v);
  //         if (c < best.v) { best.tp = 3; best.p[0]=x; best.p[1]=y; best.p[2]=u; best.p[3]=v; best.v=c; }
  //         continue;
  //       }

  //       if (coin < 86) {
  //         auto &A = rt[x];
  //         auto &B = rt[u];
  //         int ma = (int)A.size(), mb = (int)B.size();
  //         if (ma < 3 || mb < 3) continue;

  //         int ca = (int)rnd(0, ma - 2);
  //         int cb = (int)rnd(0, mb - 2);

  //         Obj c = _opt2s(x, ca, u, cb);
  //         if (c < best.v) { best.tp = 4; best.p[0]=x; best.p[1]=ca; best.p[2]=u; best.p[3]=cb; best.v=c; }
  //         continue;
  //       }

  //       if (coin < 94) {
  //         auto &A = rt[x];
  //         auto &B = rt[u];
  //         int ma = (int)A.size(), mb = (int)B.size();
  //         if (ma < 4 || mb < 4) continue;

  //         int la = min(4, ma - 2), lb = min(4, mb - 2);
  //         int lenA = (int)rnd(1, la);
  //         int lenB = (int)rnd(1, lb);

  //         int l1 = (int)rnd(1, (ma - 2) - lenA + 1);
  //         int r1 = l1 + lenA - 1;
  //         int l2 = (int)rnd(1, (mb - 2) - lenB + 1);
  //         int r2 = l2 + lenB - 1;

  //         Obj c = _xchg(x, l1, r1, u, l2, r2);
  //         if (c < best.v) { best.tp = 5; best.p[0]=x; best.p[1]=l1; best.p[2]=r1; best.p[3]=u; best.p[4]=l2; best.p[5]=r2; best.v=c; }
  //         continue;
  //       }

  //       {
  //         auto &A = rt[x];
  //         int m = (int)A.size();
  //         if (m < 6) continue;

  //         int i = (int)rnd(1, m - 4);
  //         int j = (int)rnd(i + 1, m - 3);
  //         int kk = (int)rnd(j + 1, m - 1);

  //         Obj c = _opt3(x, i, j, kk);
  //         if (c < best.v) { best.tp = 6; best.p[0]=x; best.p[1]=i; best.p[2]=j; best.p[3]=kk; best.v=c; }
  //       }
  //     }

  //     if (best.tp != -1 && best.v < O) {
  //       if (best.tp == 0) oneMove(best.p[0], best.p[1], best.p[2], best.p[3]);
  //       else if (best.tp == 1) orOpt(best.p[0], best.p[1], best.p[2], best.p[3], best.p[4]);
  //       else if (best.tp == 2) opt2(best.p[0], best.p[1], best.p[2]);
  //       else if (best.tp == 3) swp2(best.p[0], best.p[1], best.p[2], best.p[3]);
  //       else if (best.tp == 4) opt2s(best.p[0], best.p[1], best.p[2], best.p[3]);
  //       else if (best.tp == 5) xchg(best.p[0], best.p[1], best.p[2], best.p[3], best.p[4], best.p[5]);
  //       else if (best.tp == 6) opt3(best.p[0], best.p[1], best.p[2], best.p[3]);

  //       no = 0;
  //     } else {
  //       no++;
  //       if (no >= stall) {
  //         shk(10);
  //         no = 0;
  //       }
  //     }
  //   }
  // }

  // void ls(double tl, double sl/*, int tries = 96*/) {
  void ls(double tl, double sl, int tries) {
    using clk = chrono::steady_clock;
    auto t0 = clk::now();
    auto last = t0;

    auto el = [&]() { return chrono::duration<double>(clk::now() - t0).count(); };
    auto noimp = [&]() { return chrono::duration<double>(clk::now() - last).count(); };

    struct Mv { int tp, p[8]; Obj v; };

    while (el() < tl && noimp() < sl) {
      Mv best;
      best.tp = -1;
      best.v = O;

      // for (int t = 0; /*t < tries && */el() < tl; t++) {
      for (int t = 0; t < tries && el() < tl; t++) {
        int x = px(), u = pu(x);
        // int coin = (int)rnd(0, 99);
        // int coin = (int)rnd(0, 7);
        int coin = t % 8;

        // if (coin < 28) {
        if (coin < 1) {
          int y = py(x);
          if (y == -1) continue;
          int uu = (rnd_db(0, 1) < 0.80 ? u : x);
          int v = pv(uu);
          if (v == -1) continue;
          if (uu == x && v < y) swap(v, y); 
          if (uu == x && (v == y || v == y + 1)) continue;

          Obj c = _oneMove(x, y, uu, v);
          if (c < best.v) {
            best.tp = 0;
            best.p[0] = x;
            best.p[1] = y;
            best.p[2] = uu;
            best.p[3] = v;
            best.v = c;
          }
          continue;
        }

        // if (coin < 48) {
        if (coin < 2) {
          int len = (int)rnd(2, 4);
          auto &A = rt[x];
          if ((int)A.size() - 2 < len) continue;
          int y = (int)rnd(1, (int)A.size() - 2 - len + 1);

          int uu = (rnd_db(0, 1) < 0.20 ? x : u);
          int v = pv(uu);
          if (v == -1) continue;
          if (uu == x && v < y) swap(v, y);
          if (uu == x && (v >= y && v <= y + len)) continue;

          Obj c = _or(x, y, len, uu, v);
          if (c < best.v) {
            best.tp = 1;
            best.p[0] = x;
            best.p[1] = y;
            best.p[2] = len;
            best.p[3] = uu;
            best.p[4] = v;
            best.v = c;
          }
          continue;
        }

        // if (coin < 63) {
        if (coin < 3) {
          auto &A = rt[x];
          int m = (int)A.size();
          if (m < 5) continue;
          int i = (int)rnd(1, m - 3);
          int j = (int)rnd(i + 1, m - 2);

          Obj c = _opt2(x, i, j);
          if (c < best.v) {
            best.tp = 2;
            best.p[0] = x;
            best.p[1] = i;
            best.p[2] = j;
            best.v = c;
          }
          continue;
        }

        // if (coin < 75) {
        if (coin < 4) {
          int y = py(x), v = py(u);
          if (y == -1 || v == -1) continue;

          Obj c = _swp2(x, y, u, v);
          if (c < best.v) {
            best.tp = 3;
            best.p[0] = x;
            best.p[1] = y;
            best.p[2] = u;
            best.p[3] = v;
            best.v = c;
          }
          continue;
        }

        // if (coin < 86) {
        if (coin < 5) {
          auto &A = rt[x];
          auto &B = rt[u];
          int ma = (int)A.size(), mb = (int)B.size();
          if (ma < 3 || mb < 3) continue;

          int ca = (int)rnd(0, ma - 2);
          int cb = (int)rnd(0, mb - 2);

          Obj c = _opt2s(x, ca, u, cb);
          if (c < best.v) {
            best.tp = 4;
            best.p[0] = x;
            best.p[1] = ca;
            best.p[2] = u;
            best.p[3] = cb;
            best.v = c;
          }
          continue;
        }

        // if (coin < 94) {
        if (coin < 6) {
          auto &A = rt[x];
          auto &B = rt[u];
          int ma = (int)A.size(), mb = (int)B.size();
          if (ma < 4 || mb < 4) continue;

          int la = min(4, ma - 2), lb = min(4, mb - 2);
          int lenA = (int)rnd(1, la);
          int lenB = (int)rnd(1, lb);

          int l1 = (int)rnd(1, (ma - 2) - lenA + 1);
          int r1 = l1 + lenA - 1;
          int l2 = (int)rnd(1, (mb - 2) - lenB + 1);
          int r2 = l2 + lenB - 1;

          Obj c = _xchg(x, l1, r1, u, l2, r2);
          if (c < best.v) {
            best.tp = 5;
            best.p[0] = x;
            best.p[1] = l1;
            best.p[2] = r1;
            best.p[3] = u;
            best.p[4] = l2;
            best.p[5] = r2;
            best.v = c;
          }
          continue;
        }

        if (coin < 7) {
          auto &A = rt[x];
          int m = (int)A.size();
          if (m < 6) continue;

          int i = (int)rnd(1, m - 4);
          int j = (int)rnd(i + 1, m - 3);
          int kk = (int)rnd(j + 1, m - 1);

          Obj c = _opt3(x, i, j, kk);
          if (c < best.v) {
            best.tp = 6;
            best.p[0] = x;
            best.p[1] = i;
            best.p[2] = j;
            best.p[3] = kk;
            best.v = c;
          }
        }

        // swp3
        {
          auto &A = rt[x];
          auto &B = rt[u];
          int y = py(x);
          int v = py(u);
          if (y == -1 || v == -1) continue;
          if (A.size() < 5 || B.size() < 5) continue;
          Obj c = _swp3(x, y, u, v);
          if (c < best.v) {
            best.tp = 7;
            best.p[0] = x;
            best.p[1] = y;
            best.p[2] = u;
            best.p[3] = v;
            best.v = c;
          }
        }

        // {
        //   auto &A = rt[x];
        //   int m = A.size();
        //   if (m < 5) continue;
        //   Obj c = _flp(x);
        //   if (c < best.v) {
        //     best.tp = 7;
        //     best.p[0] = x;
        //     best.v = c;
        //   }
        // }

      }

      if (best.tp != -1 && best.v < O) {
        if (best.tp == 0) oneMove(best.p[0], best.p[1], best.p[2], best.p[3]);
        else if (best.tp == 1) orOpt(best.p[0], best.p[1], best.p[2], best.p[3], best.p[4]);
        else if (best.tp == 2) opt2(best.p[0], best.p[1], best.p[2]);
        else if (best.tp == 3) swp2(best.p[0], best.p[1], best.p[2], best.p[3]);
        else if (best.tp == 4) opt2s(best.p[0], best.p[1], best.p[2], best.p[3]);
        else if (best.tp == 5) xchg(best.p[0], best.p[1], best.p[2], best.p[3], best.p[4], best.p[5]);
        else if (best.tp == 6) opt3(best.p[0], best.p[1], best.p[2], best.p[3]);
        else if (best.tp == 7)
          // flp(best.p[0]);
          swp3(best.p[0], best.p[1], best.p[2], best.p[3]);

        last = clk::now(); 
      }
    }
  }

};

// bool check(int M, bool e = 0) {
//   auto tmp = greedy();
//   Solution sol(tmp, M);
// }

int main() {
  seed = chrono::steady_clock::now().time_since_epoch().count();
  // seed = 158635600997700ull;
  // seed = 159324076909900ull;
  // seed = 159313109600900ull;
  // seed = 159310867503100ull;
  // seed = 159316355843600ull;
  // seed = 159318634076300ull;
  // seed = 159321169871000ull;
  seed = 159836945789200ull;

  // cerr << seed << "\n";
  rng.seed(seed);

  // #define taskname "2"
  // for (string iext : {"in", "inp"}) {
  //   if (fopen((taskname"." + iext).c_str(), "r")) {
  //     freopen((taskname"." + iext).c_str(), "r", stdin);
  //     freopen(taskname".out", "w", stdout);
  //     break;
  //   }
  // }

  cin.tie(0)->sync_with_stdio(0);

  cin >> n >> k;
  n++;
  iota(id + 1, id + n, 1);
  int mn = inf;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      cin >> dist[i][j];
      if (i != j) minimize(mn, dist[i][j]);
    }
  }

  // auto best = greedy();
  // int lo = mn * ((n + k - 2) / k + 1), hi = best.obj;
  // while (lo <= hi) {
  //   int mid = lo + hi >> 1;
  //   if (check(mid)) 
  //     hi = lo - 1;
  //   else
  //     lo = hi - 1;
  // }

  // check(hi, 1);
  // db TL = 29;
  // db SL = 2;

  // using clk = chrono::steady_clock;
  // auto t0 = clk::now();
  // auto el = [&]() { return chrono::duration<db>(clk::now() - t0).count(); };

  // Sol s0 = greedy();
  // Solution best(s0, inf);
  // best.ls(min(SL, TL - el()));

  // while (el() < TL) {
  //   Sol s = greedy();
  //   Solution cur(s, inf);
  //   db rem = TL - el();
  //   if (rem <= 0) break;
  //   cur.ls(min(SL, rem));
  //   if (cur < best) best = cur;
  // }
  db TL = 29.5;
  db SL = 4;
  int TRIES = 96;

  using clk = chrono::steady_clock;
  auto t0 = clk::now();
  auto el = [&]() { return chrono::duration<db>(clk::now() - t0).count(); };

  Sol s0 = greedy();
  for (int it = 0; it < 50; it++) {
    Sol s = greedy();
    if (s.obj < s0.obj) 
      // s0 = s;
      s0 = move(s);
  }
  Solution best(s0, inf);
  best.ls(TL - el(), SL, TRIES);
  // best.ls(TL - el(), SL);

  while (el() < TL) {
    // rng.seed(seed = chrono::steady_clock::now().time_since_epoch().count());
    rng.seed(++seed);
    Sol s = greedy();
    for (int it = 0; it < 50; it++) {
      Sol t = greedy();
      if (t.obj < s.obj) 
        s = move(t);
    }
    double rem = TL - el();
    if (rem <= 0) break;

    Solution cur(s, inf);
    cur.ls(rem, min(SL, rem), TRIES);
    // cur.ls(rem, min(SL, rem));
    if (cur < best) best = cur;
  }

  // cerr << best.O.mx << " " << seed << "\n";

  cerr << best.O.mx << "\n" << el() * 1000. << "\n";
  cout << best;

  return 0;
}
