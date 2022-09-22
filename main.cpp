#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <filesystem>

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
    { "substring", Options::SUBSTRING }
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
    static string PrefixDelete(string arg)
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
            string arg = PrefixDelete(argv[indargv]);
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
            string args = PrefixDelete(argv[indargv]);
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

void WriteFileData(const string& filename, const map<Options, unsigned long long>& filedata)
{
    cout << endl << filename << endl;
    for (auto pair_option_count : filedata)
    {
        cout << OptName[pair_option_count.first] << ": " << pair_option_count.second << endl;
    }
}

void SetStreamDefault(ifstream& fin)
{
    fin.clear();
    fin.seekg(0);
}

unsigned long long LinesCount(ifstream& fin)
{
    unsigned long long count = 0;
    char sim;
    while (!(fin.eof()))
    {
        sim = static_cast<char>(fin.get());
        if (sim == '\n' || sim == EOF)
            count++;
    }
    SetStreamDefault(fin);
    return count;
}

unsigned long long WordsCount(ifstream& fin)
{
    unsigned long long count = 0;
    char sim;
    bool isempty = false;
    while (!fin.eof())
    {
        sim = static_cast<char>(fin.get());
        if (sim == ' ' || sim == '\n' || sim == '\r' || sim == '\t' || sim == EOF)
        {
            if(isempty)
                count++;
            isempty = false;
        } else
        {
            isempty = true;
        }
    }
    SetStreamDefault(fin);
    return count;
}

unsigned long long CharsCount(ifstream& fin)
{
    unsigned long long count = 0;
    char sim;
    while (!(fin.eof()))
    {
        sim = static_cast<char>(fin.get());
        if (isprint(sim))
            count++;
    }
    SetStreamDefault(fin);
    return count;
}

unsigned long long SubstringCount(ifstream& fin, const string& modifier)
{
    unsigned long long count = 0;
    char sim;
    string substring;
    while (!(fin.eof()))
    {
        sim = static_cast<char>(fin.get());
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
    }
    SetStreamDefault(fin);
    return count;
}

unsigned long long BytesCount(const string& filename)
{
    return filesystem::file_size(filename);
}

unsigned long long ChooseCounter(Options option, const string& modifier, const string& filename, ifstream& fin)
{
    switch (option)
    {
        case Options::LINES:
            return LinesCount(fin);
        case Options::WORDS:
            return WordsCount(fin);
        case Options::SUBSTRING:
            return SubstringCount(fin, modifier);
        case Options::CHARS:
            return CharsCount(fin);
        case Options::BYTES:
            return BytesCount(filename);
    }
}

string GetModifier(const map<Options, string>& modifiers, Options option)
{
    if (modifiers.count(option))
        return modifiers.at(option);
    else
        return "";
}

void WriteFailFileOpened(const string& filename)
{
    cout << endl << filename << endl;
    cout << "File can not be opened" << endl;
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");

    OptionsParser optionsParser = OptionsParser(argc, argv);
    for (const string& filename : optionsParser.GetFilenames())
    {
        ifstream fin(filename);
        if (fin.fail())
        {
            WriteFailFileOpened(filename);
        }
        else
        {
            map<Options, unsigned long long> filedata;
            for (Options option: optionsParser.GetOptions()) {
                string modifier = GetModifier(optionsParser.GetModifiers(), option);
                filedata[option] = ChooseCounter(option, modifier, filename, fin);
            }

            WriteFileData(filename, filedata);
        }
        fin.close();
    }
}