#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>

using namespace std;

enum class Options
{
    LINES,
    WORDS,
    CHARS,
    BYTES
};

static map <char, Options> ShortOpt = {
    { 'l', Options::LINES },
    { 'w', Options::WORDS },
    { 'm', Options::CHARS },
    { 'c', Options::BYTES }
};

static map <string, Options> LongOpt = {
    { "lines", Options::LINES },
    { "words", Options::WORDS },
    { "chars", Options::CHARS },
    { "bytes", Options::BYTES }
};

static map <Options, string> OptName =
{
    { Options::LINES, "Lines" },
    { Options::WORDS, "Words" },
    { Options::CHARS, "Chars" },
    { Options::BYTES, "Bytes" }
};

class OptionsParser
{
private:
    vector<string> filenames;
    vector<Options> options;
    static string ArgParse(string arg)
    {
        while (arg[0] == '-')
        {
            arg.erase(0, 1);
        }
        return arg;
    }

    void AddDefaultOpt()
    {
        options.push_back(Options::LINES);
        options.push_back(Options::WORDS);
        options.push_back(Options::BYTES);
    }

    void OptionParse(char* argv[], int& indargv)
    {
        if (argv[indargv][0] != '-')
        {
            filenames.push_back(static_cast<string>(argv[indargv]));
        }
        else if (argv[indargv][1] == '-')
        {
            string arg = ArgParse(argv[indargv]);
            options.push_back(LongOpt[arg]);
        }
        else
        {
            string args = ArgParse(argv[indargv]);
            for (char arg : args)
                options.push_back(ShortOpt[arg]);
        }
    }
public:
    OptionsParser(const int& argc, char* argv[])
    {
        int indargv = 1;
        for (;indargv < argc;indargv++)
            OptionParse(argv, indargv);
        if (options.empty())
            AddDefaultOpt();
    }

    vector<Options> GetOptions()
    {
        return options;
    }

    vector<string> GetFilenames()
    {
        return filenames;
    }
};

void WriteFileData(const string& filename, const map<Options, int>& filedata)
{
    cout << endl << filename << endl;
    for (auto pair_option_count : filedata)
    {
        cout << OptName[pair_option_count.first] << ": " << pair_option_count.second << endl;
    }
}

long TextCounter(Options option, const string& filename)
{
    long count = 0;
    char sim;
    string word;
    ifstream fin(filename);
    while (!(fin.eof()))
    {
        sim = static_cast<char>(fin.get());
        switch (option)
        {
            case Options::LINES:
                if (sim == '\n' || sim == EOF)
                    count++;
                break;
            case Options::WORDS:
                if (sim == ' ' || sim == '\n' || sim == '\r' || sim == '\t' || sim == EOF)
                {
                    if (!word.empty())
                        count++;
                    word = "";
                } else
                {
                    word += sim;
                }
                break;
            case Options::CHARS:
                if (isalpha(sim))
                    count++;
                break;
        }
    }
    fin.close();
    return count;
}

long BinCounter(const string& filename)
{
    long count = 0;
    ifstream fin(filename, ios::binary);
    fin.get();
    while (!(fin.eof()))
    {
        fin.get();
        count++;
    }
    fin.close();
    return count;
}

long CounterChooser(const string& filename, Options option)
{
    switch (option)
    {
    case Options::LINES:
    case Options::CHARS:
    case Options::WORDS:
        return TextCounter(option, filename);
    case Options::BYTES:
        return BinCounter(filename);
    }
}

int main(int argc, char* argv[])
{
    OptionsParser optionsParser = OptionsParser(argc, argv);
    for (const string& filename : optionsParser.GetFilenames())
    {
        map<Options, int> filedata;
        for (Options option : optionsParser.GetOptions())
            filedata[option] = CounterChooser(filename, option);

        WriteFileData(filename, filedata);
    }
}