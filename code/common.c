#include "common.h"

int findMax(float a, float b, float c, float d, int e)
{
  float max = a; // Assume the first number is the maximum
  if (b > max)
  {
    max = b; // Update max if the second number is greater
  }
  if (c > max)
  {
    max = c; // Update max if the third number is greater
  }
  if (d > max)
  {
    max = d; // Update max if the fourth number is greater
  }
  if (e > max)
  {
    max = e; // for the bp values so it is int
  }
  return max;
}

char *intToString(int num)
{
  char *str = (char *)malloc(100 * sizeof(char));
  sprintf(str, "%d", num);
  return str;
}

char *floatToString(float num)
{
  char *str = (char *)malloc(100 * sizeof(char));
  sprintf(str, "%0.2f", num);
  return str;
}

int cal_percentage(float ref, float pnt)
{
  float per;
  if (ref != 0)
  {
    // per = (float)((pnt - ref) / ref) * 100;
    per = (float)((pnt) / ref) * 100;
  }
  else
  {
    per = 0;
  }
  return (per);
}

int check_proper(float x[data_pnts_cnt], int i)
{
  if (i > 2 && i < count - 2)
  {
    if (abs(x[i - 1] - x[i]) <= 10 && abs(x[i + 1] - x[i]) <= 10) // && abs(x[i + 2] - x[i]) <= 10 && abs(x[i - 2] - x[i]) <= 10 &&
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

void show_dialog(char *msg)
{
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, msg);
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

