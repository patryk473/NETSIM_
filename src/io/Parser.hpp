#pragma once
#include <istream>
#include <ostream>
#include <string>
#include <map>

#include "Factory/factory.hpp"


enum class ElementType { RAMP, WORKER, STOREHOUSE, LINK };

struct ParsedLineData {
    ElementType type;
    std::map<std::string, std::string> parameters;
};
namespace IO {

    // Parsuje pojedynczą linię tekstu
    ParsedLineData parse_line(const std::string& line);

    // Wczytuje strukturę fabryki
    Factory load_factory_structure(std::istream& is);

    // Zapisuje strukturę fabryki
    void save_factory_structure(const Factory& factory, std::ostream& os);
}

// struct LinkSpec {
//     NodeType src_type; int src_id;
//     NodeType dest_type; int dest_id;
// };

// struct FactoryDraft {
//     struct RampSpec { int id; int di; };
//     struct WorkerSpec { int id; int pt; PackageQueueType qt; };
//     struct StoreSpec { int id; };

//     std::vector<RampSpec> ramps;
//     std::vector<WorkerSpec> workers;
//     std::vector<StoreSpec> stores;
//     std::vector<LinkSpec> links;
// };

// struct ParsingError : std::runtime_error {
//     using std::runtime_error::runtime_error;
// };

// ParsedLineData parse_line(const std::string& line);
// FactoryDraft load_factory_structure(std::istream& is);
// void save_factory_structure(const FactoryDraft& d, std::ostream& os);

// std::pair<NodeType,int> parse_node_ref(const std::string& s);
