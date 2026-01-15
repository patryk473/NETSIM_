#pragma once
#include <istream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>

#include "package/Package.hpp"

enum class ElementType { LOADING_RAMP, WORKER, STOREHOUSE, LINK };
enum class NodeType { RAMP, WORKER, STOREHOUSE };

struct ParsedLineData {
    ElementType type;
    std::unordered_map<std::string, std::string> params;
};

struct LinkSpec {
    NodeType src_type; int src_id;
    NodeType dest_type; int dest_id;
};

struct FactoryDraft {
    struct RampSpec { int id; int di; };
    struct WorkerSpec { int id; int pt; PackageQueueType qt; };
    struct StoreSpec { int id; };

    std::vector<RampSpec> ramps;
    std::vector<WorkerSpec> workers;
    std::vector<StoreSpec> stores;
    std::vector<LinkSpec> links;
};

struct ParsingError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

ParsedLineData parse_line(const std::string& line);
FactoryDraft load_factory_structure(std::istream& is);
void save_factory_structure(const FactoryDraft& d, std::ostream& os);

std::pair<NodeType,int> parse_node_ref(const std::string& s);
