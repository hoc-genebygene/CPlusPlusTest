#include <cstdlib>
#include <iostream>
#include <vector>

#include <boost/container/flat_map.hpp>

using multimap = boost::container::flat_multimap<int *, int>;

int main(int argc, char **argv)
{
    multimap map;
    std::vector<std::pair<int *, int>> key_value_pairs;

    for (int k = 0; k < 2; k++) {
        int * new_int = new int;
        map.emplace(new_int, k);
        key_value_pairs.emplace_back(new_int, k);
    }

    for (auto it = map.begin(); it != map.end();) {
        if (it->first == key_value_pairs[0].first) {
            it = map.erase(it);
        } else {
            ++it;
        }
    }

    // Should only be one map element left (key_value_pairs[1])
    auto it = map.find(key_value_pairs[1].first);
    if (it == map.end()) {
        throw std::logic_error("Could not find key");
    }

    std::cout  << "Success!" << std::endl;
	return EXIT_SUCCESS;
}
