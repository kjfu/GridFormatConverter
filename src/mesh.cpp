#include "mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>

void MMesh::read(std::string filePath, std::string fileFormat){

	// std::string substr = filePath.substr(filePath.find_last_of(".")+1);

	if (fileFormat == "su2"){
		readSU2(filePath);
	}

}


void MMesh::readSU2(std::string filePath){
	std::ifstream inFile(filePath);

	if (inFile.is_open()){
		while (inFile){
			std::string line;
			std::string keystring;
			std::getline(inFile, line);
			std::stringstream lineStream(line);
			lineStream >> keystring;				
			if (keystring == "NDIME=" 
				|| keystring == "NDIME"){			
				std::string substr = line.substr(line.find_last_of("=")+1);
				nDim = atoi(substr.c_str());
			}
			else if (keystring == "NELEM="
					|| keystring == "NELEM"){
				std::string substr = line.substr(line.find_last_of("=")+1);
				int nElems = atoi(substr.c_str());



				for (int i=0; i<nElems; i++){
					int iType=0;
					std::getline(inFile, line);
					std::stringstream lStream(line);
					lStream >> iType;

					const auto readCell = 
					[&lStream]
					(auto &container, int nVertices){
						int index;
						container.emplace_back();
						for (int j=0; j<nVertices; j++){
							lStream >> index;
							container.back().form[j] = index;
						}
						container.back().index = container.size()-1;
					};

					if (iType == 10){
						readCell(tetrahedronCells_, 4);
					}
					else if (iType == 12){
						readCell(hexahedronCells_, 8);
					}
					else if (iType == 13){
						readCell(prismCells_, 6);
					}
					else if (iType == 14){
						readCell(pyramidCells_, 5);
					}

				}
			}
			else if (keystring == "NPOIN=" 
					|| keystring =="NPOIN"){
				std::string substr = line.substr(line.find_last_of("=")+1);
				int nPoints = atoi(substr.c_str());
				for(int i=0; i<nPoints; i++){
					std::getline(inFile, line);
					std::stringstream lStream(line);
					double coordinate;
					vertices_.emplace_back();
					for(int j=0; j<nDim; j++){
						lStream >> coordinate;
						vertices_.back().pt[j] = coordinate;
						vertices_.back().index = i;
					}
					if (nDim==2){
						vertices_.back().pt[2] = 0;
					}
				}
				
			}
			else if (keystring == "NMARK="
				    || keystring == "NMARK="){
				std::string substr = line.substr(line.find_last_of("=")+1);
				nBoundry = atoi(substr.c_str());
			}
			else if (keystring == "MARKER_TAG="
					|| keystring == "MARKER_TAG")
			{
				std::string substr = line.substr(line.find_last_of("=")+1);
				boundaries_.emplace_back();
				boundaries_.back().name = substr;
				std::getline(inFile, line);
				substr = line.substr(line.find_last_of("=")+1);
				int nFacets = atoi(substr.c_str());
				for(int i=0; i<nFacets; i++){
					std::getline(inFile, line);
					std::stringstream lstream(line);
					int type;
					lstream >> type;
					const auto writeBoundaryFacets = 
					[&lstream]
					(auto &container, auto &bdContainer)
					{				
						container.emplace_back();
						container.back().index= container.size()-1;	
						for(int j=0; j<container.back().nForms; j++){
							int iVertex;
							lstream >> iVertex;
							container.back().form[j]=iVertex;	
						}

						bdContainer.emplace_back(container.back().index);
					};

					if (type==3){
						writeBoundaryFacets(lineFacets_, boundaries_.back().lineFacetIndices_);
					}
					if (type == 5){
						writeBoundaryFacets(triangleFacets_, boundaries_.back().triangleFacetIndices_);
					}
					else if(type== 9){
						writeBoundaryFacets(quadrangleFacets_, boundaries_.back().quadrangleFacetIndices_);
					}

				}
			}			
		}
	}

	inFile.close();

	
}




void MMesh::write(std::string filePath, std::string fileFormat){
	
	if (fileFormat=="vtk"
		|| fileFormat=="VTK"){
			writeVTK(filePath);
		}

}


void MMesh::writeVTK(std::string filePath){
    std::ofstream outFile(filePath);
    outFile << "# vtk DataFile Version 2.0\n";
    outFile << "Process mesh VTK file\n";
    outFile << "ASCII\n";
    outFile << "DATASET UNSTRUCTURED_GRID\n";
    outFile << "POINTS" << " " << vertices_.size() << " " << "float\n";
	
	int nVertices = static_cast<int>(vertices_.size());
    for (int i=0; i<nVertices; i++){
        
		outFile << vertices_[i].pt[0] << " "
                << vertices_[i].pt[1] << " "
                << vertices_[i].pt[2] << "\n";
    }


	
	int sum = (4+1) * tetrahedronCells_.size()
			+ (6+1) * prismCells_.size()
			+ (8+1) * hexahedronCells_.size()
			+ (5+1) * pyramidCells_.size();

    outFile << "CELLS " << numCells() << " " << sum << "\n";
	const auto writeCells = 
	[&outFile]
	(auto &container){
    	for (int i=0; i<container.size(); i++) {
        	outFile << container[i].nForms;

        	for (int j=0; j<container[i].nForms; j++){
            	outFile << " " << container[i].form[j];
        	}
        		outFile << "\n";
    	}
	};
	writeCells(tetrahedronCells_);
	writeCells(prismCells_);
	writeCells(hexahedronCells_);
	writeCells(pyramidCells_);
	

    outFile<< "CELL_TYPES " << numCells() <<"\n";
	const auto writeCellTypes = 
				[&outFile]
				(auto &container, int type){
					for (int i=0; i<container.size(); i++){
						outFile << type << "\n";
					}
				};
	writeCellTypes(tetrahedronCells_, 10);
	writeCellTypes(prismCells_, 13);
	writeCellTypes(hexahedronCells_, 11);
	writeCellTypes(pyramidCells_, 14);

	outFile.close();
}