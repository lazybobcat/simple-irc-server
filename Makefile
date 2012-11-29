CXX=g++
CXXFLAGS=-Wall -ansi -g
LDFLAGS=-lboost_filesystem -lboost_system -lpthread
EXEC=serveur_irc
SRC=config.cpp channel.cpp user.cpp session.cpp server.cpp parser.cpp mainframe.cpp main.cpp
OBJS=$(SRC:.cpp=.o)

all: $(EXEC)

serveur_irc: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)


%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	rm -rf *.o
	rm -rf $(EXEC)
