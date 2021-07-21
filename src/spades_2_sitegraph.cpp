#include "sitegraph.hpp"
#include "utils.hpp"

#include <fstream>
#include <sstream>
#include <unordered_map>

#include <iostream>

extern std::unordered_map<int, Site *> Sites;

extern std::unordered_map<std::string, Contig *> Contigs;

extern std::unordered_set<Edge *> Edges;

void construct_assembly_graph(std::string filename, std::string siteseq_1,
                              std::string siteseq_2, int overlap_length)
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
        contig->sequence = seq.substr(overlap_length - siteseq_1.length());
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

void connect_between_contigs(int overlap_length)
{
    for (auto contig_iter : Contigs) {
        auto contig = contig_iter.second;
        if (contig->sites.size() == 0)
            continue;
        auto back_dist = contig->sequence.length() - contig->sites.back()->pos;
        if (contig->next.size() != 0)
            for (auto next : contig->next) {
                if (next == nullptr)
                    continue;
                auto site_tuples = next->get_first_site(overlap_length);
                for (auto site_tuple : site_tuples) {
                    auto edge = new Edge(std::get<0>(site_tuple),
                                         back_dist + std::get<1>(site_tuple));
                    edge->via = std::get<2>(site_tuple);
                    contig->sites.back()->edges.push_back(edge);
                    Edges.insert(edge);
                }
            }
        // else std::cout << "No successor" << std::endl;
        // std::cout << std::endl;
    }
}

std::string export_sitegraph()
{
    std::ostringstream sout;
    for (auto site_iter : Sites) {
        auto site = site_iter.second;
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
    return sout.str();
}
