all::

CAT=cat
FIND=find
XARGS=xargs
SED=sed
GETVERSION=git describe

PREFIX=/usr/local

VERSION:=$(shell $(GETVERSION) 2> /dev/null)

## Provide a version of $(abspath) that can cope with spaces in the
## current directory.
myblank:=
myspace:=$(myblank) $(myblank)
MYCURDIR:=$(subst $(myspace),\$(myspace),$(CURDIR)/)
MYABSPATH=$(foreach f,$1,$(if $(patsubst /%,,$f),$(MYCURDIR)$f,$f))

sinclude $(call MYABSPATH,config.mk)
sinclude nonogram-env.mk

### Target components

binaries.c += nonogram
nonogram_obj += nonogram
nonogram_obj += context
nonogram_obj += display
nonogram_lib += -lnonogram


SOURCES:=$(filter-out $(headers),$(shell $(FIND) src/obj \( -name "*.c" -o -name "*.h" \) -printf '%P\n'))

riscos_zips += nonogram
nonogram_zrof += !ReadMe,fff

nonogram_apps += source
source_appname=Nonogram
source_rof += Docs/COPYING,fff
source_rof += Docs/VERSION,fff
source_rof += Docs/README,fff
source_rof += $(call riscos_src,$(SOURCES))

nonogram_apps += boot
boot_appname=!Boot
boot_rof += $(call riscos_bin,$(binaries.c))


nonogram_wof += nonogram.exe
nonogram_wof += Docs/COPYING.txt
nonogram_wof += Docs/VERSION.txt
nonogram_wof += Docs/README.md

include binodeps.mk



$(BINODEPS_OUTDIR)/riscos/Nonogram/Docs/README,fff: README.md
	$(MKDIR) "$(@D)"
	$(CP) "$<" "$@"

$(BINODEPS_OUTDIR)/riscos/Nonogram/Docs/COPYING,fff: LICENSE.txt
	$(MKDIR) "$(@D)"
	$(CP) "$<" "$@"

$(BINODEPS_OUTDIR)/riscos/Nonogram/Docs/VERSION,fff: VERSION
	$(MKDIR) "$(@D)"
	$(CP) "$<" "$@"



all:: installed-binaries VERSION

lc=$(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

ifneq ($(filter true t y yes on 1,$(call lc,$(ENABLE_RISCOS))),)
install:: install-riscos
all:: riscos-zips
endif

ifneq ($(VERSION),)
prepare-version::
	@$(MKDIR) tmp/
	@$(ECHO) $(VERSION) > tmp/VERSION

tmp/VERSION: | prepare-version
VERSION: tmp/VERSION
	@$(CMP) -s '$<' '$@' || $(CP) '$<' '$@'
endif

### C compilation

tmp/obj/context.o: VERSION

CFLAGS += -DVERSION='"$(file <VERSION)"'

### Installation

install:: install-binaries

### Housekeeping

tidy::
	@$(FIND) . -name "*~" -delete


### Creation of zip for Windows

out/nonogram-windows.zip: $(nonogram_wof:%=out/windows/%)
	$(CD) out/windows ; \
	$(ZIP) -rq '$(call MYABSPATH,$@)' $(nonogram_wof)

out/windows/%.exe: out/%
	@$(MKDIR) '$(@D)'
	$(CP) '$<' '$@'

out/windows/Docs/README.md: README.md
	@$(MKDIR) '$(@D)'
	$(CP) '$<' '$@'

out/windows/Docs/COPYING.txt: LICENSE.txt
	@$(MKDIR) '$(@D)'
	$(CP) '$<' '$@'

out/windows/Docs/VERSION.txt: VERSION
	@$(MKDIR) '$(@D)'
	$(CP) '$<' '$@'

out/windows/%.c: src/obj/%.c
	@$(MKDIR) '$(@D)'
	$(CP) '$<' '$@'

out/windows/%.h: src/obj/%.h
	@$(MKDIR) '$(@D)'
	$(CP) '$<' '$@'



# Set this to the comma-separated list of years that should appear in
# the licence.  Do not use characters other than [0-9,] - no spaces.
YEARS=2001,2005-7,2012

update-licence:
	$(FIND) . -name ".svn" -prune -or -type f -print0 | $(XARGS) -0 \
	$(SED) -i 's/Copyright (C) [0-9,-]\+  Steven Simpson/Copyright (C) $(YEARS)  Steven Simpson/g'

distclean:: blank
	$(RM) VERSION
