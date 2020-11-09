# Please see the README for setting up a valid build environment.

GAMES=/home/rattboi/Downloads/Naomi/Games

# The top-level binary that you wish to produce.
all: docker

docker:
	docker run --rm -it \
		-u `id -u ${USER}`:`id -g ${USER}` \
		--mount type=bind,source=`pwd`,target=/src \
		--mount type=bind,source=${GAMES},target=/games \
		test-libnaomi:latest \
		make -f Makefile.proj clean makemenu a_menu.bin

.PHONY: clean
clean:
	rm -rf build
	rm -rf a_menu.bin

