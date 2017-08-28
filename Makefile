CC = g++  -fexceptions
OUTFILE = hilinkwatchdog
OBJS = main.o 
LIBS = -lpthread -lcurl
FLAGS = -O2 -fexceptions
CXXFLAGS = -O2 -fexceptions

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
