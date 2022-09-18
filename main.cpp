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

void WriteFileData(const string& filename, const map<Options, int>& filedata)
{
    cout << endl << filename << endl;
    for (auto pair_option_count : filedata)
    {
        cout << OptName[pair_option_count.first] << ": " << pair_option_count.second << endl;
    }
}

string ArgParse(string arg)
{
    while (arg[0] == '-')
    {
        arg.erase(0, 1);
    }
    return arg;
}

void AddDefaultOpt(vector<Options> options)
{
    options.push_back(Options::LINES);
    options.push_back(Options::WORDS);
    options.push_back(Options::BYTES);
}

bool TryOptionParse(const string& argcmd, vector<Options>& options)
{
    if (argcmd == "-" || argcmd == "--" || argcmd[0] != '-')
    {
        if (options.empty())
            AddDefaultOpt(options);
        return false;
    }
    else if (argcmd[1] == '-')
    {
        string arg = ArgParse(argcmd);
        options.push_back(LongOpt[arg]);
    }
    else
    {
        string args = ArgParse(argcmd);
        for (char arg : args)
            options.push_back(ShortOpt[arg]);
    }
    return true;
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
    vector<Options> options;
    int argvind = 1;
    while (argvind < argc && TryOptionParse(argv[argvind], options))
        argvind++;

    if (argvind == argc)
    {
        cout << "Program requires filename." << std::endl;
    }
    else
    {
        for (; argvind < argc; argvind++)
        {
            string filename = argv[argvind];
            map<Options, int> filedata;
            for (Options option : options)
                filedata[option] = CounterChooser(filename, option);

            WriteFileData(filename, filedata);
        }
    }
}