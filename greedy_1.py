from ortools.linear_solver import pywraplp

def solve():
    N, K = map(int, input().split())

    dist = []
    for _ in range(N + 1):
        row = list(map(int, input().split()))
        dist.append(row)
    
    visited = []
    for _ in range(N + 1):
        visited.append(False)
    
    node = []
    sum = []
    for _ in range(K):
        node.append([0])
        sum.append(0)
        
    for _ in range(N):
        smallest = -1
        v = -1
        biggest = -1
        for i in range(K):
            for j in range(1, N + 1):
                if visited[j] == False:
                    if biggest == -1 or smallest == -1 or v == -1 or biggest > sum[i] + dist[node[i][-1]][j]:
                        smallest, v, biggest = i, j, sum[i] + dist[node[i][-1]][j]
                    

        u = node[smallest][-1]
        node[smallest].append(v)
        sum[smallest] += dist[u][v]
        visited[v] = True
        
    print(K)
    for i in range(K):
        print(len(node[i]), end = ' ')
        for x in node[i]:
            print(x, end = ' ')
        print()
        

if __name__ == "__main__":
    solve()