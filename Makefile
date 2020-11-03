# Please see the README for setting up a valid build environment.

# The top-level binary that you wish to produce.
all: docker

docker:
	docker run --rm -it -u `id -u ${USER}`:`id -g ${USER}` --mount type=bind,source=`pwd`,target=/src/code/project rattboi/naomi-build:latest make -f Makefile.proj clean a_menu.bin

.PHONY: clean
clean:
	rm -rf build
	rm -rf a_menu.bin

