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

const int N = 1e3 + 1;
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
  vector<Route*> rt;
  Sol() {
    rt.resize(k);
  }
};

Node *nodes[N];
Route *rt[K];
// multiset<int> st;

void prt() {
  cout << k << "\n";
  for (int i = 0; i < k; i++) cout << *rt[i] << "\n";
}


void greedy() {
  shuffle(id + 1, id + n, rng);
  for (int i = 0; i < k; i++) rt[i]->en->i = n;
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
  // for (int i = 0; i < k; i++) st.insert(rt[i]->weight);
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
  n++;
  iota(id + 1, id + n, 1);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      cin >> dist[i][j];
    }
  }

  for (int i = 1; i < n; i++) { nodes[i] = new Node(i); }
  for (int i = 0; i < k; i++) rt[i] = new Route();

  greedy();
  prt();
  
  return 0;
}
