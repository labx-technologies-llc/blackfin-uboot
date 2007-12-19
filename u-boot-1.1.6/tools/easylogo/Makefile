CFLAGS += -Wall

ifeq ($(GZIP),1)
CPPFLAGS += -DENABLE_GZIP
endif

all: easylogo

clean:
	rm -f easylogo *.o

.PHONY: all clean
