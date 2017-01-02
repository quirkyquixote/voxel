
# Targets to build and install binaries 
# Also, the help text for tntsh

SRC_TARGETS += all
SRC_TARGETS += install
SRC_TARGETS += clean

.PHONY: $(SRC_TARGETS)

all:
	@make -C src all

install:
	@make -C src install

clean:
	@make -C src clean


# Targets to build and install all the documentation

DOC_TARGETS += doc
DOC_TARGETS += install-doc
DOC_TARGETS += clean-doc
DOC_TARGETS += html
DOC_TARGETS += install-html
DOC_TARGETS += clean-html
DOC_TARGETS += pdf
DOC_TARGETS += install-pdf
DOC_TARGETS += clean-pdf
DOC_TARGETS += text
DOC_TARGETS += install-text
DOC_TARGETS += clean-text

.PHONY: $(DOC_TARGETS)
$(DOC_TARGETS):
	@make -C doc $@

# Targets to build and install the API documentation

APIDOC_TARGETS += apidoc
APIDOC_TARGETS += install-apidoc
APIDOC_TARGETS += clean-apidoc

.PHONY: $(APIDOC_TARGETS)
$(APIDOC_TARGETS):
	@make -C apidoc $@

# Targets to build and run tests

TEST_TARGETS += test
TEST_TARGETS += stest
TEST_TARGETS += ftest
TEST_TARGETS += clean-test

.PHONY: $(TEST_TARGETS)

test: all
	@make -C tests all

stest: all
	@make -C tests software

ftest: all
	@make -C tests firmware

clean-test:
	@make -C tests clean

# Target to clean everything

.PHONY: realclean
realclean:
	git clean -fdx

# Target to list targets

.PHONY: help
help:
	@echo "This Makefile accepts the following targets:"
	@echo "    all (the default if no target is provided)"
	@echo "    clean"
	@echo "    install"
	@echo "    doc"
	@echo "    clean-doc"
	@echo "    install-doc"
	@echo "    html"
	@echo "    clean-html"
	@echo "    install-html"
	@echo "    pdf"
	@echo "    clean-pdf"
	@echo "    install-pdf"
	@echo "    text"
	@echo "    clean-text"
	@echo "    install-text"
	@echo "    apidoc"
	@echo "    clean-apidoc"
	@echo "    install-apidoc"
	@echo "    test"
	@echo "    stest"
	@echo "    ftest"
	@echo "    clean-test"
