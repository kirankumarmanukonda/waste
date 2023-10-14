#include <sqlite3.h> //for database

#include "common.h"

extern GtkWidget *exp_tim_id;

void clear_list_store();
static int callback(void *data, int argc, char **argv, char **azColName);
void convert_date_format(char *date);
int search_database();
gboolean onclick_find(GtkWidget *widget, gpointer data);
gboolean onclickfind_enter(GtkWidget *widget, gpointer data);
int update_data_base(char id[50], char column[15], char value[5]);
int add_new_data_base(char id[50], char name[50], int age, char date[12], char time[6]);
GtkWidget *create_tree_view();
void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
int fill_list();