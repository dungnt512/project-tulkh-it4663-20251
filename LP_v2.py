from ortools.linear_solver import pywraplp

def LP_v2():
    N, K = map(int, input().split())

    d = []
    for _ in range(N + 1):
        d.append(list(map(int, input().split())))
        
    for i in range(K):
        d.append(d[0])
        
    solver = pywraplp.Solver.CreateSolver('SCIP')
    
    x = {}
    for i in range(N + K + 1):
        for j in range(N + K + 1):
            x[i, j] = solver.IntVar(0, 1, 'x[' + str(i) + ',' + str(j) + ']')
    
    y = {}
    for i in range(N + K + 1):
        y[i] = solver.IntVar(0, N - 1, 'y[' + str(i) + ']')
        
    gr = {}
    for i in range(N + K + 1):
        for k in range(K):
            gr[i, k] = solver.IntVar(0, 1, 'gr[' + str(i) + ',' + str(k) + ']')
        
            
    infinity = solver.infinity()
    f = solver.NumVar(0, infinity, 'f')    
    
    solver.Add(sum(x[i, 0] for i in range(N + K + 1)) + sum(x[0, i] for i in range(N + K + 1)) + sum(x[i, i] for i in range(N + K + 1)) == 0)
    for i in range(K):
        solver.Add(sum(x[N + i + 1, j] for j in range(1, N + 1)) == 1)
        solver.Add(gr[N + i + 1, i] == 1)
        for j in range(K):
            if i != j:
                solver.Add(gr[N + i + 1, j] == 0)
        
    M = N * K
    for i in range(N + K + 1):
        for j in range(N + K + 1):    
            if (i != j and j <= N):
                solver.Add((1 - x[i, j]) * M + y[j] >= y[i] + 1)
                solver.Add((x[i, j] - 1) * M + y[j] <= y[i] + 1)
                
                for k in range(K):
                    solver.Add((1 - x[i, j]) * M + gr[j, k] >= gr[i, k])
                    solver.Add((x[i, j] - 1) * M + gr[j, k] <= gr[i, k])
                    
    for i in range(1, N + 1):
        solver.Add(sum(x[j, i] for j in range(1, N + K + 1)) == 1)
        solver.Add(sum(x[i, j] for j in range(1, N + 1)) <= 1)
        
    solver.Add(sum(x[i, j] for i in range(N + K + 1) for j in range(1, N + 1)) == N)
    
    z = {}
    for i in range(N + K + 1):
        for j in range(N + K + 1):   
            for k in range(K):
                z[i, j, k] = solver.IntVar(0, 1, 'z[' + str(i) + ',' + str(j) + ',' + str(k) +']');      
                solver.Add(z[i, j, k] <= x[i, j])
                solver.Add(z[i, j, k] <= gr[i, k])
                solver.Add(z[i, j, k] <= gr[j, k])
                solver.Add(z[i, j, k] >= x[i, j] + gr[i, k] + gr[j, k] - 2)
    
    for k in range(K):
        route = 0
        for i in range(N + K + 1):
            for j in range(N + K + 1): 
                u = i if i <= N else 0
                v = j if j <= N else 0
                route += z[i, j, k] * d[u][v]
        solver.Add(f >= route)

    solver.Minimize(f)
    solver.set_time_limit(15000)                
    status = solver.Solve()
    
    # if status == pywraplp.Solver.OPTIMAL:
    #     print("gg")

    print(K)
    for k in range(K):
        route = [0]
        curr = N + k + 1
        while True:
            next_node = -1
            for j in range(N + 1):
                if (curr, j) in x and x[curr, j].solution_value() > 0.5:
                    next_node = j
                    break
                
            if next_node == -1:
                break
                
            curr = next_node
            route.append(curr)
            
        print(len(route))
        print(*(route))
    
          
if __name__ == "__main__":
    LP_v2()