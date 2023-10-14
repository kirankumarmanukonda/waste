
#include "pat_data_base.h"
#define sql_database


#ifdef sql_database

GtkWidget *tree_view;
GtkListStore *list_store;
sqlite3 *db;
char *errMsg;




int clear_flag = 0;
void clear_list_store()
{
  GtkTreeIter iter;
  GtkTreeModel *model = GTK_TREE_MODEL(list_store); // Assuming list_store is your GtkListStore

  // Start by getting the first row (if it exists)
  while (gtk_tree_model_get_iter_first(model, &iter))
  {
    gtk_list_store_remove(list_store, &iter);
  }
}

int data_found = 0;
static int callback(void *data, int argc, char **argv, char **azColName)
{
  if (clear_flag)
  {
    // Set the data_found flag to indicate data was found
    data_found = 1;
    clear_list_store();
    clear_flag = 0;
  }

  GtkTreeIter iter;
  gtk_list_store_append(list_store, &iter);
  for (int i = 0; i < argc; i++)
  {
    gtk_list_store_set(list_store, &iter, i, argv[i] ? argv[i] : "NULL", -1);
  }

  return 0;
}

void convert_date_format(char *date)
{
  // Use sscanf to parse the input date
  int day, month, year;
  if (sscanf(date, "%d-%d-%d", &day, &month, &year) == 3)
  {
    // Use snprintf to format the output date in the same variable
    snprintf(date, 11, "%04d-%02d-%02d", year, month, day);
  }
  else
  {
    // Handle invalid input date format
    strcpy(date, "NULL");
  }
}

char *id_find_text, *name_find_text, *from_date_text, *to_date_text;
int search_database()
{
  char searchQuery[500];

  convert_date_format(from_date_text);
  convert_date_format(to_date_text);

  // Construct the search query based on the selected option
  strcpy(id_find_text, (strlen(id_find_text) == 0) ? "NULL" : id_find_text);
  strcpy(name_find_text, (strlen(name_find_text) == 0) ? "NULL" : name_find_text);
  strcpy(from_date_text, (strlen(from_date_text) == 0) ? "NULL" : from_date_text);
  strcpy(to_date_text, (strlen(to_date_text) == 0) ? "NULL" : to_date_text);

  if (strcmp(id_find_text, "NULL") == 0 && strcmp(name_find_text, "NULL") == 0 && strcmp(from_date_text, "NULL") == 0 && strcmp(to_date_text, "NULL") == 0)
  {
    // clear_list_store();
    fill_list();
    return 0;
  }

  sprintf(searchQuery, "SELECT * FROM Patients WHERE (Date >= '%s' AND Date <= '%s') OR LOWER(Id) LIKE LOWER('%s%%') OR LOWER(Name) LIKE LOWER('%s%%');", from_date_text, to_date_text, id_find_text, name_find_text);
  // g_print(searchQuery);
  // g_print("%s %s %s %s",id_find_text, name_find_text, from_date_text, to_date_text);

  clear_flag = 1;
  // Execute the search query
  int rc = sqlite3_exec(db, searchQuery, callback, NULL, &errMsg);

  if (data_found)
  {
    data_found = 0;
  }
  else
  {
    // show_dialog("Data not found");
    g_print("Data not found\n");
    clear_list_store();
  }

  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);
    // sqlite3_close(db);
    return 1;
  }

  // select 0th row
  GtkTreeIter iter;
  if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store), &iter))
  {
    // Select the 0th row
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    gtk_tree_selection_select_iter(selection, &iter);
  }
}

gboolean onclick_find(GtkWidget *widget, gpointer data)
{
  // g_print("find clicked\n");

  GtkTextIter start, end; // starta, enda, startpid, endpid, startpdoc, endpdoc;
  GtkTextBuffer *textbuffer = gtk_text_view_get_buffer((GtkTextView *)gtk_builder_get_object(builder, "id_find"));
  gtk_text_buffer_get_bounds(textbuffer, &start, &end);
  id_find_text = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);

  textbuffer = gtk_text_view_get_buffer((GtkTextView *)gtk_builder_get_object(builder, "name_find"));
  gtk_text_buffer_get_bounds(textbuffer, &start, &end);
  name_find_text = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);

  textbuffer = gtk_text_view_get_buffer((GtkTextView *)gtk_builder_get_object(builder, "test_date_from"));
  gtk_text_buffer_get_bounds(textbuffer, &start, &end);
  from_date_text = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);

  textbuffer = gtk_text_view_get_buffer((GtkTextView *)gtk_builder_get_object(builder, "test_date_to"));
  gtk_text_buffer_get_bounds(textbuffer, &start, &end);
  to_date_text = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);

  search_database();
}

gboolean onclickfind_enter(GtkWidget *widget, gpointer data)
{
  g_print("Enter clicked \n");
}

int update_data_base(char id[50], char column[15], char value[5])
{
  int rc = sqlite3_open("database.db", &db);
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  char updateQuery[500];
  sprintf(updateQuery, "UPDATE Patients SET '%s' = '%s' WHERE Id = '%s';", column, value, id);
  rc = sqlite3_exec(db, updateQuery, NULL, NULL, &errMsg);
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);
    sqlite3_close(db);
    return 1;
  }
}

