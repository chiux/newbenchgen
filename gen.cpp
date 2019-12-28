#include "gen.h"
using namespace std;

// global parameters:
#define LAYER_NUMBER   4
#define WIRE_WIDTH     4
#define VIA_IO_WIDTH   20
#define IO_PITCH       60
#define PA_RATIO       0.3
// generative parameters:
double psd_spacing = 100;       // chip-to-boundary spacing
double chip2chip_spacing = 300; // chip-to-chip spacing
double init_space = 5;          // initial chip w/h
double grow_step = 8;
int    num_chip = 50;
double area_per_io = 25000;
int    err_limit = 1000, err_count = 0;
// IO parameters:
#define IO_PARR_WIDTH   40
#define IO_PERP_WIDTH   60
// bump parameters:
#define BUMPPAD_WIDTH   100
#define BUMPPAD_PITCH   250


bool NewBenGen::compareSFA_hor(IndexTuple i1, IndexTuple i2){

    Rect R1 = getByIndexTuple(i1);
    Rect R2 = getByIndexTuple(i2);

    return (R1.getCx() < R2.getCx()); 
}

bool NewBenGen::compareSFA_ver(IndexTuple i1, IndexTuple i2){

    Rect R1 = getByIndexTuple(i1);
    Rect R2 = getByIndexTuple(i2);

    return (R1.getCy() < R2.getCy());
} 

void NewBenGen::genVia1(){

    int package_w = 5000;
    int package_h = 5000;
    int group_num = 3;
    

    // set package boundary
    _boundary = Rect(0, 0, package_w, package_h);

    // generate Dies(3)

    _dies.emplace_back(Rect(1000, 1000, 4000, 2500));
    _dies.emplace_back(Rect(1000, 3000, 2000, 4000));
    _dies.emplace_back(Rect(3000, 3000, 4000, 4000));


    // generate IOs
    genIOs(IO_PITCH, VIA_IO_WIDTH);

    // generate PAs
    _sp.clear();
    _sp.emplace_back( SidePair(0, 0, 1, 2) );
    _sp.emplace_back( SidePair(0, 0, 2, 2) );
    _sp.emplace_back( SidePair(1, 1, 2, 3) );
    genPAs();

    // generate SFAs
    genSFAs(package_w, package_h, 0.25);

    // generate regular bump
    genBumps(BUMPPAD_WIDTH, BUMPPAD_PITCH);

    parseIO("./benchmarks/Via1/Via1.iopad");
    parseBump("./benchmarks/Via1/Via1.bump");
    parseNet("./benchmarks/Via1/Via1.net");
    parseSFA("./benchmarks/Via1/Via1.faorder", group_num);
    parseParam("./benchmarks/Via1/Via1.gp", package_w, package_h, LAYER_NUMBER, WIRE_WIDTH, VIA_IO_WIDTH, IO_PITCH);


    plot("./benchmarks/Via1/Via1.plt", package_w, package_h);


}


void NewBenGen::genVia2(){

}


void NewBenGen::genVia3(){

}


void NewBenGen::genVia4(){

}


void NewBenGen::genVia5(){

}


void NewBenGen::genBumps(double width, double pitch){



    double xoffset = width/2 + pitch ;
    double yoffset = width/2 + pitch ;
    int    xnum    = (int)(_boundary.getWidth()-xoffset)  / (int)(width+pitch);
    int    ynum    = (int)(_boundary.getHeight()-yoffset) / (int)(width+pitch);
    for (int i = 0; i < xnum; ++i) {
        for (int j = 0; j < ynum; ++j) {
            double lx = xoffset + i * (width + pitch) ;
            double rx = xoffset + i * (width + pitch) + width ;
            double by = yoffset + j * (width + pitch) ;
            double ty = yoffset + j * (width + pitch) + width ;
            _bumps.emplace_back( Rect(lx, by, rx, ty) );
        }
    }


    return ;
}


