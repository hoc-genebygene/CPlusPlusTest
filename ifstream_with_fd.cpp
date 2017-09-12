#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{
    ofstream ofs("test.txt");
    ofs << "Writing to a basic_ofstream object..." << endl;
    ofs.close();


    FILE * file_handle = ::fopen("test.txt", "r");

    string line;
    getline(is, line);
    cout << "line: " << line << std::endl;
    return 0;
}
