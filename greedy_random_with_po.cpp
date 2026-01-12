#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int n, k;
int d[1005][1005];

int calculateRouteLength(const vector<int> &route)
{
    int length = 0;
    for (int i = 0; i < (int)route.size() - 1; i++)
    {
        length += d[route[i]][route[i + 1]];
    }
    return length;
}

int getMaxRouteLength(const vector<vector<int>> &routes)
{
    int maxLen = 0;
    for (const auto &route : routes)
    {
        maxLen = max(maxLen, calculateRouteLength(route));
    }
    return maxLen;
}

bool tryRelocate(vector<vector<int>> &routes, int z)
{
    bool improved = false;
    for (int i = 0; i < (int)routes.size(); i++)
    {
        for (int j = 1; j < (int)routes[i].size(); j++)
        {
            int customer = routes[i][j];
            int oldLength = calculateRouteLength(routes[i]);
            routes[i].erase(routes[i].begin() + j);
            int newLengthI = calculateRouteLength(routes[i]);

            for (int k = 0; k < (int)routes.size(); k++)
            {
                if (k == i)
                    continue;

                int oldLengthK = calculateRouteLength(routes[k]);

                for (int pos = 1; pos <= (int)routes[k].size(); pos++)
                {
                    routes[k].insert(routes[k].begin() + pos, customer);
                    int newLengthK = calculateRouteLength(routes[k]);

                    if (newLengthK <= z && max(newLengthI, newLengthK) < max(oldLength, oldLengthK))
                    {
                        improved = true;
                        goto next_customer;
                    }

                    routes[k].erase(routes[k].begin() + pos);
                }
            }
            routes[i].insert(routes[i].begin() + j, customer);
        next_customer:;
        }
    }
    return improved;
}

bool trySwap(vector<vector<int>> &routes, int z)
{
    bool improved = false;
    for (int i = 0; i < (int)routes.size(); i++)
    {
        for (int j = 1; j < (int)routes[i].size(); j++)
        {
            for (int k = i + 1; k < (int)routes.size(); k++)
            {
                for (int l = 1; l < (int)routes[k].size(); l++)
                {
                    int oldLenI = calculateRouteLength(routes[i]);
                    int oldLenK = calculateRouteLength(routes[k]);

                    swap(routes[i][j], routes[k][l]);

                    int newLenI = calculateRouteLength(routes[i]);
                    int newLenK = calculateRouteLength(routes[k]);

                    if (newLenI <= z && newLenK <= z &&
                        max(newLenI, newLenK) < max(oldLenI, oldLenK))
                    {
                        improved = true;
                    }
                    else
                    {
                        swap(routes[i][j], routes[k][l]);
                    }
                }
            }
        }
    }
    return improved;
}

bool try2Opt(vector<vector<int>> &routes, int z)
{
    bool improved = false;
    for (auto &route : routes)
    {
        if (route.size() <= 3)
            continue;

        int oldLength = calculateRouteLength(route);

        for (int i = 1; i < (int)route.size() - 2; i++)
        {
            for (int j = i + 1; j < (int)route.size() - 1; j++)
            {
                reverse(route.begin() + i, route.begin() + j + 1);

                int newLength = calculateRouteLength(route);

                if (newLength < oldLength && newLength <= z)
                {
                    oldLength = newLength;
                    improved = true;
                }
                else
                {
                    reverse(route.begin() + i, route.begin() + j + 1);
                }
            }
        }
    }
    return improved;
}

void postOptimize(vector<vector<int>> &routes, int z, int maxIterations = 10)
{
    for (int iter = 0; iter < maxIterations; iter++)
    {
        bool improved = false;
        improved |= tryRelocate(routes, z);
        improved |= trySwap(routes, z);
        improved |= try2Opt(routes, z);

        if (!improved)
            break;
    }
}

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

    if (!bestRoutes.empty())
    {
        postOptimize(bestRoutes, lo);
    }

    int objectiveValue = getMaxRouteLength(bestRoutes);
    cerr << objectiveValue << endl;

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
