#include <bits/stdc++.h>
using namespace std;

int n, k;
int max_edge = INT_MIN, min_edge = INT_MAX;
const double EPSILON = 0.001;

vector<vector<int>> d;
vector<vector<int>> tabuList;

struct BestSolution
{
    vector<vector<int>> routes;
    double objective;

    BestSolution() : objective(DBL_MAX) {}
};

struct Node
{
    int val;
    Node *prev = nullptr;
    Node *next = nullptr;
    Node(int i, Node *prev = nullptr, Node *next = nullptr) : val(i), prev(prev), next(next) {}
};

vector<Node *> point;

int deltaRemove(int prev, int x, int next = -1)
{
    if (next == -1)
    {
        return -d[prev][x];
    }
    return d[prev][next] - d[prev][x] - d[x][next];
}

int deltaInsert(int prev, int x, int next)
{
    return -deltaRemove(prev, x, next);
}

struct Route
{
    Node *start, *end;
    int length;
    int size;

    Route()
    {
        start = new Node(0);
        end = start;
        length = 0;
        size = 1;
    }

    void push_back(Node *node)
    {
        length += d[end->val][node->val];
        end->next = node;
        node->prev = end;
        end = node;
        size++;
    }

    void insert(Node *v, int pr)
    {
        Node *cur = start;
        while (cur != nullptr && cur->val != pr)
        {
            cur = cur->next;
        }
        if (cur == nullptr)
        {
            cout << "Invalid elements to insert!\n";
            return;
        }
        if (cur->next == nullptr)
        {
            push_back(v);
            return;
        }

        int oldEdge = d[cur->val][cur->next->val];
        int newEdges = d[cur->val][v->val] + d[v->val][cur->next->val];
        length = length - oldEdge + newEdges;

        v->next = cur->next;
        v->prev = cur;
        cur->next->prev = v;
        cur->next = v;
        size++;
    }

    Node *remove(int v)
    {
        Node *cur = start;
        while (cur != nullptr && cur->val != v)
        {
            cur = cur->next;
        }
        if (cur == nullptr)
        {
            cout << "Invalid elements to remove!\n";
            return nullptr;
        }
        if (cur == start)
        {
            cout << "Cannot remove depot!\n";
            return nullptr;
        }

        int oldEdges = 0;
        int newEdges = 0;

        if (cur->prev)
        {
            oldEdges += d[cur->prev->val][cur->val];
        }
        if (cur->next)
        {
            oldEdges += d[cur->val][cur->next->val];
            if (cur->prev)
            {
                newEdges = d[cur->prev->val][cur->next->val];
            }
        }

        length = length - oldEdges + newEdges;

        if (cur->prev)
            cur->prev->next = cur->next;
        if (cur->next)
            cur->next->prev = cur->prev;
        if (cur == end)
            end = cur->prev;

        cur->prev = nullptr;
        cur->next = nullptr;
        size--;
        return cur;
    }

    int findBestInsertPosition(Node *node)
    {
        int bestPos = 0;
        int minDelta = INT_MAX;
        Node *cur = start;
        while (cur != nullptr)
        {
            int nextVal = (cur->next) ? cur->next->val : 0;
            int delta = deltaInsert(cur->val, node->val, nextVal);
            if (delta < minDelta)
            {
                minDelta = delta;
                bestPos = cur->val;
            }
            cur = cur->next;
        }
        return bestPos;
    }
};

struct Routes
{
    vector<Route *> routes;

    Routes()
    {
        routes.resize(k);
        for (int i = 0; i < k; i++)
        {
            routes[i] = new Route();
        }
    }

    void printRoutes()
    {
        cout << routes.size() << '\n';
        for (int i = 0; i < routes.size(); i++)
        {
            cout << routes[i]->size << '\n';
            Node *cur = routes[i]->start;
            while (cur != nullptr)
            {
                cout << cur->val << ' ';
                cur = cur->next;
            }
            cout << '\n';
        }
    }
};

enum MoveType
{
    RELOCATE,
    SWAP
};

struct Move
{
    MoveType m;
    int v1;
    int v2;
    int k1;
    int k2;
};

double getMaxLength(Routes *r);

