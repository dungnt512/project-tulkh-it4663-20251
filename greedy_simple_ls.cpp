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
  multiset<int> st;
  Sol() {}
  Sol(int n, int k) {
    rt.resize(k);
    for (int i = 0; i < k; i++) rt[i] = new Route();
    node.resize(n);
    for (int i = 1; i < n; i++) node[i] = new Node(i);
  }

  void init_st() {
    sum = 0;
    for (int i = 0; i < k; i++) {
      st.insert(rt[i]->weight);
      sum += rt[i]->weight;
    }
    obj = st.empty() ? inf : *rbegin(st);
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
// Node *nodes[N];

// void prt() {
//   cout << k << "\n";
//   for (int i = 0; i < k; i++) cout << *rt[i] << "\n";
// }
static inline void detach(Node* x) {
  Node* a = x->L;
  Node* b = x->R;
  a->R = b;
  b->L = a;
}

static inline void insert_after(Node* pos, Node* x) {
  Node* q = pos->R;
  pos->R = x; x->L = pos;
  x->R = q;   q->L = x;
}

static inline int delta_remove(int a, int x, int b) {
  return dist[a][b] - dist[a][x] - dist[x][b];
}

static inline int delta_insert(int p, int x, int q) {
  return dist[p][x] + dist[x][q] - dist[p][q];
}

static inline void ms_erase_one(multiset<int>& st, int val) {
  auto it = st.find(val);
  if (it != st.end()) st.erase(it);
}

struct BestMove {
  int ridA = -1, ridB = -1;
  Node* x = nullptr;
  Node* pos = nullptr;     
  int newObj = inf;
  int newSum = inf;
  int newWA = 0, newWB = 0;
  int dA = 0, dB = 0;
  bool ok = false;
};

static inline int route_argmax(const Sol& s) {
  int rid = 0;
  for (int i = 1; i < (int)s.rt.size(); i++) {
    if (s.rt[i]->weight > s.rt[rid]->weight) rid = i;
  }
  return rid;
}

static inline pair<pair<int,int>, pair<int,int>> top2_excluding(const Sol& s, int ridA) {
  int w1 = -1, id1 = -1, w2 = -1, id2 = -1;
  for (int i = 0; i < (int)s.rt.size(); i++) if (i != ridA) {
    int w = s.rt[i]->weight;
    if (w > w1) { w2 = w1; id2 = id1; w1 = w; id1 = i; }
    else if (w > w2) { w2 = w; id2 = i; }
  }
  return {{w1,id1},{w2,id2}};
}

bool best_relocate_from_max(Sol& s) {
  int ridA = route_argmax(s);
  Route* A = s.rt[ridA];

  auto [mx1, mx2] = top2_excluding(s, ridA); 
  int w1 = mx1.first, id1 = mx1.second;
  int w2 = mx2.first;

  BestMove best;
  best.ridA = ridA;
  best.newObj = s.obj;
  best.newSum = s.sum;

  for (Node* x = A->st->R; x != A->en; x = x->R) {
    Node* a = x->L;
    Node* b = x->R;

    int dA = delta_remove(a->i, x->i, b->i);
    int newWA = A->weight + dA;

    for (int ridB = 0; ridB < (int)s.rt.size(); ridB++) if (ridB != ridA) {
      Route* B = s.rt[ridB];

      int baseOther = (ridB == id1 ? w2 : w1);
      if (baseOther < 0) baseOther = 0;

      for (Node* pos = B->st; pos != B->en; pos = pos->R) {
        Node* q = pos->R;
        int dB = delta_insert(pos->i, x->i, q->i);
        int newWB = B->weight + dB;

        int newObj = max({baseOther, newWA, newWB});
        int newSum = s.sum + dA + dB;

        if (newObj < best.newObj || (newObj == best.newObj && newSum < best.newSum)) {
          best.ok = true;
          best.ridB = ridB;
          best.x = x;
          best.pos = pos;
          best.newObj = newObj;
          best.newSum = newSum;
          best.newWA = newWA;
          best.newWB = newWB;
          best.dA = dA;
          best.dB = dB;
        }
      }
    }
  }

  if (!best.ok) return false;

  int ridB = best.ridB;
  Route* B = s.rt[ridB];

  ms_erase_one(s.st, A->weight);
  ms_erase_one(s.st, B->weight);

  detach(best.x);
  insert_after(best.pos, best.x);

  A->weight = best.newWA; A->cnt--;
  B->weight = best.newWB; B->cnt++;

  s.st.insert(A->weight);
  s.st.insert(B->weight);

  s.sum = best.newSum;
  s.obj = *rbegin(s.st);
  return true;
}

void local_search(Sol& s, int itLim = 2000) {
  for (int it = 0; it < itLim; it++) {
    if (!best_relocate_from_max(s)) break;
  }
}


Sol greedy() {
  Sol sol(n, k);
  shuffle(id + 1, id + n, rng);
  for (int i = 0; i < k; i++) sol.rt[i]->en->i = n;
  for (int it = 1; it < n; it++) {
    int i = id[it];
    int mn = inf, l = -1;
    Node *best;
    for (int j = 0; j < k; j++) {
      auto &r = sol.rt[j];
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
      auto &n = sol.node[i];
      n->R = best->R;
      n->L = best;
      best->R->L = n;
      best->R = n;
      auto &r = sol.rt[l];
      r->weight = mn;
      r->cnt++;
    }
    // cerr << nodes[i]->i << " " << l << "\n";
  }
  // exit(0);
  // for (int i = 0; i < k; i++) st.insert(rt[i]->weight);
  sol.init_st();
  return move(sol);
}

int main() {
  #define taskname ""
  for (string iext : {"in", "inp"}) {
    if (fopen((taskname"." + iext).c_str(), "r")) {
      freopen((taskname"." + iext).c_str(), "r", stdin);
      freopen(taskname".out", "w", stdout);
      break;
    }
  }

  cin.tie(0)->sync_with_stdio(0);

  cin >> n >> k;
  n++;
  iota(id + 1, id + n, 1);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      cin >> dist[i][j];
    }
  }

  // sol = Sol(k);
  auto best = greedy();
  for (int it = 0; it < 100; it++) {
    // cerr << it << " ";
    auto sol = greedy();
    local_search(sol);
    minimize(best, sol);
  }
  cout << best << "\n";
  // prt();
  
  return 0;
}