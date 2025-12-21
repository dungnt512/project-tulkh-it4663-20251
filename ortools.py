from ortools.constraint_solver import pywrapcp, routing_enums_pb2


# data = sys.stdin.read().strip().split()
# it = iter(data)
# N = int(next(it))
# K = int(next(it))
# size = N + 1
# dist = [[int(next(it)) for _ in range(size)] for __ in range(size)]
N, K = [int(x) for x in input().split()]
size = N + 1
dist = [[int(x) for x in input().split()] for _ in range(size)]

manager = pywrapcp.RoutingIndexManager(size, K, [0]*K, [0]*K)
routing = pywrapcp.RoutingModel(manager)

def distance_cb(from_index, to_index):
  if routing.IsEnd(to_index):
    return 0
  i = manager.IndexToNode(from_index)
  j = manager.IndexToNode(to_index)
  return dist[i][j]

transit_index = routing.RegisterTransitCallback(distance_cb)
routing.SetArcCostEvaluatorOfAllVehicles(transit_index)

max_d = 0
for i in range(size):
  for j in range(size):
    if dist[i][j] > max_d:
      max_d = dist[i][j]
upper_sum = (N + K) * max_d
cap = max(1, upper_sum) 

routing.AddDimension(
    transit_index,
    0,          
    cap,        
    True,       
    "Distance"
)
distance_dim = routing.GetDimensionOrDie("Distance")

W = upper_sum + 1
distance_dim.SetGlobalSpanCostCoefficient(W)

for v in range(K):
  distance_dim.CumulVar(routing.End(v)).SetRange(0, cap)
  routing.AddVariableMinimizedByFinalizer(
      distance_dim.CumulVar(routing.End(v)))
  routing.AddVariableMinimizedByFinalizer(
      distance_dim.CumulVar(routing.Start(v)))

params = pywrapcp.DefaultRoutingSearchParameters()
params.first_solution_strategy = routing_enums_pb2.FirstSolutionStrategy.PARALLEL_CHEAPEST_INSERTION
params.local_search_metaheuristic = routing_enums_pb2.LocalSearchMetaheuristic.GUIDED_LOCAL_SEARCH
params.time_limit.FromSeconds(30) 
params.log_search = False

solution = routing.SolveWithParameters(params)
if solution is None:
  print(K)
  for _ in range(K):
    print(1)
    print(0)
else:
  print(K)
  for v in range(K):
    idx = routing.Start(v)
    route = [0] 
    while not routing.IsEnd(idx):
      next_idx = solution.Value(routing.NextVar(idx))
      if routing.IsEnd(next_idx):
        break
      node = manager.IndexToNode(next_idx)
      route.append(node)
      idx = next_idx
    print(len(route))
    print(*route)


