#include "MeshConsolidator.hpp"
using namespace glm;
using namespace std;

#include "cs488-framework/Exception.hpp"
#include "cs488-framework/ObjFileDecoder.hpp"


//----------------------------------------------------------------------------------------
// Default constructor
MeshConsolidator::MeshConsolidator()
{

}

//----------------------------------------------------------------------------------------
// Destructor
MeshConsolidator::~MeshConsolidator()
{

}

//----------------------------------------------------------------------------------------
template <typename T>
static void appendVector (
		std::vector<T> & dest,
		const std::vector<T> & source
) {
	// Increase capacity to hold source.size() more elements
	dest.reserve(dest.size() + source.size());

	dest.insert(dest.end(), source.begin(), source.end());
}


//----------------------------------------------------------------------------------------
MeshConsolidator::MeshConsolidator(
	int count, char **objFileList
) {

	MeshId meshId;
	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> uvs;
	BatchInfo batchInfo;
	unsigned long indexOffset(0);

    for(int i = 0; i < count; ++i) {
	    ObjFileDecoder::decode(objFileList[i], meshId, positions, normals, uvs);

	    uint numIndices = positions.size();

	    if (numIndices != normals.size()) {
		    throw Exception("Error within MeshConsolidator: "
					"positions.size() != normals.size()\n");
	    }

	    batchInfo.startIndex = indexOffset;
	    batchInfo.numIndices = numIndices;

	    m_batchInfoMap[meshId] = batchInfo;

	    appendVector(m_vertexPositionData, positions);
	    appendVector(m_vertexNormalData, normals);
	    appendVector(m_vertexUVData, uvs);

	    indexOffset += numIndices;
		std::cerr << "Object " << objFileList[i] << " v: " << positions.size() << " vn: " << normals.size() << " vt: " << uvs.size() << std::endl;
    }

}

//----------------------------------------------------------------------------------------
void MeshConsolidator::getBatchInfoMap (
		BatchInfoMap & batchInfoMap
) const {
	batchInfoMap = m_batchInfoMap;
}

//----------------------------------------------------------------------------------------
// Returns the starting memory location for vertex position data.
const float * MeshConsolidator::getVertexPositionDataPtr() const {
	return &(m_vertexPositionData[0].x);
}

//----------------------------------------------------------------------------------------
// Returns the starting memory location for vertex normal data.
const float * MeshConsolidator::getVertexNormalDataPtr() const {
    return &(m_vertexNormalData[0].x);
}

//----------------------------------------------------------------------------------------
// Returns the starting memory location for vertex UV data.
const float * MeshConsolidator::getVertexUVDataPtr() const {
    return &(m_vertexUVData[0].x);
}

//----------------------------------------------------------------------------------------
// Returns the total number of bytes of all vertex position data.
size_t MeshConsolidator::getNumVertexPositionBytes() const {
	return m_vertexPositionData.size() * sizeof(vec3);
}

//----------------------------------------------------------------------------------------
// Returns the total number of bytes of all vertex normal data.
size_t MeshConsolidator::getNumVertexNormalBytes() const {
	return m_vertexNormalData.size() * sizeof(vec3);
}

//----------------------------------------------------------------------------------------
// Returns the total number of bytes of all vertex UV data.
size_t MeshConsolidator::getNumVertexUVBytes() const {
	return m_vertexUVData.size() * sizeof(vec2);
}

