main productionLineLiquid productionLinePILLS openGL: main.cpp productionLineLiquid.cpp productionLinePILLS.cpp openGL.cpp
	g++ main.cpp -o main
	g++ productionLineLiquid.cpp -o productionLineLiquid
	g++ productionLinePILLS.cpp -o productionLinePILLS
	g++ -o openGL openGL.cpp -lGL -lGLU -lglut -lm