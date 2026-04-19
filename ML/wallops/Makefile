CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -Iincludes
LDFLAGS = -llgpio -lrt
TARGET = wallops
SRC = src/main.cpp
INCLUDES = includes/*.hpp

DATA_DIR = scan_data
SCAN_LOG_DIR = scan_logs
LIDAR_DIR = lidar
SCAN_DIRS = $(DATA_DIR) $(LOG_DIR) $(LIDAR_DIR)

LGPIO_REPO = https://github.com/joan2937/lg.git
LGPIO_DIR = /tmp/lgpio_build

.PHONY: all install-deps clean run flash setup-folders setup status debug

setup-folders:
	mkdir -p $(SCAN_DIRS)

all: setup-folders $(TARGET)

$(TARGET): $(SRC) $(INCLUDES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

install-deps:
	@echo "Checking lgpio..."
	@if ! pkg-config --exists lgpio 2>/dev/null; then \
		echo "Installing lgpio from $(LGPIO_REPO)..."; \
		rm -rf $(LGPIO_DIR); \
		git clone $(LGPIO_REPO) $(LGPIO_DIR); \
		cd $(LGPIO_DIR) && make -j4; \
		sudo make install; \
		sudo ldconfig; \
		rm -rf $(LGPIO_DIR) \
		echo "lgpio installed"; \
	else \
		echo "lgpio already installed";\
	fi
	sudo apt update && sudo apt install -y pkg-config libatomic1 git

debug: CXXFLAGS += -g --DDEBUG
debug: $(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf $(SCAN_DIRS)/*

run: setup-folders $(TARGET)
	sudo SCAN_DATA_DIR=$(DATA_DIR) SCAN_LOG_DIR=$(SCAN_LOG_DIR) ./$(TARGET)

flash: all run

setup: install-deps setup-folders all

status:
	@pkg-config --exists lgpio && echo "lgpio OK" || echo "lgpio MISSING"
	@[ -d "$(DATA_DIR)" ] && echo "$(DATA_DIR)/ OK" || echo "$(DATA_DIR)/ MISSING"
	@[ -d "$(LOG_DIR)" ] && echo "$(LOG_DIR)/ OK" || echo "$(LOG_DIR)/ MISSING"
	@[ -f "$(TARGET)" ] && echo "$(TARGET) built" || echo "$(TARGET) missing"

.PHONY: all debug clean run flash setup status setup-folders install-deps