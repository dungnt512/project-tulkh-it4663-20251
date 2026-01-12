#include <bits/stdc++.h>
using namespace std;

const int MAX_N = (int)1e3 + 5;

int n, k, dist[MAX_N][MAX_N];

int group[MAX_N], nxt[MAX_N][MAX_N];

int q[MAX_N][MAX_N], R[MAX_N], sum[MAX_N];
bool picked[MAX_N];

void process()
{
    for (int i = 1; i <= n; ++i)
        q[i][1] = sum[i] = 0, R[i] = 1;

    fill(picked + 1, picked + n + 1, false);

    for (int _ = 0; _ < n; ++_)
    {
        int sum_min = INT_MAX;
        int node, id_g, prv_node;

        for (int u = 1; u <= n; ++u)
            if (!picked[u])
            {
                for (int i = 1; i <= k; ++i)
                    for (int j = R[i]; j > 0; --j)
                    {
                        int p_node = q[i][j];
                        int n_node = nxt[p_node][i];
                        int s = sum[i] - dist[p_node][n_node] + dist[p_node][u] + dist[u][n_node];

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
        sum[id_g] = sum[id_g] - dist[prv_node][nxt_node] + dist[prv_node][node] + dist[node][nxt_node];
        q[id_g][++R[id_g]] = node;
        picked[node] = true;

        nxt[node][id_g] = nxt_node;
        nxt[prv_node][id_g] = node;
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
        int cnt = 1;
        int start = nxt[0][t];

        for (int i = 1; i <= n; ++i)
            if (group[i] == t)
            {
                ++cnt;
            }
        cout << cnt << '\n'
             << 0 << ' ';

        while (start != 0)
        {
            cout << start << ' ';
            start = nxt[start][t];
        }
        cout << '\n';
    }
}