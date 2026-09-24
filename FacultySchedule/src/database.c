#include <stdio.h>
#include <stdlib.h>
#include "database.h"

int open_database(const char *filename, sqlite3 **db)
{
    int result = sqlite3_open(filename, db);

    if (result != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n",
                sqlite3_errmsg(*db));
        return 0;
    }

    return 1;
}

void close_database(sqlite3 *db)
{
    if (db != NULL)
    {
        sqlite3_close(db);
    }
}

int execute_sql_file(sqlite3 *db, const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        fprintf(stderr, "Cannot open SQL file: %s\n", filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *sql = malloc(file_size + 1);

    if (sql == NULL)
    {
        fclose(file);
        fprintf(stderr, "Memory allocation failed.\n");
        return 0;
    }

    fread(sql, 1, file_size, file);
    sql[file_size] = '\0';

    fclose(file);

    char *error_message = NULL;

    int result = sqlite3_exec(db, sql, NULL, NULL, &error_message);

    if (result != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        free(sql);
        return 0;
    }

    free(sql);
    return 1;
}