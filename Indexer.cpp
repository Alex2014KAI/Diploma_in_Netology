#include "Indexer.h" 

namespace SPIDER
{

// #define DEBUG_INDEXER

    void Indexer::pageRequestHTML(const std::string& URL)
    {
        // For debugging, there will be a normal request below
        std::ifstream file(URL);
        if (!file) {
            throw std::logic_error("Failed to open file");
            return;
        }
        std::string line;
        while (std::getline(file, line)) { 
            originPageHTML_ += cleaningHTMLTags_.execute(line) + " ";

            std::string debugURL = cleaningHTMLTags_.getURL(line);
            if (debugURL.size() != 0) {
                linksOnTheCurrentSite.push_back(debugURL);
            }
        };
        file.close();
        // For debugging, there will be a normal request below

#ifdef DEBUG_INDEXER
        std::cout << originPageHTML_;
#endif // DEBUG_INDEXER

        

    }

    void Indexer::convertWordsLowerCase()
    {
        boost::locale::generator gen;
        std::locale loc = gen("ru_RU.UTF-8"); // "en_US.UTF-8"
        std::locale::global(loc);
        formattedPageHTML_ = boost::locale::to_lower(originPageHTML_, loc);

#ifdef DEBUG_INDEXER
        std::cout << std::endl;
        std::cout << formattedPageHTML_ << std::endl;
#endif // DEBUG_INDEXER
    }

    void Indexer::wordFrequencyAnalysisText()
    {
        std::istringstream stream(formattedPageHTML_);
        std::string word;
        while (stream >> word) {
            if (!word.empty()) {
                wordFrequency_[word]++;
            }
        }
    }

    void Indexer::saveDataDatabase()
    {// need to save asynchronously
        for (const auto& pair : wordFrequency_) {
            const std::string& key = pair.first;
            int value = pair.second;
            std::cout << key << ": " << value << std::endl;
        }

        std::vector debigUrl = getLinksOnTheCurrentSite();
        std::cout << "*******************************************" << std::endl;
        for (auto It = debigUrl.begin(); It != debigUrl.end(); It++) {
            std::cout << (*It) << std::endl;
        }

        setData();
    }

    void Indexer::setData()
    {
        // SPIDER::Database::writeDataToTable(html_, wordFrequency_);
        std::cout << "DATA SAVED" << std::endl;
    }

    void Indexer::execute(const std::string& URL)
    {
        html_ = URL;
        // if (SPIDER::Database::checkingForURLExistence(html_)) return;
        pageRequestHTML(URL);
        convertWordsLowerCase();
        wordFrequencyAnalysisText();
        saveDataDatabase();
    }

    std::vector<std::string> Indexer::getLinksOnTheCurrentSite()
    {
        std::sort(linksOnTheCurrentSite.begin(), linksOnTheCurrentSite.end());
        auto last = std::unique(linksOnTheCurrentSite.begin(), linksOnTheCurrentSite.end());
        linksOnTheCurrentSite.erase(last, linksOnTheCurrentSite.end());
        return linksOnTheCurrentSite;
    }





    std::string CleaningHTMLTags::remove_html_tags(const std::string& input) {
        return std::regex_replace(input, tags, "");
    }

    std::string CleaningHTMLTags::remove_punctuation_and_symbols(const std::string& input) {
        return std::regex_replace(input, non_words, " ");
    }

    std::string CleaningHTMLTags::normalize_spaces(const std::string& input) {
        std::string output = std::regex_replace(input, multi_spaces, " ");
        if (!output.empty() && output.front() == ' ') output.erase(output.begin());
        if (!output.empty() && output.back() == ' ') output.pop_back();
        return output;
    }

    std::string CleaningHTMLTags::execute(const std::string& line)
    {
        std::string no_tags = remove_html_tags(line);
        std::string cleaned = remove_punctuation_and_symbols(no_tags);
        return normalize_spaces(cleaned);
    }

    std::string CleaningHTMLTags::getURL(const std::string& html)
    {
        auto links_begin = std::sregex_iterator(html.begin(), html.end(), hrefTegs);
        std::smatch match;

        if (std::regex_search(html, match, hrefTegs)) {
            return match[1].str();
        }
        else {
            return "";
        }
    }

}
