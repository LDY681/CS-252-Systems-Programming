.PHONY: all
all: git-commit tests examples

.PHONY: git-commit
git-commit:
	git checkout master >> .local.git.out || echo
	git add *.c Makefile >> .local.git.out  || echo
	git commit -a -m 'Commit' >> .local.git.out || echo
	git push origin master

.PHONY: tests
tests:
	$(MAKE) -C tests

.PHONY: examples
examples:
	$(MAKE) -C examples

.PHONY: test
test: tests
	python ./runtest.py

.PHONY: clean
clean: 
	$(MAKE) -C tests clean
	$(MAKE) -C examples clean
