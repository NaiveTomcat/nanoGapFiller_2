#include "sitegraph.hpp"
#include "utils.hpp"

#include <fstream>
#include <unordered_map>

std::unordered_map<int, Site> Sites;

std::unordered_map<std::string, std::reference_wrapper<Contig>> Contigs;

void construct_assembly_graph(std::string filename, std::string siteseq_1,
                              std::string siteseq_2)
{
    std::ifstream            fin(filename);
    std::string              current_line;
    std::string              current_contig_name;
    std::vector<std::string> current_contig_successor_names;
    while (std::getline(fin, current_line)) {
        if (current_line[0] == '>') { // Edge Header Line
            std::tie(current_contig_name, current_contig_successor_names) =
                get_contig_description(current_line);
            Contigs[current_contig_name] = *new Contig(current_contig_name);
            for (auto n : current_contig_successor_names) {
                Contigs[current_contig_name].get().next.push_back(Contigs[n]);
            }
        } else {
            Contigs[current_contig_name].get().sequence += current_line;
        }
    }
    for (auto contig_iter : Contigs) {
        auto contig = contig_iter.second;
        auto seq    = contig.get().sequence;
        // Search for siteseq_1
        auto pos = seq.find_first_of(siteseq_1);
        while (pos != std::string::npos) {
            contig.get().sites.push_back(*new Site(contig.get().name, pos));
            pos = seq.find_first_of(siteseq_1, pos + 1);
        }
        // Search for siteseq_2
        pos = seq.find_first_of(siteseq_2);
        while (pos != std::string::npos) {
            contig.get().sites.push_back(*new Site(contig.get().name, pos));
            pos = seq.find_first_of(siteseq_2, pos + 1);
        }
        // Sort sites
        std::sort(contig.get().sites.begin(), contig.get().sites.end(),
                  SiteComp);
        // Add edges for sites within the same contig
        for (auto site_iter = contig.get().sites.begin();
             site_iter < contig.get().sites.end() - 1; site_iter++) {
            site_iter->get().edges.push_back(Edge((site_iter + 1)->get()));
            site_iter->get().layer1.push_back((site_iter + 1)->get());
        }
    }
}