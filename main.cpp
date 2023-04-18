// https://youtu.be/eGFJ8vugIWA?t=845

#include <iostream>
#include <vector>
#include <map>

std::string text = "the theremin is theirs, ok? yes, it is. this is a theremin.";
int order = 3;

std::map<std::string, int> ngrams;

int main()
{
    for (int i = 0; i <= text.length() - order; i++)
    {
        std::string gram = text.substr(i, 3);
        if (!ngrams.contains(gram))
            ngrams.insert({gram, 1});
        else
            ngrams[gram]++;
    }

    for (const auto &[key, value] : ngrams)
    {
        std::cout << "\"" << key << "\" - " << value << "\n";
    }
}