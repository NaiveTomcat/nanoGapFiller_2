#include "sitegraph.hpp"
#include "utils.hpp"
#include <chrono>
#include <fstream>
#include <iostream>

using namespace std;

std::unordered_map<int, Site *>           Sites;
std::unordered_map<std::string, Contig *> Contigs;
std::unordered_set<Edge *>                Edges;

int     Site::total            = 0;
int     Edge::count            = 0;
int64_t Contig::maxocc_achived = 0;

int main(int argc, char **argv)
{
    int     count   = 1;
    string  inFile  = "assembly_graph.fastg";
    string  outFile = "Output.sitegraph";
    string  s1      = "GCTCTTC";
    string  s2      = "GAAGAGC";
    int64_t overlap = 77;
    while (count < argc) {
        const string flag = argv[count];
        if (flag == "-i")
            inFile = argv[count + 1];
        else if (flag == "-o")
            outFile = argv[count + 1];
        else if (flag == "-l")
            overlap = stoi(argv[count + 1]);
        else if (flag == "-r")
            maxoccur = stoi(argv[count + 1]);
        else if (flag == "-s1")
            s1 = argv[count + 1];
        else if (flag == "-s2")
            s2 = argv[count + 1];
        // else if (flag == "-fl")
        // 	findLayer = stoi(argv[count + 1]);
        // else if (flag == "-w")
        // 	toWrite = stoi(argv[count + 1]);
        else {
            cout << "-i inFile  -o outFile -r occurCtrl -l overlap -s1 "
                    "sequence1 -s2 sequence2"
                 << endl;
            return -1;
        }
        count += 2;
    }

    std::chrono::steady_clock clock;

    auto start_time = clock.now();

    construct_assembly_graph(inFile, s1, s2, overlap);

    auto assembled_time = clock.now();
    cout << "Time preprocess: " << (assembled_time - start_time) * 1.0 / 1s
         << endl;

    connect_between_contigs();

    cout << "Maximum occurance for a contig: " << Contig::maxocc_achived
         << endl;

    auto connected_time = clock.now();
    cout << "Time constructing: "
         << (connected_time - assembled_time) * 1.0 / 1s << endl;

    ofstream fout(outFile);

    for (int i = 0; i < argc; i++)
        fout << argv[i] << " ";

    fout << endl
         << "Contigs: " << Contigs.size() << endl
         << "Sites: " /* << Site::total  */ << Sites.size() << endl
         << "Edges: " /* << Edge::count  */ << Edges.size() << endl;

    fout << "Time preprocess: " << (assembled_time - start_time) * 1.0 / 1s
         << endl
         << "Time constructing: "
         << (connected_time - assembled_time) * 1.0 / 1s << endl;

    fout << export_sitegraph();

    auto finish_time = clock.now();
    cout << "Time Writing: " << (finish_time - connected_time) * 1.0 / 1s
         << endl;
    cout << "Total time: " << (finish_time - start_time) * 1.0 / 1s << endl;

    return 0;
}