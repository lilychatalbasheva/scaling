# ===== Makefile for random.cc and graph5.cc (macOS Homebrew) =====

# Compiler
CXX := c++
CXXFLAGS := -O2 -std=c++17 -Wall -Wextra -Wshadow -fPIC -pthread -stdlib=libc++
CPPFLAGS :=
LDFLAGS  :=
LDLIBS   :=

# --- Your local Pythia8 install ---
PYTHIA8 ?= $(HOME)/Desktop/pythia8315
CPPFLAGS += -I$(PYTHIA8)/include
LDLIBS   += -L$(PYTHIA8)/lib -lpythia8
LDFLAGS  += -Wl,-rpath,$(PYTHIA8)/lib

# --- Homebrew OPT prefixes (stable symlinks) ---
ROOT_OPT     := /opt/homebrew/opt/root
FASTJET_OPT  := /opt/homebrew/opt/fastjet

# Prefer root-config if available
ROOT_CONFIG  := $(ROOT_OPT)/bin/root-config
ROOT_CFLAGS  := $(shell $(ROOT_CONFIG) --cflags 2>/dev/null)
ROOT_GLIBS   := $(shell $(ROOT_CONFIG) --glibs  2>/dev/null)

# If root-config isn’t available for some reason, fall back to opts
ifeq ($(strip $(ROOT_CFLAGS)),)
  ROOT_CFLAGS := -I$(ROOT_OPT)/include/root
  ROOT_GLIBS  := -L$(ROOT_OPT)/lib/root -lCore -lImt -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad \
                 -lROOTVecOps -lTree -lTreePlayer -lRint -lPostscript -lMatrix -lPhysics \
                 -lMathCore -lThread -lROOTNTuple -lMultiProc -lROOTDataFrame -lROOTNTupleUtil \
                 -Wl,-rpath,$(ROOT_OPT)/lib/root
endif
CPPFLAGS += $(ROOT_CFLAGS)
LDLIBS   += $(ROOT_GLIBS)

# FastJet (opt prefix has stable include/lib)
CPPFLAGS += -I$(FASTJET_OPT)/include
LDLIBS   += -L$(FASTJET_OPT)/lib -lfastjet -lfastjettools
LDFLAGS  += -Wl,-rpath,$(FASTJET_OPT)/lib

# Programs to build
PROGRAMS := random graph5
all: $(PROGRAMS)

# Generic build rule
%: %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

.PHONY: all clean
clean:
	rm -f $(PROGRAMS) *.o *.d *.so *.dylib *.root core* *~
