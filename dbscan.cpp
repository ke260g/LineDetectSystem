#include <stack>

#include "dbscan.h"

using namespace std;
/*
 * private using, not implement more setter or getter
 */
class vx {
    public:
        enum {
            vxTypeNoise = 1,
            vxTypeBorder,
            vxTypeCore,
        };

        const void * rawdata;
        int cluster = 0; // result cluster index, -1 means not valid
        int vxType = vxTypeNoise; // 1 noise 2 border 3 core
        int numDirect = 0; // number of direct-reach-vx
        vector<int> coreDirect; // each element is index of other corePoints-direct-reach in whole datasets,
        bool visited = false;
        vx(const void  * iRawdata):rawdata(iRawdata) {
            cluster = -1;
        }
};

DBSCAN::DBSCAN(const double minPts, const double minEps) {
    this->minPts = minPts;
    this->minEps = minEps;
}

void DBSCAN::set_log(const bool isLogEnable) {
    this->isLogEnable = isLogEnable;
}

void DBSCAN::set_minPts(const double minPts) {
    this->minPts = minPts;
}

void DBSCAN::set_minEps(const double minPts) {
    this->minEps = minEps;
}

void DBSCAN::cluster(const std::vector<void *> dataset, unsigned compare_type,
                vector<int> & result, int & num_cluster,
                string & log) {
    int len = dataset.size();
    stringstream sslog; // local log

    // build temp dataset
    vector<vx> dats;
    dats.reserve(len);
    for(int i = 0; i < len; ++i)
        dats.push_back(vx(dataset[i]));

    // calculates pts
    for(int i = 0; i < len; ++i) {
        for(int j = i + 1; j < len; ++j) {
            bool isEps = compare_distance(dataset[i], dataset[j]);
            if(isEps) {
                ++(dats[i].numDirect);
                ++(dats[j].numDirect);
            }
        }
    }

    // core point
    vector<int> corePoint; // index of core point in dats
    for(int i = 0; i < len; ++i) {
        // minPts should sub one, as itself
        if(dats[i].numDirect >= (minPts  -1)) {
            dats[i].vxType = vx::vxTypeCore;
            corePoint.push_back(i);
        }
    }

    // core point join another core point
    // each core points "密度直达"的 core points
    for(unsigned i = 0; i < corePoint.size(); ++i) {
        for(unsigned j = i + 1; j < corePoint.size(); ++j) {
            int x = corePoint[i];
            int y = corePoint[j];
            if(compare_distance(dats[x].rawdata, dats[y].rawdata)) {
                dats[x].coreDirect.push_back(y);
                dats[y].coreDirect.push_back(x);
            }
        }
    }

    for(unsigned i = 0; i < corePoint.size(); ++i) {
        int idx = corePoint[i];
        // parse each rawdata
        stringstream tlog;
        rawdata_string(dats[idx].rawdata, tlog);
        sslog << tlog.rdbuf();
    }

    int clusterIndex = 0;
    for(unsigned i = 0; i < corePoint.size(); ++i) {
        int idx = corePoint[i];
        if(dats[idx].visited == true)
            continue;
        stack<int> ps;
        vx *v;
        ps.push(i);
        dats[i].cluster = clusterIndex;
        while(!ps.empty()) {
            v = &dats[ps.top()];
            v->visited = true;
            ps.pop();
            for(unsigned j = 0; j < v->coreDirect.size(); ++j) {
                int m = v->coreDirect[j];
                if(dats[m].visited == true)
                    continue;
                dats[m].cluster = clusterIndex;
                dats[m].visited = true;
                ps.push(m);
            }
        }
        ++clusterIndex;
    }

    // border point, joint border point to core point
    for(int i = 0; i < len; ++i) {
        if(dats[i].vxType == vx::vxTypeCore)
            continue;
        for(unsigned j = 0; j < corePoint.size(); j++) {
            bool isNearEnough = compare_distance(dats[i].rawdata, dats[corePoint[j]].rawdata);
            if(isNearEnough) {
                dats[i].vxType = vx::vxTypeBorder;
                dats[i].cluster = dats[corePoint[j]].cluster;
                break;
            }
        }
    }

    // output result
    result.clear();
    result.resize(len);
    for(int i = 0; i < len; ++i)
        result[i] = dats[i].cluster;

    num_cluster = clusterIndex;
    log = sslog.rdbuf()->str();
}
