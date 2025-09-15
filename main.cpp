#include <iostream>
#include "Indexer.h"
#include "Database.h"


// #define DEBUG_INDEXER
#define DEBUG_DATABASE

int main()
{
#ifdef DEBUG_INDEXER
    SPIDER::Indexer indexer;
    indexer.execute("html.txt");
#endif // DEBUG_INDEXER


#ifdef DEBUG_DATABASE
    SPIDER::BDSetup bdSetup("ini.txt");
    // bdSetup.printDataBase();

    SPIDER::Database db(bdSetup.dataSetup_);
    db.deleteTables();

#endif // DEBUG_DATABASE
  

}