int add_new_data_base(char id[50], char name[50], int age, char date[12], char time[6])
{
  // Open the database or create a new one if it doesn't exist
  int rc = sqlite3_open("database.db", &db);
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  // Create a table
  char *createQuery = "CREATE TABLE IF NOT EXISTS Patients(Id TEXT, Name TEXT, Age INT, Date TEXT, Time TEXT, Perfusion TEXT, Elevation TEXT, PORH TEXT);";
  rc = sqlite3_exec(db, createQuery, NULL, NULL, &errMsg);
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);
    sqlite3_close(db);

    return 1;
  }

  // Insert data into the table
  char insertQuery[500];
  sprintf(insertQuery, "INSERT INTO Patients VALUES('%s', '%s', %d, '%s', '%s', '%s', '%s', '%s');", id, name, age, date, time, "No", "No", "No");
  rc = sqlite3_exec(db, insertQuery, NULL, NULL, &errMsg);
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);
    sqlite3_close(db);
    return 1;
  }

  g_print("Data stored to data base");
}

#define ROWS 5
#define COLS 8

GtkWidget *create_tree_view()
{
  // Create the tree view widget
  GtkWidget *tree_view = gtk_tree_view_new();
  // Create a cell renderer for displaying text
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

  // context_scrolled_head = gtk_widget_get_style_context(GTK_WIDGET(column));
  // gtk_style_context_add_class(context_scrolled_head, "scrolll_window_head");

  g_object_set(renderer, "font", "Arial 13", NULL);
  // Add columns to the tree view
  char column_names[COLS][20] = {"ID", "Name", "Age", "Date", "Time", "Perfusion", "Elevation", "PORH"};
  for (int i = 0; i < COLS; i++)
  {
    char column_title[10];
    sprintf(column_title, "%s", column_names[i]);
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(column_title, renderer, "text", i, NULL);
    if (i <= 1)
    {
      gtk_tree_view_column_set_min_width(column, 180);
    }
    else if (i == 2)
    {
      gtk_tree_view_column_set_min_width(column, 50);
    }

    else
    {
      gtk_tree_view_column_set_min_width(column, 150);
    }

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
  }

  return tree_view;
}

void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
  GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
  GtkTreeIter iter;

  // Get the clicked row from the tree path
  if (gtk_tree_model_get_iter(model, &iter, path))
  {
    gchar *row_id_text;
    gtk_tree_model_get(model, &iter, 0, &row_id_text, -1);
    g_print("Clicked Element: Row %s\n", row_id_text);

    sprintf(dirbase, "%sREPORTS/%s/", WORKING_DIR, row_id_text);

    char details[200];
    sprintf(details, "%sdetails.txt", dirbase);
    // Open the file in read mode
    FILE *file = fopen(details, "r");
    if (file == NULL)
    {
      printf("Error opening the file.");

      char msg[50];
      sprintf(msg, "%s Data not found", row_id_text);
      show_dialog(msg);
    }
    else
    {
      char buffer[100];
      int i = 0;
      while (fgets(buffer, sizeof(buffer), file) != NULL)
      {
        buffer[strcspn(buffer, "\n")] = '\0';
        if (i == 0)
        {
          pat_name = (char *)malloc((strlen(buffer) + 1) * sizeof(char));
          sprintf(pat_name, "%s", buffer);
        }
        else if (i == 1)
        {
          pat_age = (char *)malloc((strlen(buffer) + 1) * sizeof(char));
          sprintf(pat_age, "%s", buffer);
        }

        else if (i == 2)
        {
          pat_id = (char *)malloc((strlen(buffer) + 1) * sizeof(char));
          sprintf(pat_id, "%s", buffer);
        }
        else if (i == 3)
        {
          pat_gender = (char *)malloc((strlen(buffer) + 1) * sizeof(char));
          sprintf(pat_gender, "%s", buffer);
        }
        else if (i == 4)
        {
          pat_doc = (char *)malloc((strlen(buffer) + 1) * sizeof(char));
          sprintf(pat_doc, "%s", buffer);
        }
        else if (i == 5)
        {
          char *expore_time;
          expore_time = (char *)malloc((strlen(buffer) + 1) * sizeof(char));
          sprintf(expore_time, "%s", buffer);
          gtk_label_set_text(GTK_LABEL(exp_tim_id), expore_time);
          exposureTime = atof(expore_time) * 1000;
        }

        i++;
      }
      // Close the file
      fclose(file);

      // clicking perfusion programatically
      gtk_button_clicked(GTK_BUTTON(test));
    }
  }
}

int fill_list()
{
  clear_list_store();
  int rc = sqlite3_open("database.db", &db);

  char *selectQuery = "SELECT * FROM Patients;";
  rc = sqlite3_exec(db, selectQuery, callback, NULL, &errMsg);
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL error: %s\n", errMsg);
    sqlite3_free(errMsg);
    sqlite3_close(db);
    return 1;
  }

  // to select the defualt 0th row
  GtkTreeIter iter;
  if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store), &iter))
  {
    // Select the 0th row
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    gtk_tree_selection_select_iter(selection, &iter);
  }

  // for (int i = 0; i < ROWS; i++)
  //   {
  //       GtkTreeIter iter;
  //       gtk_list_store_append(list_store, &iter);
  //       for (int j = 0; j < COLS; j++)
  //       {
  //         gtk_list_store_set(list_store, &iter, j, "hello raww", -1);
  //       }
  //   }
}

#endif