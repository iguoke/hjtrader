CC=g++
INCL=-I. -Iinclude
LDFLAGS:= -pthread -lrt

TARGET:= userdemo_shfe
all: $(TARGET)

userdemo_shfe: userdemo_shfe.cpp
	$(CC) userdemo_shfe.cpp -g -O3 $(INCL) ./libXeleMdAPI64.a  $(LDFLAGS) -o $@

clean:
	@rm -f $(TARGET)

.PHONY:all clean $(TARGET)
