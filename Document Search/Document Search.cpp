#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <tuple>
#include <limits>
#include <map>
#include <cmath>

using namespace std;
const int MAX_RESULT_DOCUMENT_COUNT = 5;


class DocumentSearcher
{
private:
    const set<string> STOP_WORDS;
    const unsigned int COUNT_DOC;
    const map<string, map<unsigned int, double>> DICTIONARY_DOC;

    struct Document {
        const unsigned int ID;
        const double RELEVANCE;

        bool operator < (const Document& rhs) const {
            return  std::tie(RELEVANCE, ID) > std::tie(rhs.RELEVANCE, rhs.ID);
        }
    };

    const vector<string> readTextDoc() const
    {
        string buffer;
        vector<string> result;

        string line;
        getline(cin, line);

        for (const char c : line)
        {
            if (isspace(static_cast<unsigned char>(c)))
            {
                if (!buffer.empty())
                {
                    result.push_back(buffer);
                    buffer = "";
                }
            }
            else
            {
                buffer += c;
            }
        }
        if (!buffer.empty())
        {
            result.push_back(buffer);
            buffer = "";
        }

        return result;
    }

    const set<string> readStopWords() const
    {
        string buffer;
        set<string> result;

        string line;
        getline(cin, line);

        for (const char c : line)
        {
            if (isspace(static_cast<unsigned char>(c)))
            {
                if (!buffer.empty())
                {
                    if (result.count(buffer) == 0)
                    {
                        result.insert(buffer);
                        buffer = "";
                    }
                    
                }
            }
            else
            {
                buffer += c;
            }
        }
        if (!buffer.empty())
        {
            if (result.count(buffer) == 0)
            {
                result.insert(buffer);
                buffer = "";
            }
        }

        return result;
    }

    const unsigned int readCountDoc() const
    {
        unsigned int countDoc;
        cin >> countDoc;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        return countDoc;
    }

    const map<string, map<unsigned int, double>> createDcList() const
    {
        map<string, map<unsigned int, double>> result;

        double TF;
        for (unsigned int id = 0; id < COUNT_DOC; id++)
        {
            const vector<string> text = findstop(readTextDoc()); // Зона TF
            for (string word : text)
            {
                if (result[word].count(id) > 0) continue;
                TF = static_cast<double>(count(text.begin(), text.end(), word)) / static_cast<double>(text.size()); // Преобразование unsigned _int64 в double возможна потеря данных C4244
                result[word][id] = TF;
            }
        }
        return result;
    }

    const vector<string> findstop(const vector<string>& listquery) const
    {
        if (listquery.size() == 0 || STOP_WORDS.size() == 0) return listquery;
        vector<string> result;
        for (string el : listquery)
        {
            if (STOP_WORDS.count(el) == 0)
            {
                result.push_back(el);
            }
        }

        return result;
    }

    static void PrintResult(const set<Document>& result) {
        unsigned int step = 0;
        for (Document el : result)
        {
            if (step < MAX_RESULT_DOCUMENT_COUNT)
            {
                cout << "{ document_id = "s << el.ID << ", " << "relevance = "s << el.RELEVANCE << " }"s << endl;
                step++;
            }
            else
            {
                break;
            }
        }
    }

public:
    DocumentSearcher() : STOP_WORDS(readStopWords()), COUNT_DOC(readCountDoc()), DICTIONARY_DOC(createDcList())
    {

    }

    void ReadQuery()
    {
        vector<string> query = findstop(readTextDoc());

        set<string> find;
        set<string> nofind;

        for (string el : query)
        {
            if (el[0] == '-')
            {
                if (STOP_WORDS.count(el.substr(1, el.length() - 1)) == 0)
                {
                    nofind.insert(el.substr(1, el.length() - 1));
                }
            }
            else
            {
                find.insert(el);
            }
        }

        map<unsigned int, double> result;

        for (const string find_word : find)
        {
            if (DICTIONARY_DOC.count(find_word) > 0 && DICTIONARY_DOC.count(find_word) != COUNT_DOC)
            {
                for (pair<unsigned int, double> find_doc : DICTIONARY_DOC.at(find_word))
                {
                    double logword = (log(static_cast<double>(COUNT_DOC) / static_cast<double>(DICTIONARY_DOC.at(find_word).size())));
                    if (result.count(find_doc.first) > 0)
                    {
                        result[find_doc.first] += find_doc.second * logword;
                    }
                    else
                    {
                        result[find_doc.first] = find_doc.second * logword;

                    }
                }
            }
            else
            {
                for (pair<unsigned int, double> find_doc : DICTIONARY_DOC.at(find_word))
                {
                    result[find_doc.first] = 0;
                }
            }
        }

        for (const string nofind_word : nofind)
        {
            if (DICTIONARY_DOC.count(nofind_word) == 0) continue;

            for (pair<unsigned int, double> find_doc : DICTIONARY_DOC.at(nofind_word))
            {
                if (result.count(find_doc.first) > 0)
                {
                    result.erase(find_doc.first);
                }
            }
        }

        set<Document> inputResult;

        for (pair<unsigned int, double> el : result)
        {
            Document dc = { static_cast<unsigned int>(el.first), static_cast<double>(el.second) }; //Требует сужающего преобразования C4838
            inputResult.insert(dc);
        }

        PrintResult(inputResult);
    }
};

int main()
{
    setlocale(0, "");
    DocumentSearcher sr;
    sr.ReadQuery();
}
