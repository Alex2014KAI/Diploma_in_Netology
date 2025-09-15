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


    Database::Database(std::string dataSetup):c { dataSetup } {}

    void Database::createTables()
    {
        try {
            pqxx::work tx{ c };
            tx.exec(R"(
            CREATE TABLE IF NOT EXISTS Documents (
                id SERIAL PRIMARY KEY,
                title TEXT
            );
        )");

            std::cout << "CREATE TABLE Documents . Parametrs:  id SERIAL PRIMARY KEY, title TEXT" << std::endl;

            tx.exec(R"(
            CREATE TABLE IF NOT EXISTS Words (
                id SERIAL PRIMARY KEY,
                word TEXT UNIQUE
            );
        )");
            std::cout << "CREATE TABLE Documents . Parametrs:  id SERIAL PRIMARY KEY, word TEXT UNIQUE" << std::endl;

            tx.exec(R"(
            CREATE TABLE IF NOT EXISTS Document_Word (
                document_id INTEGER REFERENCES Documents(id),
                word_id INTEGER REFERENCES Words(id),
                frequency INTEGER,
                PRIMARY KEY (document_id, word_id)
            );
        )");
            std::cout << "CREATE TABLE Documents . Parametrs:  document_id INTEGER REFERENCES Documents(id), word_id INTEGER REFERENCES Words(id), frequency INTEGER, PRIMARY KEY(document_id, word_id)" << std::endl;

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


    };




}