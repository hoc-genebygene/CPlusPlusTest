#include <iostream>
#include <string>
#include <experimental/string_view>

int main() {
//    std::string short_string = "abc";
    std::string short_string = "suoicodilaipxecitsiligarfilacrepus";
    std::string long_string = "supercalifragilisticexpialidocious";

    std::cout << "short_string size: " << short_string.size() << std::endl;
    std::cout << "long_string size: " << long_string.size() << std::endl;
    std::cout << std::endl;

    std::experimental::string_view short_string_view(short_string);
    std::experimental::string_view long_string_view(long_string);

    std::cout << "short_string_view: " << short_string_view << " size : " << short_string_view.size() << " addr: " << (void*)std::addressof((*short_string_view.begin())) << std::endl;
    std::cout << "long_string_view: " << long_string_view << " size: "  << long_string_view.size() << " addr: " << (void*)std::addressof((*long_string_view.begin())) << std::endl;
    std::cout << std::endl;

    std::cout << "After moving: short_string = std::move(long_string);" << std::endl << std::endl;
    short_string = std::move(long_string);

    std::cout << "short_string: " << short_string << " size: " << short_string.size() << " addr: " << (void*)std::addressof((*short_string.begin())) << std::endl;
    std::cout << "long_string: " << long_string << " size: "  << long_string.size() << " addr: " << (void*)std::addressof((*long_string.begin())) << std::endl;
    std::cout << std::endl;

    std::cout << "short_string_view: " << short_string_view << " size: " << short_string_view.size() << " addr: " << (void*)std::addressof((*short_string_view.begin())) << std::endl;
    std::cout << "long_string_view: " << long_string_view << " size: " << long_string_view.size() << " addr: " << (void*)std::addressof((*long_string_view.begin())) << std::endl;
    std::cout << std::endl;
}
