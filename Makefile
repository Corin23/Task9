CC = gcc
CFLAGS = -g -Wall 
TARGET = txtproc

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c -lcurl

clean:
	$(RM) $(TARGET)
