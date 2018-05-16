#ifndef DBSCAN_H
#define DBSCAN_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

class DBSCAN {
    public:
        enum {
            distance_type_bool,
            distance_type_double,
        };
        void cluster(const std::vector<void *> dataset, unsigned compare_type,
                std::vector<int> & result, int & num_cluster,
                std::string & log);
        DBSCAN(const double minPts = 1, const double minEps = 0);
        void set_minPts(const double minPts);
        void set_minEps(const double minEps);
        void set_log(const bool isLogEnable);
    protected:
        /**
         * @return true if distance is small enough between l and r
         */
        virtual bool compare_distance(const void * l, const  void * r)  = 0;
        /**
         * @return distance between l and r, then compare to eps
         */
        virtual double calculate_distance(const void * l, const void * r) = 0;
        virtual void rawdata_string(const void * rawdata, std::stringstream & str) = 0;
    private:
        double minPts;
        double minEps;
        bool isLogEnable;
};

#endif // DBSCAN_H
