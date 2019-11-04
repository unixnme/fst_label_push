#include <iostream>
#include "fst/vector-fst.h"
#include <string>
#include <unordered_map>

void add_to_fst(fst::StdVectorFst *dictionary, std::istream &is) {
    std::unordered_map<std::string, unsigned int> str2idx;
    unsigned int epsilon = 0;

    for (std::string word; is >> word;) {
        if (str2idx.find(word) != str2idx.end())
            continue;

        auto olabel = str2idx.size() + 1;
        str2idx[word] = olabel;

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

int main(int argc, const char** argv) {
    fst::StdVectorFst dictionary;
    auto start = dictionary.AddState();
    dictionary.SetStart(start);

    if (argc > 1) {
        std::ifstream ifs{argv[1]};
        add_to_fst(&dictionary, ifs);
    } else {
        add_to_fst(&dictionary, std::cin);
    }

    dictionary.Write("dictionary.fst");
    return 0;
}