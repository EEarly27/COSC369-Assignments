#ifndef OFFICE_HOURS_H
#define OFFICE_HOURS_H

#include <sqlite3.h>

int generate_office_hours(sqlite3 *db, const char *output_filename);

#endif