void performMove(Move move, Routes *r)
{
    if (move.m == RELOCATE)
    {
        Node *node = r->routes[move.k1]->remove(move.v1);
        if (node == nullptr)
            return;
        int bestPos = r->routes[move.k2]->findBestInsertPosition(node);
        r->routes[move.k2]->insert(node, bestPos);
    }
    else if (move.m == SWAP)
    {
        Node *node1 = r->routes[move.k1]->remove(move.v1);
        Node *node2 = r->routes[move.k2]->remove(move.v2);

        if (node1 == nullptr || node2 == nullptr)
            return;

        int pos1 = (node1->prev) ? node1->prev->val : 0;
        int pos2 = (node2->prev) ? node2->prev->val : 0;

        r->routes[move.k2]->insert(node1, pos2);
        r->routes[move.k1]->insert(node2, pos1);
    }
}

double calculateDelta(Move move, Routes *r)
{
    int oldMax = 0;
    int oldSum = 0;
    for (int i = 0; i < k; i++)
    {
        oldMax = max(oldMax, r->routes[i]->length);
        oldSum += r->routes[i]->length;
    }
    double oldObjective = oldMax + EPSILON * oldSum;

    if (move.m == RELOCATE)
    {
        Node *node = r->routes[move.k1]->start;
        while (node != nullptr && node->val != move.v1)
            node = node->next;

        if (node == nullptr || node->prev == nullptr)
            return INT_MAX;

        int prevVal = node->prev->val;
        int nextVal = (node->next) ? node->next->val : -1;
        int deltaK1 = deltaRemove(prevVal, move.v1, nextVal);

        int bestPos = r->routes[move.k2]->findBestInsertPosition(node);

        Node *insertAfter = r->routes[move.k2]->start;
        while (insertAfter != nullptr && insertAfter->val != bestPos)
            insertAfter = insertAfter->next;

        if (insertAfter == nullptr)
            return INT_MAX;

        int insertNextVal = (insertAfter->next) ? insertAfter->next->val : -1;
        int deltaK2 = deltaInsert(bestPos, move.v1, insertNextVal);

        int newLengthK1 = r->routes[move.k1]->length + deltaK1;
        int newLengthK2 = r->routes[move.k2]->length + deltaK2;

        int newMax = max(newLengthK1, newLengthK2);
        int newSum = newLengthK1 + newLengthK2;
        for (int i = 0; i < k; i++)
        {
            if (i != move.k1 && i != move.k2)
            {
                newMax = max(newMax, r->routes[i]->length);
                newSum += r->routes[i]->length;
            }
        }

        double newObjective = newMax + EPSILON * newSum;
        return newObjective - oldObjective;
    }
    else if (move.m == SWAP)
    {
        Node *node1 = r->routes[move.k1]->start;
        while (node1 != nullptr && node1->val != move.v1)
            node1 = node1->next;

        Node *node2 = r->routes[move.k2]->start;
        while (node2 != nullptr && node2->val != move.v2)
            node2 = node2->next;

        if (node1 == nullptr || node2 == nullptr)
            return INT_MAX;
        if (node1->prev == nullptr || node2->prev == nullptr)
            return INT_MAX;

        int prev1 = node1->prev->val;
        int next1 = (node1->next) ? node1->next->val : -1;
        int deltaRemove1 = deltaRemove(prev1, move.v1, next1);
        int deltaInsert1 = deltaInsert(prev1, move.v2, next1);
        int deltaK1 = deltaRemove1 + deltaInsert1;

        int prev2 = node2->prev->val;
        int next2 = (node2->next) ? node2->next->val : -1;
        int deltaRemove2 = deltaRemove(prev2, move.v2, next2);
        int deltaInsert2 = deltaInsert(prev2, move.v1, next2);
        int deltaK2 = deltaRemove2 + deltaInsert2;

        int newLengthK1 = r->routes[move.k1]->length + deltaK1;
        int newLengthK2 = r->routes[move.k2]->length + deltaK2;

        int newMax = max(newLengthK1, newLengthK2);
        int newSum = newLengthK1 + newLengthK2;
        for (int i = 0; i < k; i++)
        {
            if (i != move.k1 && i != move.k2)
            {
                newMax = max(newMax, r->routes[i]->length);
                newSum += r->routes[i]->length;
            }
        }

        double newObjective = newMax + EPSILON * newSum;
        return newObjective - oldObjective;
    }

    return DBL_MAX;
}

