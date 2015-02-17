all:
	(cd src && make -j2 all)

netmush:
	(cd src && make -j2 netmush)

tools:
	(cd tools && make -j2 all)

clean:
	(cd src && make clean)
	(cd tools && make clean)

