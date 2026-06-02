#include "include/audios/file_processing.h"
#include "private_macros.h"
#include <ios>
#include <optional>

namespace audios {

BinaryGeometryReader::BinaryGeometryReader() = default;
BinaryGeometryReader::~BinaryGeometryReader() {
  if (_vertexFilePointer.is_open())
    _vertexFilePointer.close();
  if (_indexFilePointer.is_open())
    _indexFilePointer.close();
}

bool BinaryGeometryReader::openFile(std::string filePath) {
  _filePath = filePath;

  std::ifstream sizeReader(filePath, std::ios::binary & std::ios::in);
  if (!sizeReader) {
    USE_LOGGING_ERROR("Couldn't open file: " << filePath
                                             << " for binary reading.");
    return false;
  }

  _vertexFilePointer.open(filePath, std::ios::binary & std::ios::in);
  _indexFilePointer.open(filePath, std::ios::binary & std::ios::in);
  if (!_vertexFilePointer || !_indexFilePointer) {
    USE_LOGGING_ERROR("Couldn't open file: " << filePath
                                             << " for binary reading.");
    return false;
  }

  if (!sizeReader.read(reinterpret_cast<char *>(&_vertexArraySize),
                       sizeof(int64_t))) {
    USE_LOGGING_ERROR("Error reading sizes from the binary file");
    _vertexArraySize = 0;
    return false;
  }

  if (!sizeReader.read(reinterpret_cast<char *>(&_indexArraySize),
                       sizeof(int64_t))) {
    USE_LOGGING_ERROR("Error reading sizes from the binary file");
    _vertexArraySize = 0;
    return false;
  }

  _vertexFilePointer.seekg(sizeof(int64_t) * 2);
  _indexFilePointer.seekg(sizeof(int64_t) * 2 +
                          sizeof(float) * 3 * _vertexArraySize);

  USE_LOGGING("Successfully opened file " << filePath);

  return true;
}

void BinaryGeometryReader::resetReader() {
  _vertexFilePointer.seekg(sizeof(int64_t) * 2);
  _indexFilePointer.seekg(sizeof(int64_t) * 2 +
                          sizeof(float) * 3 * _vertexArraySize);

  USE_LOGGING("Reset binary geometry reader.");
}

int64_t BinaryGeometryReader::getVertexCount() { return _vertexArraySize; }
int64_t BinaryGeometryReader::getIndexCount() { return _indexArraySize; }
int64_t BinaryGeometryReader::getIndexPairCount() {
  return _indexArraySize / 3;
}

std::optional<std::vector<Vector3>> BinaryGeometryReader::readVertices() {
  std::vector<float> floatBuffer;
  floatBuffer.resize(_vertexArraySize * 3);

  if (!_vertexFilePointer.read(reinterpret_cast<char *>(floatBuffer.data()),
                               floatBuffer.size() * sizeof(float))) {
    USE_LOGGING_ERROR("Error when reading vertex data.");
    return {};
  }

  uint32_t floatCount = floatBuffer.size();
  std::vector<Vector3> vertices;
  vertices.resize(_vertexArraySize);

  for (uint32_t i = 0; i < floatCount / 3; i += 1) {
    vertices[i].x = floatBuffer[i * 3];
    vertices[i].y = floatBuffer[i * 3 + 1];
    vertices[i].z = floatBuffer[i * 3 + 2];
    vertices[i].pad = 0.0f;
  }

  USE_LOGGING("Successfully read " << vertices.size()
                                   << " vertices (size of buffer: "
                                   << vertices.size() * 4 << " floats).");
  return vertices;
}

std::optional<std::vector<uint32_t>>
BinaryGeometryReader::readIndexes(uint32_t indexPairCount) {
  std::vector<uint32_t> indexBuffer;
  indexBuffer.resize(indexPairCount * 3);

  if (!_vertexFilePointer.read(reinterpret_cast<char *>(indexBuffer.data()),
                               indexBuffer.size() * sizeof(uint32_t))) {
    USE_LOGGING_ERROR("Error when reading vertex data.");
    return std::nullopt;
  }

  std::vector<uint32_t> indexes(indexPairCount * 4);
  for (uint32_t i = 0; i < indexPairCount; i += 1) {
    indexes[i * 4] = indexBuffer[i * 3];
    indexes[i * 4 + 1] = indexBuffer[i * 3 + 1];
    indexes[i * 4 + 2] = indexBuffer[i * 3 + 2];
    indexes[i * 4 + 3] = 0;
  }

  USE_LOGGING("Successfully read " << indexes.size() / 4 * 3
                                   << " indexes (real buffer size "
                                   << indexes.size() << ").");

  return indexes;
}

} // namespace audios