#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <iterator>

// Text for generating ngrams
// https://en.wikipedia.org/wiki/Markov_chain
std::string text = "A Markov chain or Markov process is a stochastic model describing a sequence of possible events in which the probability of each event depends only on the state attained in the previous event.[1]. It is a stochastic process with a series of states that alternate between one another. Probabilities based on the current state of the Markov chain determine these transitions.[2]. Definition of Markov chain in US English by Oxford Dictionaries [3]Informally, this may be thought of as, \"What happens next depends only on the state of affairs now.\" A countably infinite sequence, in which the chain moves state at discrete time steps, gives a discrete-time Markov chain (DTMC). A continuous-time process is called a continuous-time Markov chain (CTMC). It is named after the Russian mathematician Andrey Markov. Markov chains have many applications as statistical models of real-world processes,[4][5][6] such as studying cruise control systems in motor vehicles, queues or lines of customers arriving at an airport, currency exchange rates and animal population dynamics.[7]The maximum probability decision rule can also be used by a Markov chain-based approach to determine the language. Empirical experiments demonstrated that, when compared to the N-gram approach, markov chains could recognize languages faster and with a reduced mistake rate [8]. Markov processes are the basis for general stochastic simulation methods known as Markov chain Monte Carlo, which are used for simulating sampling from complex probability distributions, and have found application in Bayesian statistics, thermodynamics, statistical mechanics, physics, chemistry, economics, finance, signal processing, information theory and speech processing.[7][9][10] The adjectives Markovian and Markov are used to describe something that is related to a Markov process.[11][12][13]";

// How many characters are used to generate ngrams
// Less - more chaotic text
// More - text closer to input text
int order = 3;

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
    std::string current_gram = text.substr(0, order);
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
    // Generate ngrams based off input text
    for (int i = 0; i <= text.length() - order; i++)
    {
        std::string gram = text.substr(i, order);
        if (ngrams.find(gram) == ngrams.end())
            ngrams[gram] = std::vector<std::string>();
        ngrams[gram].push_back(text.substr(i + order, 1));
    }

    // Generate text using ngrams
    for (int i = 0; i < 10; i++)
        std::cout << generate_text(ngrams, 200) << "\n";
}