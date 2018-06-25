# Build settings
CC = g++
CFLAGS = -Wall -std=c++11

SRCDIR = src
BUILDDIR = build

$(BUILDDIR)/collector: $(SRCDIR)/parameters.hpp $(SRCDIR)/configuration.hpp $(SRCDIR)/zeverdata.hpp $(SRCDIR)/collector.cpp
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(SRCDIR)/collector.cpp -lcurl -o $@

$(BUILDDIR)/decimalfixer: $(SRCDIR)/decimalfixer.cpp
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(SRCDIR)/decimalfixer.cpp -o $@

.PHONY: all
all: $(BUILDDIR)/collector $(BUILDDIR)/decimalfixer

.PHONY: clean
clean:
	rm -r $(BUILDDIR)