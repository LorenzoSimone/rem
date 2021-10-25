#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "utimer.cpp"
#include <thread>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <queue>
#include <ff/parallel_for.hpp>
#include <ff/ff.hpp>

using namespace std;

typedef pair<float, pair<int, int>> pairfii;
const string IN_FILE = "in.txt";
const int none = 99999;
vector<int> workers = { 1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64, 128, 256};

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
    return dict;

}

vector<pairfii> min_k (vector<pair<int, int>> pos, vector<pairfii> vec) {

    int k = pos.size();
    vector<pairfii> res(k, make_pair(none, make_pair(0, 0)));

    for (int i = 0; i < k; i++) {
        int min_i = 0;
        pairfii min_v = vec[pos[0].first];

        for (int j = 0; j < k; j++) {
            if ( vec[pos[j].first].first < min_v.first && pos[j].first < pos[j].second) {
                min_i = j;
                min_v = vec[pos[j].first];
            }
        }
        pos[min_i] = make_pair(pos[min_i].first + 1, pos[min_i].second);
        res[i] = min_v;
    }

    return res;

}

float distance(pair<float, float> x1, pair<float, float> x2) {
    return pow(x2.first - x1.first, 2) + pow(x2.second - x1.second, 2) * 1.0;
}

int main(int argc, const char *argv[]) {

    if (argc < 5) {
        cerr << "Use: k | nw" << endl;
        return -1;
    }

    int nw = atoi(argv[1]);
    int k = atoi(argv[2]);
    int pol_s = (argc > 3 && atoi(argv[3]));
    int verb = (argc > 4 && atoi(argv[4]));

    vector<pair<float, float>> dict = readFile(IN_FILE);

    int n = dict.size();

    vector <  priority_queue<pairfii> > res_pq(n, priority_queue <pairfii>());
    vector <  priority_queue<pairfii> > seq_pq(n, priority_queue <pairfii>());

    long seq;
    if (!verb) {
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
    }

    long par;
    {
        utimer ts(pol_s ? "KNNFF_S" : "KNNFF_D" + "Par ->", &par);


        ff::parallel_for(0, n, 1, pol_s, [&dict, &n, &k, &res_pq] (long r) {

            for (int j = 0; j < n; j++) {
                float dis = distance(dict[r], dict[j]);
                if (j < k) {
                    res_pq[r].push(make_pair(dis, make_pair(r, j)));
                }
                else {
                    if (r != j && (dis < res_pq[r].top().first)) {
                        res_pq[r].pop();
                        res_pq[r].push(make_pair(dis, make_pair(r, j)));
                    }
                }
            }

            return;
        }, nw);
    }

    if (verb) {
        ofstream resfile;
        resfile.open("res.csv", ios_base::app);
        string label = pol_s ? "KNNFF_S" : "KNNFF_D";
        resfile
                << label << ","
                << n << ","
                << k << ","
                << nw << ","
                << pol_s << ","
                << par << endl;
    }
    else cout << "Summary S(" << nw << ") = " << (float)(seq) / (float)(par) << endl;

    return 0;
}