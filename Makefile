# Compiler to use
CXX = g++

# Compiler flags: -std=c++17 for C++17 standard, -Wall to show all warnings.
# The -pthread flag has been removed as it's not needed for native Windows threads.
CXXFLAGS = -std=c++17 -Wall

# Linker flags: -lws2_32 is required for Windows Sockets (networking).
LDFLAGS = -lws2_32

# Directories
SRCDIR = core
OBJDIR = obj

# Executable names
SERVER_TARGET = server.exe
CLIENT_TARGET = client.exe

# List of all source files
CORE_SRCS = $(wildcard $(SRCDIR)/*.cpp)
SERVER_SRCS = server.cpp
CLIENT_SRCS = client.cpp

# Generate object file names from source file names
CORE_OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CORE_SRCS))

# Default target: build both the server and the client
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Rule to link the server executable
$(SERVER_TARGET): $(SERVER_SRCS) $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(SERVER_SRCS) $(CORE_OBJS) $(LDFLAGS)
	@echo "Server build complete."

# Rule to link the client executable
$(CLIENT_TARGET): $(CLIENT_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(CLIENT_SRCS) $(LDFLAGS)
	@echo "Client build complete."

# Rule to compile source files from the core/ directory into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Target to clean up all build files
clean:
	@echo "Cleaning up..."
	-del /Q $(SERVER_TARGET) $(CLIENT_TARGET) 2>nul
	-if exist $(OBJDIR) rmdir /S /Q $(OBJDIR) 2>nul

# Phony targets are not real files
.PHONY: all clean
