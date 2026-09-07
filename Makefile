#============================================================================================
# Small wrapper for the Hydrogenic atom demo.
#============================================================================================

TOOLS_DIR = ./Tools
HYDROGENIC_DIR = ./Hydrogenic.vX

SUBMAKE = $(MAKE) -f Makefile.in TOOLS_DIR=$(TOOLS_DIR) HYDROGENIC_DIR=$(HYDROGENIC_DIR)

all:
	$(SUBMAKE) all

run:
	$(SUBMAKE) run

clean:
	$(SUBMAKE) clean

tidy: clean
