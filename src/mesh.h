#pragma once 

#include <string>
#include <vector>



struct MVertex{
	double pt[3];
	int index;
};

template <int t_nForms>
struct MFacet{
	int form[t_nForms];
	int index;
	int parent;
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

void writeVTK(std::string filePath);

int numCells() const{
	return static_cast<int>(tetrahedronCells_.size() 
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
std::vector<MTetrahedronCell> tetrahedronCells_;
std::vector<MHexahedronCell> hexahedronCells_;
std::vector<MPrismCell> prismCells_;
std::vector<MPyramidCell> pyramidCells_;

/*boundarys*/
std::vector<MBoundaryCondition> boundaries_;

};








