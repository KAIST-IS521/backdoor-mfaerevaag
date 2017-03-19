BUILDDIR = build

# mini file
TARGET ?= test

.PHONY: clean interpreter backdoor compile run run-backdoor

all: interpreter

$(BUILDDIR):
	mkdir -p $@

interpreter:
	make -C $@

backdoor:
	make -C $@

export TARGET
export BUILDDIR

compile: $(BUILDDIR)
	make -C compiler

run: $(BUILDDIR)/$(TARGET).bc
	interpreter/$(BUILDDIR)/interpreter $^

run-backdoor: $(BUILDDIR)/$(TARGET).bc
	backdoor/$(BUILDDIR)/interpreter $^

clean:
	make -C interpreter clean
	make -C backdoor clean
