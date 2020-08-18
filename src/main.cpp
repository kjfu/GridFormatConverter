#include <fstream>
#include "mesh.h"

int main(int argc, char *argv[]){

	if (argc <3){
		return 0;
	}

	std::string inFilePath = std::string(argv[1]);
	std::string inFileFormat = inFilePath.substr(inFilePath.find_last_of(".")+1);
	std::string outFilePath = std::string(argv[2]);
	std::string outFileFormat = outFilePath.substr(outFilePath.find_last_of(".")+1);

	MMesh amesh;

	amesh.read(inFilePath, inFileFormat);
	amesh.write(outFilePath, outFileFormat);

	return 0;

}