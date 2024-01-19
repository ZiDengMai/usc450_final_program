# Define the compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g

# Define the executable name
TARGETS = serverS

# List all the object files
OBJS = serverS.o

TARGETM = serverM
OBJM = serverM.o

TARGETL = serverL
OBJL = serverL.o

TARGETH = serverH
OBJH = serverH.o

TARGETC = client
OBJC = client.o

# The first rule is the one executed when no parameters are fed into the makefile
all: $(TARGETS) $(TARGETM) $(TARGETL) $(TARGETH) $(TARGETC)

# Rule for creating the executable
$(TARGETS): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGETS) $(OBJS)

$(TARGETM): $(OBJM)
	$(CXX) $(CXXFLAGS) -o $(TARGETM) $(OBJM)

$(TARGETL): $(OBJL)
	$(CXX) $(CXXFLAGS) -o $(TARGETL) $(OBJL)

$(TARGETH): $(OBJH)
	$(CXX) $(CXXFLAGS) -o $(TARGETH) $(OBJH)

$(TARGETC): $(OBJC)
	$(CXX) $(CXXFLAGS) -o $(TARGETC) $(OBJC)

# Rule for creating object file test1.o
serverS.o: serverS.cpp
	$(CXX) $(CXXFLAGS) -c serverS.cpp

#Rule for creating object file serverM.o
serverM.o: serverM.cpp
	$(CXX) $(CXXFLAGS) -c serverM.cpp

serverL.o: serverL.cpp
	$(CXX) $(CXXFLAGS) -c serverL.cpp

serverH.o: serverH.cpp
	$(CXX) $(CXXFLAGS) -c serverH.cpp

client.o: client.cpp
	$(CXX) $(CXXFLAGS) -c client.cpp
# Rule for cleaning up
clean:
	rm -f $(TARGETS) $(OBJS) $(TARGETM) $(OBJM) $(TARGETH) $(OBJH) $(TARGETL) $(OBJL) $(TARGETLC) $(OBJC)
