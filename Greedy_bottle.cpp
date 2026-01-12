#include <bits/stdc++.h>

using namespace std;

const double INF = 1e18;

struct Result {
    double max_obj;
    vector<vector<int>> routes;
};

double calculate_route_len(const vector<int>& route, const vector<vector<double>>& dist) {
    if (route.empty()) return 0;
    double len = dist[0][route[0]];
    for (size_t i = 0; i < route.size() - 1; ++i) {
        len += dist[route[i]][route[i+1]];
    }
    return len;
}

int main() {
	//freopen("input1.txt", "r", stdin);
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    auto start_time = chrono::steady_clock::now();
    

    int N, K;
    if (!(cin >> N >> K)) return 0;
    vector<vector<double>> dist(N + 1, vector<double>(N + 1));
    for (int i = 0; i <= N; ++i)
        for (int j = 0; j <= N; ++j) cin >> dist[i][j];

    vector<int> customer_ids(N);
    iota(customer_ids.begin(), customer_ids.end(), 1);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

    Result best_res;
    best_res.max_obj = INF;

    while (chrono::duration_cast<chrono::duration<double>>(chrono::steady_clock::now() - start_time).count() < 29.5) {
    	cout<<best_res.max_obj<<endl;
        shuffle(customer_ids.begin(), customer_ids.end(), rng);
        vector<bool> visited(N + 1, false);
        visited[0] = true;
        vector<vector<int>> curr_routes(K);
        vector<double> lens(K, 0.0);
        vector<int> pos(K, 0);

        for (int i = 0; i < N; ++i) {
            int v = 0;
            for (int k = 1; k < K; ++k) if (lens[k] < lens[v]) v = k;

            int best_c = -1; double min_d = INF;
            for (int c : customer_ids) {
                if (!visited[c] && dist[pos[v]][c] < min_d) {
                    min_d = dist[pos[v]][c];
                    best_c = c;
                }
            }
            curr_routes[v].push_back(best_c);
            lens[v] += min_d;
            pos[v] = best_c;
            visited[best_c] = true;
        }

        bool improved = true;
        while (improved) {
            improved = false;
            int k_max = 0;
            for (int k = 1; k < K; ++k) if (lens[k] > lens[k_max]) k_max = k;

            for (int i = 0; i < curr_routes[k_max].size() && !improved; ++i) {
                int node = curr_routes[k_max][i];
                for (int k_target = 0; k_target < K && !improved; ++k_target) {
                    if (k_max == k_target) continue;

                    for (int j = 0; j <= curr_routes[k_target].size(); ++j) {
                        vector<int> next_max = curr_routes[k_max];
                        next_max.erase(next_max.begin() + i);
                        vector<int> next_target = curr_routes[k_target];
                        next_target.insert(next_target.begin() + j, node);

                        double new_max_len = calculate_route_len(next_max, dist);
                        double new_target_len = calculate_route_len(next_target, dist);

                        if (max(new_max_len, new_target_len) < lens[k_max] - 1e-9) {
                            curr_routes[k_max] = next_max;
                            curr_routes[k_target] = next_target;
                            lens[k_max] = new_max_len;
                            lens[k_target] = new_target_len;
                            improved = true;
                            break;
                        }
                    }
                }
            }
        }

        double final_max = 0;
        for (double l : lens) final_max = max(final_max, l);
        if (final_max < best_res.max_obj) {
            best_res.max_obj = final_max;
            best_res.routes = curr_routes;
        }
    }

    cout << K << "\n";
    for (int k = 0; k < K; ++k) {
        cout << best_res.routes[k].size() + 1 << "\n0";
        for (int c : best_res.routes[k]) cout << " " << c;
        cout << "\n";
    }
    cerr << best_res.max_obj << "\n"
     << chrono::duration_cast<chrono::milliseconds>(
            chrono::steady_clock::now() - start_time
        ).count();
    return 0;
}