CC=g++ -std=c++11
INCL=-I. -Iinclude
LDFLAGS:= -pthread -lrt

TARGET:=master
all: $(TARGET)

#md_shfe: userdemo_shfe.cpp
#	$(CC) userdemo_shfe.cpp -g -O3 $(INCL) ./libXeleMdAPI64.a  $(LDFLAGS) -o $@
master:master.cpp
	$(CC) master.cpp -g -O3 $(INCL) ./libXeleTdAPI64.so  ./libXeleMdAPI64.a $(LDFLAGS) -o $@
clean:
	@rm -f $(TARGET)

.PHONY:all clean $(TARGET)
