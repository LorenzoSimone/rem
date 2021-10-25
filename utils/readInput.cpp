#include <string>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

vector<pair<float, float>> readFile(string path) {

    vector<pair<float, float>> dict;
    ifstream ifile;
    ifile.open(path);

    string line;
    int i = 0;

    while (getline(ifile, line))
    {
        stringstream linestream(line);
        string data;
        float x, y;

        getline(linestream, data);

        x = stof(data.substr(0, data.find(",")));
        y = stof(data.substr(data.find(",") + 1, data.length()));


        dict.push_back(make_pair(x, y));
        i++;
    }
    ifile.close();
    return dict;

}