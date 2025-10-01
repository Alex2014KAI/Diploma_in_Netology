#include "Indexer.h" 

namespace SPIDER
{

// #define DEBUG_INDEXER

    void Indexer::pageRequestHTML(const std::string& HTML)
    {
        // To read from a file
        /*
        std::ifstream file(HTML);
        if (!file) {
            throw std::logic_error("Failed to open file");
            return;
        }
        */
        // To read from a file
        

        std::istringstream file(HTML);

        std::string line;
        while (std::getline(file, line)) { 
            originPageHTML_ += cleaningHTMLTags_.execute(line) + " ";

            // if (urlIsNotImage(line)) continue;   // if the link is to a picture
            std::string debugURL = cleaningHTMLTags_.getURL(line);
            if (debugURL.size() != 0) {
                if (debugURL == url_) continue; // Here is a link to the same site
           
                if (debugURL.find("http://") != 0) { // if the initial host is the same as the current site
                    debugURL.insert(0, "http://" + url_ + "/");
                }
                linksOnTheCurrentSite.push_back(debugURL); // to work with links
            }
        };

        // file.close();  // To read from a file
    }

    void Indexer::pageRequestHTML_Links(const std::string& HTML)
    {
        std::istringstream file(HTML);

        std::string line;
        while (std::getline(file, line)) {
            originPageHTML_ += cleaningHTMLTags_.execute(line) + " ";

            // if (urlIsNotImage(line)) continue;   // if the link is to a picture
            std::string debugURL = cleaningHTMLTags_.getURL(line);
            if (debugURL.size() != 0) {
                if (debugURL == url_) continue; // Here is a link to the same site
                // ѕроверка на то что ссылка не на картинку
                if (debugURL.find("http://") != 0) { // if the initial host is the same as the current site
                    debugURL.insert(0, "http://" + url_ + "/");
                }
                linksOnTheCurrentSiteLink.push_back(Link(debugURL, currentLink.currentRecursionLevel_++)); // to work with links
            }
        };
    }

    void Indexer::convertWordsLowerCase()
    {
        boost::locale::generator gen;
        std::locale loc = gen("en_US.UTF-8"); //("ru_RU.UTF-8"); // "en_US.UTF-8"
        std::locale::global(loc);
        formattedPageHTML_ = boost::locale::to_lower(originPageHTML_, loc);
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
    {
#ifdef DEBUG_PRINT_DATA
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
#endif // DEBUG_PRINT_DATA

        setData();
    }

    void Indexer::setData()
    {
        database_.writeDataToTable(url_, wordFrequency_);

#ifdef DEBUG_PRINT_DATA
        std::cout << "DATA SAVED" << std::endl;
#endif // DEBUG_PRINT_DATA
    }

    bool Indexer::urlIsNotImage(const std::string& line)
    {
        std::regex link_regex(R"(<a\s+[^>]*href=["']([^"']+)["'][^>]*>.*?</a>)", std::regex::icase);
        
        std::regex img_ext_regex(R"(\.(jpg|jpeg|png|gif|bmp|svg)$)", std::regex::icase);

        auto begin = std::sregex_iterator(line.begin(), line.end(), link_regex);
        auto end = std::sregex_iterator();

        for (auto it = begin; it != end; ++it) {
            std::smatch match = *it;
            std::string href = match[1].str();

            if (!std::regex_search(href, img_ext_regex)) {
                return false;
            }
        }
        return true;
    }


    void Indexer::execute(const std::string& URL, const std::string& html)
    {
        // html_ = html;
        url_ = URL;
        
        if (database_.checkingForURLExistence(url_)) {
            std::cout << "The URL is in the database, but the page is not processed." << std::endl;
            return;
        };
        
        pageRequestHTML(html);
        convertWordsLowerCase();
        wordFrequencyAnalysisText();
        saveDataDatabase();
    }

    void Indexer::execute(const Link& link, const std::string& html)
    {
        if (link.currentRecursionLevel_ == maxRecursionLevel_) return;
        currentLink = link;
        url_ = link.url_;
        
        if (database_.checkingForURLExistence(url_)) {
            std::cout << "The URL is in the database, but the page is not processed." << std::endl;
            return;
        };

        pageRequestHTML_Links(html);
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

    std::vector<Link> Indexer::getLinksOnTheCurrentSiteLink()
    {
        std::unordered_set<std::string> seenUrls;

        auto it = std::remove_if(linksOnTheCurrentSiteLink.begin(), linksOnTheCurrentSiteLink.end(), [&](const Link& link) {
            if (seenUrls.count(link.url_) > 0) {
                return true;
            }
            else {
                seenUrls.insert(link.url_);
                return false;
            }
            });

        linksOnTheCurrentSiteLink.erase(it, linksOnTheCurrentSiteLink.end());
        
        return linksOnTheCurrentSiteLink;
    }

    std::map<std::string, int> Indexer::getWordFrequency()
    {
        return wordFrequency_;
    }

    void Indexer::printDataServer()
    {
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
