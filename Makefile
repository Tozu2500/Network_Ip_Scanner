CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I.
LDLIBS   := -lws2_32 -liphlpapi
BUILD_DIR := build
TARGET    := $(BUILD_DIR)/netscanner.exe

SRCS := src/main.cpp \
        src/ui/console_ui.cpp \
        src/ui/table_formatter.cpp \
        src/network/adapter_info.cpp \
        src/network/arp_scanner.cpp \
        src/network/ping_sweep.cpp \
        src/oui/oui_lookup.cpp

OBJS := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