void NewBenGen::genIOs(double pitch, double width){
    
    _io_north.resize(_dies.size());
    _io_east.resize(_dies.size());
    _io_south.resize(_dies.size());
    _io_west.resize(_dies.size());

    for (size_t i = 0; i < _dies.size(); ++i)
    {
        Rect   die     = _dies[i];
        double xoffset = pitch;
        double yoffset = pitch*2;
        // North
        double lx = die.getLx() + xoffset;
        while ( (lx + width + xoffset) < die.getRx() ) {
            double rx      = lx + width ;
            double by      = die.getTy() - pitch - width;
            double ty      = die.getTy() - pitch;
            if( (rand() % 10) > 3){
                _io_north[i].emplace_back( Rect(lx, by, rx, ty) );
            }
            lx += (width + pitch);
        }

        // South
        lx = die.getLx() + xoffset;
        while ( (lx + width + xoffset) < die.getRx() ) {
            double rx      = lx + width;
            double by      = die.getBy() + pitch;
            double ty      = die.getBy() + pitch + width;
            if( (rand() % 10) > 3){
                _io_south[i].emplace_back( Rect(lx, by, rx, ty) );
            }
            lx += (width + pitch) ;
        }

        // West
        double by = die.getBy() + yoffset ;
        while ( (by + width + yoffset) < die.getTy() ) {
            double ty      = by + width ;
            double lx      = die.getLx() + pitch;
            double rx      = die.getLx() + pitch + width;
            if( (rand() % 10) > 3){
                _io_west[i].emplace_back( Rect(lx, by, rx, ty) );
            }
            by += (width + pitch) ;
        }
        // East
        by = die.getBy() + yoffset ;
        while ( (by + width + yoffset) < die.getTy() ) {
            double ty      = by + width;
            double lx      = die.getRx() - pitch - width;
            double rx      = die.getRx() - pitch ;
            if( (rand() % 10) > 3){
                _io_east[i].emplace_back( Rect(lx, by, rx, ty) );
            }
            by += (width + pitch) ;
        }
    }
    return;
}


void NewBenGen::genPAs(){

    for (size_t i = 0; i < _sp.size(); ++i){
        int d1 = _sp[i]._die1;
        int d2 = _sp[i]._die2;
        int s1 = _sp[i]._side1;
        int s2 = _sp[i]._side2;

        int num1 = 0;
        int num2 = 0;

        switch (s1) {
            case 0:
                num1 = _io_north[d1].size();
                break;
            case 1:
                num1 = _io_east[d1].size();
                break;
            case 2:
                num1 = _io_south[d1].size();
                break;
            case 3:
                num1 = _io_west[d1].size();
                break;
        }
        switch (s2) {
            case 0:
                num2 = _io_north[d2].size();
                break;
            case 1:
                num2 = _io_east[d2].size();
                break;
            case 2:
                num2 = _io_south[d2].size();
                break;
            case 3:
                num2 = _io_west[d2].size();
                break;
        }

        if (num1==0 || num2==0){
            continue;
        }

        for(int j = 0; j < min(num1,num2)/2;){

            int v1 = rand() % num1;
            int v2 = rand() % num2;

            if( (_pa.find( IndexTuple(d1,s1,v1) ) != _pa.end()) || (_pa.find( IndexTuple(d2,s2,v2) ) != _pa.end()) ){
                continue;
            }
            else{
                _pa[ IndexTuple(d1, s1, v1) ] = IndexTuple(d2, s2, v2);
                _pa[ IndexTuple(d2, s2, v2) ] = IndexTuple(d1, s1, v1);
                ++j;
            }
        }

    }

    return;
}

