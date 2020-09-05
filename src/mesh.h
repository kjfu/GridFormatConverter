#pragma once 

#include <string>
#include <vector>

enum VTK_CELL_TYPE {
    VTK_VERTEX = 1,
    VTK_POLY_VERTEX = 2,
    VTK_LINE = 3,
    VTK_POLY_LINE = 4,
    VTK_TRIANGLE = 5,
    VTK_TRIANGLE_STRIP = 6,
    VTK_POLYGON = 7,
    VTK_PIXEL = 8,
    VTK_QUAD = 9,
    VTK_TETRA = 10,
    VTK_VOXEL = 11,
    VTK_HEXAHEDRON = 12,
    VTK_WEDGE = 13,
    VTK_PYRAMID = 14,
    VTK_QUADRATIC_EDGE = 21,
    VTK_QUADRATIC_TRIANGLE = 22,
    VTK_QUADRATIC_QUAD = 23,
    VTK_QUADRATIC_TETRA = 24,
    VTK_QUADRATIC_HEXAHEDRON = 25,
    IGNORED = -1,
};


struct MVertex{
	double pt[3];
	int index;
};

template <int t_nForms>
struct MFacet{
	int form[t_nForms];
	int index;
	int parent;
	int boundaryIndex;
	static constexpr int nForms = t_nForms;
};

struct MLineFacet: MFacet<2>{};
struct MTriangleFacet: MFacet<3>{};
struct MQuadrangleFacet: MFacet<4>{};


template <int t_nForms>
struct MCell{
	int form[t_nForms];
	int index;
	static constexpr int nForms = t_nForms;
};

struct MTriangleCell: MCell<3>{};
struct MQuadrangleCell: MCell<4>{};
struct MTetrahedronCell: MCell<4>{};
struct MHexahedronCell: MCell<8>{};
struct MPyramidCell: MCell<5>{};
struct MPrismCell: MCell<6>{};


struct MBoundaryCondition{
	std::string name;
	
	std::vector<int> lineFacetIndices_;
	std::vector<int> triangleFacetIndices_;
	std::vector<int> quadrangleFacetIndices_;

};




class MMesh{

public:

	void read(std::string filePath, std::string fileFormat);
	void write(std::string filePath, std::string fileFormat);

private:


void readSU2(std::string filePath);
void readMESH(std::string filePath);
void readWRL(std::string filePath);

void writeVTK(std::string filePath);
void writeSU2(std::string filePath);
void writeMESH(std::string filePath);


int numCells() const{
	return static_cast<int>(triangleCells_.size()
							+ quadrangleCells_.size()
							+ tetrahedronCells_.size() 
							+ hexahedronCells_.size() 
							+ prismCells_.size() 
							+ pyramidCells_.size());
}



int nDim=3;
int nBoundry=0;
/*vertices*/
std::vector<MVertex> vertices_;


/*facets*/
std::vector<MLineFacet> lineFacets_;
std::vector<MTriangleFacet> triangleFacets_;
std::vector<MQuadrangleFacet> quadrangleFacets_;

/*cells*/
std::vector<MTriangleCell> triangleCells_;
std::vector<MQuadrangleCell> quadrangleCells_;
std::vector<MTetrahedronCell> tetrahedronCells_;
std::vector<MHexahedronCell> hexahedronCells_;
std::vector<MPrismCell> prismCells_;
std::vector<MPyramidCell> pyramidCells_;

/*boundarys*/
std::vector<MBoundaryCondition> boundaries_;

};








