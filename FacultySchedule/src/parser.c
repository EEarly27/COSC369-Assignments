#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

/* ---------------------------------------------------------
   Helper: remove newline characters from the end of a line
   --------------------------------------------------------- */
static void trim_newline(char *text)
{
    size_t len = strlen(text);

    while (len > 0 &&
           (text[len - 1] == '\n' || text[len - 1] == '\r'))
    {
        text[len - 1] = '\0';
        len--;
    }
}
/* ---------------------------------------------------------
   Helper: parse one CSV line.

   This handles quoted fields such as:

       "Adedoyin, Anthony"

   so the comma inside the name does not split the field.
   --------------------------------------------------------- */
static int parse_csv_line(char *line,
                          char fields[][256],
                          int max_fields)
{
    int field_count = 0;
    int field_pos = 0;
    int in_quotes = 0;

    for (int i = 0; ; i++)
    {
        char ch = line[i];

        if (ch == '"')
        {
            in_quotes = !in_quotes;
        }
        else if ((ch == ',' && !in_quotes) || ch == '\0')
        {
            fields[field_count][field_pos] = '\0';
            field_count++;

            field_pos = 0;

            if (field_count >= max_fields || ch == '\0')
            {
                break;
            }
        }
        else
        {
            if (field_pos < 255)
            {
                fields[field_count][field_pos++] = ch;
            }
        }
    }

    return field_count;
}

/* ---------------------------------------------------------
   Helper: find or create a faculty member
   --------------------------------------------------------- */
static int get_or_create_faculty(sqlite3 *db,
                                 const char *name,
                                 const char *subject,
                                 const char *office)
{
    sqlite3_stmt *stmt = NULL;
    int faculty_id = -1;

    const char *select_sql =
        "SELECT faculty_id FROM faculty WHERE name = ?;";

    if (sqlite3_prepare_v2(db,
                           select_sql,
                           -1,
                           &stmt,
                           NULL) != SQLITE_OK)
    {
        fprintf(stderr,
                "Faculty SELECT prepare error: %s\n",
                sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        faculty_id = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    if (faculty_id != -1)
    {
        if ((subject != NULL && strlen(subject) > 0) ||
            (office != NULL && strlen(office) > 0))
        {
            const char *update_sql =
                "UPDATE faculty "
                "SET subject = CASE "
                "WHEN ? <> '' THEN ? ELSE subject END, "
                "office = CASE "
                "WHEN ? <> '' THEN ? ELSE office END "
                "WHERE faculty_id = ?;";

            if (sqlite3_prepare_v2(db,
                                   update_sql,
                                   -1,
                                   &stmt,
                                   NULL) == SQLITE_OK)
            {
                sqlite3_bind_text(stmt, 1,
                                  subject ? subject : "",
                                  -1,
                                  SQLITE_TRANSIENT);

                sqlite3_bind_text(stmt, 2,
                                  subject ? subject : "",
                                  -1,
                                  SQLITE_TRANSIENT);

                sqlite3_bind_text(stmt, 3,
                                  office ? office : "",
                                  -1,
                                  SQLITE_TRANSIENT);

                sqlite3_bind_text(stmt, 4,
                                  office ? office : "",
                                  -1,
                                  SQLITE_TRANSIENT);

                sqlite3_bind_int(stmt, 5, faculty_id);

                sqlite3_step(stmt);
                sqlite3_finalize(stmt);
            }
        }

        return faculty_id;
    }

    const char *insert_sql =
        "INSERT INTO faculty(name, subject, office) "
        "VALUES(?, ?, ?);";

    if (sqlite3_prepare_v2(db,
                           insert_sql,
                           -1,
                           &stmt,
                           NULL) != SQLITE_OK)
    {
        fprintf(stderr,
                "Faculty INSERT prepare error: %s\n",
                sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);

    sqlite3_bind_text(stmt, 2,
                      subject ? subject : "",
                      -1,
                      SQLITE_TRANSIENT);

    sqlite3_bind_text(stmt, 3,
                      office ? office : "",
                      -1,
                      SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        fprintf(stderr,
                "Faculty INSERT error: %s\n",
                sqlite3_errmsg(db));

        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);

    return (int)sqlite3_last_insert_rowid(db);
}

/* ---------------------------------------------------------
   Load class_schedule.csv
   --------------------------------------------------------- */
int load_class_schedule(sqlite3 *db, const char *filename)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        fprintf(stderr,
                "Cannot open class schedule file: %s\n",
                filename);
        return 0;
    }

    char line[2048];
    char fields[7][256];

    /* Skip header */
    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file);
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        trim_newline(line);

        int count = parse_csv_line(line, fields, 7);

        if (count != 7)
        {
            fprintf(stderr,
                    "Invalid class schedule row: %s\n",
                    line);
            continue;
        }
        
        const char *course_id = fields[0];
        const char *course_title = fields[1];
        const char *days = fields[2];
        const char *start_time = fields[3];
        const char *end_time = fields[4];
        const char *room = fields[5];
        const char *faculty_name = fields[6];

        int faculty_id =
            get_or_create_faculty(db,
                                  faculty_name,
                                  "",
                                  "");

        if (faculty_id < 0)
        {
            fclose(file);
            return 0;
        }

        sqlite3_stmt *stmt = NULL;

        const char *course_sql =
            "INSERT OR IGNORE INTO courses "
            "(course_id, course_title) "
            "VALUES(?, ?);";

        if (sqlite3_prepare_v2(db,
                               course_sql,
                               -1,
                               &stmt,
                               NULL) != SQLITE_OK)
        {
            fprintf(stderr,
                    "Course prepare error: %s\n",
                    sqlite3_errmsg(db));

            fclose(file);
            return 0;
        }

        sqlite3_bind_text(stmt, 1,
                          course_id,
                          -1,
                          SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, 2,
                          course_title,
                          -1,
                          SQLITE_TRANSIENT);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        const char *schedule_sql =
            "INSERT INTO class_schedule "
            "(faculty_id, course_id, days, "
            " start_time, end_time, room) "
            "VALUES(?, ?, ?, ?, ?, ?);";

        if (sqlite3_prepare_v2(db,
                               schedule_sql,
                               -1,
                               &stmt,
                               NULL) != SQLITE_OK)
        {
            fprintf(stderr,
                    "Schedule prepare error: %s\n",
                    sqlite3_errmsg(db));

            fclose(file);
            return 0;
        }

        sqlite3_bind_int(stmt, 1, faculty_id);

        sqlite3_bind_text(stmt, 2,
                          course_id,
                          -1,
                          SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, 3,
                          days,
                          -1,
                          SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, 4,
                          start_time,
                          -1,
                          SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, 5,
                          end_time,
                          -1,
                          SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, 6,
                          room,
                          -1,
                          SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            fprintf(stderr,
                    "Schedule INSERT error: %s\n",
                    sqlite3_errmsg(db));

            sqlite3_finalize(stmt);
            fclose(file);
            return 0;
        }

        sqlite3_finalize(stmt);
    }

    fclose(file);

    printf("Class schedule loaded successfully.\n");

    return 1;
}

