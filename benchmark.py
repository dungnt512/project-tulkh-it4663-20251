import os
import subprocess
import sys
import re
import io

# --- CẤU HÌNH ---
CPP_SOURCE = "VRP_MinMax_greedy.cpp"
# Tự động thêm .exe nếu chạy trên Windows để tránh lỗi WinError 193
EXEC_NAME = "my_algorithm"
if os.name == 'nt':
    EXEC_NAME += ".exe"
EXEC_PATH = os.path.join(".", EXEC_NAME)

TESTCASE_DIR = "./testcase"
OUTPUT_FILE = "benchmark_results.txt"

# Fix lỗi hiển thị Unicode trên Terminal
if sys.stdout.encoding != 'utf-8':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

def natural_sort_key(s):
    return [int(text) if text.isdigit() else text.lower() for text in re.split(r'(\d+)', s)]

def calculate_vrp_objective(input_path, cpp_stdout):
    """ 
    Checker tính toán Objective Min-Max:
    Lộ trình: Depot(0) -> Điểm 1 -> Điểm 2 -> ... -> Điểm cuối (Không quay về 0)
    """
    try:
        # 1. Đọc ma trận d từ file .inp
        with open(input_path, 'r') as f:
            lines = f.read().splitlines()
            if not lines: return "N/A"
            header = lines[0].split()
            N = int(header[0])
            
            # Đọc N+1 dòng ma trận (từ dòng 1 đến N+1)
            dist_matrix = []
            for i in range(1, N + 2):
                dist_matrix.append(list(map(int, lines[i].split())))

        # 2. Đọc lộ trình từ stdout của C++
        output_data = cpp_stdout.strip().split('\n')
        if not output_data: return "No Output"

        K_out = int(output_data[0])
        max_dist = 0

        # Duyệt qua từng xe k
        for k in range(K_out):
            # Dòng 2k+1 chứa l_k (số điểm), dòng 2k+2 chứa dãy điểm (lộ trình)
            route_idx = 2 * k + 2
            if route_idx >= len(output_data): break
            
            route = list(map(int, output_data[route_idx].split()))
            if not route: continue

            current_route_dist = 0
            
            # Bước 1: Từ Depot (0) đến điểm đầu tiên trong lộ trình
            u = 0 
            # Bước 2: Đi qua các điểm trong lộ trình
            for v in route:
                current_route_dist += dist_matrix[u][v]
                u = v
            
            # Lưu ý: Không cộng thêm dist_matrix[u][0] vì xe không quay về depot

            max_dist = max(max_dist, current_route_dist)
            
        return max_dist
    except Exception as e:
        return f"Err: {str(e)}"

def compile_code():
    print(f"--- Đang biên dịch {CPP_SOURCE} ---")
    # -o EXEC_PATH sẽ tự tạo .exe trên Windows nếu EXEC_PATH có đuôi .exe
    compile_cmd = ["g++", "-O3", CPP_SOURCE, "-o", EXEC_PATH, "-DONLINE_JUDGE", "-march=native"]
    try:
        subprocess.check_call(compile_cmd)
        print("Biên dịch thành công!\n")
    except subprocess.CalledProcessError:
        print("Lỗi biên dịch! Vui lòng kiểm tra file C++.")
        sys.exit(1)

def run_benchmark():
    if not os.path.exists(TESTCASE_DIR):
        print(f"Lỗi: Thư mục {TESTCASE_DIR} không tồn tại")
        return

    files = [f for f in os.listdir(TESTCASE_DIR) if f.endswith('.inp')]
    files.sort(key=natural_sort_key)

    with open(OUTPUT_FILE, "w", encoding="utf-8") as out_log:
        out_log.write(f"{'Testcase':<20} | {'n':<5} | {'k':<5} | {'Objective':<12} | {'Time':<10}\n")
        out_log.write("-" * 65 + "\n")

        for filename in files:
            file_path = os.path.join(TESTCASE_DIR, filename)
            
            match_n = re.search(r'n_(\d+)', filename)
            match_k = re.search(r'k_(\d+)', filename)
            n_str = match_n.group(1) if match_n else "?"
            k_str = match_k.group(1) if match_k else "?"

            print(f"Đang chạy {filename:20}", end=" ", flush=True)

            try:
                with open(file_path, "r") as f_in:
                    process = subprocess.Popen(
                        [EXEC_PATH], 
                        stdin=f_in, 
                        stdout=subprocess.PIPE, 
                        stderr=subprocess.PIPE, 
                        text=True,
                        encoding='utf-8'
                    )
                    stdout_data, stderr_data = process.communicate()
                
                # Tính Objective (Min-Max quãng đường các xe)
                obj_value = calculate_vrp_objective(file_path, stdout_data)
                
                # Lấy Runtime từ stderr (dòng 2 mà bạn in ra cerr)
                err_parts = stderr_data.strip().split()
                runtime = err_parts[1] if len(err_parts) >= 2 else (err_parts[0] if err_parts else "N/A")

                # Ghi kết quả vào log và in ra màn hình
                out_log.write(f"{filename:<20} | {n_str:<5} | {k_str:<5} | {obj_value:<12} | {runtime}ms\n")
                print(f"-> Obj: {obj_value}, Time: {runtime}ms")
                
            except Exception as e:
                print(f"-> Lỗi hệ thống: {e}")

    print(f"\nHoàn thành! Kết quả tại: {OUTPUT_FILE}")

if __name__ == "__main__":
    compile_code()
    run_benchmark()
