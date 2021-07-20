#include "sitegraph.hpp"
#include <iostream>

std::vector<std::tuple<Site *, int64_t, std::vector<std::string>>>
Contig::get_first_site(int overlap_length, int depth, int length)
{
    if (sites.size() != 0) {
        // std::cout << "Find" << std::endl; // DEBUG
        if (sites.back()->pos >= overlap_length) {
            int  i    = 0;
            auto site = sites[i];
            while (site->pos < overlap_length)
                site = sites[++i];
            return std::vector<
                std::tuple<Site *, int64_t, std::vector<std::string>>>{
                {site, site->pos, {name}}};
        }
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
    if (length >= maxlength) {
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
        auto temp_sites = contig->get_first_site(overlap_length, depth + 1, length + sequence.length());
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
    // std::cout << "DFS at depth " << depth << " got answer" << std::endl; //
    // DEBUG
    return /* std::move */ (next_sites);
}