#pragma once

#include <list>
#include <algorithm>
#include <utility>

template <typename Node>
class NodeCollection {
public:
   
    using container_t = std::list<Node>;

    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    
    iterator begin() { return nodes_.begin(); }
    iterator end() { return nodes_.end(); }

    const_iterator begin() const { return nodes_.begin(); }
    const_iterator end() const { return nodes_.end(); }

    const_iterator cbegin() const { return nodes_.cbegin(); }
    const_iterator cend() const { return nodes_.cend(); }

    void add(Node&& node) {
        nodes_.push_back(std::move(node));   
    }

    iterator find_by_id(ElementID id) {
        return std::find_if(nodes_.begin(), nodes_.end(),
                            [id](const Node& n) { return n.get_id() == id; });
    }

    const_iterator find_by_id(ElementID id) const {
        return std::find_if(nodes_.cbegin(), nodes_.cend(),
                            [id](const Node& n) { return n.get_id() == id; });
    }

    void remove_by_id(ElementID id) {
        auto it = find_by_id(id);
        if (it != nodes_.end()) {
            nodes_.erase(it);
        }
    }

private:
    container_t nodes_;
};
