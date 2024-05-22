#ifndef LAB04_SQLITE_H
#define LAB04_SQLITE_H

#define DB_FILE "3mydb.db"


void sqlite_get_data();
void sqlite_update(int employee_id, char *fio, char *dr, char *gender, int post_id, int orga_id);

#endif
