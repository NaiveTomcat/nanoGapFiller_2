#include "sitegraph.hpp"
#include "utils.hpp"

#include <fstream>
#include <sstream>
#include <unordered_map>

void construct_assembly_graph(std::string filename, std::string siteseq_1,
                              std::string siteseq_2, int overlap_length)
{
    std::ifstream            fin(filename);
    std::string              current_line;
    std::string              current_contig_name;
    std::vector<std::string> current_contig_successor_names;
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
                Contigs[current_contig_name]->next.push_back(Contigs[n]);
            }
        } else {
            Contigs[current_contig_name]->sequence += current_line;
        }
    }
    std::vector<std::string> to_be_erased;
    for (auto contig_iter : Contigs) {
        auto contig = contig_iter.second;
        if (contig == nullptr) {
            to_be_erased.push_back(contig_iter.first);
            continue;
        }
        auto seq    = contig->sequence;
        seq = seq.substr(0, seq.length() - overlap_length + siteseq_1.length());
        contig->sequence = seq;
        // Search sites
        {
            // Search for siteseq_1
            auto pos = seq.find_first_of(siteseq_1);
            while (pos != std::string::npos) {
                contig->sites.push_back(new Site(contig->name, pos));
                pos = seq.find_first_of(siteseq_1, pos + 1);
            }
            // Search for siteseq_2
            pos = seq.find_first_of(siteseq_2);
            while (pos != std::string::npos) {
                contig->sites.push_back(new Site(contig->name, pos));
                pos = seq.find_first_of(siteseq_2, pos + 1);
            }
        }
        // Sort sites
        std::sort(contig->sites.begin(), contig->sites.end(),
                  SiteComp);
        // Add edges for sites within the same contig
        for (auto site_iter = contig->sites.begin();
             site_iter < contig->sites.end() - 1; site_iter++) {
            (*site_iter)->edges.push_back(
                Edge(*(site_iter + 1),
                     (*(site_iter + 1))->pos - (*site_iter)->pos));
        }
    }
    for (auto key: to_be_erased)
        Contigs.erase(key);
}

void connect_between_contigs()
{
    for (auto contig_iter : Contigs) {
        auto contig = contig_iter.second;
        auto  back_dist =
            contig->sequence.length() - contig->sites.back()->pos;
        for (auto next : contig->next) {
            for (auto site_tuple : next->get_first_site()) {
                contig->sites.back()->edges.push_back(
                    Edge(std::get<0>(site_tuple),
                         back_dist + std::get<1>(site_tuple)));
            }
        }
    }
}

std::string export_sitegraph()
{
    std::ostringstream sout;
    for (auto site_iter : Sites) {
        auto site = site_iter.second;
        for (Edge edge : site.edges) {
            sout << site.id << ' ' << edge.to->id << ' ' << edge.len << " :";
            for (auto via : edge.via)
                sout << ' ' << via;
            sout << std::endl;
        }
    }
    return sout.str();
}
