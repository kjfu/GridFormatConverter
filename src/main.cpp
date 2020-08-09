#include <fstream>
#include "mesh.h"

int main(int argc, char *argv[]){

	std::string str = std::string(argv[1]);
	std::string out = "out.su2";
	MMesh amesh;
	amesh.read(str, "su2");
	amesh.write(out, "su2");






	return 0;

}