#include <stdio.h>
#include "database.h"
#include "parser.h"
#include "output.h"

int main(void)
{
    sqlite3 *db = NULL;

    printf("Faculty Schedule Generator\n");
    printf("===========================\n");

    if (!open_database("schedule.db", &db))
    {
        return 1;
    }

    if (!execute_sql_file(db, "sql/schema.sql"))
    {
        close_database(db);
        return 1;
    }

    if (!load_class_schedule(db, "input/class_schedule.csv"))
    {
        close_database(db);
        return 1;
    }

    if (!load_faculty_info(db, "input/faculty_info.csv"))
    {
        close_database(db);
        return 1;
    }

    if (!generate_faculty_report(db, "output/faculty_schedule.txt"))
    {
        close_database(db);
        return 1;
    }

    close_database(db);

    printf("Schedule generation completed successfully.\n");

    return 0;
}