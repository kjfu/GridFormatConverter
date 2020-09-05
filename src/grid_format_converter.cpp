#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

class Node
{
public:
	Node(double x, double y, double z = 0)
	{
		coordinates[0] = x;
		coordinates[1] = y;
		coordinates[2] = z;
	}
	double coordinates[3];
};

class Line
{
public:
	Line(int i0, int i1)
	{
		nodeIndices[0] = i0;
		nodeIndices[1] = i1;
	}
	int nodeIndices[2];
	int tag;
};

class Triangle
{
public:
	Triangle(int i0, int i1, int i2)
	{
		nodeIndices[0] = i0;
		nodeIndices[1] = i1;
		nodeIndices[2] = i2;
	}
	int nodeIndices[3];
};

class Quadrangle
{
public:
	Quadrangle(int i1, int i2, int i3, int i4)
	{
		nodeIndices[0] = i1;
		nodeIndices[1] = i2;
		nodeIndices[2] = i3;
		nodeIndices[3] = i4;
	}
	int nodeIndices[4];
};



void readMSH();
void readSU2();

void msh2dat(std::string inFileName, std::string outFileName);
void su22vtk();




int main(int argc, char *argv[])
{

	std::string inFileName;
	std::string outFileName;

	inFileName = argv[1];
	outFileName = "testmoamgen.dat";
	for (int i = 1; i < argc; i++)
	{
		std::string str = argv[i];
		if (str == "-o")
		{
			outFileName = argv[i + 1];
			break;
		}
	}

	msh2dat(inFileName, outFileName);
	return 0;
}



void msh2dat(std::string inFileName, std::string outFileName){
	std::ifstream inFile(inFileName);
	std::ofstream outFile(outFileName);

	std::vector<Node> sNodes;
	std::vector<Triangle> sTriangles;
	std::vector<Quadrangle> sQuadrangles;
	std::vector<Line> sBoundaries;
	std::map<int, int> tagMap;
	Node testNode(1, 1);

	if (inFile.is_open())
	{
		while (inFile)
		{
			std::string keyString;
			std::string line;
			std::getline(inFile, line);
			std::stringstream lineStream(line);
			lineStream >> keyString;
			if (keyString == "$Nodes")
			{
				int nVertices;
				std::getline(inFile, line);
				std::stringstream tmpStream(line);
				tmpStream >> nVertices;
				for (int i = 0; i < nVertices; i++)
				{
					std::getline(inFile, line);
					std::stringstream nodeLine(line);
					int id;
					double x, y;
					nodeLine >> id >> x >> y;
					Node aNode(x, y);
					sNodes.push_back(aNode);
				}
			}
			else if (keyString == "$Elements")
			{
				int nElements;
				std::getline(inFile, line);
				std::stringstream tmpStream(line);
				tmpStream >> nElements;
				for (int i = 0; i < nElements; i++)
				{
					std::getline(inFile, line);
					std::stringstream elementLine(line);
					int id, type, numTags, physGrp, elemGrp, id0, id1, id2, id3;
					elementLine >> id >> type;
					if (type == 1)
					{
						elementLine >> numTags >> physGrp >> elemGrp >> id0 >> id1;
						Line aEdge(id0, id1);
						aEdge.tag = physGrp;
						sBoundaries.push_back(aEdge);
					}
					else if (type == 2)
					{
						elementLine >> numTags >> physGrp >> elemGrp >> id0 >> id1 >> id2;
						Triangle aTriangle(id0, id1, id2);
						sTriangles.push_back(aTriangle);
					}
					else if (type == 3)
					{
						elementLine >> numTags >> physGrp >> elemGrp >> id0 >> id1 >> id2 >> id3;
						Quadrangle aQuadrangle(id0, id1, id2, id3);
						sQuadrangles.push_back(aQuadrangle);
					}
				}
			}
			else if (keyString == "$PhysicalNames")
			{
				int nPhysicalGroups;
				std::getline(inFile, line);
				std::stringstream tmpStream(line);
				tmpStream >> nPhysicalGroups;
				for (int i = 0; i < nPhysicalGroups; i++)
				{
					std::getline(inFile, line);
					std::stringstream groupLine(line);
					int type, groupId;
					std::string name;
					groupLine >> type >> groupId >> name;

					// std::cout << name.c_str() << std::endl;

					if (name == "\"1\"")
					{
						tagMap[groupId] = 1;
					}
					else if (name == "\"3\"")
					{
						tagMap[groupId] = 3;
					}
					else if (name == "\"4\"")
					{
						tagMap[groupId] = 4;
					}
				}
			}
		}
	}

	outFile << "number of nodes:\n           " << sNodes.size() << "\n";
	outFile << "number of triangles:\n         " << sTriangles.size() << "\n";
	outFile << "number of quadrangle:\n         "<< sQuadrangles.size() <<"\n";
	outFile << "number of tetrahedrons:\n        0\n";
	outFile << "number of pyramids:\n            0\n";

	outFile << "number of prisms:\n            0\n";
	outFile << "number of hexahedrons:\n            0\n";

	outFile << "number of boundary lines:\n"
			<< sBoundaries.size() << "\n";

	outFile << "number of triangular boundaries:\n            0\n";
	outFile << "number of quadrangled boundaries:\n            0\n";

	for (int i = 0; i < static_cast<int>(sNodes.size()); i++)
	{
		outFile << "   " << i + 1 << " " << sNodes[i].coordinates[0] << " " << sNodes[i].coordinates[1] << "\n";
	}

	for (int i = 0; i < static_cast<int>(sTriangles.size()); i++)
	{
		outFile << "        " << i + 1 << " " << sTriangles[i].nodeIndices[0]
				<< " " << sTriangles[i].nodeIndices[1]
				<< " " << sTriangles[i].nodeIndices[2] << "\n";
	}

	for (int i = 0; i < static_cast<int>(sQuadrangles.size()); i++)
	{
		outFile << "        " << i + 1 << " " << sQuadrangles[i].nodeIndices[0]
				<< " " << sQuadrangles[i].nodeIndices[1]
				<< " " << sQuadrangles[i].nodeIndices[2] 
				<< " " << sQuadrangles[i].nodeIndices[3] << "\n";
	}	


	for (int i = 0; i < static_cast<int>(sBoundaries.size()); i++)
	{
		outFile << "           " << i + 1 << "  " << sBoundaries[i].nodeIndices[0]
				<< "  " << sBoundaries[i].nodeIndices[1]
				<< "  " << tagMap[sBoundaries[i].tag] << "\n";
	}

	inFile.close();
	outFile.close();
}