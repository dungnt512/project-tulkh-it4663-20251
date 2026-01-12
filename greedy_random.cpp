#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int n, k;
int d[1005][1005];

vector<vector<int>> greedy(int z, bool shuffle)
{
    set<int> unvisited;
    for (int i = 1; i <= n; i++)
    {
        unvisited.insert(i);
    }

    vector<vector<int>> routes;

    for (int i = 0; i < k; i++)
    {
        int cur = 0, length = 0;
        vector<int> route;
        route.push_back(0);

        while (!unvisited.empty())
        {
            vector<int> candidates;
            for (int v : unvisited)
            {
                if (length + d[cur][v] <= z)
                {
                    candidates.push_back(v);
                }
            }

            if (candidates.empty())
                break;

            if (shuffle)
            {
                random_shuffle(candidates.begin(), candidates.end());
            }

            int nxt = candidates[0];
            int minDist = d[cur][candidates[0]];
            for (int v : candidates)
            {
                if (d[cur][v] < minDist)
                {
                    minDist = d[cur][v];
                    nxt = v;
                }
            }

            route.push_back(nxt);
            length += d[cur][nxt];
            cur = nxt;
            unvisited.erase(nxt);
        }

        routes.push_back(route);
    }

    if (!unvisited.empty())
    {
        return vector<vector<int>>();
    }
    return routes;
}

pair<bool, vector<vector<int>>> can(int z, int attempts)
{
    vector<vector<int>> res = greedy(z, false);
    if (!res.empty())
    {
        return {true, res};
    }

    for (int i = 0; i < attempts; i++)
    {
        res = greedy(z, true);
        if (!res.empty())
        {
            return {true, res};
        }
    }

    return {false, vector<vector<int>>()};
}

void solve()
{
    cin >> n >> k;
    for (int i = 0; i <= n; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            cin >> d[i][j];
        }
    }

    int attempts = (n <= 500) ? 100 : 50;

    int lo = 0;
    int hi = 0;
    for (int i = 0; i <= n; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            hi = max(hi, d[i][j]);
        }
    }
    hi *= (n + 1);

    vector<vector<int>> bestRoutes;

    while (lo < hi)
    {
        int mid = (lo + hi) / 2;
        auto [ok, routes] = can(mid, attempts);
        if (ok)
        {
            hi = mid;
            bestRoutes = routes;
        }
        else
        {
            lo = mid + 1;
        }
    }

    if (bestRoutes.empty())
    {
        auto [ok, routes] = can(lo, attempts);
        bestRoutes = routes;
    }

    cout << k << endl;
    for (auto &route : bestRoutes)
    {
        cout << route.size() << endl;
        for (int i = 0; i < route.size(); i++)
        {
            if (i > 0)
                cout << " ";
            cout << route[i];
        }
        cout << endl;
    }
}

int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    srand(time(0));
    solve();
    return 0;
}
