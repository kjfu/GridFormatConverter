#include <fstream>
#include "mesh.h"

int main(int argc, char *argv[]){

	std::string str = std::string(argv[1]);
	std::string out = "out.vtk";
	MMesh amesh;
	amesh.read(str, "su2");
	amesh.write(out, "vtk");






	return 0;

}