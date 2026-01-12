#include <bits/stdc++.h>
using namespace std;

const double INF = 1e18;

/* ================= STRUCT ================= */
struct Individual {
    vector<vector<int>> routes;
    double max_len;

    bool operator<(const Individual& other) const {
        return max_len < other.max_len;
    }
};

int N, K;
vector<vector<double>> distMat;

/* ================= DIST ================= */
inline double get_dist(int i, int j) {
    return distMat[i][j];
}

/* ========== ROUTE LENGTH (OPEN ROUTE) ========== */
double calc_route_len(const vector<int>& r) {
    if (r.empty()) return 0;
    double d = get_dist(0, r[0]);
    for (int i = 0; i + 1 < (int)r.size(); i++)
        d += get_dist(r[i], r[i + 1]);
    return d;
}

/* ================= 2-OPT OPEN ROUTE ================= */
void twoOpt(vector<int>& route) {
    int n = route.size();
    if (n < 2) return;

    bool improved = true;
    while (improved) {
        improved = false;
        for (int i = 0; i < n - 1; i++) {
            for (int j = i + 1; j < n; j++) {
                int a = (i == 0 ? 0 : route[i - 1]);
                int b = route[i];
                int c = route[j];
                int d = (j + 1 < n ? route[j + 1] : -1);

                double cur = get_dist(a, b);
                if (d != -1) cur += get_dist(c, d);

                double nxt = get_dist(a, c);
                if (d != -1) nxt += get_dist(b, d);

                if (nxt + 1e-9 < cur) {
                    reverse(route.begin() + i, route.begin() + j + 1);
                    improved = true;
                }
            }
        }
    }
}

/* ================= CREATE INDIVIDUAL ================= */
Individual create_individual(bool pure_greedy) {
    Individual ind;
    ind.routes.assign(K, {});

    vector<int> customers;
    for (int i = 1; i <= N; i++) customers.push_back(i);
    if (!pure_greedy) random_shuffle(customers.begin(), customers.end());

    for (int c : customers) {
        int bestV = -1;
        double bestInc = INF;
        for (int v = 0; v < K; v++) {
            int last = ind.routes[v].empty() ? 0 : ind.routes[v].back();
            double inc = get_dist(last, c);
            if (inc < bestInc) {
                bestInc = inc;
                bestV = v;
            }
        }
        ind.routes[bestV].push_back(c);
    }

    ind.max_len = 0;
    for (auto& r : ind.routes) {
        twoOpt(r);
        ind.max_len = max(ind.max_len, calc_route_len(r));
    }
    return ind;
}

/* ================= RANDOM MUTATION ================= */
void mutate(Individual& ind) {
    int v1 = rand() % K;
    int v2 = rand() % K;
    if (ind.routes[v1].empty()) return;

    int idx = rand() % ind.routes[v1].size();
    int cust = ind.routes[v1][idx];

    ind.routes[v1].erase(ind.routes[v1].begin() + idx);
    ind.routes[v2].push_back(cust);

    twoOpt(ind.routes[v1]);
    twoOpt(ind.routes[v2]);

    ind.max_len = 0;
    for (auto& r : ind.routes)
        ind.max_len = max(ind.max_len, calc_route_len(r));
}

/* ================= CRITICAL MUTATION ================= */
int get_worst_route(const Individual& ind) {
    int idx = 0;
    double mx = -1;
    for (int i = 0; i < K; i++) {
        double len = calc_route_len(ind.routes[i]);
        if (len > mx) {
            mx = len;
            idx = i;
        }
    }
    return idx;
}

int get_best_route(const Individual& ind) {
    int idx = 0;
    double mn = INF;
    for (int i = 0; i < K; i++) {
        double len = calc_route_len(ind.routes[i]);
        if (len < mn) {
            mn = len;
            idx = i;
        }
    }
    return idx;
}

void critical_mutate(Individual& ind) {
    int worst = get_worst_route(ind);
    if (ind.routes[worst].empty()) return;

    int best = get_best_route(ind);

    double bestGain = 0;
    int bestCust = -1, bestPos = -1;

    for (int i = 0; i < (int)ind.routes[worst].size(); i++) {
        int c = ind.routes[worst][i];
        ind.routes[worst].erase(ind.routes[worst].begin() + i);

        for (int pos = 0; pos <= (int)ind.routes[best].size(); pos++) {
            ind.routes[best].insert(ind.routes[best].begin() + pos, c);

            double newMax = 0;
            for (auto& r : ind.routes)
                newMax = max(newMax, calc_route_len(r));

            double gain = ind.max_len - newMax;
            if (gain > bestGain) {
                bestGain = gain;
                bestCust = c;
                bestPos = pos;
            }
            ind.routes[best].erase(ind.routes[best].begin() + pos);
        }
        ind.routes[worst].insert(ind.routes[worst].begin() + i, c);
    }

    if (bestCust != -1) {
        auto& rw = ind.routes[worst];
        rw.erase(find(rw.begin(), rw.end(), bestCust));
        ind.routes[best].insert(ind.routes[best].begin() + bestPos, bestCust);
    }

    twoOpt(ind.routes[worst]);
    twoOpt(ind.routes[best]);

    ind.max_len = 0;
    for (auto& r : ind.routes)
        ind.max_len = max(ind.max_len, calc_route_len(r));
}

