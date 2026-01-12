#include <bits/stdc++.h>
#include <random>
#include <numeric>
#include <climits>
#include <ctime>

using namespace std;

const int MAX_N = 1005;
const int M = 10; // Kích thước cửa sổ tối ưu

int n, k, dist[MAX_N][MAX_N];
int nxt[MAX_N][MAX_N], R[MAX_N], sum[MAX_N], q_pos[MAX_N][MAX_N];
bool picked[MAX_N];
int mapping[MAX_N], current_minimum = INT_MAX;
int best_R[MAX_N], best_q[MAX_N][MAX_N];

// DP: dp[mask][i] là chi phí nhỏ nhất đi qua mask và kết thúc tại i
int dp[1 << M][M], parent_dp[1 << M][M];

std::random_device rd;
std::mt19937 g(rd());

void reset_data()
{
    for (int i = 0; i <= n; ++i)
    {
        picked[i] = false;
        R[i] = 1;
        sum[i] = 0;
        for (int j = 0; j <= n; ++j)
            nxt[i][j] = -1;
        q_pos[i][1] = 0; // Điểm bắt đầu luôn là 0
    }
}

// DP tìm thứ tự tối ưu cho một đoạn, cố định điểm đầu segment[0]
vector<int> solve_segment_dp(const vector<int> &segment, bool is_tail)
{
    int m = segment.size();
    int limit = 1 << m;
    for (int mask = 0; mask < limit; ++mask)
        for (int i = 0; i < m; ++i)
            dp[mask][i] = 1e9;

    dp[1][0] = 0; // Cố định điểm bắt đầu của cửa sổ
    for (int mask = 1; mask < limit; ++mask)
    {
        for (int i = 0; i < m; ++i)
        {
            if (dp[mask][i] >= 1e9)
                continue;
            for (int nxt_v = 0; nxt_v < m; ++nxt_v)
            {
                if (!(mask & (1 << nxt_v)))
                {
                    if (!is_tail && nxt_v == m - 1 && (mask | (1 << nxt_v)) != limit - 1)
                        continue;

                    int nm = mask | (1 << nxt_v);
                    int cost = dist[mapping[segment[i]]][mapping[segment[nxt_v]]];
                    if (dp[nm][nxt_v] > dp[mask][i] + cost)
                    {
                        dp[nm][nxt_v] = dp[mask][i] + cost;
                        parent_dp[nm][nxt_v] = i;
                    }
                }
            }
        }
    }

    int last_node = m - 1;
    if (is_tail)
    {
        int min_total = 1e9;
        for (int i = 1; i < m; ++i)
        {
            if (dp[limit - 1][i] < min_total)
            {
                min_total = dp[limit - 1][i];
                last_node = i;
            }
        }
    }

    if (dp[limit - 1][last_node] >= 1e9)
        return segment;

    vector<int> res;
    int cur_mask = limit - 1, cur_node = last_node;
    while (cur_mask > 0)
    {
        res.push_back(segment[cur_node]);
        int p = parent_dp[cur_mask][cur_node];
        cur_mask ^= (1 << cur_node);
        cur_node = p;
    }
    reverse(res.begin(), res.end());
    return res;
}

void update_best()
{
    int max_sum = 0;
    for (int i = 1; i <= k; ++i)
        max_sum = max(max_sum, sum[i]);

    if (max_sum < current_minimum)
    {
        current_minimum = max_sum;
        for (int i = 1; i <= k; ++i)
        {
            best_R[i] = R[i];
            int curr = 0;
            for (int j = 1; j <= R[i]; ++j)
            {
                best_q[i][j] = mapping[curr];
                curr = nxt[curr][i];
            }
        }
    }
}

void process_shuffle()
{
    reset_data();
    vector<int> p(n);
    iota(p.begin(), p.end(), 1);
    shuffle(p.begin(), p.end(), g);
    for (int i = 1; i <= n; ++i)
        mapping[i] = p[i - 1];
    mapping[0] = 0;

    for (int _ = 0; _ < n; ++_)
    {
        int best_delta = INT_MAX, node = -1, id_g = -1, prv_node = -1;
        for (int u = 1; u <= n; ++u)
        {
            if (picked[u])
                continue;
            for (int i = 1; i <= k; ++i)
            {
                for (int j = 1; j <= R[i]; ++j)
                {
                    int cp = q_pos[i][j], cn = nxt[cp][i];
                    int delta = (cn == -1) ? dist[mapping[cp]][mapping[u]] : dist[mapping[cp]][mapping[u]] + dist[mapping[u]][mapping[cn]] - dist[mapping[cp]][mapping[cn]];
                    if (delta < best_delta)
                    {
                        best_delta = delta;
                        id_g = i;
                        node = u;
                        prv_node = cp;
                    }
                }
            }
        }
        int old_nxt = nxt[prv_node][id_g];
        nxt[prv_node][id_g] = node;
        nxt[node][id_g] = old_nxt;
        sum[id_g] += best_delta;
        R[id_g]++;
        picked[node] = true;

        int temp = 0;
        for (int j = 1; j <= R[id_g]; j++)
        {
            q_pos[id_g][j] = temp;
            temp = nxt[temp][id_g];
        }
    }

    for (int i = 1; i <= k; ++i)
    {
        if (R[i] < 4)
            continue;

        vector<int> route_vec;
        int curr = 0;
        while (curr != -1)
        {
            route_vec.push_back(curr);
            curr = nxt[curr][i];
        }

        for (int iter = 0; iter < 2; iter++)
        {
            for (int s_pos = 0; s_pos + M <= (int)route_vec.size(); s_pos += (M / 2))
            {
                vector<int> segment;
                for (int j = 0; j < M; j++)
                    segment.push_back(route_vec[s_pos + j]);

                bool is_tail = (s_pos + M == (int)route_vec.size());
                vector<int> optimized = solve_segment_dp(segment, is_tail);

                for (int j = 0; j < M; j++)
                    route_vec[s_pos + j] = optimized[j];
            }
        }

        sum[i] = 0;
        for (int j = 0; j < (int)route_vec.size() - 1; j++)
        {
            nxt[route_vec[j]][i] = route_vec[j + 1];
            sum[i] += dist[mapping[route_vec[j]]][mapping[route_vec[j + 1]]];
        }
        nxt[route_vec.back()][i] = -1;
    }

    update_best();
}

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    if (fopen(".inp", "r"))
        freopen(".inp", "r", stdin);

    cin >> n >> k;
    for (int i = 0; i <= n; ++i)
        for (int j = 0; j <= n; ++j)
            cin >> dist[i][j];

    for (int i = 1; i <= n; ++i)
        dist[i][0] = 0;

    int n_case = 500;
    clock_t start_time = clock();
    double time_limit = 29;

    while (((double)(clock() - start_time) / CLOCKS_PER_SEC) < time_limit)
    {
        process_shuffle();
    }

    cout << k << "\n";
    for (int t = 1; t <= k; ++t)
    {
        cout << best_R[t] << ' ';
        for (int j = 1; j <= best_R[t]; ++j)
            cout << best_q[t][j] << ' ';
        cout << '\n';
    }
}