#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
using namespace std;

static const pair<int, int> NEIGHBOR[] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

class DisjointSetUnion {
public:
    DisjointSetUnion(int numNodes) {
        rootId.resize(numNodes);//初始化
        iota(rootId.begin(), rootId.end(), 0);
    }

    int getRootId(int p) {//查找
        return p == rootId[p] ? p : rootId[p] = getRootId(rootId[p]);
    }

    void join(int p1, int p2) {//合并
        int root1 = getRootId(p1), root2 = getRootId(p2);
        if(root1 != root2) {
            rootId[root1] = root2;
        }
    }

private:
    vector<int> rootId;
};

int randomInt(int low, int high);

// template<typename Iterator>
// void randomShuffle(Iterator begin, Iterator end);

#endif // UTILS_H
