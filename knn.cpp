#include <string>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <queue>
#include "utils/readInput.cpp"
#include "utimer.cpp"
#include <thread>
#include <cmath>
#include <cstring>

using namespace std;
typedef pair<float, pair<int, int>> pairfii;

const string IN_FILE = "in.txt";
const string LABEL = "KNNFF";
const int none = 99999;

vector<pair<float, float>> dict;
vector < priority_queue<pairfii>> res_pq;
vector < priority_queue<pairfii>> seq_res_pq;

float distance(pair<float, float> x1, pair<float, float> x2) {
    return pow(x2.first - x1.first, 2) +
           pow(x2.second - x1.second, 2) * 1.0;
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        cerr << "Use: num workers | k | s: scheduler OFF | verbose" << endl;
        return -1;
    }

    int nw = atoi(argv[1]);
    int k = atoi(argv[2]);
    int verb = (argc > 3 && atoi(argv[3]));

    dict = readFile(IN_FILE);
    long n = dict.size();

    res_pq = vector <  priority_queue<pairfii> >(n, priority_queue <pairfii>());
    seq_res_pq = vector <  priority_queue<pairfii> >(n, priority_queue <pairfii>());

    long seq;

    if (!verb) {

        {
            utimer ts("Stage Full seq", &seq);

            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    float dis = distance(dict[i], dict[j]);
                    if (j < k) {
                        seq_res_pq[i].push(make_pair(dis, make_pair(i, j)));
                    }
                    else {
                        if (i != j && (dis < seq_res_pq[i].top().first)) {
                            seq_res_pq[i].pop();
                            seq_res_pq[i].push(make_pair(dis, make_pair(i, j)));
                        }
                    }

                }
            }
        }
    }

    auto par_f = [&] (int thnum) {

        auto delta = n / nw;
        auto start = (thnum * delta);
        auto stop  = (thnum == (nw - 1) ? n : (thnum + 1) * delta);

        for (int i = start; i < stop; i++) {
            for (int j = 0; j < n; j++) {
                float dis = distance(dict[i], dict[j]);
                if (j < k) {
                    res_pq[i].push(make_pair(dis, make_pair(i, j)));
                }
                else {
                    if (i != j && (dis < res_pq[i].top().first)) {
                        res_pq[i].pop();
                        res_pq[i].push(make_pair(dis, make_pair(i, j)));
                    }
                }
            }
        }
        return;
    };


    vector<thread*> tids(nw);

    long par;

    {
        utimer ts("KNN Par ->", &par);
        for (int i = 0; i < nw; i++)
            tids[i] = new thread(par_f, i);
        for (int i = 0; i < nw; i++)
            tids[i]->join();
    }


    if (verb) {
        ofstream resfile;
        resfile.open("res.csv", ios_base::app);
        resfile
                << "KNN" << ","
                << n << ","
                << k << ","
                << nw << ","
                << "NULL" << ","
                << par << endl;
    }
    else cout << "Summary S(" << nw << ") = " << (float)(seq) / (float)(par) << endl;

    return 0;
}