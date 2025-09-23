#include <iostream>
#include "Indexer.h"
#include "Database.h"

#define OLD_DEBUG
//#define DEBUG_INDEXER
#define DEBUG_DATABASE

int main()
{

#ifdef OLD_DEBUG 

#ifdef DEBUG_INDEXER
    SPIDER::Indexer indexer;
    indexer.execute("html.txt");
#endif // DEBUG_INDEXER


#ifdef DEBUG_DATABASE
    SPIDER::BDSetup bdSetup("ini.txt");

    SPIDER::Database db(bdSetup.dataSetup_);

    
    db.createTables();
    std::map<std::string, int> debugWordFrequency1 = { 
        {"LExa", 5},
        {"Dima", 4 },
        {"Nina", 10},
        {"Oleg", 8},
        {"Artur", 220}
    };
    db.writeDataToTable("https//dataInfo", debugWordFrequency1);

    std::map<std::string, int> debugWordFrequency2 = {
        {"dom", 5},
        {"krovat", 4 },
        {"mebel", 10},
        {"divan", 8},
        {"stol", 220}
    };
    db.writeDataToTable("https//debugInfo", debugWordFrequency2);
    
    if (db.checkingForURLExistence("https//debugInfo")) {
        std::cout << "https//debugInfo - SUCHECTVUET!!";
    };

    // db.deleteTables();
    
#endif // DEBUG_DATABASE


#endif // OLD_DEBUG 

}