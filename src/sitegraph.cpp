#include "sitegraph.hpp"
#include <iostream>

std::vector<std::tuple<Site *, int64_t, std::vector<std::string>>>
Contig::get_first_site(int depth)
{
    if (sites.size() != 0) {
        // std::cout << "Find" << std::endl; // DEBUG
        return std::vector<
            std::tuple<Site *, int64_t, std::vector<std::string>>>{
            {sites[0], sites[0]->pos, {name}}};
    }
    if (had_memo) // Memorandum
    {
        // std::cout << "Find memo" << std::endl; // DEBUG
        return reachable_site_memo;
    }
    if (occurance >= maxoccur) {
        // std::cout << "Too many for one contig" << std::endl; // DEBUG
        return std::vector<
            std::tuple<Site *, int64_t, std::vector<std::string>>>{};
    }
    if (depth >= maxcontig) {
        // std::cout << "Too many contigs for a path" << std::endl; // DEBUG
        return std::vector<
            std::tuple<Site *, int64_t, std::vector<std::string>>>{};
    }
    if (next.size() == 0) {
        // std::cout << "No successor" << std::endl; // DEBUG
        return std::vector<
            std::tuple<Site *, int64_t, std::vector<std::string>>>{};
    }
    occurance++;
    maxocc_achived = occurance > maxocc_achived ? occurance : maxocc_achived;
    // DFS
    // std::cout << "DFS at depth " << depth << std::endl; // DEBUG
    std::vector<std::tuple<Site *, int64_t, std::vector<std::string>>>
        next_sites;
    for (auto contig : next) {
        if (contig == nullptr) {
            // std::cout << "Nullptr" << std::endl; // DEBUG
            continue;
        }
        auto temp_sites = contig->get_first_site(depth + 1);
        for (auto temp_site_iter = temp_sites.begin();
             temp_site_iter < temp_sites.end(); temp_site_iter++) {
            std::get<1>(*temp_site_iter) += sequence.length();
            std::get<2>(*temp_site_iter).push_back(name);
        }
        next_sites.insert(next_sites.end(),
                          std::make_move_iterator(temp_sites.begin()),
                          std::make_move_iterator(temp_sites.end()));
    }
    occurance--;
    reachable_site_memo = next_sites;
    had_memo            = true;
    // std::cout << "DFS at depth " << depth << " got answer" << std::endl; // DEBUG
    return /* std::move */ (next_sites);
}