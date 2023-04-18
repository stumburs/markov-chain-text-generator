#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <iterator>
#include <fstream>

// How many characters are used to generate ngrams
// Less - more chaotic text
// More - text closer to input text
int order = 6;

std::string text;
std::map<std::string, std::vector<std::string>> ngrams;

// Functions to randomly pick an interator.
// Used to randomly pick next possibility when generating text
template <typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator &g)
{
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template <typename Iter>
Iter select_randomly(Iter start, Iter end)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

// Generate text using ngrams
std::string generate_text(std::map<std::string, std::vector<std::string>> ngrams, int text_length = 100)
{
    // Select a random ngram to start generating
    std::string current_gram = text.substr(rand() % text.length(), order);
    std::string result = current_gram;
    for (int i = 0; i < text_length; i++)
    {
        std::vector<std::string> possibilities = ngrams[current_gram];
        std::string next = *select_randomly(possibilities.begin(), possibilities.end());
        result += next;
        current_gram = result.substr(result.length() - order, order);
    }
    return result;
}

int main()
{
    // Read file into string
    std::ifstream file("markov_wiki.txt");
    text = std::string((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();

    // Generate ngrams based off input text
    for (int i = 0; i <= text.length() - order; i++)
    {
        std::string gram = text.substr(i, order);
        if (ngrams.find(gram) == ngrams.end())
            ngrams[gram] = std::vector<std::string>();
        ngrams[gram].push_back(text.substr(i + order, 1));
    }

    // Generate text using ngrams
    for (int i = 0; i < 5; i++)
        std::cout << generate_text(ngrams, 100) << "\n";
}