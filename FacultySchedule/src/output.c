#include <stdio.h>
#include <string.h>
#include "output.h"
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
static int write_class_schedule(sqlite3 *db,
                                FILE *output,
                                int faculty_id)
{
    const char *sql =
        "SELECT "
        "c.course_id, "
        "c.course_title, "
        "cs.days, "
        "cs.start_time, "
        "cs.end_time, "
        "cs.room "
        "FROM class_schedule cs "
        "JOIN courses c "
        "ON cs.course_id = c.course_id "
        "WHERE cs.faculty_id = ? "
        "ORDER BY c.course_id;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)
        != SQLITE_OK)
    {
        fprintf(stderr,
                "Class schedule query error: %s\n",
                sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, faculty_id);

    int found_class = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        if (!found_class)
        {
            fprintf(output, "CLASS SCHEDULE\n");

            fprintf(output,
                    "------------------------------------------------------------\n");

            fprintf(output,
                    "%-18s %-42s %-12s\n",
                    "Course",
                    "Times",
                    "Location");

            fprintf(output,
                    "------------------------------------------------------------\n");

            found_class = 1;
        }

        const char *course_id =
            (const char *)sqlite3_column_text(stmt, 0);

        const char *course_title =
            (const char *)sqlite3_column_text(stmt, 1);

        const char *days =
            (const char *)sqlite3_column_text(stmt, 2);

        const char *start_time =
            (const char *)sqlite3_column_text(stmt, 3);

        const char *end_time =
            (const char *)sqlite3_column_text(stmt, 4);

        const char *room =
            (const char *)sqlite3_column_text(stmt, 5);

                char time_display[128];

        if (days[0] == '\0' && end_time[0] == '\0')
        {
            snprintf(time_display,
                     sizeof(time_display),
                     "%s",
                     start_time);
        }
        else
        {
            snprintf(time_display,
                     sizeof(time_display),
                     "%s %s to %s",
                     format_days(days),
                     start_time,
                     end_time);
        }

        fprintf(output,
                "%-18s %-42s %-12s\n",
                course_id,
                time_display,
                room);

        fprintf(output,
                "  (%s)\n\n",
                course_title);
    }
        if (found_class)
        {
        fprintf(output, "\n");
        }

    sqlite3_finalize(stmt);

    return 1;
}
static int write_office_hours(sqlite3 *db,
                              FILE *output,
                              int faculty_id)
{
    const char *sql =
        "SELECT "
        "oh.day, "
        "oh.start_time, "
        "oh.end_time "
        "FROM office_hours oh "
        "WHERE oh.faculty_id = ? "
        "ORDER BY "
        "CASE oh.day "
        "WHEN 'M' THEN 1 "
        "WHEN 'T' THEN 2 "
        "WHEN 'W' THEN 3 "
        "WHEN 'TR' THEN 4 "
        "WHEN 'F' THEN 5 "
        "ELSE 6 END, "
        "CASE "
        "WHEN lower(oh.start_time) LIKE '%pm' "
        "     AND CAST(oh.start_time AS INTEGER) <> 12 "
        "THEN CAST(oh.start_time AS INTEGER) + 12 "
        "WHEN lower(oh.start_time) LIKE '%am' "
        "     AND CAST(oh.start_time AS INTEGER) = 12 "
        "THEN 0 "
        "ELSE CAST(oh.start_time AS INTEGER) "
        "END, "
        "CAST(substr(oh.start_time, instr(oh.start_time, ':') + 1, 2) AS INTEGER);";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)
        != SQLITE_OK)
    {
        fprintf(stderr,
                "Office hours query error: %s\n",
                sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, faculty_id);

    int found_office_hours = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        if (!found_office_hours)
        {
            fprintf(output, "OFFICE HOURS\n");

            fprintf(output,
                    "------------------------------------------------------------\n");

            found_office_hours = 1;
        }

        const char *day =
            (const char *)sqlite3_column_text(stmt, 0);

        const char *start_time =
            (const char *)sqlite3_column_text(stmt, 1);

        const char *end_time =
            (const char *)sqlite3_column_text(stmt, 2);

        fprintf(output,
                "%-12s %s - %s\n",
                format_days(day),
                start_time,
                end_time);
    }

    if (found_office_hours)
    {
        fprintf(output, "\n");
    }

    sqlite3_finalize(stmt);

    return 1;
}

int generate_faculty_report(sqlite3 *db, const char *output_filename)
{
    FILE *output = fopen(output_filename, "w");

    if (output == NULL)
    {
        fprintf(stderr,
                "Cannot open output file: %s\n",
                output_filename);
        return 0;
    }

    /*
     * The faculty table contains the union of the professors
     * found in our two input files.
     *
     * This means the final report can include:
     *
     * 1. Faculty with class schedules and office hours
     * 2. Faculty with class schedules only
     * 3. Faculty with office hours only
     */
    const char *faculty_sql =
        "SELECT faculty_id, name, subject, office "
        "FROM faculty "
        "ORDER BY name;";

    sqlite3_stmt *faculty_stmt = NULL;

    if (sqlite3_prepare_v2(db,
                           faculty_sql,
                           -1,
                           &faculty_stmt,
                           NULL) != SQLITE_OK)
    {
        fprintf(stderr,
                "Faculty query error: %s\n",
                sqlite3_errmsg(db));

        fclose(output);
        return 0;
    }

    while (sqlite3_step(faculty_stmt) == SQLITE_ROW)
    {
        int faculty_id =
            sqlite3_column_int(faculty_stmt, 0);

        const char *faculty_name =
            (const char *)sqlite3_column_text(
                faculty_stmt, 1);

        fprintf(output,
                "FACULTY: %s\n",
                faculty_name);

        fprintf(output, "\n");
                if (!write_class_schedule(db, output, faculty_id))
                    {
                        sqlite3_finalize(faculty_stmt);
                        fclose(output);
                        return 0;
                    }
            if (!write_office_hours(db, output, faculty_id))
            {
             sqlite3_finalize(faculty_stmt);
             fclose(output);
                return 0;
            }
        fprintf(output,
                "============================================================\n\n");
    }

    sqlite3_finalize(faculty_stmt);
    fclose(output);

    printf("Combined faculty report generated successfully.\n");

    return 1;
}