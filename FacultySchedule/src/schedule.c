#include <stdio.h>
#include <string.h>
#include "schedule.h"

/*
 * Converts the day abbreviations used in class_schedule.csv
 * into readable day names for the final report.
 *
 * Important:
 * In our input data:
 * M  = Monday
 * T  = Tuesday
 * W  = Wednesday
 * TR = Thursday
 * F  = Friday
 * MW = Monday & Wednesday
 */
static const char *format_days(const char *days)
{
    if (strcmp(days, "M") == 0)
        return "Monday";

    if (strcmp(days, "T") == 0)
        return "Tuesday";

    if (strcmp(days, "W") == 0)
        return "Wednesday";

    if (strcmp(days, "TR") == 0)
        return "Thursday";

    if (strcmp(days, "F") == 0)
        return "Friday";

    if (strcmp(days, "MW") == 0)
        return "Monday & Wednesday";

    return days;
}

int generate_class_schedule(sqlite3 *db, const char *output_filename)
{
    FILE *output = fopen(output_filename, "w");

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
        "c.course_id, "
        "c.course_title, "
        "cs.days, "
        "cs.start_time, "
        "cs.end_time, "
        "cs.room "
        "FROM class_schedule cs "
        "JOIN faculty f "
        "ON cs.faculty_id = f.faculty_id "
        "JOIN courses c "
        "ON cs.course_id = c.course_id "
        "ORDER BY f.name, c.course_id;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)
        != SQLITE_OK)
    {
        fprintf(stderr,
                "Class schedule query error: %s\n",
                sqlite3_errmsg(db));

        fclose(output);
        return 0;
    }

    char previous_faculty[256] = "";

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char *faculty =
            (const char *)sqlite3_column_text(stmt, 0);

        const char *course_id =
            (const char *)sqlite3_column_text(stmt, 1);

        const char *course_title =
            (const char *)sqlite3_column_text(stmt, 2);

        const char *days =
            (const char *)sqlite3_column_text(stmt, 3);

        const char *start_time =
            (const char *)sqlite3_column_text(stmt, 4);

        const char *end_time =
            (const char *)sqlite3_column_text(stmt, 5);

        const char *room =
            (const char *)sqlite3_column_text(stmt, 6);

        /*
         * When we encounter a new professor, start a
         * new faculty schedule section.
         */
        if (strcmp(previous_faculty, faculty) != 0)
        {
            if (strlen(previous_faculty) > 0)
            {
                fprintf(output, "\n\n");
            }

            fprintf(output,
                    "FACULTY: %s\n\n",
                    faculty);

            fprintf(output,
                    "CLASS SCHEDULE\n");

            fprintf(output,
                    "------------------------------------------------------------\n");

            fprintf(output,
                    "%-15s %-32s %-12s\n",
                    "Course",
                    "Times",
                    "Location");

            fprintf(output,
                    "------------------------------------------------------------\n");

            strncpy(previous_faculty,
                    faculty,
                    sizeof(previous_faculty) - 1);

            previous_faculty[
                sizeof(previous_faculty) - 1
            ] = '\0';
        }

        fprintf(output,
                "%-15s %-32s %-12s\n",
                course_id,
                "",
                room);

        fprintf(output,
                "%-15s %s %s to %s\n",
                "",
                format_days(days),
                start_time,
                end_time);

        fprintf(output,
                "  %s\n",
                course_title);

        fprintf(output, "\n");
    }

    sqlite3_finalize(stmt);
    fclose(output);

    printf("Class schedule generated successfully.\n");

    return 1;
}