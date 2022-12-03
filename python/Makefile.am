EXTRA_DIST = dns.c example src setup.py 

if ENABLE_PYTHON

install-exec-local:
	$(PYTHON) -m pip install --prefix=$(DESTDIR)$(prefix) .

endif
