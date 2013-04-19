CROSS := @
CC := $(CROSS)gcc
AR := $(CROSS)ar
STRIP := $(CROSS)strip
MKDIR := @mkdir -p
ECHO := @echo -e
RM := @rm -Rf
MAKE := @$(MAKE)
LS := @ls
CP := @cp -Rf
MV := @mv

TARGET := remote-login
OBJ_DIR	= ./tmp

SRC +=
SRC += ddns_util.c
SRC += ddns_dyndns.c
SRC += ddns.c
SRC += IniFile.c
SRC += tools.c
SRC += upnp.c
SRC += remote.c

OBJ := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
DEP := $(patsubst %.c,$(OBJ_DIR)/%.d,$(SRC))

INC := -I.
CFLAGS := -g3 -O0 -Wall -DFALSE=0 -DTRUE=1 -DLINUX=2 $(INC)
LDFLAGS := -L. -lm -lpthread

.PHONY: clean all

all : $(TARGET)

$(OBJ_DIR)/%.o : %.c
	$(ECHO) "\033[33mmaking $<...\033[0m"
	$(CC) $(CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o $@ -c $<
	
$(TARGET) : $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS) 

sinclude $(DEP)

clean:
	$(RM) $(OBJ) $(DEP) $(TARGET)
