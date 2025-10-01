#include "Database.h"

namespace SPIDER
{
    BDSetup::BDSetup(const std::string& iniFile)
    {
        std::ifstream file(iniFile);
        if (!file) {
            throw std::logic_error("Failed to open file");
            return;
        }

        std::getline(file, host_);
        std::getline(file, port_);
        std::getline(file, dbname_);
        std::getline(file, user_);
        std::getline(file, password_);
        file.close();
        dataSetup_ = host_ + " " + port_ + " " + dbname_ + " " + user_ + " " + password_;
    }

    void BDSetup::printDataBase() { std::cout << dataSetup_; };


    Database::Database(const std::string& dataSetup):c { dataSetup } {}

    void Database::createTables()
    {
        try {
            pqxx::work tx{ c };
            tx.exec(R"(
            CREATE TABLE IF NOT EXISTS Documents (
                id SERIAL PRIMARY KEY,
                url TEXT
            );
        )");

            // std::cout << "CREATE TABLE Documents . Parametrs:  id SERIAL PRIMARY KEY, url TEXT" << std::endl; // title

            tx.exec(R"(
            CREATE TABLE IF NOT EXISTS Words (
                id SERIAL PRIMARY KEY,
                word TEXT UNIQUE
            );
        )");
            // std::cout << "CREATE TABLE Words . Parametrs:  id SERIAL PRIMARY KEY, word TEXT UNIQUE" << std::endl;

            tx.exec(R"(
            CREATE TABLE IF NOT EXISTS DocumentWords (
                document_id INTEGER REFERENCES Documents(id),
                word_id INTEGER REFERENCES Words(id),
                frequency INTEGER,
                PRIMARY KEY (document_id, word_id)
            );
        )");
            // std::cout << "CREATE TABLE DocumentWords . Parametrs:  document_id INTEGER REFERENCES Documents(id), word_id INTEGER REFERENCES Words(id), frequency INTEGER, PRIMARY KEY(document_id, word_id)" << std::endl;

            tx.commit();
        }
        catch (const pqxx::sql_error& e) {
            std::cerr << "SQL error: " << e.what() << "\nQuery: " << e.query() << std::endl;
            return;
        }
        catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            return;
        }
    }

    void Database::deleteTables()
    {
        try {
            pqxx::work tx{ c };
            tx.exec(
                R"(
                DO $$ DECLARE
                    rec RECORD;
                BEGIN
                    FOR rec IN
                        SELECT tablename FROM pg_tables WHERE schemaname = 'public'
                    LOOP
                        EXECUTE 'DROP TABLE IF EXISTS ' || quote_ident(rec.tablename) || ' CASCADE;';
                    END LOOP;
                END $$;
            )"
            );

            tx.commit();
            std::cout << "DELETE TABLES" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << "\n";
            return;
        }


    }

    int Database::addURL(const std::string& url)
    {     
        try {
            pqxx::work tx{ c };
            pqxx::result r = tx.exec(
                "INSERT INTO Documents (url) VALUES (" + tx.quote(url) + ") RETURNING id"
            );
            tx.commit();
            return r[0][0].as<int>();
        }
        catch (const std::exception& e) {
            std::cerr << "ERROR insertDocument: " << e.what() << "\n";
            return -1;
        }
    }
    int Database::addWord(const std::string& word)
    {
        try {
            pqxx::work tx{ c };

            pqxx::result r = tx.exec("INSERT INTO Words (word) VALUES (" + tx.quote(word) + ") ON CONFLICT(word) DO UPDATE SET word = EXCLUDED.word RETURNING id");

            tx.commit();
            return r[0][0].as<int>();
        }
        catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << "\n";
            return -1;
        }

    }
    void Database::insertOrUpdateFrequency(int document_id, int word_id, int frequency)
    {
        try {
            pqxx::work tx{ c };
            tx.exec(pqxx::zview("INSERT INTO DocumentWords(document_id, word_id, frequency) VALUES (" + tx.quote(document_id) + ", " + tx.quote(word_id) + ", " + tx.quote(frequency) + ") ON CONFLICT(document_id, word_id) DO UPDATE SET frequency = EXCLUDED.frequency"));

            tx.commit();
        }
        catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << "\n";
            return;
        }
        

    };
    void Database::writeDataToTable(const std::string& url, const std::map<std::string, int>& wordFrequency)
    {
        try {
            int document_id = addURL(url);
            if (document_id < 0) {
                throw std::exception("Received incorrect document id");
                return;
            }
            for (auto It = wordFrequency.begin(); It != wordFrequency.end(); It++) {
                int word_id = addWord((*It).first);
#ifdef DEBUG_PRINT_DATA
                std::cout << (*It).first << std::endl;
                std::cout << word_id;
#endif // DEBUG_PRINT_DATA
                if (word_id < 0) {
                    throw std::exception("Received incorrect word id");
                    return;
                }
                insertOrUpdateFrequency(document_id, word_id, (*It).second);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << "\n";
            return;
        }
    };
    bool Database::checkingForURLExistence(const std::string& url)
    {
        try {
            pqxx::work tx{ c };
            pqxx::result r = tx.exec("SELECT 1 FROM Documents WHERE url = " + tx.quote(url) + " LIMIT 1");
            return !r.empty(); // Returns true if at least one result was found
        }
        catch (const std::exception& e) {
            std::cerr << "Error checking link existence: " << e.what() << std::endl;
            return false;
        }
    };





}