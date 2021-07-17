#include "sitegraph.hpp"

std::vector<std::tuple<Site *, int64_t, std::vector<std::string>>>
Contig::get_first_site()
{
    if (!sites.empty())
        return std::vector<
            std::tuple<Site *, int64_t, std::vector<std::string>>>{
            {&(sites[0].get()), sites[0].get().pos, {name}}};
    if (!reachable_site_memo.empty()) // Memorandum
        return reachable_site_memo;
    if (occurance > maxoccur)
        return std::vector<
            std::tuple<Site *, int64_t, std::vector<std::string>>>{};
    occurance++;
    // DFS
    std::vector<std::tuple<Site *, int64_t, std::vector<std::string>>>
        next_sites;
    for (auto contig : next) {
        auto temp_sites = contig.get().get_first_site();
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
    return std::move(next_sites);
}