CC = g++  
OUTFILE = hilinkwatchdog
OBJS = main.o 
LIBS = -lpthread -lcurl
FLAGS = -O2 
CXXFLAGS = -O2 

all: $(OUTFILE)
main.o: main.cpp 
		$(CC) $(CXXFLAGS) -c $(INCS) main.cpp
           

$(OUTFILE): $(OBJS)
	$(CC) $(CXXFLAGS) $(FLAGS) $(OBJS) $(LIBS) -o $(OUTFILE)
	
clean:
	rm *.o	
	rm *.gch
	rm $(OUTFILE)

cleanall: clean  
	  rm  $(OUTFILE)
