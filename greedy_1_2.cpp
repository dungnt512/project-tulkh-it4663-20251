#include <bits/stdc++.h>
using namespace std;

const int MAX_N = (int)1e3 + 5;

int n, k, dist[MAX_N][MAX_N];

int group[MAX_N], nxt[MAX_N][MAX_N];

int q[MAX_N][MAX_N], R[MAX_N], sum[MAX_N];
bool picked[MAX_N];

void process()
{
    for (int i = 1; i <= k; ++i)
    {
        R[i] = 1;
        q[i][1] = 0;
        sum[i] = 0;
    }

    fill(picked + 1, picked + n + 1, false);

    int current_max = 0;

    for (int iter = 0; iter < n; ++iter)
    {
        int best_node = -1;
        int best_route = -1;
        int best_regret = -1;
        int best_best_impact = INT_MAX;

        for (int u = 1; u <= n; ++u)
            if (!picked[u])
            {
                int min1 = INT_MAX, min2 = INT_MAX;
                int min1_route = -1;

                for (int i = 1; i <= k; ++i)
                {
                    int last = q[i][R[i]];
                    int impact = max(current_max, sum[i] + dist[last][u]);

                    if (impact < min1)
                    {
                        min2 = min1;
                        min1 = impact;
                        min1_route = i;
                    }
                    else if (impact < min2)
                    {
                        min2 = impact;
                    }
                }

                int regret = min2 - min1;

                if (regret > best_regret ||
                    (regret == best_regret && min1 < best_best_impact))
                {
                    best_regret = regret;
                    best_best_impact = min1;
                    best_node = u;
                    best_route = min1_route;
                }
            }

        int u = best_node;
        int i = best_route;

        int last = q[i][R[i]];
        sum[i] += dist[last][u];
        q[i][++R[i]] = u;
        group[u] = i;
        picked[u] = true;

        current_max = max(current_max, sum[i]);
    }
}

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    if (fopen(".inp", "r"))
        freopen(".inp", "r", stdin);

    cin >> n >> k;
    cout << k << endl;
    for (int i = 0; i <= n; ++i)
        for (int j = 0; j <= n; ++j)
            cin >> dist[i][j];

    for (int i = 1; i <= n; ++i)
        dist[i][0] = 0;

    process();

    // return 0;
    for (int t = 1; t <= k; ++t)
    {
        cout << R[t] << " ";
        for (int i = 1; i <= R[t]; ++i)
            cout << q[t][i] << " ";
        cout << endl;
    }
}