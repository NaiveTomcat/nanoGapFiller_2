#pragma once
#ifndef _SITEGRAPH_H_
#define _SITEGRAPH_H_

#include <functional>
#include <string>
#include <tuple>
#include <vector>

class Site;

class Edge
{
  public:
    Site &                   to;
    int                      len;
    std::vector<std::string> via;

    inline Edge(Site &b) : to(b) {}
};

class Site
{
  public:
    static int total;

    int                                       id;
    uint_fast64_t                             pos;
    std::string                               of;
    std::vector<Edge>                         edges;
    std::vector<std::reference_wrapper<Site>> layer1;
    std::vector<std::reference_wrapper<Site>> layer2;
    std::vector<std::reference_wrapper<Site>> layer3;

    Site(std::string of_, uint_fast64_t pos_) : of(of_), pos(pos_) { id = ++total; }
};

inline bool SiteComp(const Site &a, const Site &b) { return a.id < b.id; }

int Site::total = 0;

class Contig
{
  public:
    std::string                                 name;
    std::vector<std::reference_wrapper<Contig>> next;
    std::string                                 sequence;
    std::vector<std::reference_wrapper<Site>>   sites;

    Contig(std::string n) : name(n) {}
};

#endif