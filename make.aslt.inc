.SUFFIXES: .nrlex .rdppgen

.nrlex.c:
	$(NRLEX) < $< > $@

.rdppgen.c:
	$(RDPPGEN) $<

