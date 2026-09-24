#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

int open_database(const char *filename, sqlite3 **db);
void close_database(sqlite3 *db);
int execute_sql_file(sqlite3 *db, const char *filename);

#endif