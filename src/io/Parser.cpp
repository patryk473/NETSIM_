#include "Parser.hpp"

#include <sstream>
#include <vector>
#include <stdexcept>

// =======================================================
// Funkcje pomocnicze
// =======================================================

// Dzieli linię na tokeny oddzielone spacjami
static std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// =======================================================
// Parsowanie jednej linii
// =======================================================

ParsedLineData IO::parse_line(const std::string& line) {
    auto tokens = split(line);
    if (tokens.empty()) {
        throw std::logic_error("Empty line");
    }

    ParsedLineData result;

    // Rozpoznanie typu elementu
    if (tokens[0] == "RAMP") {
        result.type = ElementType::RAMP;
    } else if (tokens[0] == "WORKER") {
        result.type = ElementType::WORKER;
    } else if (tokens[0] == "STOREHOUSE") {
        result.type = ElementType::STOREHOUSE;
    } else if (tokens[0] == "LINK") {
        result.type = ElementType::LINK;
    } else {
        throw std::logic_error("Unknown element type");
    }

    // Parsowanie par klucz=wartość
    for (size_t i = 1; i < tokens.size(); ++i) {
        auto pos = tokens[i].find('=');
        if (pos == std::string::npos) {
            throw std::logic_error("Invalid parameter format");
        }

        std::string key = tokens[i].substr(0, pos);
        std::string value = tokens[i].substr(pos + 1);

        result.parameters[key] = value;
    }

    return result;
}

// =======================================================
// Wczytywanie struktury fabryki
// =======================================================

Factory IO::load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;

    while (std::getline(is, line)) {
        // Pomijamy puste linie i komentarze
        if (line.empty() || line[0] == '#') {
            continue;
        }

        ParsedLineData data = parse_line(line);

        // ---------------------------------------------------
        // RAMP
        // ---------------------------------------------------
        if (data.type == ElementType::RAMP) {
            ElementID id = std::stoi(data.parameters.at("id"));
            TimeOffset di = std::stoi(data.parameters.at("delivery-interval"));

            factory.add_ramp(Ramp(id, di));
        }

        // ---------------------------------------------------
        // WORKER
        // ---------------------------------------------------
        else if (data.type == ElementType::WORKER) {
            ElementID id = std::stoi(data.parameters.at("id"));
            TimeOffset pd = std::stoi(data.parameters.at("processing-time"));

            std::string q = data.parameters.at("queue-type");
            PackageQueueType qt =
                (q == "FIFO") ? PackageQueueType::FIFO : PackageQueueType::LIFO;

            factory.add_worker(
                Worker(id, pd,
                    std::unique_ptr<IPackageQueue>(
                        new PackageQueue(qt)))
            );
        }

        // ---------------------------------------------------
        // STOREHOUSE
        // ---------------------------------------------------
        else if (data.type == ElementType::STOREHOUSE) {
            ElementID id = std::stoi(data.parameters.at("id"));

            factory.add_storehouse(Storehouse(id));
        }

        // ---------------------------------------------------
        // LINK
        // ---------------------------------------------------
        else if (data.type == ElementType::LINK) {
            ElementID src = std::stoi(data.parameters.at("src"));
            ElementID dest = std::stoi(data.parameters.at("dest"));

            // Szukamy nadawcy
            PackageSender* sender = nullptr;

            for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
                if (it->get_id() == src) {
                    sender = const_cast<Ramp*>(&(*it));
                    break;
                }
            }
            for (auto it = factory.worker_cbegin(); it != factory.worker_cend() && !sender; ++it) {
                if (it->get_id() == src) {
                    sender = const_cast<Worker*>(&(*it));
                    break;
                }
            }

            if (!sender) {
                throw std::logic_error("Sender not found");
            }

            // Szukamy odbiorcy
            IPackageReceiver* receiver = nullptr;

            for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
                if (it->get_id() == dest) {
                    receiver = const_cast<Worker*>(&(*it));
                    break;
                }
            }
            for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend() && !receiver; ++it) {
                if (it->get_id() == dest) {
                    receiver = const_cast<Storehouse*>(&(*it));
                    break;
                }
            }

            if (!receiver) {
                throw std::logic_error("Receiver not found");
            }

            sender->receiver_preferences.add_receiver(receiver);
        }
    }

    return factory;
}

// =======================================================
// Zapis struktury fabryki
// =======================================================

void IO::save_factory_structure(const Factory& factory, std::ostream& os) {

    // RAMP
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        os << "RAMP id=" << it->get_id()
           << " delivery-interval=" << it->get_delivery_interval()
           << "\n";
    }

    // WORKER
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        os << "WORKER id=" << it->get_id()
           << " processing-time=" << it->get_processing_duration()
           << " queue-type="
           << (it->get_queue()->getQueueType() == PackageQueueType::FIFO ? "FIFO" : "LIFO")
           << "\n";
    }

    // STOREHOUSE
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        os << "STOREHOUSE id=" << it->get_id() << "\n";
    }

    // LINK
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        for (auto pref = it->receiver_preferences.begin();
             pref != it->receiver_preferences.end(); ++pref) {

            os << "LINK src=" << it->get_id()
               << " dest=" << pref->first->get_id()
               << "\n";
        }
    }

    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        for (auto pref = it->receiver_preferences.begin();
             pref != it->receiver_preferences.end(); ++pref) {

            os << "LINK src=" << it->get_id()
               << " dest=" << pref->first->get_id()
               << "\n";
        }
    }
}
