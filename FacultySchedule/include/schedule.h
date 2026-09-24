#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <sqlite3.h>

int generate_class_schedule(sqlite3 *db, const char *output_filename);

#endif