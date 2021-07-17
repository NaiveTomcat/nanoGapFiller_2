#pragma once
#ifndef __SPADESREADER_HPP_
#define __SPADESREADER_HPP_

#include <regex>
#include <string>
#include <tuple>
#include <vector>

inline std::string get_contig_name(std::string description) {
    return description.find('\'') == std::string::npos ? description.substr(5, description.find_first_of('_',4) - 3) : description.substr(5, description.find_first_of('_',4) - 3) + 'r';
}

inline std::pair<std::string, std::vector<std::string>>
get_contig_description(std::string contig_header)
{
    // ref:
    // >EDGE_2_length_277_cov_47.1:EDGE_7_length_368_cov_91.000000,EDGE_8_length_304_cov_91.000000,EDGE_9_length_100155_cov_91.000000;
    auto current_contig =
        contig_header.substr(1, contig_header.find_first_of(':') - 1);
    auto reminder =
        contig_header.substr(contig_header.find_first_of(':') + 1,
                             contig_header.find_first_of(';') -
                                 contig_header.find_first_of(':') - 1);
    auto current_name = get_contig_name(current_contig);
    std::vector<std::string> successor = {};
    while (reminder.find_first_of(',') != std::string::npos) {
        successor.push_back(get_contig_name(reminder.substr(0,reminder.find_first_of(','))));
        reminder = reminder.substr(reminder.find_first_of(',')+1);
    }
    successor.push_back(get_contig_name(reminder));
    return std::pair<std::string, std::vector<std::string>>(current_name, successor);
}

std::string export_sitegraph();

void connect_between_contigs();

void construct_assembly_graph(std::string filename, std::string siteseq_1,
                              std::string siteseq_2, int overlap_length = 127);

static std::unordered_map<int, Site> Sites;

static std::unordered_map<std::string, std::reference_wrapper<Contig>> Contigs;

#endif