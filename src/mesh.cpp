#include "mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <unordered_map>


void MMesh::read(std::string filePath, std::string fileFormat){

	// std::string substr = filePath.substr(filePath.find_last_of(".")+1);


	if (fileFormat == "su2" || fileFormat == "SU2"){
		readSU2(filePath);
	}
	else if (fileFormat == "mesh" || fileFormat=="MESH"){
		readMESH(filePath);
	}
	else if (fileFormat == "wrl" || fileFormat=="WRL"){
		readWRL(filePath);
	}
	else
	{
		exit(1);	
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

					if (iType == VTK_TRIANGLE){
						readCell(triangleCells_, 3);
					}
					else if (iType == VTK_QUAD){
						readCell(quadrangleCells_, 4);
					}
					else if (iType == VTK_TETRA){
						readCell(tetrahedronCells_, 4);
					}
					else if (iType == VTK_HEXAHEDRON){
						readCell(hexahedronCells_, 8);
					}
					else if (iType == VTK_WEDGE){
						readCell(prismCells_, 6);
					}
					else if (iType == VTK_PYRAMID){
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
						lStream >> std::setprecision(15) >> coordinate;
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
					const auto readBoundaryFacets = 
					[&lstream, this]
					(auto &container, auto &bdContainer)
					{				
						container.emplace_back();
						container.back().index= container.size()-1;
						container.back().boundaryIndex = boundaries_.size() - 1;	
						for(int j=0; j<container.back().nForms; j++){
							int iVertex;
							lstream >> iVertex;
							container.back().form[j]=iVertex;	
						}

						bdContainer.emplace_back(container.back().index);
					};

					if (type==VTK_LINE){
						readBoundaryFacets(lineFacets_, boundaries_.back().lineFacetIndices_);
					}
					if (type == VTK_TRIANGLE){
						readBoundaryFacets(triangleFacets_, boundaries_.back().triangleFacetIndices_);
					}
					else if(type== VTK_QUAD){
						readBoundaryFacets(quadrangleFacets_, boundaries_.back().quadrangleFacetIndices_);
					}

				}
			}			
		}
	}

	inFile.close();

	
}

void MMesh::readMESH(std::string filePath){


	std::ifstream inFile(filePath);
	if (inFile.is_open()){
		while(inFile){
			std::string line;
			std::string keystring;
			std::getline(inFile, line);
			std::stringstream lineStream(line);
			lineStream >> keystring;

			const auto readCells =
			[&inFile, this]
			(auto &container){
				std::string line;
				std::getline(inFile, line);
				std::stringstream lineStream(line);
				int nCells;
				lineStream >> nCells;
				for (int i=0; i<nCells; i++){
					std::getline(inFile, line);
					lineStream.clear();
					lineStream.str(line);
					container.emplace_back();
					for(int j=0; j<container.back().nForms; j++){
						lineStream >> container.back().form[j];
					}
				}
			};

			// TODO
			const auto readBoundadryFacets = 
			[&inFile, this]
			(auto &container){

			};

			if (keystring == "Dimension"){
				lineStream >> nDim;
			}
			else if (keystring == "Vertices"){
				std::getline(inFile, line);
				lineStream.clear();
				lineStream.str(line);
				int nVertices;
				lineStream >> nVertices;
				for (int i=0; i<nVertices; i++){
					std::getline(inFile, line);
					lineStream.clear();
					lineStream.str(line);
					vertices_.emplace_back();						
					lineStream >> vertices_.back().pt[0] >> vertices_.back().pt[1];
					if (nDim==2){
						vertices_.back().pt[2] = 0;
					}
					else if (nDim==3){
						lineStream >> vertices_.back().pt[2];
					}

					vertices_.back().index = vertices_.size() - 1;

				}
			}
			else if (keystring == "Triangles"){
				
				if (nDim == 2){
					readCells(triangleCells_);
				}
				else{

				}
			}
			else if (keystring == "Tetrahedra")
			{
				readCells(tetrahedronCells_);
			}
			

		}
	}	
}

void MMesh::readWRL(std::string filePath){
	std::ifstream inFile(filePath);
	if (inFile.is_open()){
		while (inFile){
			std::string line;
			std::string keystring;
			std::getline(inFile, line);
			std::stringstream lineStream(line);
			lineStream >> keystring;
			if (keystring == "point"){
				std::getline(inFile, line);
				while(line.find(",") != std::string::npos){
					lineStream.clear();
					lineStream.str(line);
					vertices_.emplace_back();
					lineStream >> vertices_.back().pt[0] >> vertices_.back().pt[1] >> vertices_.back().pt[2]; 
					
					std::getline(inFile, line);
				}
				
			}
			else if (keystring == "coordIndex"){
				std::getline(inFile, line);
				while(line.find(",") != std::string::npos){
					lineStream.clear();
					lineStream.str(line);

					quadrangleCells_.emplace_back();

					auto getNum = 
					[&lineStream]
					( ){
						std::string token;
						lineStream >> token;
						token.substr(0, token.find(","));
						return std::stoi(token.c_str());
					};
					
					
					quadrangleCells_.back().form[0] = getNum();
					quadrangleCells_.back().form[1] = getNum();
					quadrangleCells_.back().form[2] = getNum();
					quadrangleCells_.back().form[3] = getNum();

					
					std::getline(inFile, line);
				}
			}
			
		}
		
	}


}