void
NewBenGen::genSFAs (int Width, int Height, double b_ratio)
{

    for (int i = 0; i < _dies.size(); ++i) {
        
        for (int j = 0; j < _io_north[i].size(); ++j) {
            if(_pa.find(IndexTuple(i,0,j))!=_pa.end()){
                continue;
            }
            Rect R = _io_north[i][j];
            if( R.getCx() > Width*b_ratio && R.getCx() < Width*(1-b_ratio) && R.getCy() > Height*b_ratio && R.getCy() < Height*(1-b_ratio) ){
                continue;
            }
            
            if( (rand() % 10) > 3){
                continue;
            }

            // West
            if( R.getCx() < Width*b_ratio ){
                _sfa_west.emplace_back( IndexTuple(i,0,j) );
            }
            // East
            else if( R.getCx() > Width*(1-b_ratio) ){
                _sfa_east.emplace_back( IndexTuple(i,0,j) );
            }
            // South
            else if(R.getCx() > Width*b_ratio && R.getCx() < Width*(1-b_ratio) && R.getCy() < Height*b_ratio){
                _sfa_south.emplace_back( IndexTuple(i,0,j) );
            }
            // North
            else if(R.getCx() > Width*b_ratio && R.getCx() < Width*(1-b_ratio) && R.getCy() > Height*(1-b_ratio) ){
                _sfa_north.emplace_back( IndexTuple(i,0,j) );
            }
            
        }

        for (int j = 0; j < _io_east[i].size(); ++j) {
            if(_pa.find(IndexTuple(i,1,j))!=_pa.end()){
                continue;
            }
            Rect R = _io_east[i][j];

            if( R.getCx() > Width*b_ratio && R.getCx() < Width*(1-b_ratio) && R.getCy() > Height*b_ratio && R.getCy() < Height*(1-b_ratio) ){
                continue;
            }

            if( (rand() % 10) > 3){
                continue;
            }
            
            // West
            if( R.getCx() < Width*b_ratio ){
                _sfa_west.emplace_back( IndexTuple(i,1,j) );
            }
            // East
            else if( R.getCx() > Width*(1-b_ratio) ){
                _sfa_east.emplace_back( IndexTuple(i,1,j) );
            }
            // South
            else if(R.getCx() > Width*b_ratio && R.getCx() < Width*(1-b_ratio) && R.getCy() < Height*b_ratio){
                _sfa_south.emplace_back( IndexTuple(i,1,j) );
            }
            // North
            else if(R.getCx() > Width*b_ratio && R.getCx() < Width*(1-b_ratio) && R.getCy() > Height*(1-b_ratio) ){
                _sfa_north.emplace_back( IndexTuple(i,1,j) );
            }

        }

        for (int j = 0; j < _io_south[i].size(); ++j) {
            if(_pa.find(IndexTuple(i,2,j))!=_pa.end()){
                continue;
            }
            Rect R = _io_south[i][j];
            if( R.getCx() > Width*b_ratio && R.getCx() < Width*(1-b_ratio) && R.getCy() > Height*b_ratio && R.getCy() < Height*(1-b_ratio) ){
                continue;
            }

            if( (rand() % 10) > 3){
                continue;
            }
            
            // West
            if( R.getCx() < Width*b_ratio ){
                _sfa_west.emplace_back( IndexTuple(i,2,j) );
            }
            // East
            else if( R.getCx() > Width*(1-b_ratio) ){
                _sfa_east.emplace_back( IndexTuple(i,2,j) );
            }
            // South
            else if(R.getCx() > Width*b_ratio && R.getCx() < Width*(1-b_ratio) && R.getCy() < Height*b_ratio){
                _sfa_south.emplace_back( IndexTuple(i,2,j) );
            }
            // North
            else if(R.getCx() > Width*b_ratio && R.getCx() < Width*(1-b_ratio) && R.getCy() > Height*(1-b_ratio) ){
                _sfa_north.emplace_back( IndexTuple(i,2,j) );
            }

        }

        for (int j = 0; j < _io_west[i].size(); ++j) {
            if(_pa.find(IndexTuple(i,3,j))!=_pa.end()){
                continue;
            }
            Rect R = _io_west[i][j];
            if( R.getCx() > Width*b_ratio && R.getCx() < Width*(1-b_ratio) && R.getCy() > Height*b_ratio && R.getCy() < Height*(1-b_ratio) ){
                continue;
            }

            if( (rand() % 10) > 3){
                continue;
            }
            
            // West
            if( R.getCx() < Width*b_ratio ){
                _sfa_west.emplace_back( IndexTuple(i,3,j) );
            }
            // East
            else if( R.getCx() > Width*(1-b_ratio) ){
                _sfa_east.emplace_back( IndexTuple(i,3,j) );
            }
            // South
            else if(R.getCx() > Width*b_ratio && R.getCx() < Width*(1-b_ratio) && R.getCy() < Height*b_ratio){
                _sfa_south.emplace_back( IndexTuple(i,3,j) );
            }
            // North
            else if(R.getCx() > Width*b_ratio && R.getCx() < Width*(1-b_ratio) && R.getCy() > Height*(1-b_ratio) ){
                _sfa_north.emplace_back( IndexTuple(i,3,j) );
            }

        }
    }


    // sort SFAs

    sort(_sfa_north.begin(), _sfa_north.end(), 
        [&] (const IndexTuple& i1, const IndexTuple& i2) {
            return getByIndexTuple(i1).getCx() < getByIndexTuple(i2).getCx();
    });
    sort(_sfa_south.begin(), _sfa_south.end(), 
        [&] (const IndexTuple& i1, const IndexTuple& i2) {
            return getByIndexTuple(i1).getCx() < getByIndexTuple(i2).getCx();
    });


    sort(_sfa_west.begin(), _sfa_west.end(), 
        [&] (const IndexTuple& i1, const IndexTuple& i2) {
            return getByIndexTuple(i1).getCy() < getByIndexTuple(i2).getCy();
    });
    sort(_sfa_east.begin(), _sfa_east.end(), 
        [&] (const IndexTuple& i1, const IndexTuple& i2) {
            return getByIndexTuple(i1).getCy() < getByIndexTuple(i2).getCy();
    });

    return;
    
}




