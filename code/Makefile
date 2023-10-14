# Makefile for lsci

# Compiler and flags
CXX := g++
NVCC := nvcc
CXXFLAGS := -O8 -fopenmp
LIBS := -lslope -lhpdf -lbluetooth -lmosquitto -lsqlite3 -ltiff -lpng

# Executable target
TARGET := lsci

# Default target
all: $(TARGET)

$(TARGET): lsci_cu.o rblue.o test.o bp_data.o
	$(CXX) $^ $(CXXFLAGS) -o $@ `pkg-config --cflags --libs cuda-12.0 cudart-12.0 gtk+-3.0 MagickWand /usr/local/lib/x86_64-linux-gnu/pkgconfig/aravis-0.8.pc` $(LIBS)

lsci_cu.o: lsci.cu main.h
	$(NVCC) -c lsci.cu -o $@

test.o: test.c main.h
	$(CXX) -c -w -o $@ test.c `pkg-config --cflags --libs gtk+-3.0 MagickWand /usr/local/lib/x86_64-linux-gnu/pkgconfig/aravis-0.8.pc` -lslope -lhpdf -lsqlite3 -ltiff -lpng

bp_data.o: bp_data.c
	$(CXX) -c bp_data.c -o $@ -lmosquitto

rblue.o: raspi_bluetooth.c
	$(CXX) -c raspi_bluetooth.c -o $@ `pkg-config --cflags --libs gtk+-3.0` -lbluetooth


exe: lsci
	sudo ./lsci
clean:
	rm -f $(TARGET) lsci_cu.o test.o bp_data.o rblue.o


