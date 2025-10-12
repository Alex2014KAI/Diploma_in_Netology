#include "WordSearchEngineDatabase.h"
namespace SPIDER
{

	std::vector<DocumentFrequency> WordSearchEngineDatabase::getDocumentsByWords(const std::vector<std::string>& words)
	{
        std::vector<DocumentFrequency> results;
        if (words.empty()) return results;

        try {
            pqxx::work txn{ conn };

            // ‘ормируем массив из слов дл€ передачи в запрос
            std::string words_pg_array = "{";
            for (size_t i = 0; i < words.size(); ++i) {
                words_pg_array += pqxx::to_string(words[i]);
                if (i != words.size() - 1)
                    words_pg_array += ",";
            }
            words_pg_array += "}";

            // ¬ыполн€ем параметризованный запрос
            std::string sql_query = "WITH word_ids AS (SELECT id FROM Words WHERE word = ANY($1::text[])),";
            sql_query += "doc_words AS (SELECT document_id, word_id, frequency FROM DocumentWords WHERE word_id IN(SELECT id FROM word_ids) ), ";
            sql_query += "matched_docs AS (SELECT document_id, SUM(frequency) AS total_frequency, COUNT(DISTINCT word_id) AS matched_words_count FROM doc_words GROUP BY document_id )";
            sql_query += "SELECT d.id, d.url, md.total_frequency FROM matched_docs md JOIN Documents d ON d.id = md.document_id WHERE md.matched_words_count = (SELECT COUNT(*) FROM word_ids) ORDER BY md.total_frequency DESC; ";

            pqxx::result res = txn.exec(sql_query, words);

            for (const auto& row : res) {
                DocumentFrequency df;
                df.document_id = row["id"].as<int>();
                df.url = row["url"].as<std::string>();
                df.total_frequency = row["total_frequency"].as<int>();
                results.push_back(df);
            }

            txn.commit();
        }
        catch (const pqxx::sql_error& e) {
            std::cerr << "SQL error: " << e.what() << "\nQuery: " << e.query() << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        return results;
	}
}