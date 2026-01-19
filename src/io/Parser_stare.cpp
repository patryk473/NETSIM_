#include "io/Parser.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

static std::string trim(std::string s) {
    auto not_space = [](unsigned char c){ return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    return s;
}

static ElementType tag_to_type(const std::string& tag) {
    if (tag == "LOADING_RAMP") return ElementType::LOADING_RAMP;
    if (tag == "WORKER") return ElementType::WORKER;
    if (tag == "STOREHOUSE") return ElementType::STOREHOUSE;
    if (tag == "LINK") return ElementType::LINK;
    throw ParsingError("Unknown tag: " + tag);
}

static PackageQueueType parse_qt(const std::string& s) {
    if (s == "FIFO") return PackageQueueType::FIFO;
    if (s == "LIFO") return PackageQueueType::LIFO;
    throw ParsingError("Invalid queue-type: " + s);
}

static NodeType parse_nt(const std::string& s) {
    if (s == "ramp") return NodeType::RAMP;
    if (s == "worker") return NodeType::WORKER;
    if (s == "storehouse" || s == "store") return NodeType::STOREHOUSE;
    throw ParsingError("Invalid node type: " + s);
}

std::pair<NodeType,int> parse_node_ref(const std::string& s) {
    auto pos = s.find('-');
    if (pos == std::string::npos) throw ParsingError("Invalid node ref: " + s);
    NodeType t = parse_nt(s.substr(0, pos));
    int id = std::stoi(s.substr(pos + 1));
    return {t, id};
}

ParsedLineData parse_line(const std::string& raw) {
    std::string line = trim(raw);
    if (line.empty()) throw ParsingError("Empty line");

    std::istringstream iss(line);
    std::string tag;
    iss >> tag;

    ParsedLineData out;
    out.type = tag_to_type(tag);

    std::string token;
    while (iss >> token) {
        auto pos = token.find('=');
        if (pos == std::string::npos) throw ParsingError("Bad token: " + token);
        out.params[token.substr(0, pos)] = token.substr(pos + 1);
    }
    return out;
}

FactoryDraft load_factory_structure(std::istream& is) {
    FactoryDraft d;
    std::string line;
    int ln = 0;

    while (std::getline(is, line)) {
        ln++;
        std::string t = trim(line);
        if (t.empty() || t[0] == ';') continue;

        ParsedLineData p;
        try { p = parse_line(t); }
        catch (const ParsingError& e) { throw ParsingError("Line " + std::to_string(ln) + ": " + e.what()); }

        try {
            switch (p.type) {
                case ElementType::LOADING_RAMP: {
                    int id = std::stoi(p.params.at("id"));
                    int di = std::stoi(p.params.at("delivery-interval"));
                    d.ramps.push_back({id, di});
                } break;
                case ElementType::WORKER: {
                    int id = std::stoi(p.params.at("id"));
                    int pt = std::stoi(p.params.at("processing-time"));
                    auto qt = parse_qt(p.params.at("queue-type"));
                    d.workers.push_back({id, pt, qt});
                } break;
                case ElementType::STOREHOUSE: {
                    int id = std::stoi(p.params.at("id"));
                    d.stores.push_back({id});
                } break;
                case ElementType::LINK: {
                    auto [st, sid] = parse_node_ref(p.params.at("src"));
                    auto [dt, did] = parse_node_ref(p.params.at("dest"));
                    d.links.push_back({st, sid, dt, did});
                } break;
            }
        } catch (const std::out_of_range&) {
            throw ParsingError("Line " + std::to_string(ln) + ": missing key");
        }
    }
    return d;
}

void save_factory_structure(const FactoryDraft& d, std::ostream& os) {
    for (auto& r : d.ramps)
        os << "LOADING_RAMP id=" << r.id << " delivery-interval=" << r.di << "\n";
    for (auto& w : d.workers)
        os << "WORKER id=" << w.id << " processing-time=" << w.pt
           << " queue-type=" << (w.qt == PackageQueueType::FIFO ? "FIFO" : "LIFO") << "\n";
    for (auto& s : d.stores)
        os << "STOREHOUSE id=" << s.id << "\n";
    for (auto& l : d.links) {
        auto nt = [](NodeType t){
            if (t == NodeType::RAMP) return "ramp";
            if (t == NodeType::WORKER) return "worker";
            return "storehouse";
        };
        os << "LINK src=" << nt(l.src_type) << "-" << l.src_id
           << " dest=" << nt(l.dest_type) << "-" << l.dest_id << "\n";
    }
}