void initialize()
{
    cin >> n >> k;
    d.assign(n + 1, vector<int>(n + 1, 0));
    for (int i = 0; i <= n; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            cin >> d[i][j];
            max_edge = max(max_edge, d[i][j]);
            min_edge = min(min_edge, d[i][j]);
        }
    }
    for (int i = 0; i < n; i++)
    {
        point.push_back(new Node(i + 1));
    }
    tabuList.assign(n + 1, vector<int>(k, 0));
}

int getRandom(int n)
{
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dist(0, n - 1);
    return dist(gen);
}

void greedyInitial(Routes *r)
{
    vector<int> unassigned;
    for (int i = 1; i <= n; i++)
    {
        unassigned.push_back(i);
    }

    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(unassigned.begin(), unassigned.end(), mt19937(seed));

    for (int customer : unassigned)
    {
        int bestRoute = -1;
        int bestPosition = -1;
        int bestCost = INT_MAX;

        for (int routeIdx = 0; routeIdx < k; routeIdx++)
        {
            Node *cur = r->routes[routeIdx]->start;
            while (cur != nullptr)
            {
                int nextVal = (cur->next) ? cur->next->val : -1;

                int insertCost;
                if (nextVal == -1)
                {
                    insertCost = d[cur->val][customer];
                }
                else
                {
                    insertCost = d[cur->val][customer] + d[customer][nextVal] - d[cur->val][nextVal];
                }

                if (insertCost < bestCost)
                {
                    bestCost = insertCost;
                    bestRoute = routeIdx;
                    bestPosition = cur->val;
                }

                cur = cur->next;
            }
        }

        if (bestRoute != -1 && bestPosition != -1)
        {
            Node *newNode = new Node(customer);
            r->routes[bestRoute]->insert(newNode, bestPosition);
        }
    }
}

int calculateRouteLength(Route *route)
{
    int length = 0;
    Node *cur = route->start;
    while (cur != nullptr && cur->next != nullptr)
    {
        length += d[cur->val][cur->next->val];
        cur = cur->next;
    }
    return length;
}

double getMaxLength(Routes *r)
{
    int maxLen = 0;
    int sumLen = 0;
    for (int i = 0; i < r->routes.size(); i++)
    {
        int actualLength = calculateRouteLength(r->routes[i]);
        r->routes[i]->length = actualLength;
        maxLen = max(maxLen, actualLength);
        sumLen += actualLength;
    }
    return maxLen + EPSILON * sumLen;
}

Routes *copyRoutes(Routes *r)
{
    Routes *newRoutes = new Routes();
    for (int i = 0; i < k; i++)
    {
        Node *cur = r->routes[i]->start->next;
        while (cur != nullptr)
        {
            newRoutes->routes[i]->push_back(new Node(cur->val));
            cur = cur->next;
        }
    }
    return newRoutes;
}

