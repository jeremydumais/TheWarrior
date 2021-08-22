#include "gameMapStorage.hpp"
#include "binaryFileStream.hpp"
#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <stdexcept>

using namespace std;
using namespace boost::algorithm;

void GameMapStorage::loadMap(const string &fileName, shared_ptr<GameMap> map) 
{
    if (trim_copy(fileName).empty()) {
        throw invalid_argument("The filename cannot be empty!");
    }
    if (map == nullptr) {
        throw invalid_argument("The map cannot be null!");
    }
    if (bfs == nullptr) {
        bfs = make_unique<BinaryFileStream<GameMap>>(fileName);
    }
	if (!bfs->open()) {
		throw runtime_error(fmt::format("Unable to open the map {0}", fileName));
	}
	if (!bfs->readAllInto(*map)) {
		throw runtime_error(fmt::format("Unable to read the content of the map {0}", fileName));
	}
	if (!bfs->close()) {
		throw runtime_error(fmt::format("Unable to close the map file {0}", fileName));
	}
}

void GameMapStorage::setFileStream(std::unique_ptr<IBinaryFileStream<GameMap>> bfs) 
{
    this->bfs = move(bfs);
}
