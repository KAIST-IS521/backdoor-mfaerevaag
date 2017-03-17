BUILDDIR = build

# mini file
TARGET ?= test

.PHONY: clean interpreter compile run

all: interpreter

$(BUILDDIR):
	mkdir -p $@

interpreter:
	make -C $@

export TARGET
export BUILDDIR

compile: $(BUILDDIR)
	make -C compiler

run: $(BUILDDIR)/$(TARGET).bc
	interpreter/$(BUILDDIR)/interpreter $^

clean:
	make -C interpreter clean