vector<Move> generateNeighborhood(Routes *r)
{
    vector<Move> moves;

    vector<pair<int, int>> routeLengths;
    for (int i = 0; i < k; i++)
    {
        routeLengths.push_back({r->routes[i]->length, i});
    }
    sort(routeLengths.rbegin(), routeLengths.rend());

    int numCritical = min(max(3, k / 2), k);
    vector<int> criticalRoutes;
    for (int i = 0; i < numCritical; i++)
    {
        criticalRoutes.push_back(routeLengths[i].second);
    }

    for (int criticalRoute : criticalRoutes)
    {
        int k1 = criticalRoute;
        Node *cur = r->routes[k1]->start->next;
        while (cur != nullptr)
        {
            for (int k2 = 0; k2 < k; k2++)
            {
                if (k1 != k2)
                {
                    Move m;
                    m.m = RELOCATE;
                    m.v1 = cur->val;
                    m.v2 = -1;
                    m.k1 = k1;
                    m.k2 = k2;
                    moves.push_back(m);

                    if (r->routes[k2]->length < r->routes[k1]->length / 2)
                    {
                        moves.push_back(m);
                    }
                }
            }
            cur = cur->next;
        }
    }

    for (int criticalRoute : criticalRoutes)
    {
        int k1 = criticalRoute;
        int k2 = (k1 + 1) % k;
        if (k2 != k1)
        {
            Node *cur1 = r->routes[k1]->start->next;
            while (cur1 != nullptr)
            {
                Node *cur2 = r->routes[k2]->start->next;
                while (cur2 != nullptr)
                {
                    Move m;
                    m.m = SWAP;
                    m.v1 = cur1->val;
                    m.v2 = cur2->val;
                    m.k1 = k1;
                    m.k2 = k2;
                    moves.push_back(m);
                    cur2 = cur2->next;
                }
                cur1 = cur1->next;
            }
        }

        int k3 = (k1 - 1 + k) % k;
        if (k3 != k1)
        {
            Node *cur1 = r->routes[k1]->start->next;
            while (cur1 != nullptr)
            {
                Node *cur2 = r->routes[k3]->start->next;
                while (cur2 != nullptr)
                {
                    Move m;
                    m.m = SWAP;
                    m.v1 = cur1->val;
                    m.v2 = cur2->val;
                    m.k1 = k1;
                    m.k2 = k3;
                    moves.push_back(m);
                    cur2 = cur2->next;
                }
                cur1 = cur1->next;
            }
        }
    }

    int maxNeighborhoodSize = 200;
    if (n >= 100)
    {
        maxNeighborhoodSize = min(500, 200 + (n - 100) / 5);
    }
    else if (n >= 50)
    {
        maxNeighborhoodSize = 300;
    }

    if (moves.size() > maxNeighborhoodSize)
    {
        vector<Move> sampledMoves;
        vector<int> indices(moves.size());
        iota(indices.begin(), indices.end(), 0);

        unsigned seed = chrono::system_clock::now().time_since_epoch().count();
        shuffle(indices.begin(), indices.end(), mt19937(seed));

        for (int i = 0; i < maxNeighborhoodSize; i++)
        {
            sampledMoves.push_back(moves[indices[i]]);
        }
        return sampledMoves;
    }

    return moves;
}

bool isTabu(Move move, int iteration)
{
    if (move.m == RELOCATE)
    {
        return tabuList[move.v1][move.k2] > iteration;
    }
    else
    {
        return (tabuList[move.v1][move.k2] > iteration) ||
               (tabuList[move.v2][move.k1] > iteration);
    }
}

void addToTabu(Move move, int iteration)
{
    int tenure;
    if (move.m == RELOCATE)
    {
        tenure = max(5, (int)(0.04 * n));
        tabuList[move.v1][move.k2] = iteration + tenure;
    }
    else
    {
        tenure = 2 * max(5, (int)(0.04 * n));
        tabuList[move.v1][move.k2] = iteration + tenure;
        tabuList[move.v2][move.k1] = iteration + tenure;
    }
}

void twoOptRoute(Route *route)
{
    if (route->size <= 3)
        return;

    bool improved = true;
    while (improved)
    {
        improved = false;
        vector<Node *> nodes;
        Node *cur = route->start;
        while (cur != nullptr)
        {
            nodes.push_back(cur);
            cur = cur->next;
        }

        int sz = nodes.size();
        for (int i = 1; i < sz - 2; i++)
        {
            for (int j = i + 1; j < sz - 1; j++)
            {
                int v1 = nodes[i - 1]->val;
                int v2 = nodes[i]->val;
                int v3 = nodes[j]->val;
                int v4 = nodes[j + 1]->val;

                int oldCost = d[v1][v2] + d[v3][v4];
                int newCost = d[v1][v3] + d[v2][v4];

                if (newCost < oldCost)
                {
                    reverse(nodes.begin() + i, nodes.begin() + j + 1);

                    route->start->next = nullptr;
                    route->end = route->start;
                    route->length = 0;
                    route->size = 1;

                    for (int p = 1; p < sz; p++)
                    {
                        nodes[p]->prev = nullptr;
                        nodes[p]->next = nullptr;
                        route->push_back(nodes[p]);
                    }
                    improved = true;
                    break;
                }
            }
            if (improved)
                break;
        }
    }
}

