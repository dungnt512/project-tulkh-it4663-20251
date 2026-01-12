import os
import subprocess
import sys
import time
import re
import io

# --- CẤU HÌNH ---
CPP_SOURCE = "greedy_2.cpp"          # Tên file source code của bạn
EXEC_PATH = "./my_algorithm"     # Tên file sau khi biên dịch
TESTCASE_DIR = "./testcase"      # Thư mục chứa các file .inp
OUTPUT_FILE = "benchmark_results.txt"

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8')

def natural_sort_key(s):
    """ Hàm bổ trợ để sắp xếp theo thứ tự số: test_n_6 < test_n_10 < test_n_100 """
    return [int(text) if text.isdigit() else text.lower() for text in re.split(r'(\d+)', s)]

def compile_code():
    print(f"--- Đang biên dịch {CPP_SOURCE} ---")
    # Thêm -DONLINE_JUDGE để vô hiệu hóa đoạn freopen bên trong #ifndef ONLINE_JUDGE
    compile_cmd = ["g++", "-O3", CPP_SOURCE, "-o", EXEC_PATH, "-DONLINE_JUDGE", "-march=native"]
    try:
        subprocess.check_call(compile_cmd)
        print("Biên dịch thành công!\n")
    except subprocess.CalledProcessError:
        print("Lỗi biên dịch! Vui lòng kiểm tra lại file C++.")
        exit(1)

def run_benchmark():
    if not os.path.exists(TESTCASE_DIR):
        print(f"Lỗi: Không tìm thấy thư mục {TESTCASE_DIR}")
        return

    files = [f for f in os.listdir(TESTCASE_DIR) if f.endswith('.inp')]
    files.sort(key=natural_sort_key)

    with open(OUTPUT_FILE, "w", encoding="utf-8") as out_log:
        for filename in files:
            file_path = os.path.join(TESTCASE_DIR, filename)
            
            # Trích xuất n và k từ tên file
            match_n = re.search(r'n_(\d+)', filename)
            match_k = re.search(r'k_(\d+)', filename)
            n_val = match_n.group(1) if match_n else "?"
            k_val = match_k.group(1) if match_k else "?"

            print(f"Đang chạy {filename}...", end=" ", flush=True)

            with open(file_path, "r") as f_in:
                
                # Chạy và bắt stderr (cerr)
                process = subprocess.Popen(
                    [EXEC_PATH], 
                    stdin=f_in, 
                    stdout=subprocess.PIPE, 
                    stderr=subprocess.PIPE, 
                    text=True
                )
                
                _, stderr = process.communicate()
            # Tìm Objective value từ cerr
            output_data = stderr.strip().split()
            
            if len(output_data) >= 2:
                obj_value = output_data[0]
                runtime = output_data[1]
            else:
                obj_value = "Error"
                runtime = "Error"

            # Ghi kết quả
            out_log.write(f"n = {n_val}, k = {k_val}\n")
            out_log.write(f"Objective value = {obj_value}\n")
            out_log.write(f"Runtime = {runtime}ms\n")
            out_log.write("-" * 25 + "\n")
            print(f"Done! ({runtime}ms)")

    print(f"\nHoàn thành! Kết quả lưu tại: {OUTPUT_FILE}")

if __name__ == "__main__":
    compile_code()
    run_benchmark()
