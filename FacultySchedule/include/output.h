#ifndef OUTPUT_H
#define OUTPUT_H

#include <sqlite3.h>

int generate_faculty_report(sqlite3 *db, const char *output_filename);

#endif