void diversify(Routes *r)
{
    vector<pair<int, int>> routeLengths;
    for (int i = 0; i < k; i++)
    {
        routeLengths.push_back({r->routes[i]->length, i});
    }
    sort(routeLengths.rbegin(), routeLengths.rend());

    int numSwaps = max(5, n / 6);
    int swapsPerformed = 0;

    for (int iter = 0; iter < numSwaps && swapsPerformed < numSwaps; iter++)
    {
        int k1, k2;
        if (iter < k / 2)
        {
            k1 = routeLengths[iter % k].second;
            k2 = routeLengths[(k - 1 - iter) % k].second;
        }
        else
        {
            k1 = getRandom(k);
            k2 = getRandom(k);
        }

        if (k1 == k2 || r->routes[k1]->size <= 1 || r->routes[k2]->size <= 1)
            continue;

        int idx1 = getRandom(r->routes[k1]->size - 1) + 1;
        int idx2 = getRandom(r->routes[k2]->size - 1) + 1;

        Node *node1 = r->routes[k1]->start;
        for (int j = 0; j < idx1; j++)
            node1 = node1->next;

        Node *node2 = r->routes[k2]->start;
        for (int j = 0; j < idx2; j++)
            node2 = node2->next;

        if (node1 != nullptr && node2 != nullptr)
        {
            Move m;
            m.m = SWAP;
            m.v1 = node1->val;
            m.v2 = node2->val;
            m.k1 = k1;
            m.k2 = k2;
            performMove(m, r);
            swapsPerformed++;
        }
    }

    for (int i = 0; i < k; i++)
    {
        twoOptRoute(r->routes[i]);
    }
}

void balanceRoutes(Routes *r, double timeLimit = 2.0)
{
    using clk = chrono::steady_clock;
    auto t0 = clk::now();

    auto elapsed = [&]()
    {
        return chrono::duration<double>(clk::now() - t0).count();
    };

    while (elapsed() < timeLimit)
    {
        int maxIdx = 0, minIdx = 0;
        for (int i = 0; i < k; i++)
        {
            if (r->routes[i]->length > r->routes[maxIdx]->length)
                maxIdx = i;
            if (r->routes[i]->length < r->routes[minIdx]->length)
                minIdx = i;
        }

        int gap = r->routes[maxIdx]->length - r->routes[minIdx]->length;
        if (gap <= 10)
            break;

        Node *bestNode = nullptr;
        int bestDelta = INT_MAX;

        Node *cur = r->routes[maxIdx]->start->next;
        while (cur != nullptr)
        {
            int removeFromMax = (cur->prev ? d[cur->prev->val][cur->next ? cur->next->val : 0] : 0) - (cur->prev ? d[cur->prev->val][cur->val] : 0) - (cur->next ? d[cur->val][cur->next->val] : 0);

            int bestPosMin = r->routes[minIdx]->findBestInsertPosition(cur);
            Node *insertPos = r->routes[minIdx]->start;
            while (insertPos && insertPos->val != bestPosMin)
                insertPos = insertPos->next;

            int addToMin = 0;
            if (insertPos && insertPos->next)
                addToMin = d[insertPos->val][cur->val] + d[cur->val][insertPos->next->val] - d[insertPos->val][insertPos->next->val];
            else if (insertPos)
                addToMin = d[insertPos->val][cur->val];

            int newMaxLen = r->routes[maxIdx]->length + removeFromMax;
            int newMinLen = r->routes[minIdx]->length + addToMin;
            int newMax = max(newMaxLen, newMinLen);

            double currentMax = getMaxLength(r);
            double delta = newMax - currentMax;

            if (delta <= 0 && abs(newMaxLen - newMinLen) < gap)
            {
                if (delta < bestDelta)
                {
                    bestDelta = delta;
                    bestNode = cur;
                }
            }
            cur = cur->next;
        }

        if (bestNode != nullptr)
        {
            Move m;
            m.m = RELOCATE;
            m.v1 = bestNode->val;
            m.k1 = maxIdx;
            m.k2 = minIdx;
            performMove(m, r);
        }
        else
        {
            break;
        }
    }
}

