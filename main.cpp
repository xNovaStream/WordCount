#include <iostream>
#include <vector>
#include <fstream>
#include <map>

using namespace std;

enum class Options
{
    LINES,
    WORDS,
    SUBSTRING,
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
    { "bytes", Options::BYTES },
    { "substring", Options::SUBSTRING}
};

static map <Options, string> OptName =
{
    { Options::LINES, "Lines" },
    { Options::WORDS, "Words" },
    { Options::CHARS, "Chars" },
    { Options::BYTES, "Bytes" },
    { Options::SUBSTRING, "Substring" }
};

class OptionsParser
{
private:
    vector<string> filenames;
    vector<Options> options;
    map<Options, string> modifiers;
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
            if (arg.find('=') != string::npos)
            {
                string modifier = arg.substr(arg.find('=') + 1);
                arg = arg.substr(0, arg.find('='));
                modifiers[LongOpt[arg]] = modifier;
            }
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

    map<Options, string> GetModifiers()
    {
        return modifiers;
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

long TextCounter(Options option, const string& filename, const string& modifier)
{
    long count = 0;
    char sim;
    string substring;
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
                    if (!substring.empty())
                        count++;
                    substring = "";
                } else
                {
                    substring += sim;
                }
                break;
            case Options::CHARS:
                if (isalpha(sim))
                    count++;
                break;
            case Options::SUBSTRING:
                if (modifier.empty())
                {
                    throw invalid_argument("Modifier for --substring can not be empty");
                }
                else
                {
                    if (substring.length() == modifier.length())
                    {
                        if (substring == modifier)
                            count++;
                        substring = substring.erase(0, 1);
                    }
                    substring += sim;
                }
                break;
            default:
                throw invalid_argument("Using option is not for text");
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

long CounterChooser(Options option, const string& filename, const string& modifier)
{
    switch (option)
    {
    case Options::LINES:
    case Options::CHARS:
    case Options::WORDS:
    case Options::SUBSTRING:
        return TextCounter(option, filename, modifier);
    case Options::BYTES:
        return BinCounter(filename);
    }
}

string GetModifier(const map<Options, string>& modifiers, Options option)
{
    if (modifiers.count(option))
        return modifiers.at(option);
    else
        return "";
}

int main(int argc, char* argv[])
{
    OptionsParser optionsParser = OptionsParser(argc, argv);
    for (const string& filename : optionsParser.GetFilenames())
    {
        map<Options, int> filedata;
        for (Options option : optionsParser.GetOptions())
        {
            string modifier = GetModifier(optionsParser.GetModifiers(), option);
            filedata[option] = CounterChooser(option, filename, modifier);
        }

        WriteFileData(filename, filedata);
    }
}