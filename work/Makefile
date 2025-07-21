CC = g++
CFLAGS = -std=c++17 -Wall -Wextra -fPIC
LDFLAGS = -shared

TARGET = liblogger.so
APP = liblogger_app

.PHONY: all clean

all: $(TARGET) $(APP)

$(TARGET): Main/lib.cpp include/lib.h
		$(CC) $(CFLAGS) $(LDFLAGS) -Iinclude -o $@ $<

$(APP): Main/main.cpp $(TARGET)
		$(CC) $(CFLAGS) -Iinclude -o $@ $< -L. -llogger -pthread

clean:
		rm -f $(TARGET) $(APP) *.o *.log
