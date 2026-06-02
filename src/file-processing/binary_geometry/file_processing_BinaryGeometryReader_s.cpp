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

std::optional<std::vector<Vertex3>> BinaryGeometryReader::readVertices() {
  std::vector<Vertex3> vertices;
  vertices.resize(_vertexArraySize);

  if (!_vertexFilePointer.read(reinterpret_cast<char *>(vertices.data()),
                               _vertexArraySize * sizeof(Vertex3))) {
    USE_LOGGING_ERROR("Error when reading vertex data.");
    return {};
  }

  USE_LOGGING("Successfully read " << _vertexArraySize << " vertices.");
  USE_LOGGING(vertices.size());
  return vertices;
}

std::optional<std::vector<uint32_t>>
BinaryGeometryReader::readIndexes(uint32_t indexCount) {
  std::vector<uint32_t> indexes;
  indexes.resize(indexCount);

  if (!_vertexFilePointer.read(reinterpret_cast<char *>(indexes.data()),
                               indexCount * sizeof(uint32_t))) {
    USE_LOGGING_ERROR("Error when reading vertex data.");
    return std::nullopt;
  }

  USE_LOGGING("Successfully read " << indexCount << " indexes.");

  return indexes;
}

} // namespace audios