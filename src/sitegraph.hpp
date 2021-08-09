#pragma once
#ifndef _SITEGRAPH_H_
#define _SITEGRAPH_H_

#include <functional>
#include <string>
#include <tuple>
#include <vector>

// Global flags
extern int64_t maxcontig   ;
extern int64_t maxoccur    ;
extern int64_t maxlength   ;
extern bool    find_layers ;
extern bool    record_path ;

class Site;

class Edge
{
  public:
    Site *                   to;
    int64_t                  len;
    std::vector<std::string> via; // NOTE: Contig names in reverse order
    static int               count;

    inline Edge(Site *b, int64_t len_) : to(b), len(len_) { count++; }
};

class Site
{
  public:
    static int total;

    int                                       id;
    int64_t                                   pos;
    std::string                               of;
    std::vector<Edge *>                       edges;
    std::unordered_multimap<int64_t, int64_t> edgelengths;
    // std::vector<std::reference_wrapper<Site>> layer1;
    // std::vector<std::reference_wrapper<Site>> layer2;
    // std::vector<std::reference_wrapper<Site>> layer3;

    Site(std::string of_, uint64_t pos_) : of(of_), pos(pos_) { id = ++total; }
};

inline bool SiteComp(const Site *a, const Site *b) { return a->id < b->id; }

class Contig
{
  private:
    int64_t occurance;
    // std::unordered_map<
    //     int, std::vector<std::tuple<Site *, int64_t,
    //     std::vector<std::string>>>> reachable_site_memo;
    // // bool had_memo;
    // std::unordered_map<int, bool> had_memo;
    std::unordered_multimap<int64_t, Contig*> edges_map;

  public:
    std::string              name;
    std::vector<Contig *>    next;
    std::vector<std::string> next_names;
    std::string              sequence;
    std::vector<Site *>      sites;

    static int64_t maxocc_achived;

    Contig(std::string n) : name(n), occurance(0) {}
    std::vector<std::tuple<Site *, int64_t, std::vector<std::string>>>
    get_first_site(int overlap_length, Contig *from, int simplify_range = -1,
                   int depth = 0, int length = 0);
};

#endif