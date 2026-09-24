#include <stdio.h>
#include <string.h>
#include "office_hours.h"

/*
 * Convert the office-hour day abbreviations
 * into full weekday names.
 */
static const char *format_day(const char *day)
{
    if (strcmp(day, "M") == 0)
        return "Monday";

    if (strcmp(day, "T") == 0)
        return "Tuesday";

    if (strcmp(day, "W") == 0)
        return "Wednesday";

    if (strcmp(day, "TR") == 0)
        return "Thursday";

    if (strcmp(day, "F") == 0)
        return "Friday";

    return day;
}

int generate_office_hours(sqlite3 *db, const char *output_filename)
{
    /*
     * Use append mode because schedule.c has already
     * written the class schedules to this file.
     */
    FILE *output = fopen(output_filename, "a");

    if (output == NULL)
    {
        fprintf(stderr,
                "Cannot open output file: %s\n",
                output_filename);
        return 0;
    }

    const char *sql =
        "SELECT "
        "f.name, "
        "f.subject, "
        "f.office, "
        "oh.day, "
        "oh.start_time, "
        "oh.end_time "
        "FROM office_hours oh "
        "JOIN faculty f "
        "ON oh.faculty_id = f.faculty_id "
        "ORDER BY f.name, "
        "CASE oh.day "
        "WHEN 'M' THEN 1 "
        "WHEN 'T' THEN 2 "
        "WHEN 'W' THEN 3 "
        "WHEN 'TR' THEN 4 "
        "WHEN 'F' THEN 5 "
        "ELSE 6 END, "
        "oh.start_time;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)
        != SQLITE_OK)
    {
        fprintf(stderr,
                "Office hours query error: %s\n",
                sqlite3_errmsg(db));

        fclose(output);
        return 0;
    }

    char previous_faculty[256] = "";

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char *faculty =
            (const char *)sqlite3_column_text(stmt, 0);

        const char *subject =
            (const char *)sqlite3_column_text(stmt, 1);

        const char *office =
            (const char *)sqlite3_column_text(stmt, 2);

        const char *day =
            (const char *)sqlite3_column_text(stmt, 3);

        const char *start_time =
            (const char *)sqlite3_column_text(stmt, 4);

        const char *end_time =
            (const char *)sqlite3_column_text(stmt, 5);

        /*
         * Start a new OFFICE HOURS section whenever
         * we reach a different faculty member.
         */
        if (strcmp(previous_faculty, faculty) != 0)
        {
            fprintf(output, "\n\n");

            fprintf(output,
                    "FACULTY: %s\n\n",
                    faculty);

            fprintf(output,
                    "OFFICE HOURS\n");

            fprintf(output,
                    "------------------------------------------------------------\n");

            if (subject != NULL && strlen(subject) > 0)
            {
                fprintf(output,
                        "Subject: %s\n",
                        subject);
            }

            if (office != NULL && strlen(office) > 0)
            {
                fprintf(output,
                        "Office: %s\n",
                        office);
            }

            fprintf(output, "\n");

            strncpy(previous_faculty,
                    faculty,
                    sizeof(previous_faculty) - 1);

            previous_faculty[
                sizeof(previous_faculty) - 1
            ] = '\0';
        }

        fprintf(output,
                "%-12s %s - %s\n",
                format_day(day),
                start_time,
                end_time);
    }

    sqlite3_finalize(stmt);
    fclose(output);

    printf("Office hours generated successfully.\n");

    return 1;
}