Rect NewBenGen::getByIndexTuple(IndexTuple tup)
{
    if (tup._sindex == 0) {
        return _io_north[tup._dindex][tup._vindex];
    } else if (tup._sindex == 1) {
        return _io_east[tup._dindex][tup._vindex];
    } else if (tup._sindex == 2) {
        return _io_south[tup._dindex][tup._vindex];
    } else if (tup._sindex == 3) {
        return _io_west[tup._dindex][tup._vindex];
    }
    cout << "ERROR!" << endl;
    return Rect(0,0,0,0);
}

void NewBenGen::parseParam(const char* filename, double pkgw, double pkgh, double lnum, double ww, double vw, double p)
{
    ofstream fout;
    fout.open(filename);
    fout << "[PACKAGEWIDTH] " << pkgw << endl;
    fout << "[PACKAGEHEIGHT] " << pkgh << endl;
    fout << "[LAYERNUMBER] " << lnum << endl;
    fout << "[WIREWIDTH] " << ww << endl;
    fout << "[VIAWIDTH] " << vw << endl;
    fout << "[PITCH] " << p << endl;
    fout.close();
}

void
NewBenGen::parseBump (const char* filename)
{
    ofstream fout;
    fout.open(filename);
    for (Rect bump : _bumps) {
        fout << bump.getLx() << " " << bump.getBy() << " " << bump.getRx() << " " << bump.getTy() << endl;
    }
    fout.close();
}

void
NewBenGen::parseIO (const char* filename)
{
    ofstream fout;
    fout.open(filename);
    for (int i = 0; i < _dies.size(); ++i) {
        // die declaration and name
        fout << "[DIE] " << "die" << i << endl;
        // io pads
        for (int j = 0; j < _io_north[i].size(); ++j) {
            // if(_pa.find(IndexTuple(i,0,j))==_pa.end()){
            //     continue;
            // }
            Rect iopad = _io_north[i][j];
            fout << "io_d" << i << "_0_" << j << " "
                 << iopad.getLx() << " " << iopad.getBy() << " " << iopad.getRx() << " " << iopad.getTy() << endl;
        }
        for (int j = 0; j < _io_east[i].size(); ++j) {
            // if(_pa.find(IndexTuple(i,1,j))==_pa.end()){
            //     continue;
            // }
            Rect iopad = _io_east[i][j];
            fout << "io_d" << i << "_1_" << j << " "
                 << iopad.getLx() << " " << iopad.getBy() << " " << iopad.getRx() << " " << iopad.getTy() << endl;
        }
        for (int j = 0; j < _io_south[i].size(); ++j) {
            // if(_pa.find(IndexTuple(i,2,j))==_pa.end()){
            //     continue;
            // }
            Rect iopad = _io_south[i][j];
            fout << "io_d" << i << "_2_" << j << " "
                 << iopad.getLx() << " " << iopad.getBy() << " " << iopad.getRx() << " " << iopad.getTy() << endl;
        }
        for (int j = 0; j < _io_west[i].size(); ++j) {
            // if(_pa.find(IndexTuple(i,3,j))==_pa.end()){
            //     continue;
            // }
            Rect iopad = _io_west[i][j];
            fout << "io_d" << i << "_3_" << j << " "
                 << iopad.getLx() << " " << iopad.getBy() << " " << iopad.getRx() << " " << iopad.getTy() << endl;
        }
    }
    fout.close();
}

