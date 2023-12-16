rasterizer:
	g++ *.cpp -std=c++14 -o rasterizer
run:
	rm rasterizer && g++ *.cpp -std=c++14 -o rasterizer
clean:
	rm *.ppm *.png rasterizer
