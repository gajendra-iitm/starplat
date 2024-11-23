
all:
	g++ generatePoints.cpp -o gen
	./gen
	# g++ circularPoints.cpp -o cir
	# ./cir
	hipcc convexHull.hip -o convexHull
	./convexHull < testcase.txt
	