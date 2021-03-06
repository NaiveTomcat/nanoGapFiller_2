#include "sitegraph.hpp"
#include "utils.hpp"

#include <fstream>
#include <sstream>
#include <unordered_map>

#include <iostream>

extern std::unordered_map<int, Site *> Sites;

extern std::unordered_map<std::string, Contig *> Contigs;

extern std::unordered_set<Edge *> Edges;

void construct_assembly_graph(const std::string &filename,
                              const std::string &siteseq_1,
                              const std::string &siteseq_2,
                              const int &        overlap_length)
{
    std::ifstream            fin(filename);
    std::string              current_line;
    std::string              current_contig_name;
    std::vector<std::string> current_contig_successor_names;

    // std::ofstream csvdump("sites.csv"); // DEBUG

    while (std::getline(fin, current_line)) {
        if (current_line[0] == '>') { // Edge Header Line
            std::tie(current_contig_name, current_contig_successor_names) =
                get_contig_description(current_line);
            // Contigs.insert({current_contig_name,
            //                 new Contig(current_contig_name)});
            // Contigs[current_contig_name] = std::reference_wrapper<Contig>(
            //     std::ref(*(new Contig(current_contig_name))));
            Contigs[current_contig_name] = new Contig(current_contig_name);
            for (auto n : current_contig_successor_names) {
                Contigs[current_contig_name]->next_names.push_back(n);
            }
        } else {
            Contigs[current_contig_name]->sequence += current_line;
        }
    }

    for (auto contig_iter : Contigs) {
        auto contig = contig_iter.second;
        for (auto n : contig->next_names) {
            contig->next.push_back(Contigs[n]);
        }
    }

    std::cout << "File read." << std::endl;

    std::vector<std::string> to_be_erased;

    std::vector<std::size_t> sites_location{};

    for (auto contig_iter : Contigs) {
        auto contig = contig_iter.second;
        if (contig == nullptr) {
            to_be_erased.push_back(contig_iter.first);
            continue;
        }
        auto seq = contig->sequence;
        // seq = seq.substr(0, seq.length() - overlap_length +
        // siteseq_1.length());
        contig->sequence =
            seq.substr(overlap_length /*  - siteseq_1.length() */);
        // Search sites
        {
            // Search for siteseq_1
            sites_location = {};
            auto pos       = seq.find(siteseq_1);
            while (pos != std::string::npos) {
                // auto site = new Site(contig->name, pos);
                // contig->sites.push_back(site);
                // Sites[site->id] = site;
                sites_location.push_back(pos);
                pos = seq.find(siteseq_1, pos + 1);
            }
            // Search for siteseq_2
            pos = seq.find(siteseq_2);
            while (pos != std::string::npos) {
                // auto site = new Site(contig->name, pos);
                // contig->sites.push_back(site);
                // Sites[site->id] = site;
                sites_location.push_back(pos);
                pos = seq.find(siteseq_2, pos + 1);
            }
            std::sort(sites_location.begin(), sites_location.end());
            for (auto pos : sites_location) {
                auto site = new Site(contig->name, pos);
                contig->sites.push_back(site);
                Sites[site->id] = site;
            }
        }
        // Sort sites
        if (contig->sites.size() != 0) {
            // std::sort(contig->sites.begin(), contig->sites.end(), SiteComp);
            // Add edges for sites within the same contig
            for (auto site_iter = contig->sites.begin();
                 site_iter < contig->sites.end() - 1; site_iter++) {
                auto edge = new Edge(*(site_iter + 1), (*(site_iter + 1))->pos -
                                                           (*site_iter)->pos);
                (*site_iter)->edges.push_back(edge);
                Edges.insert(edge);
            }
        }
        // csvdump << contig->name << "," << contig->sites.size() << std::endl;
    }
    for (auto key : to_be_erased)
        Contigs.erase(key);
}

void connect_between_contigs(int overlap_length, int64_t simplify)
{
    std::cout << "Simplify graph using parameter " << simplify << std::endl;
    for (auto contig_iter : Contigs) {
        auto contig = contig_iter.second;
        if (contig->sites.size() == 0)
            continue;
        auto back_dist = contig->sequence.length() - contig->sites.back()->pos;
        if (contig->next.size() != 0)
            for (auto next : contig->next) {
                if (next == nullptr)
                    continue;
                std::vector<
                    std::tuple<Site *, int64_t, std::vector<std::string>>>
                    site_tuples;
                site_tuples =
                    next->get_first_site(overlap_length, contig, simplify);
                for (auto site_tuple : site_tuples) {
                    if (back_dist + std::get<1>(site_tuple) >= maxlength)
                        continue;
                    auto edge = new Edge(std::get<0>(site_tuple),
                                         back_dist + std::get<1>(site_tuple));
                    edge->via = std::get<2>(site_tuple);
                    contig->sites.back()->edges.push_back(edge);
                    if (simplify >= 0)
                        contig->sites.back()->edgelengths.insert(
                            {std::get<0>(site_tuple)->id,
                             back_dist + std::get<1>(site_tuple)});
                    Edges.insert(edge);
                }
            }
        // else std::cout << "No successor" << std::endl;
        // std::cout << std::endl;
    }
}

std::tuple<std::string, int, int, int> export_sitegraph()
{
    std::ostringstream sout;
    int                totald1 = 0, totald2 = 0, totald3 = 0;
    for (auto site_iter : Sites) {
        auto site = site_iter.second;
        int  deg2 = 0, deg3 = 0;
        if (find_layers) {
            sout << "Site has " << site->edges.size() << " out degree."
                 << std::endl;
            for (auto edge : site->edges) {
                deg2 += edge->to->edges.size();
                for (auto edge3 : edge->to->edges)
                    deg3 += edge3->to->edges.size();
            }
            sout << deg2 << " layer 2 and " << deg3 << " layer 3." << std::endl;
            totald1 += site->edges.size();
            totald2 += deg2;
            totald3 += deg3;
        }
        if (record_path) {
            for (auto edge : site->edges) {
                sout << site->id << " of " << site->of << ", " << edge->to->id
                     << " of " << edge->to->of << ", " << edge->len << " :";
                // for (auto via : edge->via)
                //     sout << ' ' << via;
                sout << ' ' << site->of;
                for (auto v = edge->via.rbegin(); v < edge->via.rend(); v++)
                    sout << ' ' << *v;
                sout << std::endl;
            }
        }
        sout << std::endl;
    }
    return std::make_tuple(sout.str(), totald1, totald2, totald3);
}
