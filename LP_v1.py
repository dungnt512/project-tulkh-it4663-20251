from ortools.linear_solver import pywraplp

def solve():
    N, K = map(int, input().split())

    dist_matrix = []
    for _ in range(N + 1):
        row = list(map(int, input().split()))
        dist_matrix.append(row)
    
    solver = pywraplp.Solver.CreateSolver('SCIP')
    dummy_node = N + 1
    
    all_nodes = range(N + 2) 
    customers = range(1, N + 1) 

    x = {}
    for k in range(K):
        for i in all_nodes:
            for j in all_nodes:
                if i != j and i != dummy_node and j != 0:
                    x[k, i, j] = solver.IntVar(0, 1, f'x_{k}_{i}_{j}')

    u = {}
    for i in range(1, N + 2):
        u[i] = solver.IntVar(1, N + 2, f'u_{i}')

    infinity = solver.infinity()
    z = solver.NumVar(0, infinity, 'z')

    for i in customers:
        solver.Add(sum(x[k, j, i] for k in range(K) for j in all_nodes if (k, j, i) in x) == 1)


    for k in range(K):
        for h in customers:
            flow_in = sum(x[k, i, h] for i in all_nodes if (k, i, h) in x)
            flow_out = sum(x[k, h, j] for j in all_nodes if (k, h, j) in x)
            solver.Add(flow_in == flow_out)

    for k in range(K):
        solver.Add(sum(x[k, 0, j] for j in all_nodes if (k, 0, j) in x) == 1)
        solver.Add(sum(x[k, i, dummy_node] for i in customers if (k, i, dummy_node) in x) == 1)

    M_mtz = N + 2
    for k in range(K):
        for i in customers:
            for j in range(1, N + 2):
                if i != j and (k, i, j) in x:
                    solver.Add(u[i] - u[j] + M_mtz * x[k, i, j] <= M_mtz - 1)

    for k in range(K):
        route_dist = 0
        for i in range(N + 1):
            for j in range(1, N + 1):
                if i != j and (k, i, j) in x:
                    route_dist += x[k, i, j] * dist_matrix[i][j]
        solver.Add(z >= route_dist)

    solver.Minimize(z)
    solver.set_time_limit(5000)
    solver.Solve()

    print(K)
    for k in range(K):
        route = [0]
        curr = 0
        while True:
            next_node = -1
            for j in all_nodes:
                if (k, curr, j) in x and x[k, curr, j].solution_value() > 0.5:
                    next_node = j
                    break
                
            if next_node == -1 or next_node == dummy_node:
                break
                
            curr = next_node
            route.append(curr)
            
        print(len(route))
        print(*(route))

if __name__ == "__main__":
    solve()