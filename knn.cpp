/*
 * This file is part of {PDS: Investigating an unblocking parallel
 * implementation for the K-NN algorithm}.
 *
 * Developed for the PDS21 project application on K-NN algorithm.
 *
 * STL Thread implementation mimicking a Parallel For-Reduce construct
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

/**
 * Compute Euclidean Distance without sqrt.
 *
 * @param p1 pair of (x,y) coordinates
 * @param p2 pair of (x,y) coordinates
 * @return Euclidean Distance without sqrt for efficiency purposes
 */

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
    vector < string >  closest (n, "");
    long seq;

    if (!verb) {

        {
            utimer ts("Stage Full seq", &seq);

            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    float dis = distance(dict[i], dict[j]);
                    if (seq_res_pq[i].size() < k && i != j) {
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
                if (res_pq[i].size() < k && i != j) {
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

    //ofstream outfile;
    //outfile.open("outputSTL.csv", fstream::out);
    //for (int r = 0; r < n; r++) outfile << r << " " << closest[r] << endl;

    /**
    * Building a csv appending the results 
    * for statistical purposes
    **/

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