#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <tuple>
#include <queue>
#include <functional>
#include <math.h>
#include <fstream>
#include <iostream>
#include <random>
#include <iomanip>

using namespace std;

const string INPUT_FILE = "in.txt";
constexpr int FLOAT_MIN = -100;
constexpr int FLOAT_MAX = 100;
constexpr int PRECISION = 10;


void writePts(int n){

    fstream ifile;
    random_device rd;
    default_random_engine eng(rd());
    uniform_real_distribution<> distr(FLOAT_MIN, FLOAT_MAX);
    setprecision(PRECISION);

    ifile.open(INPUT_FILE,fstream::out);

    for (int i = 0; i < n; i++)
        ifile << distr(eng) << "," << distr(eng) << endl;

    ifile.close();
}

int main(int argc, const char *argv[]) {

    int n = argc >= 2 ? atoi(argv[1]) : 10;
    writePts(n);

}
