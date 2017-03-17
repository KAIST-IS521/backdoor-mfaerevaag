# ml compiler and flags
MLC = ocaml
MLCFLAGS = /usr/lib/ocaml/str.cma

# mini compiler
MINIC = compiler.ml

# byte code to compile. should be set through make argument
_TARGET = ../$(TARGET)/$(TARGET)
_BUILDDIR = ../$(BUILDDIR)

.PHONY: compile

all: compile

compile: $(_TARGET).mini $(_BUILDDIR)
	$(MLC) $(MLCFLAGS) $(MINIC) $(_TARGET).mini $(_BUILDDIR)/$(TARGET).bc