/* ---------------------------------------------------------
   Load faculty_info.csv
   --------------------------------------------------------- */
int load_faculty_info(sqlite3 *db, const char *filename)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        fprintf(stderr,
                "Cannot open faculty information file: %s\n",
                filename);
        return 0;
    }

    char line[2048];
    char fields[6][256];

    /* Skip header */
    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file);
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        trim_newline(line);

        int count = parse_csv_line(line, fields, 6);

        if (count != 6)
        {
            fprintf(stderr,
                    "Invalid faculty information row: %s\n",
                    line);
            continue;
        }
        
        const char *faculty_name = fields[0];
        const char *subject = fields[1];
        const char *office = fields[2];
        const char *day = fields[3];
        const char *start_time = fields[4];
        const char *end_time = fields[5];

        int faculty_id =
            get_or_create_faculty(db,
                                  faculty_name,
                                  subject,
                                  office);

        if (faculty_id < 0)
        {
            fclose(file);
            return 0;
        }

        sqlite3_stmt *stmt = NULL;

        const char *office_sql =
            "INSERT INTO office_hours "
            "(faculty_id, day, start_time, end_time) "
            "VALUES(?, ?, ?, ?);";

        if (sqlite3_prepare_v2(db,
                               office_sql,
                               -1,
                               &stmt,
                               NULL) != SQLITE_OK)
        {
            fprintf(stderr,
                    "Office hours prepare error: %s\n",
                    sqlite3_errmsg(db));

            fclose(file);
            return 0;
        }

        sqlite3_bind_int(stmt, 1, faculty_id);

        sqlite3_bind_text(stmt, 2,
                          day,
                          -1,
                          SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, 3,
                          start_time,
                          -1,
                          SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, 4,
                          end_time,
                          -1,
                          SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            fprintf(stderr,
                    "Office hours INSERT error: %s\n",
                    sqlite3_errmsg(db));

            sqlite3_finalize(stmt);
            fclose(file);
            return 0;
        }

        sqlite3_finalize(stmt);
    }

    fclose(file);

    printf("Faculty information loaded successfully.\n");

    return 1;
}