IDIR := Include
SDIR := Source
ODIR := .OBJ

CC := g++
CLFAGS := -std=c++11
#CFLAGS += -Ofast
#CFLAGS += -Wall
CFLAGS += -I$(IDIR) -MMD

_SRC := $(shell find $(SDIR) -name "*.cpp")
ALL_OBJ := $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(_SRC))

MAINS := $(ODIR)/main.o $(ODIR)/sender.o $(ODIR)/receiver.o $(ODIR)/cleaner.o

OBJ := $(filter-out $(MAINS), $(ALL_OBJ))

DEPS := $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.d, $(_SRC))

$(ODIR):
	@mkdir $(ODIR)

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: Client Receiver Sender Cleaner

Client: $(ODIR) $(ALL_OBJ)
	$(CC) $(OBJ) $(ODIR)/main.o -o $@

Receiver: $(ODIR) $(ALL_OBJ)
	$(CC) $(OBJ) $(ODIR)/receiver.o -o $@

Sender: $(ODIR) $(ALL_OBJ)
	$(CC) $(OBJ) $(ODIR)/sender.o -o $@

Cleaner: $(ODIR) $(ALL_OBJ)
	$(CC) $(OBJ) $(ODIR)/cleaner.o -o $@

.PHONY: clean

clean:
	rm -rf Client Receiver Sender Cleaner $(ODIR)

$(VERBOSE).SILENT:

-include $(DEPS)