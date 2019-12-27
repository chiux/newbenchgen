#ifndef BENCHMARK_GENERATOR_H
#define BENCHMARK_GENERATOR_H
#include "rect.h"
#include "simple_plotter.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib> // rand()
#include <cmath>  // sqrt()
#include <algorithm> // sort()
#include <map>
#include <set>

class IndexTuple {
  public:
    IndexTuple(){}
    IndexTuple(int die_index, int side_index, int vec_index)
      : _dindex(die_index), _sindex(side_index), _vindex(vec_index) {}
    
    int _dindex;
    int _sindex;
    int _vindex;
};
class IndexTupleLesser {
  public:
    bool operator() (const IndexTuple a, const IndexTuple b) const {
        if (a._dindex != b._dindex) { return a._dindex < b._dindex; }
        else if (a._sindex != b._sindex) { return a._sindex < b._sindex; }
        else { return a._vindex < b._vindex; }
    }
};

class SidePair {

    public:
    SidePair(int d1, int s1, int d2, int s2){
        _die1 = d1;
        _side1 = s1;

        _die2 = d2;
        _side2 = s2;
    }

    int _die1;
    int _die2;
    int _side1;
    int _side2;

};


class NewBenGen {

    public:
    NewBenGen(){};

    // generate benchmarks

    void genVia1();
    void genVia2();
    void genVia3();
    void genVia4();
    void genVia5();


    void genBumps(const char* filename, double width, double pitch);
    void genIOs(double pitch, double width);
    void genPAs();
    Rect getByIndexTuple(IndexTuple tup);
    void parseIO (const char* filename);
    void parseNet (const char* filename);

    void plot(std::string filename, int Width, int Height);


    Rect              _boundary;
    std::vector<Rect> _dies;
    std::vector<Rect> _bumps;


    std::vector< std::vector<Rect> > _io_north;
    std::vector< std::vector<Rect> > _io_east;
    std::vector< std::vector<Rect> > _io_south;
    std::vector< std::vector<Rect> > _io_west;

    std::map< IndexTuple, IndexTuple, IndexTupleLesser > _pa;
    std::vector<SidePair> _sp;
};



#endif