void NewBenGen::parseNet  (const char* filename)
{
    ofstream fout;
    fout.open(filename);
    std::set< IndexTuple, IndexTupleLesser > already_parsed;
    
    int net_index = 0;
    for (auto pair : _pa) {
        if ( already_parsed.count(pair.first) > 0 ) { continue; }
        fout << "net" << net_index++ << " "
             << "io_d" << pair.first._dindex << "_" << pair.first._sindex << "_" << pair.first._vindex << " "
             << "io_d" << pair.second._dindex << "_" << pair.second._sindex << "_" << pair.second._vindex << endl;
        already_parsed.insert(pair.first);
        already_parsed.insert(pair.second);
    }
    fout.close();
}

void NewBenGen::parseSFA (const char* filename, int group_num)
{
    ofstream fout;
    fout.open(filename);


    fout << "[East]" << endl;
    for (size_t i = 0; i < _sfa_east.size(); ++i){
        if ( (i % group_num)==0){
            fout << "<Group>" << endl;
        }
        IndexTuple t = _sfa_east[i];
        fout << "io_d" << t._dindex << "_" << t._sindex << "_" << t._vindex << endl;
    }

    fout << "[West]" << endl;
    for (size_t i = 0; i < _sfa_west.size(); ++i){
        if ( (i % group_num)==0){
            fout << "<Group>" << endl;
        }
        IndexTuple t = _sfa_west[i];
        fout << "io_d" << t._dindex << "_" << t._sindex << "_" << t._vindex << endl;
    }

    fout << "[South]" << endl;
    for (size_t i = 0; i < _sfa_south.size(); ++i){
        if ( (i % group_num)==0){
            fout << "<Group>" << endl;
        }
        IndexTuple t = _sfa_south[i];
        fout << "io_d" << t._dindex << "_" << t._sindex << "_" << t._vindex << endl;
    }

    fout << "[North]" << endl;
    for (size_t i = 0; i < _sfa_north.size(); ++i){
        if ( (i % group_num)==0){
            fout << "<Group>" << endl;
        }
        IndexTuple t = _sfa_north[i];
        fout << "io_d" << t._dindex << "_" << t._sindex << "_" << t._vindex << endl;
    }
    fout.close();
}

