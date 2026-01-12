#include <bits/stdc++.h>
#include <random>
#include <numeric>
#include <climits>
#include <ctime>
using namespace std;

const int MAX_N = (int)1e3 + 5;

int n, k, dist[MAX_N][MAX_N];

int group[MAX_N], nxt[MAX_N][MAX_N];

int q[MAX_N][MAX_N], R[MAX_N], sum[MAX_N];
bool picked[MAX_N];
int current_minimum = INT_MAX;
int mapping[MAX_N];

std::random_device rd;
std::mt19937 g(rd());

void reset_data()
{
    for (int i = 0; i <= n; ++i)
    {
        group[i] = 0;
        picked[i] = false;
        R[i] = 1;
        sum[i] = 0;
        for (int j = 0; j <= n; ++j)
        {
            nxt[i][j] = 0;
            q[i][j] = 0;
        }

        mapping[i] = i;
    }
}

int best_sum[MAX_N], best_R[MAX_N], best_q[MAX_N][MAX_N], best_nxt[MAX_N][MAX_N];
void update_best_shuffle()
{
    int mini = 0;
    for (int i = 1; i <= k; ++i)
        mini = max(mini, sum[i]);

    if (mini < current_minimum)
    {
        current_minimum = mini;

        for (int i = 1; i <= k; ++i)
        {
            best_sum[i] = sum[i];
            best_R[i] = R[i];
            // for (int j = 0; j <= n; ++j)
            //     best_nxt[j][i] = nxt[j][i];

            best_q[i][1] = 0;
            int start = nxt[0][i];
            for (int j = 2; j <= R[i]; ++j)
            {
                best_q[i][j] = mapping[start];
                start = nxt[start][i];
            }
        }
    }
}

void process_shuffle()
{
    reset_data();

    std::vector<int> p(n);
    std::iota(p.begin(), p.end(), 1);
    std::shuffle(p.begin(), p.end(), g);
    for (int i = 1; i <= n; ++i)
        mapping[i] = p[i - 1];
    mapping[0] = 0;

    for (int _ = 0; _ < n; ++_)
    {
        int sum_min = INT_MAX;
        int node, id_g, prv_node;

        for (int u = 1; u <= n; ++u)
            if (!picked[u])
            {
                for (int i = 1; i <= k; ++i)
                    for (int j = R[i]; j >= R[i]; --j)
                    {
                        int p_node = q[i][j];
                        int n_node = nxt[p_node][i];

                        int s = sum[i] - dist[mapping[p_node]][mapping[n_node]] + dist[mapping[p_node]][mapping[u]] + dist[mapping[u]][mapping[n_node]];

                        if (sum_min > s)
                        {
                            sum_min = s;
                            id_g = i;
                            node = u;
                            prv_node = p_node;
                        }
                    }
            }

        int nxt_node = nxt[prv_node][id_g];
        group[node] = id_g;
        sum[id_g] = sum[id_g] - dist[mapping[prv_node]][mapping[nxt_node]] + dist[mapping[prv_node]][mapping[node]] + dist[mapping[node]][mapping[nxt_node]];

        q[id_g][++R[id_g]] = node;
        picked[node] = true;

        nxt[node][id_g] = nxt_node;
        nxt[prv_node][id_g] = node;
    }

    update_best_shuffle();
}

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    // if (fopen(".inp", "r"))
    //     freopen(".inp", "r", stdin);

    clock_t start_time = clock();

    cin >> n >> k;
    for (int i = 0; i <= n; ++i)
        for (int j = 0; j <= n; ++j)
            cin >> dist[i][j];

    for (int i = 1; i <= n; ++i)
        dist[i][0] = 0;

    int n_case = 500;
    double time_limit = 29.8;

    while (((double)(clock() - start_time) / CLOCKS_PER_SEC) < time_limit)
    {
        process_shuffle();
    }

    cerr << current_minimum << endl;

    cout
        << k << "\n";
    for (int t = 1; t <= k; ++t)
    {
        cout << best_R[t] << ' ';
        for (int j = 1; j <= best_R[t]; ++j)
            cout << best_q[t][j] << ' ';
        cout << '\n';
    }

    cerr << (int)(double)(clock() - start_time) * 1000 / CLOCKS_PER_SEC << endl;
}
