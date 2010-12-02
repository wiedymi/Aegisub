include ../Makefile.inc

.SUFFIXES: .po .mo

PO = \
	ca.po \
	cs.po \
	da.po \
	de.po \
	el.po \
	es.po \
	fi.po \
	fr_FR.po \
	hu.po \
	it.po \
	ja.po \
	ko.po \
	pt_BR.po \
	ru.po \
	vi.po \
	zh_CN.po \
	zh_TW.po

MO = $(addsuffix .mo, $(basename $(PO)))

all: $(MO)

%.mo: %.po
	$(BIN_MSGFMT) -o $@ $<


CLEANFILES = $(MO)

include ../Makefile.target
