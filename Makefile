# Please see the README for setting up a valid build environment.
TARGET=a_menu.bin

GAMES=/home/rattboi/Downloads/Naomi/Games
FLYCAST=/home/rattboi/code/flycast/flycast_libretro.so
#
# The top-level binary that you wish to produce.
all: clean build

.PHONY: build
build:
	docker run --rm -it \
		-u `id -u ${USER}`:`id -g ${USER}` \
		--mount type=bind,source=`pwd`,target=/src \
		rattboi/naomi-build:latest \
		make -f Makefile.proj ${TARGET}

.PHONY: menu
menu:
	docker run --rm -it \
		-u `id -u ${USER}`:`id -g ${USER}` \
		--mount type=bind,source=`pwd`,target=/src \
		--mount type=bind,source=${GAMES},target=/games \
		rattboi/naomi-build:latest \
		make -f Makefile.proj menu

.PHONY: clean
clean:
	rm -rf build
	rm -rf ${TARGET}

.PHONY: test
test:
	retroarch -L ${FLYCAST} ${TARGET}
