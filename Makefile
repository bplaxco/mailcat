PKGNAME := mailcat
VERSION := 0.1.0
RELEASE := f25
RPMSPECDIR := .
RPMSPEC := $(RPMSPECDIR)/rpm.spec
SOURCEDIR=$(PKGNAME)-$(VERSION)

all: $(PKGNAME)

runserver:
	sudo ./$(PKGNAME) 25

rpm.spec: rpm.spec.in
	cat $< | \
	sed "s/%VERSION%/$(VERSION)/" | \
	sed "s/%RELEASE%/$(RELEASE)/" | \
	sed "s/%PKGNAME%/$(PKGNAME)/" \
	> $@

clean:
	@find . -type f -regex ".*\.[o]$$" -delete
	@find . -type f \( -name "*~" -or -name "#*" \) -delete
	@rm -fR dist rpm-build rpm.spec
	@rm -f ./$(PKGNAME)

install: $(PKGNAME)
	mkdir -p $(DESTDIR)/usr/bin
	cp ./$(PKGNAME) $(DESTDIR)/usr/bin/

sdist: clean rpm.spec
	mkdir -p ./$(SOURCEDIR) && mkdir -p dist &&\
	cp -r $(shell ls | grep -v $(SOURCEDIR)) ./$(SOURCEDIR)/ && \
	tar -czf ./dist/$(SOURCEDIR).tar.gz $(SOURCEDIR)
	rm -r $(SOURCEDIR)

rpm: sdist
	@mkdir -p rpm-build
	@cp dist/*.gz rpm-build/
	rpmbuild --define "_topdir %(pwd)/rpm-build" \
	--define "_builddir %{_topdir}" \
	--define "_rpmdir %{_topdir}" \
	--define "_srcrpmdir %{_topdir}" \
	--define "_specdir $(RPMSPECDIR)" \
	--define "_sourcedir %{_topdir}" \
	-ba $(RPMSPEC)
	#############################################################################
	@echo "$(PKGNAME) RPMs are built:"
	@find rpm-build -maxdepth 2 -name '$(PKGNAME)*.rpm' | awk '{print "* " $$1}'
	#############################################################################
