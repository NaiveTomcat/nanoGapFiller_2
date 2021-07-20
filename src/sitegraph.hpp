#pragma once
#ifndef _SITEGRAPH_H_
#define _SITEGRAPH_H_

#include <functional>
#include <string>
#include <tuple>
#include <vector>

// Global flags
static int64_t maxcontig   = 40;
static int64_t maxoccur    = 4;
static bool    find_layers = false;
static bool    record_path = false;

class Site;

class Edge
{
  public:
    Site *                   to;
    int                      len;
    std::vector<std::string> via;
    static int               count;

    inline Edge(Site *b, int len_) : to(b), len(len_) { count++; }
};

class Site
{
  public:
    static int total;

    int               id;
    int64_t          pos;
    std::string       of;
    std::vector<Edge*> edges;
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
    std::vector<std::tuple<Site *, int64_t, std::vector<std::string>>>
        reachable_site_memo;

  public:
    std::string           name;
    std::vector<Contig *> next;
    std::string           sequence;
    std::vector<Site *>   sites;

    Contig(std::string n) : name(n) {}
    std::vector<std::tuple<Site *, int64_t, std::vector<std::string>>>
    get_first_site();
};

#endif