FLAGS = -Wall -std=gnu++11 -pedantic
LIBS  = -I incl C:\MinGW\lib\libgdi32.a
SRC   =  src/xwordmain.cpp src/xword.cpp
#SRC   =  src/test.cpp
TGT   = bin/xword.exe
ARGS  = HUMAN PROTHEAN ASARI SALARIAN TURIAN KROGAN QUARIAN BATARIAN
# human prothean vorcha krogan asari salarian turian batarian quarian

all:
	g++ $(SRC) -o $(TGT) $(FLAGS) $(LIBS)

run:
	$(TGT) $(ARGS)

runmem:
	drmemory -batch $(TGT) $(ARGS)

clean:
	rm $(TGT)
