# Compiler to use
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall

# Linker flags (WinSock for networking targets)
LDFLAGS = -lws2_32

# Directories
SRCDIR = core
OBJDIR = obj
TESTDIR = tests

# Executable names
SERVER_TARGET = server.exe
CLIENT_TARGET = client.exe
BANKCORE_TARGET = BankCore.exe
TEST_TARGET = unit_tests.exe

# List of source files
CORE_SRCS = $(wildcard $(SRCDIR)/*.cpp)
SERVER_SRCS = server.cpp
CLIENT_SRCS = client.cpp
BANKCORE_SRCS = BankCore.cpp
TEST_SRCS = $(TESTDIR)/test_bankcore.cpp

# Generate object file names
CORE_OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CORE_SRCS))

# Default target: build server, client, BankCore, and unit tests
all: $(BANKCORE_TARGET) $(SERVER_TARGET) $(CLIENT_TARGET) $(TEST_TARGET)

# Rule to link BankCore interactive executable
$(BANKCORE_TARGET): $(BANKCORE_SRCS) $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(BANKCORE_SRCS) $(CORE_OBJS)
	@echo "BankCore build complete."

# Rule to link the server executable
$(SERVER_TARGET): $(SERVER_SRCS) $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(SERVER_SRCS) $(CORE_OBJS) $(LDFLAGS)
	@echo "Server build complete."

# Rule to link the client executable
$(CLIENT_TARGET): $(CLIENT_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(CLIENT_SRCS) $(LDFLAGS)
	@echo "Client build complete."

# Rule to link unit test executable
$(TEST_TARGET): $(TEST_SRCS) $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -I. -o $@ $(TEST_SRCS) $(CORE_OBJS)
	@echo "Unit test suite build complete."

# Rule to compile core source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Target to run C++ Unit Tests
test: $(TEST_TARGET)
	@echo "Executing C++ Unit Tests..."
	./$(TEST_TARGET)

# Target to run C++ Unit Tests with AddressSanitizer (Memory Leak & Safety Check)
test-asan:
	@echo "Building and running unit tests with LLVM AddressSanitizer (ASan)..."
	$(CXX) $(CXXFLAGS) -fsanitize=address -g -I. -o $(TEST_TARGET) $(TEST_SRCS) $(CORE_SRCS)
	./$(TEST_TARGET)

# Target to run unit tests under Valgrind (Linux/WSL environment)
valgrind: $(TEST_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_TARGET)

# Target to run Python pytest harness
pytest: $(BANKCORE_TARGET)
	pytest -v

# Clean target
clean:
	@echo "Cleaning up build artifacts..."
	-del /Q $(SERVER_TARGET) $(CLIENT_TARGET) $(BANKCORE_TARGET) $(TEST_TARGET) 2>nul
	-if exist $(OBJDIR) rmdir /S /Q $(OBJDIR) 2>nul

.PHONY: all test test-asan valgrind pytest clean
