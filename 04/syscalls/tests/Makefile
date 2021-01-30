CC = gcc -static
TARGETS = test_topuser test_timeuser test_kernelps test_freeblocks test_pidtoname

all: $(TARGETS)

clean:
	rm $(TARGETS)

run:
	./test_topuser
	./test_timeuser
	./test_pidtoname `echo $$`
	./test_freeblocks "./""
	./test_kernelps