# use "gcc" to compile source files.
CC = gcc
LD = gcc
CFLAGS = -g -Wall
LDFLAGS =
RM = /bin/rm -f
OBJS = tokenscan.o rtfprep.o
SRCS = Makefile rtf-controls rtfprep.c standard-names tokenscan.c tokenscan.h
PROG = rtfprep

all: $(PROG)
	./$(PROG)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

# rule for cleaning re-compilable files.
clean:
	$(RM) $(PROG) $(OBJS) 
	
realclean:
	$(RM) rtf-ctrl rtf-ctrldef.h rtf-namedef.h stdcharnames.h

