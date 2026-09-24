#ifndef PARSER_H
#define PARSER_H

#include <sqlite3.h>

int load_class_schedule(sqlite3 *db, const char *filename);
int load_faculty_info(sqlite3 *db, const char *filename);

#endif