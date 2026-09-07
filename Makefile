#============================================================================================
# Small wrapper for the Hydrogenic atom demo.
#============================================================================================

TOOLS_DIR = ./Tools
HYDROGENIC_DIR = ./Hydrogenic.vX

SUBMAKE = $(MAKE) -f Makefile.in TOOLS_DIR=$(TOOLS_DIR) HYDROGENIC_DIR=$(HYDROGENIC_DIR)

.PHONY: all run update-modules clean tidy

all:
	$(SUBMAKE) all

run:
	$(SUBMAKE) run

update-modules:
	$(SUBMAKE) update-modules

clean:
	$(SUBMAKE) clean

tidy: clean

#============================================================================================
#============================================================================================
