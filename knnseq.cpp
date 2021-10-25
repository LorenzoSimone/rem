#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <thread>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <queue>
#include "../utimer.cpp"
#include "../utils/readInput.cpp"


using namespace std;

typedef pair<float, pair<int, int>> pairfii;
const string IN_FILE = "in.txt";
const int none = 99999;
vector<int> workers = { 1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64, 128, 256};

float distance(pair<float, float> x1, pair<float, float> x2) {
    return pow(x2.first - x1.first, 2) + pow(x2.second - x1.second, 2) * 1.0;
}

int main(int argc, const char *argv[]) {

    if (argc < 2) {
        cerr << "Use: k | nw" << endl;
        return -1;
    }

    int k = atoi(argv[1]);

    vector<pair<float, float>> dict = readFile(IN_FILE);

    int n = dict.size();

    vector <  priority_queue<pairfii> > seq_pq(n, priority_queue <pairfii>());

    long seq;
    {
        utimer ts("Stage Full seq", &seq);

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                float dis = distance(dict[i], dict[j]);
                if (j < k) {
                    seq_pq[i].push(make_pair(dis, make_pair(i, j)));
                }
                else {
                    if (i != j && (dis < seq_pq[i].top().first)) {
                        seq_pq[i].pop();
                        seq_pq[i].push(make_pair(dis, make_pair(i, j)));
                    }
                }
            }
        }

    }

    ofstream resfile;
    resfile.open("res.csv", ios_base::app);

    //cout << "Summary S(" << nw << ") = " << (float)(seq) / (float)(par_1 + par_2 + par_3) << endl;
    //cout << "------HEADER------" << endl;
    //cout << "label, n, k, nw, s1, s2, s3, tot" << endl;
    for (int w : workers) {
        resfile
                << "seq" << ","
                << n << ","
                << k << ","
                << w << ","
                << "NULL" << ","
                << (float)seq / (float)w << endl;
    }

}