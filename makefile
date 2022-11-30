
CFLAGS := ./ \
	redfs/include/ \
	redfs/core/include/ \
	redfs/os/freertos/include/

CFLAGS := $(patsubst %,-I%,$(CFLAGS))
LDFLAGS := -lpthread

SRC := redfs/core/driver \
	redfs/posix \
	redfs/util/ \
	redfs/os/linux \
	#redfs/os/freertos/services/ \

SRC := $(wildcard $(foreach src,$(SRC),$(src)/*.c))
OBJ := $(patsubst %.c,%.o,$(SRC))


LINKOBJ := $(patsubst $(SRC)/%, % ,$(OBJ))

all : mkimage
	@echo "build finshed..."
	@echo "create filesystem..."
	@./mkimage rootfs
	@echo "file system finished..."

mkimage : mkimage.c $(LINKOBJ)
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o : %.c
	gcc $(CFLAGS) -o $@ -c $<


clean :
	find . -name *.o |xargs rm -f 
	rm -rf mkimage
