#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>

#include "fst/vector-fst.h"
#include "fst/fstlib.h"


static const unsigned int epsilon = 0;
static std::unordered_map<std::string, unsigned int> str2idx;
static std::vector<std::string> idx2str{std::string()};

void add_to_fst(fst::StdVectorFst *dictionary, std::istream &is) {
    for (std::string word; is >> word;) {
        if (str2idx.find(word) != str2idx.end())
            continue;

        auto olabel = str2idx.size() + 1;
        str2idx[word] = olabel;
        idx2str.emplace_back(word);

        auto src = dictionary->Start();
        auto dst = dictionary->AddState();

        // first arc
        dictionary->AddArc(src, fst::StdArc(word.front(), olabel, 0, dst));
        src = dst;

        for (size_t i = 1; i < word.size(); ++i) {
            dst = dictionary->AddState();
            dictionary->AddArc(src, fst::StdArc(word[i], epsilon, 0, dst));
            src = dst;
        }

        dictionary->SetFinal(src, 0);
    }
}

std::unordered_map<std::string, int> get_stats(fst::StdVectorFst *dictionary) {
    std::unordered_map<std::string, int> label_pushed_words;
    int invalid = 0;

    auto start = dictionary->Start();
    std::vector<std::pair<int, int>> queue{{start, 0}};
    while (!queue.empty()) {
        auto src = queue.back().first;
        auto depth = queue.back().second;
        queue.pop_back();

        for (fst::ArcIterator<fst::StdVectorFst> arcIterator{*dictionary, src};
             !arcIterator.Done(); arcIterator.Next()) {
            const auto arc = arcIterator.Value();
            if (arc.olabel == epsilon)
                queue.emplace_back(arc.nextstate, depth + 1);
            else if (dictionary->Final(arc.nextstate) == fst::TropicalWeight::Zero()) {
                const std::string &word = idx2str[arc.olabel];
                label_pushed_words[word] = word.size() - depth - 1;
            }
            else
                ++invalid;
        }
    }

    if (label_pushed_words.size() + invalid != str2idx.size())
        throw std::runtime_error("some words not reached");
    return label_pushed_words;
}

int main(int argc, const char **argv) {
    fst::StdVectorFst dictionary;
    auto start = dictionary.AddState();
    dictionary.SetStart(start);

    if (argc > 1) {
        std::ifstream ifs{argv[1]};
        add_to_fst(&dictionary, ifs);
    } else {
        add_to_fst(&dictionary, std::cin);
    }

    fst::Determinize(dictionary, &dictionary);
//    fst::Minimize(&dictionary);

    auto result = get_stats(&dictionary);
    for (const auto &item : result) {
        std::cout << item.first << " " << item.second << std::endl;
    }

//    dictionary.Write("dictionary.fst");
    return 0;
}