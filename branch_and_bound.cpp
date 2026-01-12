#include <bits/stdc++.h>
using namespace std;

int n, k;
vector<int> y, routeCost;
vector<vector<int>> cost;
vector<bool> visited;
int visitedCount;
int bestCost;
int curCost;
int cmin;
vector<vector<int>> routes;
vector<vector<int>> bestRoutes;
constexpr double TIME_LIMIT_SECONDS = 29.5;
chrono::steady_clock::time_point startTime;

void extend_route(int k, int cur);

inline bool timeExceeded()
{
    return chrono::duration<double>(chrono::steady_clock::now() - startTime).count() >= TIME_LIMIT_SECONDS;
}

void printRoutes()
{
    cout << k << endl;
    for (int i = 1; i <= k; i++)
    {
        cout << bestRoutes[i].size() + 1 << endl;
        cout << "0 ";
        for (auto c : bestRoutes[i])
        {
            cout << c << ' ';
        }
        cout << endl;
    }
}

void search_vehicle(int l)
{
    if (timeExceeded())
        return;
    int start = 0;
    if (l - 1 >= 1 && y[l - 1] > 0)
        start = y[l - 1] + 1;
    for (int v = start; v <= n; ++v)
    {
        if (v > 0)
        {
            if (visited[v])
                continue;
        }
        y[l] = v;
        if (v > 0)
        {
            visited[v] = true;
            visitedCount++;
            routeCost[l] += cost[0][v];
            int prev = curCost;
            curCost = max(curCost, routeCost[l]);
            routes[l].push_back(v);
            if (curCost + cmin * ((n - visitedCount) / k) < bestCost)
                extend_route(l, v);
            routes[l].pop_back();
            routeCost[l] -= cost[0][v];
            curCost = prev;
            visited[v] = false;
            visitedCount--;
        }
        else
        {
            if (l == k)
            {
                if (visitedCount == n && curCost < bestCost)
                {
                    bestCost = curCost;
                    bestRoutes = routes;
                }
            }
            else
            {
                search_vehicle(l + 1);
            }
        }
        y[l] = 0;
    }
}

void extend_route(int l, int cur)
{
    if (timeExceeded())
        return;
    for (int u = 1; u <= n; ++u)
    {
        if (visited[u])
            continue;
        visited[u] = true;
        visitedCount++;
        routeCost[l] += cost[cur][u];
        int prev = curCost;
        curCost = max(curCost, routeCost[l]);
        routes[l].push_back(u);
        if (curCost < bestCost)
            extend_route(l, u);
        routes[l].pop_back();
        routeCost[l] -= cost[cur][u];
        curCost = prev;
        visited[u] = false;
        visitedCount--;
    }
    if (l == k)
    {
        if (visitedCount == n && curCost < bestCost)
        {
            bestCost = curCost;
            bestRoutes = routes;
        }
    }
    else
    {
        search_vehicle(l + 1);
    }
}

void solve()
{
    cin >> n >> k;
    cost.assign(n + 1, vector<int>(n + 1));
    for (int i = 0; i <= n; i++)
        for (int j = 0; j <= n; j++)
            cin >> cost[i][j];
    cmin = INT_MAX;
    for (int i = 0; i <= n; i++)
        for (int j = 0; j <= n; j++)
            if (i != j)
                cmin = min(cmin, cost[i][j]);
    y.assign(k + 1, 0);
    visited.assign(n + 1, false);
    routes.assign(k + 1, vector<int>());
    routeCost.assign(k + 1, 0);
    visitedCount = 0;
    curCost = 0;
    bestCost = INT_MAX;
    startTime = chrono::steady_clock::now();
    search_vehicle(1);
    printRoutes();
    cerr << bestCost;
}

int main()
{
    solve();
    return 0;
}
