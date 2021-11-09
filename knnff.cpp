/*
 * This file is part of {PDS: Investigating an unblocking parallel
 * implementation for the K-NN algorithm}.
 *
 * Developed for the PDS21 project application on K-NN algorithm.
 *
 * FastFlow implementation utilizing a fast parallel_for method
 * mimicking a Parallel For-Reduce construct
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

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
#include <ff/parallel_for.hpp>
#include "../utils/readInput.cpp"
#include "utimer.cpp"
#include <ff/ff.hpp>

using namespace std;

typedef pair<float, pair<int, int>> pairfii;
const string IN_FILE = "in.txt";
const int none = 99999;
vector<int> workers = { 1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64, 128, 256};

/**
 * Compute Euclidean Distance without sqrt.
 *
 * @param p1 pair of (x,y) coordinates
 * @param p2 pair of (x,y) coordinates
 * @return Euclidean Distance without sqrt for efficiency purposes
 */
float distance(pair<float, float> x1, pair<float, float> x2) {
    return pow(x2.first - x1.first, 2) + pow(x2.second - x1.second, 2) * 1.0;
}

int main(int argc, const char *argv[]) {

    if (argc < 3) {
        cerr << "Use: nw | k" << endl;
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
    vector < string >  closest (n, "");
    ofstream outfile;


    long seq;
    if (!verb) {
        {
            utimer ts("Stage Full seq", &seq);

            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    float dis = distance(dict[i], dict[j]);
                    if (seq_pq[i].size() < k && i != j) {
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
        utimer ts("KNNFF", &par);
        ff::parallel_for(0, n, 1, pol_s, [&] (long r) {

            for (int j = 0; j < n; j++) {
                float dis = distance(dict[r], dict[j]);
                if (res_pq[r].size() < k && r != j) {
                    res_pq[r].push(make_pair(dis, make_pair(r, j)));
                }
                else {
                    if (r != j && (dis < res_pq[r].top().first)) {
                        res_pq[r].pop();
                        res_pq[r].push(make_pair(dis, make_pair(r, j)));
                    }
                }
            }
            /*
            while (!res_pq[r].empty())
            {
                closest[r] += to_string(res_pq[r].top().second.second) + ",";
                res_pq[r].pop();
            }

            closest[r].pop_back();
            reverse(closest[r].begin(), closest[r].end());

            return;
            */
        }, nw);
    }

    //outfile.open("outputFF.csv", fstream::out);
    //for (int r = 0; r < n; r++) outfile << r << " " << closest[r] << endl;

    /**
    * Building a csv appending the results
    * for statistical purposes
    **/
        
    if (verb) {
        ofstream resfile;
        resfile.open("res.csv", ios_base::app);
        string label = "KNNFF";
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