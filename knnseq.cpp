/*
 * This file is part of {PDS: Investigating an unblocking parallel
 * implementation for the K-NN algorithm}.
 *
 * Developed for the PDS21 project application on K-NN algorithm.
 *
 * Sequential implementation
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
#include "utimer.cpp"
#include "utils/readInput.cpp"


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
        cerr << "Use: k" << endl;
        return -1;
    }

    int k = atoi(argv[1]);

    vector<pair<float, float>> dict = readFile(IN_FILE);

    int n = dict.size();

    vector <  priority_queue<pairfii> > seq_pq(n, priority_queue <pairfii>());
    vector < string >  closest (n, "");

    ofstream outfile;
    outfile.open("outputSEQ.csv", fstream::out);


    long seq;
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

            outfile << i << " ";
            while (!seq_pq[i].empty())
            {
                closest[i] += to_string(seq_pq[i].top().second.second) + ",";
                seq_pq[i].pop();
            }

            closest[i].pop_back();
            reverse(closest[i].begin(), closest[i].end());
            outfile << closest[i] << endl;
        }
        outfile.close();
    }

    /**
    * Building a csv appending the results
    * for statistical purposes
    **/

    ofstream resfile;
    resfile.open("res.csv", ios_base::app);

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