/* ================= CROSSOVER ================= */
Individual crossover(const Individual& p1, const Individual& p2) {
    Individual child;
    child.routes.assign(K, {});
    vector<bool> used(N + 1, false);

    vector<pair<double, vector<int>>> pool;
    for (int i = 0; i < K; i++) {
        if (!p1.routes[i].empty())
            pool.push_back({calc_route_len(p1.routes[i]), p1.routes[i]});
        if (!p2.routes[i].empty())
            pool.push_back({calc_route_len(p2.routes[i]), p2.routes[i]});
    }

    sort(pool.begin(), pool.end());

    int cur = 0;
    for (auto& pr : pool) {
        if (cur >= K) break;
        bool ok = true;
        for (int c : pr.second)
            if (used[c]) ok = false;
        if (!ok) continue;

        child.routes[cur] = pr.second;
        for (int c : pr.second) used[c] = true;
        cur++;
    }

    vector<int> rem;
    for (int i = 1; i <= N; i++)
        if (!used[i]) rem.push_back(i);

    for (int c : rem) {
        int bestV = 0;
        double best = INF;
        for (int v = 0; v < K; v++) {
            int last = child.routes[v].empty() ? 0 : child.routes[v].back();
            double inc = get_dist(last, c);
            if (inc < best) {
                best = inc;
                bestV = v;
            }
        }
        child.routes[bestV].push_back(c);
    }

    child.max_len = 0;
    for (auto& r : child.routes) {
        twoOpt(r);
        child.max_len = max(child.max_len, calc_route_len(r));
    }
    return child;
}

void critical_mutate_fast(Individual& ind) {
    int worst = get_worst_route(ind);
    int best  = get_best_route(ind);
    if (worst == best || ind.routes[worst].empty()) return;

    auto& rw = ind.routes[worst];
    auto& rb = ind.routes[best];

    double len_w = calc_route_len(rw);
    double len_b = calc_route_len(rb);

    int trials = min(5, (int)rw.size()); // chỉ xét 5 khách đầu
    double bestGain = 0;
    int bestIdx = -1;

    for (int t = 0; t < trials; t++) {
        int i = rand() % rw.size();
        int c = rw[i];

        int prev = (i == 0 ? 0 : rw[i - 1]);
        int next = (i + 1 < rw.size() ? rw[i + 1] : -1);

        double remove_cost = get_dist(prev, c);
        if (next != -1) remove_cost += get_dist(c, next);

        double add_cost = get_dist(prev, next == -1 ? 0 : next);

        double new_len_w = len_w - remove_cost + add_cost;

        int last = rb.empty() ? 0 : rb.back();
        double new_len_b = len_b + get_dist(last, c);

        double newMax = max(new_len_w, new_len_b);
        double oldMax = max(len_w, len_b);

        double gain = oldMax - newMax;

        if (gain > bestGain) {
            bestGain = gain;
            bestIdx = i;
        }
    }

    if (bestIdx == -1) return;

    int c = rw[bestIdx];
    rw.erase(rw.begin() + bestIdx);
    rb.push_back(c);

    twoOpt(rw);
    twoOpt(rb);

    ind.max_len = 0;
    for (auto& r : ind.routes)
        ind.max_len = max(ind.max_len, calc_route_len(r));
}


/* ================= MAIN ================= */
int main() {
	//freopen("input1.txt", "r", stdin);
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    srand(time(NULL));

    cin >> N >> K;
    distMat.assign(N + 1, vector<double>(N + 1));
    for (int i = 0; i <= N; i++)
        for (int j = 0; j <= N; j++)
            cin >> distMat[i][j];

    int popSize = 30;
    vector<Individual> population;

    for (int i = 0; i < popSize; i++)
        population.push_back(create_individual(i < popSize * 0.2));

    auto start = chrono::steady_clock::now();
    int gen = 0;
	
	cout<<"start"<<endl;
    while (chrono::duration_cast<chrono::milliseconds>(
               chrono::steady_clock::now() - start).count() < 29000) {

        sort(population.begin(), population.end());
        if (gen % 100 == 0)
            cout << "[Gen] " << gen << " Best = " 
                 << fixed << setprecision(6) 
                 << population[0].max_len << "\n";

        vector<Individual> nextGen;
        for (int i = 0; i < 5; i++)
            nextGen.push_back(population[i]);

        while ((int)nextGen.size() < popSize) {
            int a = rand() % 10;
            int b = rand() % 10;
            Individual child;

            if (rand() % 100 < 70)
                child = crossover(population[a], population[b]);
            else {
                child = population[rand() % 5];
                mutate(child);
            }

            if (rand() % 100 < 50)
                critical_mutate_fast(child);
            else if (rand() % 100 < 10)
				critical_mutate(child);
            else 
				mutate(child);

            nextGen.push_back(child);
        }

        /* ==== RANDOM IMMIGRANTS ==== */
        if (gen % 200 == 0) {
            int rep = popSize * 0.2;
            for (int i = popSize - rep; i < popSize; i++)
                nextGen[i] = create_individual(false);
        }

        population = nextGen;
        gen++;
    }

    sort(population.begin(), population.end());
    Individual best = population[0];

    /* ========= OUTPUT ========= */
    cout << K << "\n";
    for (int i = 0; i < K; i++) {
        cout << best.routes[i].size() + 1 << "\n";
        cout << "0 ";
        for (int c : best.routes[i]) cout << c << " ";
        cout << "\n";
    }
    
    cerr << population[0].max_len <<"\n"<<chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count();
    return 0;
}
