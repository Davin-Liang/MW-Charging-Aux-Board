# 编译器设置
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -g -Iinclude
LDFLAGS = -lmodbus

# 目标可执行文件
TARGET = test_main

# 源文件
SRCS = test_main.cpp turntable_controller.cpp

# 对象文件
OBJS = $(SRCS:.cpp=.o)

# 默认目标
all: $(TARGET)

# 链接目标可执行文件
$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

# 编译源文件为对象文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 显式声明 turntable_controller.o 依赖于头文件
turntable_controller.o: include/turntable_controller.h
	$(CXX) $(CXXFLAGS) -c turntable_controller.cpp -o $@

# test_main.o 依赖于头文件
test_main.o: include/turntable_controller.h
	$(CXX) $(CXXFLAGS) -c test_main.cpp -o $@

# 清理生成的文件
clean:
	rm -f $(TARGET) $(OBJS)

# 安装依赖（如果需要）
install-deps:
	sudo apt update
	sudo apt install libmodbus-dev

# 显示帮助信息
help:
	@echo "可用目标:"
	@echo "  all        - 编译项目 (默认)"
	@echo "  clean      - 清理编译生成的文件"
	@echo "  install-deps - 安装依赖库"
	@echo "  help       - 显示此帮助信息"

.PHONY: all clean install-deps help