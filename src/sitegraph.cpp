#include "sitegraph.hpp"
#include <cmath>
#include <iostream>

inline int64_t shrink(int length, int sim_range)
{
    return length / (sim_range + 1);
}

std::vector<std::tuple<Site *, int64_t, std::vector<std::string>>>
Contig::get_first_site(int overlap_length, Contig *from, int simplify_range,
                       int depth, int length)
{
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
    if (sites.size() != 0) {
        // std::cout << "Find" << std::endl; // DEBUG
        if (sites.back()->pos >= overlap_length) {
            int  i    = 0;
            auto site = sites[i];
            while (site->pos < overlap_length)
                site = sites[++i];
            auto total_length = site->pos + length;
            if (total_length >= maxlength)
                return {};
            if (simplify_range >= 0) {
                auto pr = edges_map.equal_range(total_length);
                while (pr.first != pr.second) {
                    if (pr.first->second == from)
                        return {};
                    pr.first++;
                }
                for (int i = 1; i <= simplify_range; i++) {
                    pr = edges_map.equal_range(total_length + i);
                    while (pr.first != pr.second) {
                        if (pr.first->second == from)
                            return {};
                        pr.first++;
                    }
                }
                edges_map.insert({total_length, from});
            }
            return std::vector<
                std::tuple<Site *, int64_t, std::vector<std::string>>>{
                {site, total_length, {name}}};
        }
    }
    // if (had_memo[occurance]) // Memorandum
    // {
    //     // std::cout << "Find memo" << std::endl; // DEBUG
    //     return reachable_site_memo[occurance];
    // }
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
    std::unordered_multimap<Site *, int64_t> lengths;
    for (auto contig : next) {
        if (contig == nullptr) {
            // std::cout << "Nullptr" << std::endl; // DEBUG
            continue;
        }
        auto temp_sites =
            contig->get_first_site(overlap_length, from, simplify_range,
                                   depth + 1, length + sequence.length());
        if (record_path)
            for (auto temp_site_iter = temp_sites.begin();
                 temp_site_iter < temp_sites.end(); temp_site_iter++) {
                // std::get<1>(*temp_site_iter) += sequence.length();
                std::get<2>(*temp_site_iter).push_back(name);
            }
        // if (simplify_range < 0)
        next_sites.insert(next_sites.end(),
                          std::make_move_iterator(temp_sites.begin()),
                          std::make_move_iterator(temp_sites.end()));
        // else
        //     std::copy_if(
        //         std::make_move_iterator(temp_sites.begin()),
        //         std::make_move_iterator(temp_sites.end()),
        //         std::back_inserter(next_sites),
        //         [&simplify_range, &lengths](
        //             const std::tuple<Site *, int64_t,
        //             std::vector<std::string>>
        //                 &v) {
        //             // for (auto l : lengths)
        //             //     if ((l.first == std::get<0>(v)->id) &&
        //             //         (std::abs(l.second - std::get<1>(v)) <=
        //             //          simplify_range))
        //             //         return false;
        //             // lengths.push_back({std::get<0>(v)->id,
        //             std::get<1>(v)}); auto pr =
        //             lengths.equal_range(std::get<0>(v)); while (pr.first !=
        //             pr.second) {
        //                 if (std::abs(pr.first->second - std::get<1>(v)) <=
        //                     simplify_range)
        //                     return false;
        //                 pr.first++;
        //             }
        //             lengths.insert({std::get<0>(v), std::get<1>(v)});
        //             return true;
        //         });
    }
    // reachable_site_memo[occurance] = next_sites;
    // had_memo[occurance]            = true;
    occurance--;
    return /* std::move */ (next_sites);
}