void MMesh::write(std::string filePath, std::string fileFormat){
	
	if (fileFormat=="vtk"
		|| fileFormat=="VTK"){
			writeVTK(filePath);
	}
	else if (fileFormat=="SU2"
		|| fileFormat == "su2"){
			writeSU2(filePath);
	}
	else if (fileFormat=="mesh"
		|| fileFormat == "MESH"){
			writeMESH(filePath);
	}
	else{
		exit(1);
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


	
	int sum = (3+1) * triangleCells_.size()
			+ (4+1) * quadrangleCells_.size()
			+ (4+1) * tetrahedronCells_.size()
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
	writeCells(triangleCells_);
	writeCells(quadrangleCells_);
	writeCells(tetrahedronCells_);
	writeCells(prismCells_);
	writeCells(hexahedronCells_);
	writeCells(pyramidCells_);
	

    outFile<< "CELL_TYPES " << numCells() <<"\n";
	const auto writeCellTypes = 
				[&outFile]
				(auto &container, VTK_CELL_TYPE type){
					for (int i=0; i<container.size(); i++){
						outFile << type << "\n";
					}
				};
	writeCellTypes(triangleCells_, VTK_TRIANGLE);
	writeCellTypes(quadrangleCells_,VTK_QUAD);
	writeCellTypes(tetrahedronCells_, VTK_TETRA);
	writeCellTypes(prismCells_, VTK_WEDGE);
	writeCellTypes(hexahedronCells_, VTK_HEXAHEDRON);
	writeCellTypes(pyramidCells_, VTK_PYRAMID);

	outFile.close();
}


void MMesh::writeSU2(std::string filePath){
	std::ofstream outFile(filePath);
	outFile << "NDIME= " << nDim <<"\n";
	outFile << "NELEM= " << this->numCells()<<"\n";
	
	/* write elements*/
	int index = 0;
	const auto writeElement =
	[&outFile, &index]
	(auto &container, VTK_CELL_TYPE type)
	{

		for(auto &cell:container){		
			outFile << type;
			for (auto &p: cell.form){
				outFile << "	"<< p ;
			}		
			outFile << "	" << index <<"\n";
			++index;
		}

	};

	if (nDim==2){
		writeElement(triangleCells_, VTK_TRIANGLE);
		writeElement(quadrangleCells_, VTK_QUAD);
	}
	else if (nDim==3){
		writeElement(prismCells_, VTK_WEDGE);
		writeElement(hexahedronCells_, VTK_HEXAHEDRON);
		writeElement(tetrahedronCells_, VTK_TETRA);
		writeElement(pyramidCells_, VTK_PYRAMID);
	}

	/* write point*/
	outFile << "NPOIN= "<< vertices_.size() << "\n";
	index=0;
	for(auto v: vertices_){
		for(int i=0; i<nDim; i++){
			outFile << std::setprecision(15) << v.pt[i] << " ";
		}
		outFile << index <<"\n";
		++index;
	}

	outFile << "NMARK= "<< boundaries_.size() << "\n";

	for(auto &bd:boundaries_){
		outFile << "MARKER_TAG= " << bd.name << "\n";
		outFile << "MARKER_ELEMS= " << bd.lineFacetIndices_.size() + bd.triangleFacetIndices_.size() + bd.quadrangleFacetIndices_.size() << "\n";
		const auto &writeBoundry = 
		[&outFile]
		(auto &container, auto &fcontainer, VTK_CELL_TYPE type){
			for (int i=0; i<container.size(); i++){
				outFile<< type;
				for(auto &p: fcontainer[container[i]].form){
					outFile << " " << p;
				}			
				outFile << "\n";
			}

		};

		writeBoundry(bd.lineFacetIndices_, lineFacets_,VTK_LINE);
		writeBoundry(bd.quadrangleFacetIndices_, quadrangleFacets_, VTK_QUAD);
		writeBoundry(bd.triangleFacetIndices_, quadrangleFacets_, VTK_TRIANGLE); 
	}
}



void MMesh::writeMESH(std::string filePath){
	std::ofstream outFile(filePath);

	outFile << "MeshVersionFormatted 2\n\n\n";
	outFile << "Dimension " << nDim << "\n\n\n";
	
	
	outFile << "Vertices\n" << vertices_.size()<<"\n";
	for (auto &v: vertices_){
		for(int i=0; i<nDim; ++i){
			outFile << v.pt[i] << "	";
		}
		outFile << "1\n";
	}

	const auto &writeElement = 
	[&outFile]
	(auto &container, std::string typeName){
		if (container.empty()){
			return ;	
		}

		outFile << typeName.c_str() << "\n" << container.size()<<"\n";
		for (auto &v: container){
			for(auto &f: v.form){
				outFile << f << "	";
			}
			outFile << "1\n";
		}
	};
	writeElement(triangleCells_, "Triangles");
	writeElement(quadrangleCells_, "Quadrilaterals");
	writeElement(tetrahedronCells_, "Tetrahedra");
	writeElement(hexahedronCells_, "Hexaedra");

	const auto &writeBoundary = 
	[&outFile]
	(auto &container, std::string typeName){
		
		if (container.empty()){
			return;	
		}

		outFile << typeName.c_str() << "\n" << container.size()<<"\n";
		
		for (auto &v: container){
			for(auto &f: v.form){
				outFile << f << "	";
			}
			outFile << v.boundaryIndex << "\n";
		}
	};

	writeBoundary(lineFacets_, "Edges");
	writeBoundary(triangleFacets_, "Triangles");
	writeBoundary(quadrangleFacets_, "Quadrilaterals");

	outFile.close();
}