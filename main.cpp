#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <iterator>
#include <fstream>
#include <string>

const std::string version = "0.1";

using Ngrams = std::map<std::string, std::vector<std::string>>;

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

Ngrams generate_ngrams(std::ifstream &file, std::size_t order)
{
    // Read file into string
    std::string text = std::string((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());

    // Generate ngrams based off input text
    Ngrams ngrams;
    for (int i = 0; i <= text.length() - order; i++)
    {
        std::string gram = text.substr(i, order);
        if (ngrams.find(gram) == ngrams.end())
            ngrams[gram] = std::vector<std::string>();
        ngrams[gram].push_back(text.substr(i + order, 1));
    }

    return ngrams;
}

// Generate text using ngrams
std::string generate_text(const Ngrams &ngrams, std::size_t order, int text_length)
{
    // Select a random ngram to start generating
    auto random_iterator = std::next(std::begin(ngrams), rand() % ngrams.size());
    std::string current_gram = random_iterator->first;
    std::string result = current_gram;

    for (int i = 0; i < text_length; i++)
    {
        const std::vector<std::string> &possibilities = ngrams.at(current_gram);
        std::string next = *select_randomly(possibilities.begin(), possibilities.end());
        result += next;
        // Perform boundary check
        if (result.length() < order)
        {
            // If the length of result is less than the order, update current_gram accordingly
            current_gram = result;
        }
        else
        {
            // If the length of result is equal to or greater than the order, update current_gram based on the last 'order' characters
            current_gram = result.substr(result.length() - order, order);
        }
        // current_gram = result.substr(result.length() - order, order);
    }

    return result;
}

void write_string(std::ofstream &file, const std::string &str)
{
    std::size_t size = str.size();
    file.write(reinterpret_cast<const char *>(&size), sizeof(std::size_t));
    file.write(str.c_str(), size);
}

void write_ngrams_to_binary(const std::string file_name, const Ngrams &ngrams)
{
    std::ofstream output_file(file_name, std::ios::binary);

    if (!output_file.is_open())
    {
        std::cout << "Failed to open output file.\n";
        return;
    }

    std::size_t size = ngrams.size();
    output_file.write(reinterpret_cast<const char *>(&size), sizeof(std::size_t));

    for (const auto &entry : ngrams)
    {
        write_string(output_file, entry.first);

        const auto &values = entry.second;
        std::size_t values_size = values.size();
        output_file.write(reinterpret_cast<const char *>(&values_size), sizeof(std::size_t));

        for (const auto &value : values)
        {
            write_string(output_file, value);
        }
    }
    output_file.close();
}

std::string read_string(std::ifstream &file)
{
    std::size_t size;
    file.read(reinterpret_cast<char *>(&size), sizeof(std::size_t));

    std::string str;
    str.resize(size);
    file.read(&str[0], size);

    return str;
}

Ngrams read_bin_to_ngrams(std::ifstream &file)
{
    std::size_t size;

    file.read(reinterpret_cast<char *>(&size), sizeof(std::size_t));

    std::map<std::string, std::vector<std::string>> data;

    for (std::size_t i = 0; i < size; ++i)
    {
        std::string key = read_string(file);

        std::size_t values_size;
        file.read(reinterpret_cast<char *>(&values_size), sizeof(std::size_t));

        std::vector<std::string> values;
        for (std::size_t j = 0; j < values_size; ++j)
        {
            values.push_back(read_string(file));
        }

        data[key] = values;
    }
    return data;
}

void write_generated_text_to_file(std::ofstream &file, Ngrams ngrams, std::size_t order, std::size_t length)
{
    file << generate_text(ngrams, order, length);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "No arguments provided. use --help for more info." << std::endl;
        return 0;
    }

    if (std::string(argv[1]) == "--help")
    {
        std::cout << "Available commands:\n\n\t";
        std::cout << "--help\t\tDisplays this screen\n\t";
        std::cout << "--version\tDisplays installed version\n\t";
        std::cout << "new <input file> <order> <output file name>\t\t\tGenerate ngrams from input file\n\t";
        std::cout << "gen <ngram file> <order>\t\t\t\t\tGenerate text to console from ngram file\n\t";
        std::cout << "save <input ngram file> <order> <output file name>\t\tGenerate text and save it to text file\n";
        return 0;
    }

    if (std::string(argv[1]) == "--version")
    {
        std::cout << "markov-chain-text-generator " << version << "\nhttps://github.com/stumburs" << std::endl;
    }

    // Init random seed
    srand(time(nullptr));

    if (std::string(argv[1]) == "new")
    {
        if (argc != 5)
        {
            std::cout << "Not enough arguments supplied." << std::endl;
            return 0;
        }

        std::string input_file_name = std::string(argv[2]);
        std::size_t order = std::stoull(std::string(argv[3]));
        std::string output_file_name = std::string(argv[4]);

        std::ifstream input_file(input_file_name);

        std::cout << "Generating ngrams..." << std::endl;
        Ngrams ngrams = generate_ngrams(input_file, order);

        std::cout << "Writing ngrams to file..." << std::endl;
        write_ngrams_to_binary(output_file_name, ngrams);

        std::cout << "Operation complete!" << std::endl;
        return 0;
    }

    if (std::string(argv[1]) == "gen")
    {
        if (argc != 4)
        {
            std::cout << "Not enough arguments supplied." << std::endl;
            return 0;
        }

        std::string ngram_file = std::string(argv[2]);
        std::size_t order = std::stoull(std::string(argv[3]));

        std::ifstream input_file(ngram_file, std::ios::binary);
        Ngrams ngrams = read_bin_to_ngrams(input_file);
        input_file.close();

        std::cout << generate_text(ngrams, order, 250) << std::endl;
        return 0;
    }

    if (std::string(argv[1]) == "save")
    {
        if (argc != 5)
        {
            std::cout << "Not enough arguments supplied." << std::endl;
            return 0;
        }

        std::string ngram_file = std::string(argv[2]);
        std::size_t order = std::stoull(std::string(argv[3]));
        std::string output_file_name = std::string(argv[4]);

        std::cout << "Loading ngrams..." << std::endl;
        std::ifstream input_file(ngram_file, std::ios::binary);
        Ngrams ngrams = read_bin_to_ngrams(input_file);
        input_file.close();

        std::cout << "Generating text and writing to file..." << std::endl;
        std::ofstream output_file(output_file_name);
        write_generated_text_to_file(output_file, ngrams, order, 250);
        output_file.close();

        std::cout << "Operation complete!" << std::endl;
        return 0;

        return 0;
    }
}