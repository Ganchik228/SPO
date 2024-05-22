#include <sqlite3.h>
#include <stdio.h>
#include "lab04_sqlite.h"

int callback(void *, int, char **, char **);

void sqlite_update(int employee_id, char *fio, char *dr, char *gender, int post_id, int orga_id)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql_update_temp = "UPDATE employees \
                                SET \
                                fio = ?, \
                                dr = ?, \
                                gender = ?, \
                                post_id = ?, \
                                orga_id = ? \
                            WHERE \
                                employees_id = ?;";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql_update_temp, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare SQL statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, fio, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, dr, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, gender, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, post_id);
    sqlite3_bind_int(stmt, 5, orga_id);
    sqlite3_bind_int(stmt, 6, employee_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Failed to execute SQL statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void sqlite_get_data()
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql = "SELECT \
                    employees.employees_id, \
                    employees.fio, \
                    employees.dr, \
                    employees.gender, \
                    post.post_id, \
                    post.name AS post_name, \
                    orga.orga_id, \
                    orga.name AS orga_name, \
                    orga.inn \
                FROM \
                    employees \
                INNER JOIN post ON employees.post_id = post.post_id \
                INNER JOIN orga ON employees.orga_id = orga.orga_id;";
    rc = sqlite3_exec(db, sql, callback, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}