void tabuSearch(Routes *curRoutes, BestSolution *bestSolution, double time_limit, double stall_time)
{
    using clk = chrono::steady_clock;
    auto t0 = clk::now();
    auto last_improve = t0;

    auto elapsed = [&]()
    {
        return chrono::duration<double>(clk::now() - t0).count();
    };

    auto stagnated = [&]()
    {
        return chrono::duration<double>(clk::now() - last_improve).count();
    };

    bestSolution->routes.clear();
    bestSolution->routes.resize(k);
    for (int i = 0; i < k; i++)
    {
        bestSolution->routes[i].clear();
        Node *cur = curRoutes->routes[i]->start->next;
        while (cur != nullptr)
        {
            bestSolution->routes[i].push_back(cur->val);
            cur = cur->next;
        }
    }

    double bestObjective = getMaxLength(curRoutes);
    bestSolution->objective = bestObjective;
    double currentObjective = bestObjective;
    int iteration = 0;

    while (elapsed() < time_limit)
    {
        double currentStagnation = stagnated();
        if (currentStagnation > stall_time)
        {
            diversify(curRoutes);
            currentObjective = getMaxLength(curRoutes);
            last_improve = clk::now();
            stall_time = max(1.0, stall_time * 0.9);
        }
        vector<Move> neighborhood = generateNeighborhood(curRoutes);
        Move bestMove;
        double bestDelta = DBL_MAX;
        bool foundMove = false;

        double minDeltaSeen = DBL_MAX;

        for (const Move &move : neighborhood)
        {
            double delta = calculateDelta(move, curRoutes);
            if (delta < minDeltaSeen)
                minDeltaSeen = delta;

            double projectedObjective = currentObjective + delta;
            bool aspirationSatisfied = (projectedObjective < bestObjective);

            if (!isTabu(move, iteration) || aspirationSatisfied)
            {
                if (delta < bestDelta)
                {
                    bestDelta = delta;
                    bestMove = move;
                    foundMove = true;
                }
            }
        }

        if (!foundMove)
        {
            for (const Move &move : neighborhood)
            {
                double delta = calculateDelta(move, curRoutes);
                if (delta < bestDelta)
                {
                    bestDelta = delta;
                    bestMove = move;
                    foundMove = true;
                }
            }
        }

        if (foundMove)
        {
            performMove(bestMove, curRoutes);
            double oldCurrent = currentObjective;
            currentObjective = getMaxLength(curRoutes);

            addToTabu(bestMove, iteration);
            if (currentObjective < bestObjective)
            {
                if (currentObjective > 0)
                {
                    bestSolution->routes.clear();
                    bestSolution->routes.resize(k);
                    for (int i = 0; i < k; i++)
                    {
                        bestSolution->routes[i].clear();
                        Node *cur = curRoutes->routes[i]->start->next;
                        while (cur != nullptr)
                        {
                            bestSolution->routes[i].push_back(cur->val);
                            cur = cur->next;
                        }
                    }
                    bestObjective = currentObjective;
                    bestSolution->objective = bestObjective;

                    last_improve = clk::now();
                }
            }
        }

        iteration++;
    }
}

void solve()
{
    using clk = chrono::steady_clock;
    auto globalStart = clk::now();
    double totalTimeLimit = 29.5;

    auto globalElapsed = [&]()
    {
        return chrono::duration<double>(clk::now() - globalStart).count();
    };

    initialize();

    Routes *initialRoutes = new Routes();
    greedyInitial(initialRoutes);

    BestSolution bestSolution;

    double tabuTimeLimit = 28.5;
    double stall_time = 1.5;
    tabuSearch(initialRoutes, &bestSolution, tabuTimeLimit, stall_time);

    Routes finalRoutes;
    for (int i = 0; i < k; i++)
    {
        for (int val : bestSolution.routes[i])
        {
            finalRoutes.routes[i]->push_back(new Node(val));
        }
    }

    double timeRemaining = totalTimeLimit - globalElapsed();
    if (timeRemaining > 1)
    {
        for (int i = 0; i < k; i++)
        {
            if (globalElapsed() >= totalTimeLimit)
                break;
            twoOptRoute(finalRoutes.routes[i]);
        }
    }

    timeRemaining = totalTimeLimit - globalElapsed();
    if (timeRemaining > 0.5)
    {
        balanceRoutes(&finalRoutes, timeRemaining - 0.2);
    }

    double finalObjective = getMaxLength(&finalRoutes);

    if (finalObjective <= bestSolution.objective)
    {
        bestSolution.routes.clear();
        bestSolution.routes.resize(k);
        for (int i = 0; i < k; i++)
        {
            bestSolution.routes[i].clear();
            Node *cur = finalRoutes.routes[i]->start->next;
            while (cur != nullptr)
            {
                bestSolution.routes[i].push_back(cur->val);
                cur = cur->next;
            }
        }
        bestSolution.objective = finalObjective;
    }

    finalRoutes.printRoutes();
}

int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    solve();
    return 0;
}