void
NewBenGen::plot(std::string filename, int Width, int Height)
{
    std::ofstream fout;
    fout.open(filename);
    simple_plotter::Plotter plotter;
    plotter.AddStyle("line", "lc rgb 'black' lw 3"); // <- style index = 1
    plotter.AddStyle("line", "lc rgb 'blue' lw 2"); // <- style index = 2
    plotter.AddStyle("line", "lc rgb '#009900' lw 2"); // <- style index = 3 green
    plotter.AddStyle("line", "lc rgb '#CC6600' lw 2"); // <- style index = 4 orange
    plotter.AddStyle("line", "lc rgb '#FF3399' lw 2"); // <- style index = 5 pink
    plotter.AddStyle("line", "lc rgb '#6600CC' lw 2"); // <- style index = 6 purple
    plotter.AddStyle("line", "lc rgb '#AA0000' lw 2"); // <- style index = 7
    
    // draw package round
    plotter.AddRect(_boundary.getLx(), _boundary.getBy(), _boundary.getRx(), _boundary.getTy(), 1);
    for (auto die : _dies) {
        plotter.AddRect(die.getLx(), die.getBy(), die.getRx(), die.getTy(), 2);
    }
    
    // draw bumps
    for (auto bump : _bumps) {
        double midspace = bump.getWidth() / (1 + 2/sqrt(2)) * 1/sqrt(2);
        plotter.AddSegment(bump.getLx(), bump.getBy()+midspace, bump.getLx(), bump.getTy()-midspace, 4);
        plotter.AddSegment(bump.getLx(), bump.getTy()-midspace, bump.getLx()+midspace, bump.getTy(), 4);
        plotter.AddSegment(bump.getLx()+midspace, bump.getTy(), bump.getRx()-midspace, bump.getTy(), 4);
        plotter.AddSegment(bump.getRx()-midspace, bump.getTy(), bump.getRx(), bump.getTy()-midspace, 4);
        plotter.AddSegment(bump.getRx(), bump.getTy()-midspace, bump.getRx(), bump.getBy()+midspace, 4);
        plotter.AddSegment(bump.getRx(), bump.getBy()+midspace, bump.getRx()-midspace, bump.getBy(), 4);
        plotter.AddSegment(bump.getRx()-midspace, bump.getBy(), bump.getLx()+midspace, bump.getBy(), 4);
        plotter.AddSegment(bump.getLx()+midspace, bump.getBy(), bump.getLx(), bump.getBy()+midspace, 4);
    }
    
    // draw ios
    for (auto vec : _io_north) {
        for (auto io : vec) {
            plotter.AddRect(io.getLx(), io.getBy(), io.getRx(), io.getTy(), 3);
        }
    }
    for (auto vec : _io_south) {
        for (auto io : vec) {
            plotter.AddRect(io.getLx(), io.getBy(), io.getRx(), io.getTy(), 3);
        }
    }
    for (auto vec : _io_west) {
        for (auto io : vec) {
            plotter.AddRect(io.getLx(), io.getBy(), io.getRx(), io.getTy(), 3);
        }
    }
    for (auto vec : _io_east) {
        for (auto io : vec) {
            plotter.AddRect(io.getLx(), io.getBy(), io.getRx(), io.getTy(), 3);
        }
    }

    
    // draw nets
    // for (auto index_pair : _pa) {
    //     Rect p1 = getByIndexTuple(index_pair.first);
    //     Rect p2 = getByIndexTuple(index_pair.second);
    //     if (_single_pa.count(index_pair.first) > 0) { continue; }
    //     plotter.AddSegment(p1.getCx(), p1.getCy(), p2.getCx(), p2.getCy(), 5);
    // }
    for (auto index_pair : _pa) {
        Rect p1 = getByIndexTuple(index_pair.first);
        Rect p2 = getByIndexTuple(index_pair.second);
        plotter.AddSegment(p1.getCx(), p1.getCy(), p2.getCx(), p2.getCy(), 6);
    }

    // draw SFA
    for (int j = 0; j < _sfa_north.size(); ++j) {

        IndexTuple t = _sfa_north[j];
        int D = t._dindex;
        int S = t._sindex;
        int V = t._vindex;

        if(S==0){
            Rect R = _io_north[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), Width/2, Height, 7);
        }
        else if(S==1){
            Rect R = _io_east[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), Width/2, Height, 7);
        }
        else if(S==2){
            Rect R = _io_south[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), Width/2, Height, 7);
        }
        else if(S==3){
            Rect R = _io_west[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), Width/2, Height, 7);
        }     
    }

    for (int j = 0; j < _sfa_south.size(); ++j) {
        
        IndexTuple t = _sfa_south[j];
        int D = t._dindex;
        int S = t._sindex;
        int V = t._vindex;

        if(S==0){
            Rect R = _io_north[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), Width/2, 0, 7);
        }
        else if(S==1){
            Rect R = _io_east[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), Width/2, 0, 7);
        }
        else if(S==2){
            Rect R = _io_south[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), Width/2, 0, 7);
        }
        else if(S==3){
            Rect R = _io_west[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), Width/2, 0, 7);
        }     
    }

    for (int j = 0; j < _sfa_west.size(); ++j) {
        
        IndexTuple t = _sfa_west[j];
        int D = t._dindex;
        int S = t._sindex;
        int V = t._vindex;

        if(S==0){
            Rect R = _io_north[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), 0, Height/2, 7);
        }
        else if(S==1){
            Rect R = _io_east[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), 0, Height/2, 7);
        }
        else if(S==2){
            Rect R = _io_south[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), 0, Height/2, 7);
        }
        else if(S==3){
            Rect R = _io_west[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), 0, Height/2, 7);
        }     
    }

    for (int j = 0; j < _sfa_east.size(); ++j) {
        
        IndexTuple t = _sfa_east[j];
        int D = t._dindex;
        int S = t._sindex;
        int V = t._vindex;

        if(S==0){
            Rect R = _io_north[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), Width, Height/2, 7);
        }
        else if(S==1){
            Rect R = _io_east[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), Width, Height/2, 7);
        }
        else if(S==2){
            Rect R = _io_south[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), Width, Height/2, 7);
        }
        else if(S==3){
            Rect R = _io_west[D][V];
            plotter.AddSegment(R.getCx(), R.getCy(), Width, Height/2, 7);
        }     
    }

    plotter.Plot(fout, "package");
}