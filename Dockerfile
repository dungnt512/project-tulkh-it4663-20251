# Sử dụng image Ubuntu mới nhất
FROM ubuntu:latest

# Cài đặt g++ và python
RUN apt-get update && apt-get install -y \
    build-essential \
    python3 \
    && rm -rf /var/lib/apt/lists/*

# Thiết lập thư mục làm việc trong container
WORKDIR /app

# Khi khởi động, container sẽ đợi lệnh từ bạn
CMD ["bash"]