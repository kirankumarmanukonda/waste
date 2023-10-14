#include <sys/wait.h>

#include <sys/time.h>

#include <assert.h>
#include <omp.h>
/* Standard headers */
#include <png.h>
#include <ctype.h>

// #include <opencv2/opencv.hpp>

#include <stdio.h>
#include <stdint.h> //get uint16_t
#include <stdlib.h> //rand function
// #include <wiringPi.h>

// #include <mosquitto.h> // for mqtt protocol

#include <math.h>
#include <time.h>
#include <slope/slope.h>// plot graphs


#define WORKING_DIR "/home/ayati/Videos/gladetest_v2.5.1/gladetest/"
#define WIDGET_WINDOW "window"

#include "main.h"
extern rgb *buffer_rgb;
extern float *Z_val;

extern float cmap_min_val;
extern float cmap_max_val;
extern int dynamic_alloc_once;

float *avg_Z;


#include <hpdf.h>
#include <wand/MagickWand.h>
#include <dirent.h> // to access the directories
#include <sys/stat.h>

/*********************        For Debugging         **************************/
#define graph
#define table
#define img_save
#define bluetooth_raspi
#define bp_read
#define report_bug
#define sql_database
// total number of points that can collect (based on porh time and frame rate it shold change)

int come_to_initial_state(void); // same page fuction but declaration is need to call anywherea
/*************    for BP Recording  (bp_data.c)    *************/
extern int bp_val;

int mqttinit(void);
void fetchvalue(void);
void stop_fetch(void);

/*****************  raspberrypi initilizations    ************/
#ifdef bluetooth_raspi
void rec_img(char *);
void sen(char *);
int raspi_init(void);
#endif


////////////////graph initialisation/////////

SlopeScale *scalep, *scalebp;
SlopeItem *series, *series2, *seriesp, *series3, *seriesbp;
double *x, *y, *xp, *yp, *x2, *y2, *x3, *y3, *xbp, *ybp;
long n = 200;
const double dx = 4.0 * G_PI / 200;
GtkWidget *chartp, *chartbp, *chartp_porh;
SlopeFigure *figurep, *figurebp, *figurep_porh;

float perfusionh = 0, perfusionw = 0, perfusion2 = 0, perfusion3 = 0, calib_SG = 0, calib_beta = 0;

float perfusionh_norm = 0, perfusionw_norm = 0, perfusion2_norm = 0, perfusion3_norm = 0;

////////////////////////////////////////////

///////////signals///////////////////
// gboolean savelsciscreen = FALSE;
gboolean savebpscreen = FALSE;
gboolean savepmscreen = FALSE;


/*********	preview button Status ************/
gboolean active, active_sim;
int prev_flag = 0;
gint camera_id, idsim;
/**************************************/




/*****      store bp data      *****/
int lsci_started = 0;
// int *data = (int *)malloc(200 * sizeof(int));

/******      image dimensions      ******/
#define wi 1920
#define h 1200

#define size wi *h


/*************************  Report generation   ****************/

const char *page_title = "ANGIOCAM";


int only_once = 1;
cairo_t *cr_free, *cr_final;


void show_dialog(char *msg)
{
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                  GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_INFO,
                                  GTK_BUTTONS_OK,
                                  msg);
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}





/************************RoI*******************************/

void calib_roi(GtkWidget *event_box)
{
  int xhp = (float)760 / perf_scale_x;
  int yhp = (float)400 / perf_scale_y;
  int xhr = (float)830 / perf_scale_x;
  int yhr = (float)620 / perf_scale_y;

  cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(event_box));
  cairo_rectangle(cr, xhp, yhp, xhr - xhp, yhr - yhp);
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_stroke(cr);

  xhp = (float)1070 / perf_scale_x;
  yhp = (float)460 / perf_scale_y;
  xhr = (float)1130 / perf_scale_x;
  yhr = (float)620 / perf_scale_y;

  cr = gdk_cairo_create(gtk_widget_get_window(event_box));
  cairo_rectangle(cr, xhp, yhp, xhr - xhp, yhr - yhp);
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_stroke(cr);
}
int draw_calib_roi()
{
  calib_roi(event_box);
  return TRUE;
}

int roi = 0;

void *draw_ptr;

void draw_rect_roi_eventbox(GtkWidget *event_box)
{
  cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(event_box));
  for (int i = 0; i < 4; i++)
  {
    double red, green, blue;

    if (i == 0)
    {
      red = 0;
      green = 0;
      blue = 1;
    }
    else if (i == 1)
    {
      red = 1;
      green = 0;
      blue = 0;
    }
    else if (i == 2)
    {
      red = 0;
      green = 1;
      blue = 0;
    }
    else if (i == 3)
    {
      red = 0.4;
      green = 0.2;
      blue = 0;
    }

    if (control_rect_roi[i])
    {
      cairo_rectangle(cr, (rois[i][0] / perf_scale_x), (rois[i][2] / perf_scale_y), (rois[i][1] / perf_scale_x - rois[i][0] / perf_scale_x), (rois[i][3] / perf_scale_y - rois[i][2] / perf_scale_y));
    }
    else
    {
      cairo_arc(cr, circle_rois[i][0] / perf_scale_x, circle_rois[i][1] / perf_scale_y, circle_rois[i][2] / sqrt(perf_scale_x * perf_scale_y), 0, 2 * M_PI);
    }

    cairo_set_source_rgb(cr, red, green, blue);
    cairo_stroke(cr);
  }

  cairo_destroy(cr);

  /******************   Normal image ROI    **********************/
#ifdef normal_img_roi
  cairo_t *cr_n = gdk_cairo_create(gtk_widget_get_window(event_box_normal));
  cairo_rectangle(cr_n, (float)xhp / 1.5, (float)yhp / 1.497, (float)xhr / 1.5 - (float)xhp / 1.5, (float)yhr / 1.497 - (float)yhp / 1.497);
  cairo_set_source_rgb(cr_n, 0, 0, 1);
  cairo_stroke(cr_n);

  cairo_rectangle(cr_n, (float)xwp / 1.5, (float)ywp / 1.497, (float)xwr / 1.5 - (float)xwp / 1.5, (float)ywr / 1.497 - (float)ywp / 1.497);
  cairo_set_source_rgb(cr_n, 1, 0, 0);
  cairo_stroke(cr_n);

  cairo_rectangle(cr_n, (float)x2p / 1.5, (float)y2p / 1.497, (float)x2r / 1.5 - (float)x2p / 1.5, (float)y2r / 1.497 - (float)y2p / 1.497);
  cairo_set_source_rgb(cr_n, 0, 1, 0);
  cairo_stroke(cr_n);

  cairo_rectangle(cr_n, (float)x3p / 1.5, (float)y3p / 1.497, (float)x3r / 1.5 - (float)x3p / 1.5, (float)y3r / 1.497 - (float)y3p / 1.497);
  cairo_set_source_rgb(cr_n, 0.4, 0.2, 0.0);
  cairo_stroke(cr_n);
  cairo_destroy(cr_n);
#endif
}
void draw_roi()
{
  draw_rect_roi_eventbox(event_box);
}

void draw_roi_eventbox(GtkWidget *event_box, float x_val, float y_val)
{
  for (int i = 0; i < free_roi_num; i++)
  {
    // g_print("Free roi num%d\n",free_roi_num);
    cairo_t *cr_f = gdk_cairo_create(gtk_widget_get_window(event_box));
    if (!strcmp(free_roi_color[i], "Blue"))
    {
      cairo_set_source_rgb(cr_f, 0, 0, 1);
    }
    else if (!strcmp(free_roi_color[i], "Red"))
    {
      cairo_set_source_rgb(cr_f, 1, 0, 0);
    }
    else if (!strcmp(free_roi_color[i], "Green"))
    {
      cairo_set_source_rgb(cr_f, 0, 1, 0);
    }
    else if (!strcmp(free_roi_color[i], "Brown"))
    {
      cairo_set_source_rgb(cr_f, 0.4, 0.2, 0.0);
    }

    cairo_move_to(cr_f, (float)polygon[i][0].x / x_val, (float)polygon[i][0].y / y_val);
    for (int j = 0; j < roi_vertexes[i]; j++)
    {
      cairo_line_to(cr_f, (float)polygon[i][j].x / x_val, (float)polygon[i][j].y / y_val);
      cairo_move_to(cr_f, (float)polygon[i][j].x / x_val, (float)polygon[i][j].y / y_val);

      // g_print("acc %.0f %.0f %d\n ", polygon[i][j].x / perf_scale_x, polygon[i][j].y / perf_scale_y, roi_vertexes[i]);
    }
    cairo_line_to(cr_f, (float)polygon[i][0].x / x_val, (float)polygon[i][0].y / y_val);
    cairo_stroke(cr_f);

    cairo_destroy(cr_f);
  }
}
void draw_free_roi()
{
  char *colors[] = {"Blue", "Red", "Green", "Brown"};
  for (int i = 0; i < 4; i++)
  {
    if (control_roi[i] == 1)
    {
      free_roi_color[i] = colors[i];
    }
  }

  draw_roi_eventbox(event_box, perf_scale_x, perf_scale_y);
#ifdef normal_img_roi
  draw_roi_eventbox(event_box_normal, 3.2, 3.592);
#endif
}

void show_avg_perf_img(int save_exposure)
{
  GdkPixbuf *pixbuf;
  colormap_avg_img(image_no);
  pixbuf = gdk_pixbuf_new_from_data((guchar *)buffer_rgb, GDK_COLORSPACE_RGB, FALSE, 8, 1920, 1200, 1920 * sizeof(rgb), NULL, NULL);
  // resize image
  pixbuf = gdk_pixbuf_scale_simple(pixbuf, 900, 500, GDK_INTERP_BILINEAR);
  // gdk_pixbuf_apply_lut(pixbuf, lookup_table, has_alpha);
  gtk_image_set_from_pixbuf(GTK_IMAGE(lsciimage), pixbuf);

  char img_file[200];
  sprintf(img_file, "%s/prev/avg.png", current_working_dir);
  if (!gdk_pixbuf_save(pixbuf, img_file, "png", NULL, NULL))
  {
    printf("Unable to save image: %s\n", NULL);
  }
  g_object_unref(pixbuf);

  if (save_exposure)
  {
    // save the exposure time for further analysis
    char details[250];
    sprintf(details, "%sdetails.txt", dirbase);

    // Open the file in write mode
    FILE *file = fopen(details, "a");
    // Check if the file was opened successfully
    if (file == NULL)
    {
      printf("Error opening the file.");
    }

    // Write the strings to the file
    fprintf(file, "%.1fms\n", exposure_tim);
    // Close the file
    fclose(file);
  }
}

float scale = 1.0;
int rect_roi = 1;
double radius, center_x_val, center_y_val;
gboolean mouse_press_callback(GtkWidget *event_box, GdkEventButton *event, gpointer data)
{
  if (prev_flag) //(prev_flag && !calib_flag)
  {
    static double press_x = 0, press_y = 0, rel_x = 0, rel_y = 0, width_box = 900, height_box = 500;
    GtkAllocation ebox;
    gint img1_x_offset = 0, img1_y_offset = 0;
    gtk_widget_get_allocation(event_box, &ebox);
    img1_x_offset = (ebox.width - width_box) / 2;
    img1_y_offset = (ebox.height - height_box) / 2;
    if (!free_select)
    {
      roi++;
    }

    if (event->type == GDK_BUTTON_PRESS)
    {
      press_x = event->x - img1_x_offset;
      press_y = event->y - img1_y_offset;

      if (free_select)
      {
        if (only_once)
        {
          only_once = 0;
          polygon[free_roi_num][vertex].x = press_x * perf_scale_x;
          polygon[free_roi_num][vertex].y = press_y * perf_scale_y;
          // g_print("The poly with size %d \t %d\n", free_roi_num, vertex);
          // g_print("%d\tx %f y %f \n",free_roi_num, polygon[free_roi_num][vertex].x ,polygon[free_roi_num][vertex].y);

          cr_free = gdk_cairo_create(gtk_widget_get_window(event_box));
          cairo_set_source_rgb(cr_free, 0, 0, 0);
          cairo_move_to(cr_free, press_x, press_y);

          double dashes[] = {5.0, 3.0};
          cr_final = gdk_cairo_create(gtk_widget_get_window(event_box));
          cairo_set_source_rgb(cr_final, 1, 0, 0);
          cairo_move_to(cr_final, press_x, press_y);
          cairo_set_dash(cr_final, dashes, 3, 0.0);
        }
      }
      else
      {
        if (rect_roi)
        {
          if (roi >= 1 && roi <= 7 && roi % 2 == 1) // for case where roi is 1, 3, 5, or 7
          {
            int index = (roi - 1) / 2; // for index var 0, 1, 2, 3
            rois[index][0] = press_x * perf_scale_x;
            rois[index][2] = press_y * perf_scale_y;
          }
          else
          {
            // Handle the case where roi is not 1, 3, 5, or 7
            show_dialog("Already Selected 4 Rois!");
            g_timeout_add(500, (GSourceFunc)draw_roi, draw_ptr);
            g_timeout_add(500, (GSourceFunc)draw_free_roi, draw_ptr);
          }
        }
      }
    }
    else if (event->type == GDK_BUTTON_RELEASE)
    {
      rel_x = event->x - img1_x_offset;
      rel_y = event->y - img1_y_offset;

      if (free_select && roi <= 8)
      {
        control_roi[control_roi_num] = 1;
        polygon[free_roi_num][vertex].x = press_x * perf_scale_x;
        polygon[free_roi_num][vertex].y = press_y * perf_scale_y;
        // g_print("%d\tx %f y %f \n",free_roi_num, polygon[free_roi_num][vertex].x ,polygon[free_roi_num][vertex].y);
        vertex++;
        cairo_line_to(cr_free, rel_x, rel_y);

        cairo_move_to(cr_final, (float)polygon[free_roi_num][0].x / perf_scale_x, (float)polygon[free_roi_num][0].y / perf_scale_y);
        cairo_line_to(cr_final, rel_x, rel_y);

        cairo_stroke(cr_final);
        cairo_stroke(cr_free);

        cairo_move_to(cr_free, rel_x, rel_y);
        // g_print("%lf %lf vertex no : %d\n ", polygon[free_roi_num][vertex].x, polygon[free_roi_num][vertex].y, vertex);
        if (vertex == 10)
        {
          // cairo_destroy(cr_free);
          show_dialog("Max Points reached !!!");
          g_timeout_add(500, (GSourceFunc)draw_roi, draw_ptr);
          g_timeout_add(500, (GSourceFunc)draw_free_roi, draw_ptr);

          // g_signal_emit_by_name(free_select_button, "nothing");
        }
      }
      else if (!free_select)
      {
        control_roi[control_roi_num] = 0;
        control_roi_num++;
        if (rect_roi)
        {
          control_rect_roi[control_rect_roi_num] = 1;
          control_rect_roi_num++;
          if (roi >= 1 && roi <= 8 && roi % 2 == 0) // for case where roi is 2, 4, 6, or 8
          {
            int index = (roi - 2) / 2; // for index var 0, 1, 2, 3
            rois[index][1] = rel_x * perf_scale_x;
            rois[index][3] = rel_y * perf_scale_y;
          }
        }
        else
        {
          control_rect_roi[control_rect_roi_num] = 0;
          control_rect_roi_num++;

          center_x_val = ((press_x + rel_x) / 2) * perf_scale_x;
          center_y_val = ((press_y + rel_y) / 2) * perf_scale_y;
          radius = sqrt(pow(abs(rel_x * perf_scale_x - center_x_val), 2) + pow(abs(rel_y * perf_scale_y - center_y_val), 2));

          if (roi >= 1 && roi <= 8 && roi % 2 == 0) // for case where roi is 2, 4, 6, or 8
          {
            int index = (roi - 2) / 2; // for index var 0, 1, 2, 3
            circle_rois[index][0] = center_x_val;
            circle_rois[index][1] = center_y_val;
            circle_rois[index][2] = radius;
          }
        }

        /******************* draw roi box **********************/
        cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(event_box));
        if (rect_roi)
        {
          cairo_rectangle(cr, press_x, press_y, rel_x - press_x, rel_y - press_y);
        }
        else
        {
          cairo_arc(cr, center_x_val / perf_scale_x, center_y_val / perf_scale_y, radius / sqrt(perf_scale_x * perf_scale_y), 0, 2 * M_PI);
        }

        /**************     x_lsci/x_normal = 1.5 and y_lsci/y_normal = 1.497   ********************/
        cairo_t *cr_n = gdk_cairo_create(gtk_widget_get_window(event_box_normal));
#ifdef normal_img_roi
        cairo_rectangle(cr_n, (float)press_x / 1.5, (float)press_y / 1.497, (float)rel_x / 1.5 - (float)press_x / 1.5, (float)rel_y / 1.497 - (float)press_y / 1.497);

        // cairo_arc(cr_n, press_x, press_y, sqrt(pow(rel_x / 1.5 - press_x, 2) + pow(rel_y / 1.497 - press_y, 2)) , 0, 2 * M_PI);
#endif

        double color[3] = {0, 0, 1}; // Default color
        if (roi == 3 || roi == 4)
        {
          color[0] = 1;
          color[2] = 0;
        }
        else if (roi == 5 || roi == 6)
        {
          color[1] = 1;
          color[2] = 0;
        }
        else if (roi == 7 || roi == 8)
        {
          color[0] = 0.4;
          color[1] = 0.2;
          color[2] = 0.0;
        }

        if (roi <= 8)
        {
          cairo_set_source_rgb(cr, color[0], color[1], color[2]);
          cairo_set_source_rgb(cr_n, color[0], color[1], color[2]);
          cairo_stroke(cr_n);
          cairo_stroke(cr);

          cairo_destroy(cr_n);
          cairo_destroy(cr);
        }
      }
      else
      {
        show_dialog("4 Rois Already Selected !");
        g_timeout_add(500, (GSourceFunc)draw_roi, draw_ptr);
        g_timeout_add(500, (GSourceFunc)draw_free_roi, draw_ptr);
      }
    }
  }
  // g_print("printing control num %d",control_roi_num);
  return TRUE;
}

gboolean mouse_press_callback_normal(GtkWidget *event_box_normal,
                                     GdkEventButton *event,
                                     gpointer data)
{
#ifdef normal_img_roi
  if (prev_flag && !calib_flag)
  {
    static gint press_x = 0, press_y = 0, rel_x = 0, rel_y = 0, width_box = 600, height_box = 334;
    GtkAllocation ebox;
    gint img1_x_offset = 0, img1_y_offset = 0;
    gtk_widget_get_allocation(event_box_normal, &ebox);
    img1_x_offset = (ebox.width - width_box) / 2;
    img1_y_offset = (ebox.height - height_box) / 2;
    roi++;

    if (event->type == GDK_BUTTON_PRESS)
    {
      press_x = event->x - img1_x_offset;
      press_y = event->y - img1_y_offset;
      if (roi == 1)
      {
        rois[0][0] = (float)press_x * 3.2;   // 1920/width_box
        rois[0][2] = (float)press_y * 3.592; // 1200/height_box
      }
      else if (roi == 3)
      {
        rois[1][0] = (float)press_x * 3.2;   // 1920/width_box
        rois[1][2] = (float)press_y * 3.592; // 1200/height_box
      }
      else if (roi == 5)
      {
        rois[2][0] = (float)press_x * 3.2;   // 1920/width_box
        rois[2][2] = (float)press_y * 3.592; // 1200/height_box
      }
      else if (roi == 7)
      {
        rois[3][0] = (float)press_x * 3.2;   // 1920/width_box
        rois[3][2] = (float)press_y * 3.592; // 1200/height_box
      }
      else
      {
        // printf("Already 4 ROI points selected press");
        show_dialog("4 Rois Already Selected !");
        g_timeout_add(500, (GSourceFunc)draw_roi, draw_ptr);
        g_timeout_add(500, (GSourceFunc)draw_free_roi, draw_ptr);
      }
      // printf("Event box clicked at coordinates %d,%d\n",
      //        rois[0][0], rois[0][2]);
    }
    else if (event->type == GDK_BUTTON_RELEASE)
    {
      rel_x = event->x - img1_x_offset;
      rel_y = event->y - img1_y_offset;
      if (roi == 2)
      {
        rois[0][1] = (int)rel_x * 3.2;
        rois[0][3] = (int)rel_y * 3.592;
      }
      else if (roi == 4)
      {
        rois[1][1] = (int)rel_x * 3.2;
        rois[1][3] = (int)rel_y * 3.592;
      }
      else if (roi == 6)
      {
        rois[2][1] = (int)rel_x * 3.2;
        rois[2][3] = (int)rel_y * 3.592;
      }
      else if (roi == 8)
      {
        rois[3][1] = (int)rel_x * 3.2;
        rois[3][3] = (int)rel_y * 3.592;
      }
      else
      {
        // printf("\nAlready 4 ROI points selected Release\n");
      }

      // printf("Event box released at coordinates %d,%d\n",
      //        rois[0][1], rois[0][3]);

      /******************* draw roi box **********************/

      // roi(event_box);
      cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(event_box_normal));
      cairo_rectangle(cr, press_x, press_y, rel_x - press_x, rel_y - press_y);

      /**************     x_lsci/x_normal = 1.5 and y_lsci/y_normal = 1.497   ********************/

      cairo_t *cr_n = gdk_cairo_create(gtk_widget_get_window(event_box));
      cairo_rectangle(cr_n, (float)press_x * 1.5, (float)press_y * 1.497, (float)rel_x * 1.5 - (float)press_x * 1.5, (float)rel_y * 1.497 - (float)press_y * 1.497);

      if (roi == 1 || roi == 2)
      {
        cairo_set_source_rgb(cr, 0, 0, 1);
        cairo_set_source_rgb(cr_n, 0, 0, 1);
      }
      else if (roi == 3 || roi == 4)
      {
        cairo_set_source_rgb(cr, 1, 0, 0);
        cairo_set_source_rgb(cr_n, 1, 0, 0);
      }
      else if (roi == 5 || roi == 6)
      {

        cairo_set_source_rgb(cr, 0, 1, 0);
        cairo_set_source_rgb(cr_n, 0, 1, 0);
      }
      else if (roi == 7 || roi == 8)
      {
        cairo_set_source_rgb(cr, 0.4, 0.2, 0.0);
        cairo_set_source_rgb(cr_n, 0.4, 0.2, 0.0);
      }
      if (roi <= 8)
      {
        cairo_stroke(cr_n);
        cairo_stroke(cr);
      }

      cairo_destroy(cr_n);
      cairo_destroy(cr);
    }
  }
#endif
  return TRUE;
}

void snapshot_callback() { ; }

void saveplotpm(GtkWidget *widget)
{
  GtkAllocation allocation_w;
  GError *error;
  GFile *file;
  GOutputStream *stream;
  GdkPixbuf *pixbuf;

  // printf("Entered Saveplot\n");
  sprintf(filename, "%s/PM_%s.pgm", current_working_dir, pat_name);
  gtk_widget_get_allocation(widget, &allocation_w);
  GdkWindow *window = gtk_widget_get_parent_window(widget);
  // printf("Entering Window\n");
  if (window)
  {
    // printf("Entered Window\n");
    pixbuf = gdk_pixbuf_get_from_window(window, allocation_w.x, allocation_w.y, allocation_w.width, allocation_w.height);
    if (pixbuf)
    {
      // printf("Entered pixbuf\n");
      file = g_file_new_build_filename(filename, NULL);
      // g_free(filename);
      stream = (GOutputStream *)g_file_replace(file, NULL, FALSE, G_FILE_CREATE_NONE, NULL, &error);
      if (stream == NULL)
      {
        if (error != NULL)
        {
          g_warning("%s\n", error->message);
          g_clear_error(&error);
        }
        g_object_unref(pixbuf);
        g_object_unref(file);
        return;
      }
      gdk_pixbuf_save_to_stream_async(pixbuf, stream, "png", NULL, (GAsyncReadyCallback)snapshot_callback, (gpointer)g_file_get_basename(file), NULL);

      g_object_unref(stream);
      g_object_unref(pixbuf);
      g_object_unref(file);
    }
  }

  // g_object_unref(textbuffer);
}


/***********    Ray casting algorithm to know the point inside the polygon    ***********/
bool check_point_inside_polygon(Point point, Point polygon[num_roi][num_vertex + 1], int num_vertices)
{
  int num_intersections = 0;

  // Loop over all edges of the polygon
  for (int i = 0; i < num_vertices; i++)
  {
    Point p1 = polygon[free_roi_num][i];
    Point p2 = polygon[free_roi_num][(i + 1) % num_vertices];

    // Check if the edge intersects with the ray
    if (((p1.y > point.y) != (p2.y > point.y)) &&
        (point.x < (p2.x - p1.x) * (point.y - p1.y) / (p2.y - p1.y) + p1.x))
    {
      num_intersections++;
    }
  }
  return num_intersections % 2 == 1;
}

float pixels = 0;
float perfusion_sum = 0;
float perfusion_sum_norm = 0;
void free_roi_cal()
{
  int x_vert_max = 0;
  int x_vert_min = 1920;

  int y_vert_max = 0;
  int y_vert_min = 1200;

  // g_print("   %.0f %.0f\n",polygon[free_roi_num][0].x, polygon[free_roi_num][0].y);
  //  g_print("polygon pnts %d %d\n", roi_vertexes[free_roi_num], free_roi_num);
  /************   finding the x and y max, min in the vertices   ***********/
  for (int i = 0; i < roi_vertexes[free_roi_num]; i++)
  {
    // g_print("%d\tx %.0f y %.0f \n",free_roi_num, polygon[free_roi_num][i].x ,polygon[free_roi_num][i].y);
    if (polygon[free_roi_num][i].x > (double)x_vert_max)
      x_vert_max = (int)polygon[free_roi_num][i].x;
    if (polygon[free_roi_num][i].x < (double)x_vert_min)
      x_vert_min = (int)polygon[free_roi_num][i].x;

    if (polygon[free_roi_num][i].y > (double)y_vert_max)
      y_vert_max = (int)polygon[free_roi_num][i].y;
    if (polygon[free_roi_num][i].y < (double)y_vert_min)
      y_vert_min = (int)polygon[free_roi_num][i].y;

    // g_print("   %.0f %.0f",polygon[free_roi_num][i].x, polygon[free_roi_num][i].y);
  }

  // g_print("\ny vertmin %d max %d \n",y_vert_min ,y_vert_max);
  // g_print("x vertmin %d max %d \n",x_vert_min ,x_vert_max);
  for (int i = y_vert_min; i < y_vert_max; i++)
  {
    for (int j = x_vert_min; j < x_vert_max; j++)
    {
      Point point = {j, i};

      if (check_point_inside_polygon(point, polygon, roi_vertexes[free_roi_num]))
      {

        perfusion_sum = perfusion_sum + Z_val[(i) * (wi) + (j)];
        pixels++;

        // if(free_roi_num == 1)
        // {
        //   g_print("value %0.1f",Z_val[(i) * (wi) + (j)]);
        // }
      }
    }
    // g_print("hello %d ",free_roi_num);
  }
}
int max_value = 0;
int max_x, min_x, max_y, min_y;
void rect_roi_cal(int x_press, int x_rel, int y_press, int y_rel)
{
  if (y_press > y_rel)
  {
    max_y = y_press;
    min_y = y_rel;
  }
  else
  {
    min_y = y_press;
    max_y = y_rel;
  }

  if (x_press > x_rel)
  {
    max_x = x_press;
    min_x = x_rel;
  }
  else
  {
    min_x = x_press;
    max_x = x_rel;
  }

  // #pragma omp parallel for
  for (int i = min_y; i < max_y; i++)
  {
    // #pragma omp parallel for
    for (int j = min_x; j < max_x; j++)
    {
      if (Z_val[(i) * (wi) + (j)] > max_value)
      {
        max_value = Z_val[(i) * (wi) + (j)];
      }
      perfusion_sum = perfusion_sum + Z_val[(i) * (wi) + (j)];
      pixels++;
    }
  }
}

void circle_roi_cal(int centerX, int centerY, int radius)
{
  // #pragma omp parallel for
  for (int i = centerY - radius - 1; i < centerY + radius + 1; i++)
  {
    // #pragma omp parallel for
    for (int j = centerX - radius - 1; j < centerX + radius + 1; j++)
    {
      // Calculate the distance between the current point and the circle's center
      double distance = sqrt((j - centerX) * (j - centerX) + (i - centerY) * (i - centerY));

      // Check if the point is inside the circle
      if (distance <= radius)
      {
        perfusion_sum = perfusion_sum + Z_val[(i) * (wi) + (j)];
        pixels++;
      }
    }
  }
}



#define MAX_SSID_LENGTH 100
char *getConnectedWifiSSID()
{
  char command[MAX_SSID_LENGTH + 20];               // Command to execute
  char *ssid = (char *)malloc(MAX_SSID_LENGTH + 1); // Buffer to store the SSID
  // Build the command
  snprintf(command, sizeof(command), "iwgetid -r");
  // Open the command for reading
  FILE *fp = popen(command, "r");
  if (fp == NULL)
  {
    fprintf(stderr, "Failed to run command\n");
    exit(1);
  }
  // Read the output of the command into the ssid buffer
  fgets(ssid, MAX_SSID_LENGTH, fp);
  // Remove the newline character at the end
  ssid[strcspn(ssid, "\n")] = '\0';
  // Close the command
  pclose(fp);
  return ssid;
}

char exe[500];
gboolean onclick_bp_connect(GtkWidget *widget, gpointer data)
{
#ifdef bp_read
  char *ssid = getConnectedWifiSSID();
  if (strcmp(ssid, "ESP8266 Access Point"))
  {
    char output[data_pnts_cnt];
    FILE *fp;
    system("sudo nmcli device wifi rescan");
    sprintf(exe, "sudo nmcli device wifi connect \"ESP8266 Access Point\" password 12345678 2>&1");
    fp = popen(exe, "r");
    if (fp == NULL)
    {
      printf("Failed to execute the command.\n");
      return 1;
    }
    fgets(output, sizeof(output), fp);
    int status = pclose(fp);

    if (status == 0)
    {
      gtk_widget_hide(bp_connect);
      mqttinit();
      bp = 1;
    }
    else
    {
      printf(" %s\n", output);
      show_dialog(output);
    }
  }
  else
  {
    gtk_widget_hide(bp_connect);
    mqttinit();
    bp = 1;
  }

#endif
}
void reset_roi()
{

  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      rois[i][j] = 0;
    }
  }

  for (int i = 0; i < free_roi_num; i++)
  {
    for (int j = 0; j < roi_vertexes[i]; j++)
    {
      polygon[i][j].x = 0;
      polygon[i][j].y = 0;
    }
  }
  for (int i = 0; i < 3; i++)
  {
    control_roi[i] = 0;
  }
}

GtkSpinner *spinner = NULL;
GtkWidget *dialog_spin = NULL;

// Callback function to stop the spinner
gboolean stop_spinner()
{
  if (dialog_spin != NULL)
  {
    gtk_widget_destroy(dialog_spin);
    dialog_spin = NULL;
  }
  return G_SOURCE_REMOVE;
}
void show_spinner(char msg[100], int time, GtkWidget *window)
{

  dialog_spin = gtk_message_dialog_new(GTK_WINDOW(window),
                                       GTK_DIALOG_MODAL,
                                       GTK_MESSAGE_INFO,
                                       GTK_BUTTONS_NONE,
                                       msg);
  gtk_window_set_transient_for(GTK_WINDOW(dialog_spin), GTK_WINDOW(window));

  // Create a vertical box container for the dialog content
  GtkWidget *contentBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog_spin))), contentBox);

  // gtk_widget_set_size_request(contentBox, 500, 300);

  // Create the spinner
  spinner = GTK_SPINNER(gtk_spinner_new());

  // gtk_widget_set_size_request(GTK_WIDGET(spinner), 100, 100);

  gtk_box_pack_start(GTK_BOX(contentBox), GTK_WIDGET(spinner), TRUE, TRUE, 0);
  gtk_spinner_start(spinner); // Start the spinner animation

  // Show the dialog and run it
  gtk_widget_show_all(dialog_spin);

  while (gtk_events_pending())
    gtk_main_iteration();

  g_timeout_add_seconds(time, (GSourceFunc)stop_spinner, NULL);

  if (time > 5)
    gtk_dialog_run(GTK_DIALOG(dialog_spin));
}

void camera_init_dialog()
{
  // #ifdef bluetooth_raspi
  // sen("c_on");
  // #endif
  char msg[100];
  sprintf(msg, "%s", "\t\tWait!\n\t\tCamera Initializing...!\t\t");
  show_spinner(msg, 13, window); // show for 13 seconds because the camera takes max 13 seconds to connect
#ifdef bluetooth_raspi
  sen("m_on"); // after camera is connected(blue indication) only turn on laser
#endif
  g_print("camera is On\n");
  if (!history_flag)
  {
    g_clear_object(&stream);
    g_clear_object(&gcamera);
    camerainit();
  }
}
int rasp_once = 1;
gboolean onclick_calibrate(GtkWidget *widget, gpointer data)
{
  calib_flag = 1;

  gtk_button_set_label(GTK_BUTTON(mainevent), "Calibrate");
  gtk_widget_hide(back_elevation);
  gtk_widget_hide(cmap_scale_box);
  gtk_widget_hide(back_perfusion);
  gtk_widget_hide(bpbox);
  gtk_widget_hide(bp_prompt);
  gtk_widget_hide(bp_connect);
  gtk_widget_hide(roi_table);
  gtk_widget_hide(roi_table_elevation);
  gtk_widget_hide(roi_table_porh);
  gtk_widget_hide(back_perfusion);
  gtk_widget_hide(elevation_buttons);
  gtk_widget_hide(window1);

  rois[0][0] = 1110, rois[0][1] = 1160, rois[0][2] = 345, rois[0][3] = 565;
  rois[1][0] = 790, rois[1][1] = 840, rois[1][2] = 345, rois[1][3] = 565;
  rois[2][0] = 0, rois[2][1] = 0, rois[2][2] = 0, rois[2][3] = 0;
  rois[3][0] = 0, rois[3][1] = 0, rois[3][2] = 0, rois[3][3] = 0;
  g_print("calibration\n");

  gtk_widget_show(window);

  if (rasp_once)
  {
#ifdef bluetooth_raspi
    rasp_once = 0;
    show_spinner("\nConnecting Bluetooth !\t\n\n", 3, window);

    if (raspi_init() == -1)
    {
      show_dialog("Please Turn on Device !");
      rasp_once = 1;
      gtk_widget_show(window1);
      gtk_widget_hide(window2);
    }
    else
    {
      // gtk_widget_show(window2);
      // gtk_widget_hide(window1);
    }

    // g_timeout_add(1, (GSourceFunc)raspi_init, NULL); // once window opens then connect to bluetooth
#endif
  }
}

gboolean ontogglepreviewlsci(GtkWidget *widget, gpointer data)
{

  exposureTime = (int)exposure_tim * 1000;
  arv_camera_set_exposure_time(gcamera, exposureTime, &error);
  sprintf(exp_tim_string, "%.1f ms", exposure_tim);
  gtk_label_set_text(GTK_LABEL(exp_tim_id), exp_tim_string);

  if (!calib_flag)
  {
#ifndef fixed_roi
    reset_roi();
#endif
  }

#ifdef bp_read
  if (!bp && control_porh && !history_flag)
  {
    g_signal_emit_by_name(bp_connect, "clicked");
    // skip the execution untill the wifi connected
    return TRUE;
  }
#endif
  // active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
  // if (active)
  // {
  prev_flag = 1;
  if (preview_flag == 0 && history_flag == 0)
  {

    gtk_widget_hide(GTK_WIDGET(normalimg));
    gtk_widget_show(GTK_WIDGET(spinner_image));
    gtk_widget_show(GTK_WIDGET(warning_msg));

    while (gtk_events_pending())
      gtk_main_iteration();
    g_print("1st click preview\n");

    // take_img();

    if (!ARV_IS_CAMERA(gcamera))
    {
      g_timeout_add_seconds(1, (GSourceFunc)take_img, NULL);
      camera_init_dialog();
    }
    else
    {
      take_img();
    }

    prev_lsci();
    // #ifdef bluetooth_raspi
    //     sen("m_off");
    // #endif
    gtk_widget_hide(GTK_WIDGET(spinner_image));
    gtk_widget_hide(GTK_WIDGET(warning_msg));
    gtk_widget_show(GTK_WIDGET(normalimg));
    roi = 0;
    free_roi_num = 0;
    control_roi_num = 0;
    start_mov_avg = 1;
    free_select_exe_once = 1;
  }
  else
  {
    g_print("2nd click show captured img preview\n");
    put_lsciimg();
    if (history_flag)
    {
      roi = 0;
      free_roi_num = 0;
      control_roi_num = 0;
      start_mov_avg = 1;
      free_select_exe_once = 1;
      control_rect_roi_num = 0;
    }
    else
    {
      roi = 0;
      free_roi_num = 0;
      control_roi_num = 0;
      start_mov_avg = 1; // to reset the moving average
      free_select_exe_once = 1;
      control_rect_roi_num = 0;
      // roi = 2;
    }
    put_img();
    draw_points(event_box, NULL);
  }

  //   gtk_button_set_label(GTK_BUTTON(widget), "Select ROI");

  gtk_tool_button_set_label(GTK_TOOL_BUTTON(widget), "Reset");

  while (gtk_events_pending())
    gtk_main_iteration();

  // draw_roi();
  // draw_free_roi();
  // g_timeout_add(500, (GSourceFunc)draw_roi, draw_ptr);

  return TRUE;
}

gboolean onclickmainevent(GtkWidget *widget, gpointer data)
{

  if (calib_flag)
  {

#ifdef bluetooth_raspi
    sen("p_off");
#endif

    if (ARV_IS_STREAM(stream))
    {
      int i = 0;
      printf("Entering Stream\n");

#ifdef bluetooth_raspi
      // sen("m_on");
#endif
      if (error == NULL)
        /* Start the acquisition */
        arv_camera_start_acquisition(gcamera, &error);
      if (error == NULL)
      {

        max_time = 5;
        camera_id = g_idle_add((GSourceFunc)camera, widget);

        while (gtk_events_pending())
          gtk_main_iteration();
      }
    }
    arv_camera_stop_acquisition(gcamera, &error);
    cal_data();
    calib_flag = 0;
#ifdef bluetooth_raspi
    sen("mc_off"); // turning off the main lasers
    g_clear_object(&gcamera);
#endif

    clear_graph();
    elapsed_time = 0;
    timer_flag = 0;
    g_print("The Bottle, Background (Signal Gain, Beta) %f %f \n ", mean_calib_SG, mean_calib_beta);

    char config[250];
    sprintf(config, "%scode/camera.config", WORKING_DIR);
    char cmd[500];
    sprintf(cmd, "sudo rm %s", config);
    system(cmd);

    saveConfig(config, exposureTime, fps, window_size, rois, mean_calib_beta, mean_calib_SG);

    gtk_button_set_label(GTK_BUTTON(widget), "Start");

    gtk_widget_show(window1);
    gtk_widget_hide(window);
    show_dialog("The Device is properly Calibrated.....!");
  }
  else
  {

#ifdef fixed_roi
    if (0)
#else
    if (roi < 7)
#endif

    {
      // printf("/nPlease select atleast 2 ROI s");
      GtkWidget *dialog;
      dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                      GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_INFO,
                                      GTK_BUTTONS_OK,
                                      "Please select atleast 4 ROI s");
      gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
      gtk_dialog_run(GTK_DIALOG(dialog));
      gtk_widget_destroy(dialog);

      g_timeout_add(500, (GSourceFunc)draw_roi, draw_ptr);
      g_timeout_add(500, (GSourceFunc)draw_free_roi, draw_ptr);
    }
    else
    {

      preview_flag = 1;

      if (lsci_started == 0)
      {

#ifdef bluetooth_raspi
        sen("p_off");

#endif
        g_print("phase 0\n");
        lsci_started++;

        if (control_porh == 1)
        {
#ifdef bp_read
          fetchvalue();
#endif
        }
        gtk_button_set_label(GTK_BUTTON(widget), "Stop");
        if (ARV_IS_STREAM(stream))
        {
          int i = 0;
          printf("Entering Stream\n");

#ifdef bluetooth_raspi
          sen("m_on");
#endif

          if (error == NULL)
            /* Start the acquisition */
            arv_camera_start_acquisition(gcamera, &error);
          if (error == NULL)
          {

            // camera(widget);
            if (control_porh)
            {
              max_time = 180; // 180
            }
            else
            {
              max_time = 10; // 15
            }
            camera_id = g_idle_add((GSourceFunc)camera, widget);

            while (gtk_events_pending())
              gtk_main_iteration();
            // draw_roi();

            if (control_porh)
            {
              char bp_name[200];
              sprintf(bp_name, "%sbp_values.txt", current_working_dir);

              FILE *fp;
              fp = fopen(bp_name, "w");
              for (int i = 0; i < image_no; i++)
              {
                fprintf(fp, "%d ", bp_points[i]);
              }
              fclose(fp);
            }
            char time_name[200];
            sprintf(time_name, "%stime_values.txt", current_working_dir);
            g_print("time vaules saving\n");
            FILE *fp_time;
            fp_time = fopen(time_name, "w");
            for (int i = 0; i < image_no; i++)
            {
              fprintf(fp_time, "%f ", time_axis[i]);
            }
            fclose(fp_time);
            if (control_porh)
            {
              cal_porh_data();
            }
            else
            {
              cal_data();
            }

            g_print("Total images :%d\n", image_no);
            report();

            report_num++;
          }
        }

        if (control_porh == 1)
        {
#ifdef bp_read
          stop_fetch();
#endif
          gtk_widget_hide(normalbox);
          gtk_widget_show(roi_table_porh);
        }
        else
        {
          show_avg_perf_img(1);
        }

#ifdef bluetooth_raspi
        sen("mc_off"); // turning off the main lasers
        g_clear_object(&gcamera);
#endif

        gtk_button_set_label(GTK_BUTTON(widget), "Reanalyse");
        saveplotpm(chartp);
        savefullwindow(fixed);
        if (error == NULL)
          // Stop the acquisition  & Destroy the stream object
          arv_camera_stop_acquisition(gcamera, &error);

        g_timeout_add(500, (GSourceFunc)draw_roi, draw_ptr);
        g_timeout_add(500, (GSourceFunc)draw_free_roi, draw_ptr);
        lsci_started++;
      }

      else if (lsci_started == 1)
      {
        g_print("phase  1\n");
        g_source_remove(camera_id);
      }
      else if (lsci_started == 2)
      {
        g_print("phase 2\n");
        if (reanalyse_flag)
        {
          if (PAUSE)
            memset(avg_Z, 0, wi * h * sizeof(float)); // clear the colormap before the reanalyse
          gtk_button_set_label(GTK_BUTTON(widget), "Pause");
          reanalyse_completed = 0;
          reanalyse_flag = 0;
          PAUSE = 1;
          reanalyse_start = 1;
          count = 0; // for graph ploting points
          idsim = g_idle_add((GSourceFunc)reanalyse, widget);
        }
        else
        {
          reanalyse_flag = 1;
          PAUSE = 0;
          g_timeout_add(500, (GSourceFunc)draw_roi, draw_ptr);
          g_timeout_add(500, (GSourceFunc)draw_free_roi, draw_ptr);
          // g_source_remove(idsim);
          gtk_button_set_label(GTK_BUTTON(widget), "Reanalyse");
        }
      }

      return TRUE;
    }
  }
}

void retrive_bp_data()
{
  char bp_name[500];
  sprintf(bp_name, "%sbp_values.txt", current_working_dir);

  g_print("%s bp val", bp_name);
  FILE *fp;
  fp = fopen(bp_name, "r");

  if (fp == NULL)
  {
    gtk_widget_set_size_request(chartp, 850, 500);
    gtk_widget_show(roi_table);
    gtk_widget_hide(bp_connect);
    gtk_widget_hide(bpbox);
    gtk_widget_hide(bp_prompt);
    control_porh = 0;
  }
  else
  {
    gtk_widget_set_size_request(chartp, 850, 450);
    gtk_widget_hide(roi_table);
    gtk_widget_show(bp_connect);
    gtk_widget_show(bpbox);
    gtk_widget_show(bp_prompt);
    gtk_widget_hide(bp_connect);
    for (int i = 0; i < image_no; i++)
    {
      fscanf(fp, "%d ", &bp_points[i]);
    }
    fclose(fp);
    control_porh = 1;
  }
}

void retrive_time_data()
{
  char time_name[200];
  sprintf(time_name, "%stime_values.txt", current_working_dir);

  FILE *fp;
  fp = fopen(time_name, "r");

  if (fp == NULL)
  {
    printf("Error opening file may be time_values.txt not found\n");
  }
  else
  {
    for (int i = 0; i < image_no; i++)
    {
      fscanf(fp, "%f ", &time_axis[i]);
    }
    fclose(fp);
  }
}

gboolean onclick_history(GtkWidget *widget, gpointer data)
{
  come_to_initial_state();
  fill_list();
  history_flag = 1; // to control the preview while in history mode

  gtk_widget_hide(window1);
  gtk_widget_show_all(history_window);
  gtk_widget_hide(calendar);
  gtk_widget_show(history_window);
}

gboolean onclick_home(GtkWidget *widget, gpointer data)
{
  gtk_widget_show(window1);
  gtk_widget_hide(window2);
  come_to_initial_state();
  gtk_widget_hide(history_window);
}

void remove_trailing_spaces(char *str)
{
  // If there are spaces at the end of the name then it will create
  // naming error so thats why removing them and replace in between
  // spaces with under score and removes '\' '/'
  int len = strlen(str);

  if (len == 0)
  {
    // Empty string, nothing to do
    return;
  }

  // Start from the end of the string
  int i = len - 1;

  // Remove trailing spaces
  while (i >= 0 && isspace(str[i]))
  {
    str[i] = '\0';
    i--;
  }

  // Replace spaces between words with underscores and remove slashes
  for (i = 0; i < len; i++)
  {
    if (isspace(str[i]))
    {
      str[i] = '_';
    }
    if (str[i] == '/' || str[i] == '\\')
    {
      str[i] = '_';
    }
  }

  g_print(str);
}

int toggle_login = 0;
GtkTextBuffer *textbuffer_nm, *textbuffer_ag, *textbuffer_doc;
GtkComboBox *combobox_gender;
gboolean onclick_save(GtkWidget *widget, gpointer data)
{
  if (!toggle_login)
  {
    toggle_login = 1;
    g_print("Saving patient Details.....!\n ");
    gtk_button_set_label(GTK_BUTTON(save), "Logout Pt");

    GtkTextIter startn, endn, starta, enda, startpdoc, endpdoc;
    textbuffer_nm = gtk_text_view_get_buffer((GtkTextView *)gtk_builder_get_object(builder, "named"));
    gtk_text_buffer_get_bounds(textbuffer_nm, &startn, &endn);
    textbuffer_ag = gtk_text_view_get_buffer((GtkTextView *)gtk_builder_get_object(builder, "aged"));
    gtk_text_buffer_get_bounds(textbuffer_ag, &starta, &enda);
    combobox_gender = GTK_COMBO_BOX(gtk_builder_get_object(builder, "genderd"));
    GtkTreeIter iter;
    gint active = gtk_combo_box_get_active(combobox_gender);
    if (active != -1)
    {
      gchar *value;
      GtkTreeModel *model = gtk_combo_box_get_model(combobox_gender);
      GtkTreeIter iter;
      GtkTreePath *path = gtk_tree_path_new_from_indices(active, -1);
      gtk_tree_model_get_iter(model, &iter, path);
      gtk_tree_path_free(path);
      gtk_tree_model_get(model, &iter, 0, &value, -1);
      // g_print("Selected value: %s\n", value);
      // g_free(value);
      if (value == NULL)
      {
        sprintf(pat_gender, "%s", " ");
      }
      else
      {
        pat_gender = (char *)value;
      }
    }
    textbuffer_doc = gtk_text_view_get_buffer((GtkTextView *)gtk_builder_get_object(builder, "dr_named"));
    gtk_text_buffer_get_bounds(textbuffer_doc, &startpdoc, &endpdoc);

    if (gtk_text_iter_equal(&startn, &endn) || gtk_text_iter_equal(&starta, &enda) || active == -1)
    {
      show_dialog("Please Register the Patient Details before Test");
      gtk_button_set_label(GTK_BUTTON(save), "Login Pt");
    }
    else
    {
      gtk_widget_show(alltests_buttons);
      pat_name = gtk_text_buffer_get_text(textbuffer_nm, &startn, &endn, FALSE);
      remove_trailing_spaces(pat_name);
      pat_age = gtk_text_buffer_get_text(textbuffer_ag, &starta, &enda, FALSE);
      pat_doc = gtk_text_buffer_get_text(textbuffer_doc, &startpdoc, &endpdoc, FALSE);

      /***********      Create the Main folder for reports     ***********/
      if (main_create_dir && history_flag == 0)
      {
        main_create_dir = 0;
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        struct tm *str_tim = localtime(&t);
        char pat_date[20], pat_time[20];
        strftime(pat_date, sizeof(pat_date), "%Y-%m-%d", str_tim);
        strftime(pat_time, sizeof(pat_time), "%H:%M:%S", str_tim);
        // g_print("%s %s",pat_date, pat_time);

        pat_id = (char *)malloc(20 * sizeof(char));
        sprintf(pat_id, "%d%02d%02d%02d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        sprintf(dirbase, "%sREPORTS/%s/", WORKING_DIR, pat_id);
        GFile *File = g_file_new_for_path(dirbase);
        g_file_make_directory(File, NULL, NULL);

#ifdef sql_database
        char name[50];
        sprintf(name, "%s", pat_name);
        add_new_data_base(pat_id, name, atoi(pat_age), pat_date, pat_time);
//        g_print("dirbase %s", dirbase);
#endif
      }
    }

    /***************      save data in a file       *****************/
    char details[250];
    sprintf(details, "%sdetails.txt", dirbase);
    // Open the file in write mode
    FILE *file = fopen(details, "w");
    // Check if the file was opened successfully
    if (file == NULL)
    {
      printf("Error opening the file.");
      return 1;
    }
    // Write the strings to the file
    fprintf(file, "%s\n", pat_name);
    fprintf(file, "%s\n", pat_age);
    fprintf(file, "%s\n", pat_id);
    fprintf(file, "%s\n", pat_gender);
    fprintf(file, "%s\n", pat_doc);

    // Close the file
    fclose(file);
  }
  else
  {
    toggle_login = 0;
    g_print("     logout\n");
    gtk_text_buffer_set_text(textbuffer_nm, "", -1);
    gtk_text_buffer_set_text(textbuffer_ag, "", -1);
    gtk_text_buffer_set_text(textbuffer_doc, "", -1);
    gtk_combo_box_set_active(combobox_gender, -1);

    gtk_button_set_label(GTK_BUTTON(save), "Login Pt");
    gtk_widget_hide(alltests_buttons);
    // gtk_widget_show(window1);
    // gtk_widget_hide(window2);
    // come_to_initial_state();
  }

  return FALSE;
}

int date_from_state = 1;
static void date_selected(GtkCalendar *calendar, gpointer user_data)
{
  guint year, month, day;
  gtk_calendar_get_date(calendar, &year, &month, &day);
  char date_record[20];
  sprintf(date_record, "%02d-%02d-%04d", day, month + 1, year);
  g_message("Selected date: %s", date_record);
  gtk_widget_hide(GTK_WIDGET(calendar));

  if (date_from_state)
  {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(test_date_from));
    gtk_text_buffer_set_text(buffer, "", -1); // to clear buffer
    gtk_text_buffer_insert_at_cursor(buffer, date_record, -1);
  }
  else
  {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(test_date_to));
    gtk_text_buffer_set_text(buffer, "", -1); // to clear buffer
    gtk_text_buffer_insert_at_cursor(buffer, date_record, -1);
  }
}

void cal_drop_function()
{
  if (!gtk_widget_get_visible(calendar))
  {
    gtk_widget_show(GTK_WIDGET(calendar));
  }
  else
  {
    gtk_widget_hide(GTK_WIDGET(calendar));
  }
}

gboolean calender_dropped_from(GtkWidget *widget, gpointer data)
{
  date_from_state = 1;
  gtk_fixed_move(GTK_FIXED(fixed4), cal_box, 860, 280);
  cal_drop_function();
}

gboolean calender_dropped_to(GtkWidget *widget, gpointer data)
{
  date_from_state = 0;
  gtk_fixed_move(GTK_FIXED(fixed4), cal_box, 860, 345);
  cal_drop_function();
}

gboolean onclick_new_test(GtkWidget *widget, gpointer data)
{
  if (calib_device)
  {
    // to stop creating the new folder
    history_flag = 0; // to control the preview while in history mode
    gtk_widget_hide(alltests_buttons);
    gtk_widget_show(window2);
    gtk_widget_hide(window1);

    if (rasp_once)
    {
#ifdef bluetooth_raspi
      rasp_once = 0;
      show_spinner("\nConnecting Bluetooth !\t\n\n", 3, window2);

      if (raspi_init() == -1)
      {
        show_dialog("Please Turn on Device !");
        rasp_once = 1;
        gtk_widget_show(window1);
        gtk_widget_hide(window2);
      }
      else
      {
        // gtk_widget_show(window2);
        // gtk_widget_hide(window1);
      }

      // g_timeout_add(1, (GSourceFunc)raspi_init, NULL); // once window opens then connect to bluetooth
#endif
    }
    come_to_initial_state();
  }
  else
  {
    show_dialog("Calibrate the Device.....!");
  }
}

void perfusion_widgets()
{
  gtk_widget_set_size_request(chartp, 850, 500);
  if (elev)
  {
    gtk_window_set_title(GTK_WINDOW(window), "Elevation Analysis");
    gtk_widget_hide(back_elevation);
    gtk_widget_hide(cmap_scale_box);

    gtk_fixed_move(GTK_FIXED(fixed), roi_table, 720, 700);
    gtk_widget_show(roi_table_elevation);
    gtk_widget_show(back_perfusion);
    gtk_widget_show(elevation_buttons);
  }
  else
  {
    gtk_window_set_title(GTK_WINDOW(window), "Perfusion Analysis");
    gtk_widget_hide(back_perfusion);
    gtk_widget_hide(roi_table_elevation);
    gtk_widget_hide(elevation_buttons);

    gtk_widget_show(back_elevation);
    gtk_widget_show(cmap_scale_box);

    gtk_widget_set_size_request(chartp, 850, 500);
    gtk_fixed_move(GTK_FIXED(fixed), roi_table, 1150, 700);
  }

  gtk_widget_show(normalbox);
  gtk_widget_show(roi_table);
  gtk_widget_show(back_porh);
  gtk_widget_show(window);

  gtk_widget_hide(bpbox);
  gtk_widget_hide(bp_prompt);
  gtk_widget_hide(bp_connect);
  gtk_widget_hide(roi_table_porh);
  gtk_widget_hide(window2);
}
void porh_widgets()
{
  gtk_widget_set_size_request(chartp, 850, 450);
  gtk_window_set_title(GTK_WINDOW(window), "PORH Analysis");
  gtk_widget_show(bpbox);
  gtk_widget_show(bp_prompt);
  if (history_flag)
  {
    gtk_widget_hide(bp_connect);
  }
  else
  {
    gtk_widget_show(bp_connect);
  }
  gtk_widget_show(back_perfusion);
  gtk_widget_show(back_elevation);
  gtk_widget_show(mainevent);
  gtk_widget_show(normalbox);
  gtk_widget_show(window);

  gtk_widget_hide(cmap_scale_box);
  gtk_widget_hide(back_porh);
  gtk_widget_hide(roi_table);
  gtk_widget_hide(roi_table_elevation);
  gtk_widget_hide(elevation_buttons);
  gtk_widget_hide(roi_table_porh);
  gtk_widget_hide(window2);
}

int switch_analysis()
{
#ifdef bluetooth_raspi
  sen("mpc_off"); // turning off the main lasers
  g_clear_object(&gcamera);
  if (ARV_IS_CAMERA(gcamera))
  {
    g_print("its failed!");
  }
#endif
  preview_flag = 0;

  roi = 0;           // to count the roi clicks 4 rois 8 clicks
  free_roi_num = -1; // to control the free_roi number
  lsci_started = 0;  // to control the same button for the start and reanlyse

  elapsed_time = 0; // total time elapsed after the start button clicked
  image_no = 1;     // total number of images recorded till now
  timer_flag = 0;   // to record the time moment start button clicked in the camera() loop

  count = 0;          // count total number of images
  reanalyse_flag = 1; // to use the reanayse button as pause and Reanlyse

  data_receive_flag = 0;    // to check the perfusion data received or not
  bp = 0;                   // to check the wifi status connected or not for bp
  data_receive_flag_bp = 0; // to check the bp data reviceived or not

  reanalyse_start = 0;    // for recording the elasped time while pausing the reanlyse
  dynamic_alloc_once = 1; // for the moving_avg of the images to display
  img = 1;                // to control the reanalyse images

  max_y_val_graph = 0; // for ploting the graph in the report

  calib_flag = 0; // to come to initaial state for calib

  gtk_button_set_label(GTK_BUTTON(save), "Login Pt");

  free(avg_Z);
  avg_Z = (float *)malloc((wi) * (h) * sizeof(float)); // clearing the static average perfusion image
  memset(avg_Z, 0, wi * h * sizeof(float));

  // g_source_remove(camera_id);
  // g_source_remove(idsim);

  gtk_button_set_label(GTK_BUTTON(mainevent), "Start");
  gtk_tool_button_set_label(GTK_TOOL_BUTTON(preview), "Preview");
  if (GTK_IS_STYLE_CONTEXT(context))
  {
    gtk_style_context_remove_class(context, "ref_selection");
  }

  clear_graph();

  // g_idle_add((GSourceFunc)timer_callbackp, (gpointer)chartp);
  /******************     images      ****************/
  gtk_image_set_from_pixbuf(GTK_IMAGE(normalimg), NULL);

  char logo[500];
  sprintf(logo, "%scode/ui_data/logo.png", WORKING_DIR);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(logo, NULL);
  pixbuf = gdk_pixbuf_scale_simple(pixbuf, 900, 500, GDK_INTERP_BILINEAR);
  gtk_image_set_from_pixbuf(GTK_IMAGE(lsciimage), pixbuf);

  g_object_unref(pixbuf);
  /*******************      ROI table   **************/
  if (!switch_elevation_flag)
  {
    gtk_label_set_text(GTK_LABEL(r1_mean), "");
    gtk_label_set_text(GTK_LABEL(r2_mean), "");
    gtk_label_set_text(GTK_LABEL(r3_mean), "");
    gtk_label_set_text(GTK_LABEL(r4_mean), "");

    gtk_label_set_text(GTK_LABEL(r1_sd), "");
    gtk_label_set_text(GTK_LABEL(r2_sd), "");
    gtk_label_set_text(GTK_LABEL(r3_sd), "");
    gtk_label_set_text(GTK_LABEL(r4_sd), "");

    gtk_label_set_text(GTK_LABEL(r1_perc), "");
    gtk_label_set_text(GTK_LABEL(r2_perc), "");
    gtk_label_set_text(GTK_LABEL(r3_perc), "");
    gtk_label_set_text(GTK_LABEL(r4_perc), "");

    gtk_label_set_text(GTK_LABEL(r1_mean_elev), "");
    gtk_label_set_text(GTK_LABEL(r2_mean_elev), "");
    gtk_label_set_text(GTK_LABEL(r3_mean_elev), "");
    gtk_label_set_text(GTK_LABEL(r4_mean_elev), "");

    gtk_label_set_text(GTK_LABEL(r1_sd_elev), "");
    gtk_label_set_text(GTK_LABEL(r2_sd_elev), "");
    gtk_label_set_text(GTK_LABEL(r3_sd_elev), "");
    gtk_label_set_text(GTK_LABEL(r4_sd_elev), "");

    gtk_label_set_text(GTK_LABEL(r1_perc_elev), "");
    gtk_label_set_text(GTK_LABEL(r2_perc_elev), "");
    gtk_label_set_text(GTK_LABEL(r3_perc_elev), "");
    gtk_label_set_text(GTK_LABEL(r4_perc_elev), "");

    gtk_label_set_text(GTK_LABEL(r1_ratio_elev), "");
    gtk_label_set_text(GTK_LABEL(r2_ratio_elev), "");
    gtk_label_set_text(GTK_LABEL(r3_ratio_elev), "");
    gtk_label_set_text(GTK_LABEL(r4_ratio_elev), "");
  }

  // gtk_widget_destroy(porh_window);

  return FALSE;
}

int old_elev, old_control_porh, old_switch_elevation_flag, old_image_no;
char old_image_location_preview[250], old_image_location[250], old_current_working_dir[250];
void saving_old_state()
{
  old_elev = elev;
  old_control_porh = control_porh;
  old_switch_elevation_flag = switch_elevation_flag;
  old_image_no = image_no;

  sprintf(old_image_location_preview, image_location_preview);
  sprintf(old_image_location, image_location);
  sprintf(old_current_working_dir, current_working_dir);
}
void get_back_old_state()
{
  elev = old_elev;
  control_porh = old_control_porh;
  switch_elevation_flag = old_switch_elevation_flag;
  image_no = old_image_no;

  sprintf(image_location_preview, old_image_location_preview);
  sprintf(image_location, old_image_location);
  sprintf(current_working_dir, old_current_working_dir);

  gtk_button_set_label(GTK_BUTTON(mainevent), "Reanalyse");
  lsci_started = 2;
}

void make_folders(char folder_name[20], char elev_folder[10])
{
  // creating the main folder with folder_name
  char dir_name[200];

  sprintf(dir_name, "%s/%s/", dirbase, folder_name);
  GFile *File = g_file_new_for_path(dir_name);
  g_file_make_directory(File, NULL, NULL);

  if (elev && !control_porh)
  {
    sprintf(dir_name, "%s/%s/%s/", dirbase, folder_name, elev_folder);
    GFile *File = g_file_new_for_path(dir_name);
    g_file_make_directory(File, NULL, NULL);
  }

  // creating the prev folder
  sprintf(dir_name, "%s/prev/", dir_name);
  File = g_file_new_for_path(dir_name);
  g_file_make_directory(File, NULL, NULL);
  sprintf(image_location_preview, "%s", dir_name);

  // creating the raw_images folder
  if (elev && !control_porh)
  {
    sprintf(dir_name, "%s/%s/%s/", dirbase, folder_name, elev_folder);
  }
  else
  {
    sprintf(dir_name, "%s/%s/", dirbase, folder_name);
  }
  sprintf(current_working_dir, "%s", dir_name); // for accessing the current working directory form any where
  sprintf(dir_name, "%s/raw_images/", dir_name);
  File = g_file_new_for_path(dir_name);
  g_file_make_directory(File, NULL, NULL);
  sprintf(image_location, "%s", dir_name);
}

gboolean simulate_delayed_button_click(gpointer button)
{
  // Simulate a button click by emitting the "clicked" signal
  g_signal_emit_by_name(GTK_BUTTON(button), "clicked");

  // Stop the timeout by returning FALSE
  return FALSE;
}

int reanalyse_folder_check(char folder_name[20])
{
  /**************************************
   *
   * It will return 0 if data not found
   *
   ***************************************/

  if (elev && switch_elevation_flag)
  {
    sprintf(image_location_preview, "%s%s/elev/prev/", dirbase, folder_name);
    sprintf(image_location, "%s%s/elev/raw_images/", dirbase, folder_name);
    sprintf(current_working_dir, "%s%s/elev/", dirbase, folder_name);
  }
  else if (elev && !switch_elevation_flag)
  {
    sprintf(image_location_preview, "%s%s/perf/prev/", dirbase, folder_name);
    sprintf(image_location, "%s%s/perf/raw_images/", dirbase, folder_name);
    sprintf(current_working_dir, "%s%s/perf/", dirbase, folder_name);
  }
  else if (!elev || control_porh)
  {
    sprintf(current_working_dir, "%s%s/", dirbase, folder_name);
    sprintf(image_location, "%sraw_images/", current_working_dir);
  }

  int count = 0;
  DIR *dir;
  struct dirent *entry;

  dir = opendir(image_location); // Open the current directory
  if (dir == NULL)
  {
    g_print("Unable to open directory!\n");

    char msg[70];
    sprintf(msg, "%s \n\n\t\t\t\t\thas No %s found!", pat_id, folder_name);
    show_dialog(msg);
    return -1;
  }
  else
  {
    // to count the number of images previously saved
    while ((entry = readdir(dir)) != NULL)
    {
      if (entry->d_type == DT_REG)
      { // If the entry is a regular file
        count++;
      }
    }
    closedir(dir);
    image_no = count;
    retrive_time_data();

    if (control_porh)
    {
      retrive_bp_data();
    }
    gtk_widget_hide(history_window);
    gtk_button_set_label(GTK_BUTTON(mainevent), "Reanalyse");
    lsci_started = 2;

    if (count == 0)
    {

      return -1;
      g_print("-1\n");
    }
  }

  return 1;
}

gboolean onclick_perfusion(GtkWidget *widget, gpointer data)
{
  saving_old_state();
  switch_analysis();
  elev = 0;
  control_porh = 0;

  if (history_flag)
  {
    int status = reanalyse_folder_check("Perfusion_data");
    if (!status)
    {
      switch_analysis();
      get_back_old_state();
      return TRUE;
    }
    else if (status == -1)
    {
      perfusion_widgets();
      char msg[70];
      sprintf(msg, "%s \n\n\t\t\t\t\thas No Perfusion data found!", pat_id);
      show_dialog(msg);
      g_timeout_add(0, simulate_delayed_button_click, back_elevation);
      return TRUE;
    }
  }
  else
  {
    make_folders("Perfusion_data", "");
    update_data_base(pat_id, "Perfusion", "Yes");
  }
  perfusion_widgets();

  return TRUE;
}

gboolean onclick_elevation(GtkWidget *widget, gpointer data)
{
  saving_old_state();
  switch_analysis();
  elev = 1;
  control_porh = 0;

  if (history_flag)
  {
    int status = reanalyse_folder_check("Elevation_data");
    if (!status)
    {
      switch_analysis();
      get_back_old_state();
      return TRUE;
    }
    else if (status == -1)
    {
      perfusion_widgets();
      char msg[70];
      sprintf(msg, "%s \n\n\t\t\t\t\thas No Elevation data found!", pat_id);
      show_dialog(msg);
      g_timeout_add(0, simulate_delayed_button_click, back_porh);
      return TRUE;
    }
  }
  else
  {
    make_folders("Elevation_data", "elev");
    make_folders("Elevation_data", "perf");
    update_data_base(pat_id, "Elevation", "Yes");
  }

  perfusion_widgets();

  return TRUE;
}

gboolean onclick_porh(GtkWidget *widget, gpointer data)
{
  saving_old_state();
  switch_analysis();
  control_porh = 1;
  elev = 0;

  if (history_flag)
  {
    int status = reanalyse_folder_check("Porh_data");
    if (!status)
    {
      switch_analysis();
      get_back_old_state();
      return TRUE;
    }
    else if (status == -1)
    {
      porh_widgets();
      char msg[70];
      sprintf(msg, "Data Not Fould ! \n\n Redirecting to History Page......!");
      show_dialog(msg);
      gtk_widget_show(history_window);
      return TRUE;
    }
  }
  else
  {
    make_folders("Porh_data", "");
    update_data_base(pat_id, "PORH", "Yes");
  }

  porh_widgets();

  return TRUE;
}

gboolean switch_elevation(GtkWidget *widget, gpointer data)
{
  saving_old_state();
  if (switch_elevation_flag)
  {
    switch_elevation_flag = 0;

    switch_analysis();
    sprintf(image_location_preview, "%sElevation_data/perf/prev/", dirbase);
    sprintf(image_location, "%sElevation_data/perf/raw_images/", dirbase);
    sprintf(current_working_dir, "%sElevation_data/perf/", dirbase);

    gtk_label_set_text(GTK_LABEL(elevation_label), "Perfusion");

    if (history_flag)
    {

      int status = reanalyse_folder_check("Elevation_data");
      if (!status)
      {
        switch_analysis();
        get_back_old_state();
        return TRUE;
      }
      else if (status == -1)
      {
        perfusion_widgets();
        char msg[70];
        sprintf(msg, "%s \n\n\t\t\t\t\thas No Elevation data found!", pat_id);
        show_dialog(msg);

        return TRUE;
      }
    }
  }
  else
  {
    switch_elevation_flag = 1;
    r1_mean_val = mean_d_norm_h;
    r2_mean_val = mean_d_norm_w;
    r3_mean_val = mean_d_norm_2;
    r4_mean_val = mean_d_norm_3;

    switch_analysis();
    sprintf(image_location_preview, "%sElevation_data/elev/prev/", dirbase);
    sprintf(image_location, "%sElevation_data/elev/raw_images/", dirbase);
    sprintf(current_working_dir, "%sElevation_data/elev/", dirbase);

    gtk_label_set_text(GTK_LABEL(elevation_label), "Elevation");

    if (history_flag)
    {
      int status = reanalyse_folder_check("Elevation_data");
      if (!status)
      {
        switch_analysis();
        get_back_old_state();
        return TRUE;
      }
      else if (status == -1)
      {
        perfusion_widgets();
        char msg[70];
        sprintf(msg, "%s \n\n\t\t\t\t\thas No Elevation data found!", pat_id);
        show_dialog(msg);

        return TRUE;
      }
    }
  }
}

int analysis_counter = 0;
int gen_counter = 0;
int someflag = 0;
int open_report()
{
  char cmd[300];
  // sprintf(cmd, "sudo -u ayati evince --fullscreen -w %s", rep_name); //-w option for the print but it skip the --fullscreen
  sprintf(cmd, "sudo -u ayati xdg-open %s", rep_name);

  int status = system(cmd);

  if (status == -1)
  {
    // Failed to execute the command
    printf("Failed to execute the command.\n");
    show_dialog("Failed to open pdf file");
  }
}

GtkWidget *check[10];
GtkTextBuffer *clinical_buffer;
GtkTextIter start_cli, end_cli;
void on_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data)
{
  if (response_id == GTK_RESPONSE_OK)
  {
    gtk_text_buffer_get_bounds(clinical_buffer, &start_cli, &end_cli);

    if (gtk_text_iter_equal(&start_cli, &end_cli))
    {
      printf("No Patient clinition in the box\n");
      pat_clin = " ";
    }
    else
    {
      pat_clin = gtk_text_buffer_get_text(clinical_buffer, &start_cli, &end_cli, FALSE);
    }

    int last;
    char del_pg[500] = "";
    g_print("report number %d \n", report_num);

#ifdef report_bug
    // Retrieve the state of the check buttons //(control_porh ? pg_num/2 : report_num)
    for (int i = 0; i < (control_porh ? pg_num : report_num); (control_porh ? i = i + 2 : i++))
    {
      gboolean check_state;
      if (control_porh)
      {
        check_state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check[i / 2]));
      }
      else
      {
        check_state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check[i]));
      }

      // Do something with the check button states
      g_print("Check button %d state: %d\n", i, check_state);
      if (check_state == 0)
      {
        // HPDF_RemovePage(pdf, i);
        if (control_porh)
        {
          HPDF_Page_SetHeight(page[i], 10);
          HPDF_Page_SetHeight(page[i + 1], 10);
        }
        else
        {
          HPDF_Page_SetHeight(page[i], 10);
        }
      }
      else
      {
        if (control_porh)
        {
          sprintf(del_pg, "%s%d %d ", del_pg, i + 1, i + 2);
          HPDF_Page_SetHeight(page[i], 841); // original height of the page
          HPDF_Page_SetHeight(page[i + 1], 841);
          last = i + 1;
        }
        else
        {
          sprintf(del_pg, "%s%d ", del_pg, i + 1);
          HPDF_Page_SetHeight(page[i], 841); // original height of the page
          last = i;
        }
      }
    }

#endif
    // report is generated with reducing the height for unwanted (not efficient)

    final_report(last);
#ifdef report_bug
    // create the command to remove the unwanted pages from the pdf
    char command[800], output[500];
    sprintf(output, "%soutput.pdf", current_working_dir);
    sprintf(command, "pdftk %s cat %soutput %s compress", rep_name, del_pg, output);
    system(command);
    // g_print(command);
    remove(rep_name);
    rename(output, rep_name);

#endif

    open_report();
  }

  gtk_widget_destroy(GTK_WIDGET(dialog));
}

char analysis_name[50];
gboolean onclick_report_gen(GtkWidget *widget, gpointer user_data)
{

  GtkWidget *dialog, *cancel_button, *ok_button, *label, *label_per, *label_elev, *label_porh, *scroll_window;
  // Create the dialog widget
  dialog = gtk_dialog_new_with_buttons("Generate Report", GTK_WINDOW(user_data), GTK_DIALOG_MODAL,
                                       "Cancel", GTK_RESPONSE_CANCEL, "Print", GTK_RESPONSE_OK, NULL);

  gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 250);

  cancel_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

  clinical_box_buttons = gtk_widget_get_style_context(cancel_button);
  gtk_style_context_add_class(clinical_box_buttons, "clinical_box_cancel");
  clinical_box_buttons = gtk_widget_get_style_context(ok_button);
  gtk_style_context_add_class(clinical_box_buttons, "clinical_box_ok");

  // Set the size of the cancel and OK buttons
  gtk_widget_set_size_request(cancel_button, 100, 40);
  gtk_widget_set_size_request(ok_button, 100, 40);

  GtkWidget *reports_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 80);
  GtkWidget *reports_perf_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  GtkWidget *reports_elev_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  GtkWidget *reports_porh_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);

  label_per = gtk_label_new("Perfusion");
  label_porh = gtk_label_new("PORH");
  label_elev = gtk_label_new("Elevation");

  gtk_box_pack_start(GTK_BOX(reports_perf_box), label_per, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(reports_elev_box), label_elev, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(reports_porh_box), label_porh, FALSE, FALSE, 10);

  int per_i = 1, ele_per_i = 1, ele_ele_i = 1, porh_i = 1;
  // Create the check buttons and add them to the dialog
  for (int i = 0; i < report_num; i++)
  {
    if (control_reports[i] == 0)
    {
      sprintf(analysis_name, "Analysis %d", per_i);
      per_i++;
      check[i] = gtk_check_button_new_with_label(analysis_name);
      gtk_box_pack_start(GTK_BOX(reports_perf_box), check[i], FALSE, FALSE, 3);
    }
    else if (control_reports[i] == 10)
    {
      sprintf(analysis_name, "Perf %d", ele_per_i);
      ele_per_i++;
      check[i] = gtk_check_button_new_with_label(analysis_name);
      gtk_box_pack_start(GTK_BOX(reports_elev_box), check[i], FALSE, FALSE, 3);
    }
    else if (control_reports[i] == 11)
    {
      sprintf(analysis_name, "Elev %d", ele_ele_i);
      ele_ele_i++;
      check[i] = gtk_check_button_new_with_label(analysis_name);
      gtk_box_pack_start(GTK_BOX(reports_elev_box), check[i], FALSE, FALSE, 3);
    }
    else if (control_reports[i] == 2)
    {
      sprintf(analysis_name, "Analysis %d", porh_i);
      porh_i++;
      check[i] = gtk_check_button_new_with_label(analysis_name);
      gtk_box_pack_start(GTK_BOX(reports_porh_box), check[i], FALSE, FALSE, 3);
    }
  }

  gtk_box_pack_start(GTK_BOX(reports_box), reports_perf_box, FALSE, FALSE, 20);
  gtk_box_pack_start(GTK_BOX(reports_box), reports_elev_box, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(reports_box), reports_porh_box, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), reports_box, FALSE, FALSE, 10);
  // Create a new GtkLabel widget
  label = gtk_label_new("Clinical Impression :");
  gtk_label_set_xalign(GTK_LABEL(label), 0.0);

  clinical_buffer = gtk_text_buffer_new(NULL);
  GtkWidget *text_view = gtk_text_view_new_with_buffer(clinical_buffer);

  scroll_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(scroll_window), text_view);
  gtk_widget_set_size_request(scroll_window, 100, 100);

  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label, FALSE, FALSE, 15);

  // Set the initial text in the entry
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), scroll_window, FALSE, FALSE, 10);

  // Connect the "response" signal to the dialog
  g_signal_connect(dialog, "response", G_CALLBACK(on_dialog_response), check[0]);

  // adding styles
  clinical_impress = gtk_widget_get_style_context(dialog);
  gtk_style_context_add_class(clinical_impress, "clinical_impress_dialog");

  clinical_box = gtk_widget_get_style_context(scroll_window);
  gtk_style_context_add_class(clinical_box, "clinical_box");

  // Show the dialog
  gtk_widget_show_all(dialog);

  return TRUE;
}

void color_active_roi(GtkWidget *widget)
{
  gtk_style_context_remove_class(context_roi_type, "roi_type_selection");
  context_roi_type = gtk_widget_get_style_context(widget);
  gtk_style_context_add_class(context_roi_type, "roi_type_selection");
}

gboolean onclick_free_select(GtkWidget *widget, gpointer data)
{
  color_active_roi(widget);
  if (free_select_exe_once)
  {
    free_select_exe_once = 0;
    free_select = 1;
  }
  else
  {
    free_select = 1;
    if (control_roi_num > 3)
    {
      show_dialog("4 Rois Already Selected !");
      g_timeout_add(500, (GSourceFunc)draw_roi, draw_ptr);
      g_timeout_add(500, (GSourceFunc)draw_free_roi, draw_ptr);
    }

    if (cr_free != NULL && cr_final != NULL)
    {
      roi = roi + 2;
      control_roi_num++;
      free_roi_num++;
      // g_print("Roi number %d\n", roi);
      roi_vertexes[free_roi_num - 1] = vertex;
      vertex = 0;
      only_once = 1;

      cairo_destroy(cr_final);
      cairo_destroy(cr_free);
      cr_free = NULL;
      cr_final = NULL;
    }
  }

  // g_print("\t\t\t\t\t\t numb is %d\n", free_roi_num);
}

gboolean onclick_rect_select(GtkWidget *widget, gpointer data)
{

  color_active_roi(widget);
  free_select = 0;
  rect_roi = 1;
  g_print("free select off && rect on\n");
}

gboolean onclick_circle_select(GtkWidget *widget, gpointer data)
{
  color_active_roi(widget);

  free_select = 0;
  rect_roi = 0;

  g_print("free select off Circle on\n");
}

gboolean show_roi_clicked(GtkWidget *widget, gpointer data)
{
  if (prev_flag)
  {
    g_timeout_add(500, (GSourceFunc)draw_roi, draw_ptr);
    g_timeout_add(500, (GSourceFunc)draw_free_roi, draw_ptr);
  }

  // g_timeout_add(500, (GSourceFunc)draw_calib_roi, draw_ptr);
}

void clear_graph()
{
  count = 0;
  /******************     graph     ***********/
  free(x);
  free(y);
  free(xp);
  free(yp);
  free(x2);
  free(y2);
  free(x3);
  free(y3);
  free(xbp);
  free(ybp);
  x = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));
  y = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));

  xp = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));
  yp = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));

  x2 = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));
  y2 = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));

  x3 = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));
  y3 = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));

  xbp = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));
  ybp = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));

  long k;
  for (k = 0; k < data_pnts_cnt; ++k)
  {
    x[k] = k * dx;
    y[k] = NULL;

    xp[k] = k * dx;
    yp[k] = NULL;

    x2[k] = k * dx;
    y2[k] = NULL;

    x3[k] = k * dx;
    y3[k] = NULL;

    xbp[k] = k * dx;
    ybp[k] = NULL;
  }

  slope_xyseries_update_data(SLOPE_XYSERIES(seriesp), xp, yp, count);
  slope_xyseries_update_data(SLOPE_XYSERIES(series), x, y, count);
  slope_xyseries_update_data(SLOPE_XYSERIES(series2), x2, y2, count);
  slope_xyseries_update_data(SLOPE_XYSERIES(series3), x3, y3, count);

  slope_xyseries_update_data(SLOPE_XYSERIES(seriesbp), xbp, ybp, countbp);

  slope_view_redraw(SLOPE_VIEW(chartp));
  slope_view_redraw(SLOPE_VIEW(chartbp));
}

void clear_text_view(GtkTextView *text_view)
{
  GtkTextBuffer *empty_buffer = gtk_text_buffer_new(NULL);
  gtk_text_view_set_buffer(text_view, empty_buffer);
}

int come_to_initial_state()
{
#ifdef bluetooth_raspi
  sen("mpc_off"); // turning off the main lasers
  g_clear_object(&gcamera);
  if (ARV_IS_CAMERA(gcamera))
  {
    g_print("its failed!");
  }
#endif
  preview_flag = 0;
  main_create_dir = 1; // to create the main folder /REPORTS/pat_id/
  p_create_dir = 0;    // to create the preview folder /REPORTS/pat_id/prev/
  create_dir = 0;      // to create the raw_imgs folder /REPORTS/pat_id/raw_imges/

  roi = 0;           // to count the roi clicks 4 rois 8 clicks
  free_roi_num = -1; // to control the free_roi number
  lsci_started = 0;  // to control the same button for the start and reanlyse
  report_num = 0;    // to count the total number of reports till now saved
  elapsed_time = 0;  // total time elapsed after the start button clicked
  image_no = 1;      // total number of images recorded till now
  timer_flag = 0;    // to record the time moment start button clicked in the camera() loop

  count = 0;          // count total number of images
  reanalyse_flag = 1; // to use the reanayse button as pause and Reanlyse

  data_receive_flag = 0;    // to check the perfusion data received or not
  bp = 0;                   // to check the wifi status connected or not for bp
  data_receive_flag_bp = 0; // to check the bp data reviceived or not

  reanalyse_start = 0;    // for recording the elasped time while pausing the reanlyse
  dynamic_alloc_once = 1; // for the moving_avg of the images to display
  img = 1;                // to control the reanalyse images

  max_y_val_graph = 0; // for ploting the graph in the report

  if (init_pdf == 0)
  {
    init_pdf = 1;
    HPDF_Free(pdf);
  }

  pg_num = 0;       // to control the report pdf page number
  calib_flag = 0;   // to come to initaial state for calib
  toggle_login = 0; // to toggle the login page 1 -> login 0 -> logout

  gtk_button_set_label(GTK_BUTTON(save), "Login Pt");

  free(avg_Z);
  avg_Z = (float *)malloc((wi) * (h) * sizeof(float)); // clearing the static average perfusion image
  memset(avg_Z, 0, wi * h * sizeof(float));

  // g_source_remove(camera_id);
  // g_source_remove(idsim);

  color_active_roi(rect_select_button);

  gtk_button_set_label(GTK_BUTTON(mainevent), "Start");
  gtk_tool_button_set_label(GTK_TOOL_BUTTON(preview), "Preview");
  if (GTK_IS_STYLE_CONTEXT(context))
  {
    gtk_style_context_remove_class(context, "ref_selection");
  }

  clear_graph();

  /******************     images      ****************/
  gtk_image_set_from_pixbuf(GTK_IMAGE(normalimg), NULL);

  char logo[500];
  sprintf(logo, "%scode/ui_data/logo.png", WORKING_DIR);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(logo, NULL);
  pixbuf = gdk_pixbuf_scale_simple(pixbuf, 900, 500, GDK_INTERP_BILINEAR);
  gtk_image_set_from_pixbuf(GTK_IMAGE(lsciimage), pixbuf);

  g_object_unref(pixbuf);
  /*******************      ROI table   **************/
  gtk_label_set_text(GTK_LABEL(r1_mean), "");
  gtk_label_set_text(GTK_LABEL(r2_mean), "");
  gtk_label_set_text(GTK_LABEL(r3_mean), "");
  gtk_label_set_text(GTK_LABEL(r4_mean), "");

  gtk_label_set_text(GTK_LABEL(r1_area), "");
  gtk_label_set_text(GTK_LABEL(r2_area), "");
  gtk_label_set_text(GTK_LABEL(r3_area), "");
  gtk_label_set_text(GTK_LABEL(r4_area), "");

  gtk_label_set_text(GTK_LABEL(r1_sd), "");
  gtk_label_set_text(GTK_LABEL(r2_sd), "");
  gtk_label_set_text(GTK_LABEL(r3_sd), "");
  gtk_label_set_text(GTK_LABEL(r4_sd), "");

  gtk_label_set_text(GTK_LABEL(r1_perc), "");
  gtk_label_set_text(GTK_LABEL(r2_perc), "");
  gtk_label_set_text(GTK_LABEL(r3_perc), "");
  gtk_label_set_text(GTK_LABEL(r4_perc), "");

  gtk_label_set_text(GTK_LABEL(r1_mean_elev), "");
  gtk_label_set_text(GTK_LABEL(r2_mean_elev), "");
  gtk_label_set_text(GTK_LABEL(r3_mean_elev), "");
  gtk_label_set_text(GTK_LABEL(r4_mean_elev), "");

  gtk_label_set_text(GTK_LABEL(r1_sd_elev), "");
  gtk_label_set_text(GTK_LABEL(r2_sd_elev), "");
  gtk_label_set_text(GTK_LABEL(r3_sd_elev), "");
  gtk_label_set_text(GTK_LABEL(r4_sd_elev), "");

  gtk_label_set_text(GTK_LABEL(r1_perc_elev), "");
  gtk_label_set_text(GTK_LABEL(r2_perc_elev), "");
  gtk_label_set_text(GTK_LABEL(r3_perc_elev), "");
  gtk_label_set_text(GTK_LABEL(r4_perc_elev), "");

  gtk_label_set_text(GTK_LABEL(r1_ratio_elev), "");
  gtk_label_set_text(GTK_LABEL(r2_ratio_elev), "");
  gtk_label_set_text(GTK_LABEL(r3_ratio_elev), "");
  gtk_label_set_text(GTK_LABEL(r4_ratio_elev), "");

  // clearing the patient form
  GtkTextView *text_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "named"));
  clear_text_view(text_view);
  text_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "aged"));
  clear_text_view(text_view);
  text_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "dr_named"));
  clear_text_view(text_view);
  text_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "pat_history"));
  clear_text_view(text_view);

  GtkComboBox *combobox = GTK_COMBO_BOX(gtk_builder_get_object(builder, "genderd"));
  gtk_combo_box_set_active(combobox, -1);

  return FALSE;
}

void on_home_response(GtkDialog *dialog, gint response_id, gpointer user_data)
{
  if (response_id == GTK_RESPONSE_OK)
  {
    history_flag = 0; // to control the preview while in history mode
    gtk_widget_hide(window);
    gtk_widget_show(window1);

    gtk_widget_show(back_perfusion);
    gtk_widget_show(back_porh);
    if (GTK_IS_STYLE_CONTEXT(context))
    {
      gtk_style_context_remove_class(context, "ref_selection");
    }

    g_source_remove(camera_id);
    g_source_remove(idsim);

    // need some time to continue execution from the statements that after while(gtk_pending)
    g_timeout_add(500, (GSourceFunc)come_to_initial_state, NULL);
  }
  else if (response_id == GTK_RESPONSE_CANCEL)
  {
  }
  gtk_widget_destroy(GTK_WIDGET(dialog));
}

gboolean on_back_home_clicked(GtkWidget *widget, gpointer data)
{

  GtkWidget *dialog;
  dialog = gtk_dialog_new_with_buttons("Discard Patient Data ?", GTK_WINDOW(window), GTK_DIALOG_MODAL, "Cancel", GTK_RESPONSE_CANCEL, "OK", GTK_RESPONSE_OK, NULL);

  gtk_window_set_default_size(GTK_WINDOW(dialog), 450, 100);

  g_signal_connect(dialog, "response", G_CALLBACK(on_home_response), NULL);
  gtk_dialog_run(GTK_DIALOG(dialog));

  return FALSE;
}

/**********************   ROI Table   actions    **********************/
void color_selection(GtkWidget *widget, GtkWidget *roi_widget)
{
  if (!control_porh)
  {
    gtk_button_set_image(GTK_BUTTON(r1_button), NULL);
    gtk_button_set_image(GTK_BUTTON(r2_button), NULL);
    gtk_button_set_image(GTK_BUTTON(r3_button), NULL);
    gtk_button_set_image(GTK_BUTTON(r4_button), NULL);
  }
  else
  {
  }
  if (GTK_IS_STYLE_CONTEXT(context))
  {
    gtk_style_context_remove_class(context, "ref_selection");
  }
  context = gtk_widget_get_style_context(roi_widget);
  gtk_style_context_add_class(context, "ref_selection");

  char logo[100];
  sprintf(logo, "%scode/ui_data/select.png", WORKING_DIR);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(logo, NULL);
  GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 40, 50, GDK_INTERP_BILINEAR);
  GtkWidget *image = gtk_image_new_from_pixbuf(scaled_pixbuf);
  g_object_unref(pixbuf); // release the original pixbuf

  gtk_button_set_image(GTK_BUTTON(widget), image);
}

gboolean on_click_r1(GtkWidget *widget, gpointer data)
{
  // GdkRGBA button_color;
  // gdk_rgba_parse(&button_color, "#FF0000");
  // gtk_widget_override_background_color(button, &button_color);
  if (!control_porh)
  {
    color_selection(widget, roi_1);
    gtk_label_set_text(GTK_LABEL(r1_perc), "Ref");
    gtk_label_set_text(GTK_LABEL(r2_perc), intToString(cal_percentage(mean_d_h, mean_d_w)));
    gtk_label_set_text(GTK_LABEL(r3_perc), intToString(cal_percentage(mean_d_h, mean_d_2)));
    gtk_label_set_text(GTK_LABEL(r4_perc), intToString(cal_percentage(mean_d_h, mean_d_3)));
  }
  // else
  // {
  //   color_selection(r1_button1, roi_5);
  //   gtk_label_set_text(GTK_LABEL(r1_percnt), "Ref");
  //   gtk_label_set_text(GTK_LABEL(r2_percnt), intToString(cal_percentage(mean_d_h, mean_d_w)));
  //   gtk_label_set_text(GTK_LABEL(r3_percnt), intToString(cal_percentage(mean_d_h, mean_d_2)));
  //   gtk_label_set_text(GTK_LABEL(r4_percnt), intToString(cal_percentage(mean_d_h, mean_d_3)));
  // }
}

gboolean on_click_r2(GtkWidget *widget, gpointer data)
{
  if (!control_porh)
  {
    color_selection(widget, roi_2);
    gtk_label_set_text(GTK_LABEL(r1_perc), intToString(cal_percentage(mean_d_w, mean_d_h)));
    gtk_label_set_text(GTK_LABEL(r2_perc), "Ref");
    gtk_label_set_text(GTK_LABEL(r3_perc), intToString(cal_percentage(mean_d_w, mean_d_2)));
    gtk_label_set_text(GTK_LABEL(r4_perc), intToString(cal_percentage(mean_d_w, mean_d_3)));
  }
  // else
  // {

  //   color_selection(r2_button1, roi_6);
  //   gtk_label_set_text(GTK_LABEL(r1_percnt), intToString(cal_percentage(mean_d_w, mean_d_h)));
  //   gtk_label_set_text(GTK_LABEL(r2_percnt), "Ref");
  //   gtk_label_set_text(GTK_LABEL(r3_percnt), intToString(cal_percentage(mean_d_w, mean_d_2)));
  //   gtk_label_set_text(GTK_LABEL(r4_percnt), intToString(cal_percentage(mean_d_w, mean_d_3)));
  // }
}

gboolean on_click_r3(GtkWidget *widget, gpointer data)
{
  if (!control_porh)
  {
    color_selection(widget, roi_3);
    gtk_label_set_text(GTK_LABEL(r1_perc), intToString(cal_percentage(mean_d_2, mean_d_h)));
    gtk_label_set_text(GTK_LABEL(r2_perc), intToString(cal_percentage(mean_d_2, mean_d_w)));
    gtk_label_set_text(GTK_LABEL(r3_perc), "Ref");
    gtk_label_set_text(GTK_LABEL(r4_perc), intToString(cal_percentage(mean_d_2, mean_d_3)));
  }
  // else
  // {
  //   color_selection(r3_button1, roi_7);
  //   gtk_label_set_text(GTK_LABEL(r1_percnt), intToString(cal_percentage(mean_d_2, mean_d_h)));
  //   gtk_label_set_text(GTK_LABEL(r2_percnt), intToString(cal_percentage(mean_d_2, mean_d_w)));
  //   gtk_label_set_text(GTK_LABEL(r3_percnt), "Ref");
  //   gtk_label_set_text(GTK_LABEL(r4_percnt), intToString(cal_percentage(mean_d_2, mean_d_3)));
  // }
}

gboolean on_click_r4(GtkWidget *widget, gpointer data)
{
  if (!control_porh)
  {
    color_selection(widget, roi_4);
    gtk_label_set_text(GTK_LABEL(r1_perc), intToString(cal_percentage(mean_d_3, mean_d_h)));
    gtk_label_set_text(GTK_LABEL(r2_perc), intToString(cal_percentage(mean_d_3, mean_d_w)));
    gtk_label_set_text(GTK_LABEL(r3_perc), intToString(cal_percentage(mean_d_3, mean_d_2)));
    gtk_label_set_text(GTK_LABEL(r4_perc), "Ref");
  }
  // else
  // {
  //   color_selection(r4_button1, roi_8);
  //   gtk_label_set_text(GTK_LABEL(r1_percnt), intToString(cal_percentage(mean_d_3, mean_d_h)));
  //   gtk_label_set_text(GTK_LABEL(r2_percnt), intToString(cal_percentage(mean_d_3, mean_d_w)));
  //   gtk_label_set_text(GTK_LABEL(r3_percnt), intToString(cal_percentage(mean_d_3, mean_d_2)));
  //   gtk_label_set_text(GTK_LABEL(r4_percnt), "Ref");
  // }
}
// float scale = 1.0;
char image_name[300];
void zooming(float scale, int width, int height, char image_name[300], GtkWidget *lsciimage)
{
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_name, NULL);
  if (GDK_IS_PIXBUF(pixbuf))
  {
    pixbuf = gdk_pixbuf_scale_simple(pixbuf, width * scale, height * scale, GDK_INTERP_BILINEAR);
    pixbuf = gdk_pixbuf_new_subpixbuf(pixbuf, (width * scale - width) / 2, (height * scale - height) / 2, width, height);
    // pixbuf = gdk_pixbuf_scale_simple(pixbuf, width , height, GDK_INTERP_BILINEAR);
    gtk_image_set_from_pixbuf(GTK_IMAGE(lsciimage), pixbuf);

    g_object_unref(pixbuf);
  }
}

void exposure_increase(GtkWidget *widget, gpointer data)
{
  if (!history_flag)
  {
    exposure_tim = exposure_tim + 2.5;
    if (exposure_tim >= 20)
    {
      exposure_tim = 20;
    }
    exposureTime = (int)exposure_tim * 1000;
    arv_camera_set_exposure_time(gcamera, exposureTime, &error);
    sprintf(exp_tim_string, "%.1f ms", exposure_tim);
    gtk_label_set_text(GTK_LABEL(exp_tim_id), exp_tim_string);
  }
}
void exposure_decrease(GtkWidget *widget, gpointer data)
{
  if (!history_flag)
  {
    exposure_tim = exposure_tim - 2.5;
    if (exposure_tim <= 2.5)
    {
      exposure_tim = 2.5;
    }
    exposureTime = (int)exposure_tim * 1000;
    arv_camera_set_exposure_time(gcamera, exposureTime, &error);
    sprintf(exp_tim_string, "%.1f ms", exposure_tim);
    gtk_label_set_text(GTK_LABEL(exp_tim_id), exp_tim_string);
  }
}
void zoom_in(GtkWidget *widget, gpointer data)
{
  scale += 0.1;
  if (scale > 2)
  {
    scale = 2;
  }

  sprintf(image_name, "%sprev/normal_pic.jpg", current_working_dir);
  zooming(scale, 600, 334, image_name, normalimg);
  sprintf(image_name, "%sprev/lsci.png", current_working_dir);
  zooming(scale, 900, 500, image_name, lsciimage);
}

void zoom_out(GtkWidget *widget, gpointer data)
{
  scale -= 0.1;
  if (scale < 1)
  {
    scale = 1;
  }
  sprintf(image_name, "%sprev/normal_pic.jpg", current_working_dir);
  zooming(scale, 600, 334, image_name, normalimg);
  sprintf(image_name, "%sprev/lsci.png", current_working_dir);
  zooming(scale, 900, 500, image_name, lsciimage);
}

int interaction;
void init_graph()
{
  /*************************      chart BP     **************************/
  xbp = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));
  ybp = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));

  chartbp = slope_view_new();
  figurebp = slope_figure_new();
  // slope_figure_set_background_color(figurebp, SLOPE_BLACK);
  slope_view_set_figure(SLOPE_VIEW(chartbp), figurebp);
  scalebp = slope_xyscale_new();
  slope_figure_add_scale(SLOPE_FIGURE(figurebp), scalebp);
  seriesbp = slope_xyseries_new_filled("BP data", xbp, ybp, n, "b-");

  slope_scale_add_item(scalebp, seriesbp);

  gtk_container_remove(GTK_CONTAINER(bpbox), bpgraph);
  gtk_box_pack_start(GTK_BOX(bpbox), chartbp, TRUE, TRUE, 0);

  gtk_widget_set_size_request(chartbp, 850, 450);
  gtk_widget_show(chartbp);

  /*************************      chart perfusion     **************************/
  /* create some sinusoidal data points */
  x = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));
  y = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));

  xp = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));
  yp = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));

  x2 = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));
  y2 = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));

  x3 = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));
  y3 = (double_t *)g_malloc(data_pnts_cnt * sizeof(double));

  /* the amplitude for the sine wave gives the SCALE of the plot */

  long k;
  for (k = 0; k < data_pnts_cnt; ++k)
  {
    x[k] = k * dx;
    y[k] = 0;

    xp[k] = k * dx;
    yp[k] = 0;

    x2[k] = k * dx;
    y2[k] = 0;

    x3[k] = k * dx;
    y3[k] = 0;

    xbp[k] = k * dx;
    ybp[k] = 0;
  }

  chartp = slope_view_new();
  figurep = slope_figure_new();
  slope_view_set_figure(SLOPE_VIEW(chartp), figurep);
  // slope_view_set_figure(SLOPE_VIEW(chart), figurep);

  scalep = slope_xyscale_new();

  slope_figure_add_scale(SLOPE_FIGURE(figurep), scalep);
  // slope_xyscale_set_axis(scalep, SLOPE_XYSCALE_FRAME_AXIS_GRID)

  seriesp = slope_xyseries_new_filled("ROI 1(Ref)", xp, yp, n, "b-");
  // slope_legend_get_position(me,&a,&b);
  // printf("The cioreordiatios %f  %f",a,b);
  series = slope_xyseries_new_filled("ROI 2", x, y, n, "r-");
  series2 = slope_xyseries_new_filled("ROI 3", x2, y2, n, "g-");
  series3 = slope_xyseries_new_filled("ROI 4", x3, y3, n, "m-"); // brown

  slope_scale_add_item(scalep, seriesp);
  slope_scale_add_item(scalep, series);
  slope_scale_add_item(scalep, series2);
  slope_scale_add_item(scalep, series3);

  gtk_container_remove(GTK_CONTAINER(pmbox), pmgraph);
  gtk_box_pack_start(GTK_BOX(pmbox), chartp, TRUE, TRUE, 0);
  gtk_widget_set_size_request(chartp, 850, 450);
  gtk_widget_show(chartp);

  slope_xyseries_update_data(SLOPE_XYSERIES(seriesp), xp, yp, count);
  slope_xyseries_update_data(SLOPE_XYSERIES(series), x, y, count);
  slope_xyseries_update_data(SLOPE_XYSERIES(series2), x2, y2, count);
  slope_xyseries_update_data(SLOPE_XYSERIES(series3), x3, y3, count);

  // slope_view_redraw(SLOPE_VIEW(chartp));

  slope_xyseries_update_data(SLOPE_XYSERIES(seriesbp), xbp, ybp, countbp);
  slope_view_redraw(SLOPE_VIEW(chartbp));
}

GtkWidget *scale_widgetu, *scale_widgetl;
static void on_scale_value_changed_upper(GtkWidget *widget, gpointer data)
{
  GtkScale *scale = GTK_SCALE(widget);
  gdouble value = gtk_range_get_value(GTK_RANGE(scale));
  // Update the volume or perform any other action here with 'value'
  cmap_max_val = value;
  if (img == 1)
  {
    show_avg_perf_img(0);
  }
}

static void on_scale_value_changed_lower(GtkWidget *widget, gpointer data)
{
  GtkScale *scale = GTK_SCALE(widget);
  gdouble value = gtk_range_get_value(GTK_RANGE(scale));
  // Update the volume or perform any other action here with 'value'
  cmap_min_val = value;
  if (img == 1)
  {
    show_avg_perf_img(0);
  }
}




void on_window_destroy(GtkWidget *widget, gpointer user_data)
{
  gtk_main_quit();
}

GtkWidget *tool_bar;
void activate(GtkApplication *app, gpointer user_data)
{
  char ui_file[200];
  sprintf(ui_file, "%s/code/testglade.glade", WORKING_DIR);
  builder = gtk_builder_new();

  if (gtk_builder_add_from_file(builder, ui_file, NULL) == 0)
  {
    g_error("Error loading UI file");
    // Handle the error here, such as showing an error message to the user.
    exit(1);
  }

  char config[200];
  sprintf(config, "%scode/camera.config", WORKING_DIR);
  loadConfig(config, &exposureTime, &fps, &window_size, rois, &beta, &SG);
  // g_print("%d %d %d %d %f",exposureTime, fps, window_size, roi1[0], SG);

  window = GTK_WIDGET(gtk_builder_get_object(builder, WIDGET_WINDOW));
  window1 = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
  window2 = GTK_WIDGET(gtk_builder_get_object(builder, "window2"));
  history_window = GTK_WIDGET(gtk_builder_get_object(builder, "history"));

  gtk_window_set_position(GTK_WINDOW(window1), GTK_WIN_POS_CENTER);
  // style = gtk_widget_get_style_context(GTK_WIDGET(window));
  // gtk_widget_override_background_color(GTK_WIDGET(window), GTK_STATE_FLAG_NORMAL, &color);
  fixed = GTK_WIDGET(gtk_builder_get_object(builder, "fixed"));
  fixed4 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed4"));
  event_box = GTK_WIDGET(gtk_builder_get_object(builder, "event_box"));
  event_box_normal = GTK_WIDGET(gtk_builder_get_object(builder, "event_box_normal"));
  normalbox = GTK_WIDGET(gtk_builder_get_object(builder, "normalbox"));

  lsciimage = GTK_WIDGET(gtk_builder_get_object(builder, "lsciimage"));
  normalimg = GTK_WIDGET(gtk_builder_get_object(builder, "normalimg"));
  heading_angio = GTK_WIDGET(gtk_builder_get_object(builder, "heading_angio"));
  lscibox = GTK_WIDGET(gtk_builder_get_object(builder, "lscibox"));

  spinner_image = GTK_WIDGET(gtk_builder_get_object(builder, "spinner_image"));
  warning_msg = GTK_WIDGET(gtk_builder_get_object(builder, "warning_msg"));
  elevation_label = GTK_WIDGET(gtk_builder_get_object(builder, "elevation_label"));
  // gtk_spinner_start(spinner_image);

  char logo[500];
  sprintf(logo, "%scode/ui_data/logo.png", WORKING_DIR);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(logo, NULL);
  pixbuf = gdk_pixbuf_scale_simple(pixbuf, 900, 500, GDK_INTERP_BILINEAR);
  gtk_image_set_from_pixbuf(GTK_IMAGE(lsciimage), pixbuf);

  sprintf(logo, "%scode/ui_data/Angiocam.png", WORKING_DIR);
  pixbuf = gdk_pixbuf_new_from_file(logo, NULL);
  pixbuf = gdk_pixbuf_scale_simple(pixbuf, 600, 130, GDK_INTERP_BILINEAR);
  gtk_image_set_from_pixbuf(GTK_IMAGE(heading_angio), pixbuf);

  previewlsci = GTK_WIDGET(gtk_builder_get_object(builder, "previewlsci"));
  pmgraph = GTK_WIDGET(gtk_builder_get_object(builder, "perfusiongraph"));
  pmbox = GTK_WIDGET(gtk_builder_get_object(builder, "perfusionbox"));

  /*********************    porh page   **********************/
  pmgraph1 = GTK_WIDGET(gtk_builder_get_object(builder, "perfusiongraph1"));
  pmbox1 = GTK_WIDGET(gtk_builder_get_object(builder, "perfusionbox1"));

  bpbox = GTK_WIDGET(gtk_builder_get_object(builder, "bp_box"));
  bpgraph = GTK_WIDGET(gtk_builder_get_object(builder, "bpgraph"));
  savepm = GTK_WIDGET(gtk_builder_get_object(builder, "savepm"));
  bp_prompt = GTK_WIDGET(gtk_builder_get_object(builder, "bp_prompt"));
  bp_prompt_val = GTK_WIDGET(gtk_builder_get_object(builder, "bp_prompt_val"));

  preview = GTK_WIDGET(gtk_builder_get_object(builder, "preview"));
  calibrate = GTK_WIDGET(gtk_builder_get_object(builder, "calibrate"));
  mainevent = GTK_WIDGET(gtk_builder_get_object(builder, "mainevent"));

  // save_analysis = GTK_WIDGET(gtk_builder_get_object(builder, "save_analysis"));
  report_gen = GTK_WIDGET(gtk_builder_get_object(builder, "report_gen"));

  free_select_button = GTK_WIDGET(gtk_builder_get_object(builder, "free_select_button"));
  rect_select_button = GTK_WIDGET(gtk_builder_get_object(builder, "rect_select_button"));

  exp_tim_id = GTK_WIDGET(gtk_builder_get_object(builder, "exp_tim_id"));
  exp_decrease = GTK_WIDGET(gtk_builder_get_object(builder, "exp_decrease"));
  exp_increase = GTK_WIDGET(gtk_builder_get_object(builder, "exp_increase"));

  // rect_select_button = GTK_WIDGET(gtk_builder_get_object(builder, "show_roi_clicked"));

  // GtkWidget image = gtk_image_new_from_file("myimage.png");
  // gtk_tool_button_set_image(GTK_TOOL_BUTTON(free_select_button), image);

  bp_connect = GTK_WIDGET(gtk_builder_get_object(builder, "bp_connect"));

  zoom_in_button = GTK_WIDGET(gtk_builder_get_object(builder, "zoom_in_button"));
  zoom_out_button = GTK_WIDGET(gtk_builder_get_object(builder, "zoom_out_button"));

  back_home_button = GTK_WIDGET(gtk_builder_get_object(builder, "back_home_button"));
  back_porh = GTK_WIDGET(gtk_builder_get_object(builder, "back_porh"));
  back_perfusion = GTK_WIDGET(gtk_builder_get_object(builder, "back_perfusion"));
  back_elevation = GTK_WIDGET(gtk_builder_get_object(builder, "back_elevation"));

  tool_bar = GTK_WIDGET(gtk_builder_get_object(builder, "tool_bar"));

  alltests_buttons = GTK_WIDGET(gtk_builder_get_object(builder, "alltests_buttons"));
  test = GTK_WIDGET(gtk_builder_get_object(builder, "test"));
  start_new_test = GTK_WIDGET(gtk_builder_get_object(builder, "start_new_test"));
  history_button = GTK_WIDGET(gtk_builder_get_object(builder, "history_button"));

  // test     = GTK_WIDGET(gtk_builder_get_object(builder, "test"));
  home = GTK_WIDGET(gtk_builder_get_object(builder, "home"));
  save = GTK_WIDGET(gtk_builder_get_object(builder, "save"));

  roi_table = GTK_WIDGET(gtk_builder_get_object(builder, "roi_table"));
  roi_table_porh = GTK_WIDGET(gtk_builder_get_object(builder, "roi_table_porh"));
  elevation_buttons = GTK_WIDGET(gtk_builder_get_object(builder, "elevation_buttons"));
  roi_table_elevation = GTK_WIDGET(gtk_builder_get_object(builder, "roi_table_elevation"));
  porh_label_box = GTK_WIDGET(gtk_builder_get_object(builder, "porh_label_box"));

  r1_button = GTK_WIDGET(gtk_builder_get_object(builder, "r1_button"));
  r2_button = GTK_WIDGET(gtk_builder_get_object(builder, "r2_button"));
  r3_button = GTK_WIDGET(gtk_builder_get_object(builder, "r3_button"));
  r4_button = GTK_WIDGET(gtk_builder_get_object(builder, "r4_button"));

  roi_1 = GTK_WIDGET(gtk_builder_get_object(builder, "roi_1"));
  roi_2 = GTK_WIDGET(gtk_builder_get_object(builder, "roi_2"));
  roi_3 = GTK_WIDGET(gtk_builder_get_object(builder, "roi_3"));
  roi_4 = GTK_WIDGET(gtk_builder_get_object(builder, "roi_4"));
  roi_5 = GTK_WIDGET(gtk_builder_get_object(builder, "roi_5"));
  roi_6 = GTK_WIDGET(gtk_builder_get_object(builder, "roi_6"));
  roi_7 = GTK_WIDGET(gtk_builder_get_object(builder, "roi_7"));
  roi_8 = GTK_WIDGET(gtk_builder_get_object(builder, "roi_8"));

  r1_area = GTK_WIDGET(gtk_builder_get_object(builder, "r1_area"));
  r2_area = GTK_WIDGET(gtk_builder_get_object(builder, "r2_area"));
  r3_area = GTK_WIDGET(gtk_builder_get_object(builder, "r3_area"));
  r4_area = GTK_WIDGET(gtk_builder_get_object(builder, "r4_area"));

  r1_mean = GTK_WIDGET(gtk_builder_get_object(builder, "r1_mean"));
  r2_mean = GTK_WIDGET(gtk_builder_get_object(builder, "r2_mean"));
  r3_mean = GTK_WIDGET(gtk_builder_get_object(builder, "r3_mean"));
  r4_mean = GTK_WIDGET(gtk_builder_get_object(builder, "r4_mean"));

  r1_perc = GTK_WIDGET(gtk_builder_get_object(builder, "r1_perc"));
  r2_perc = GTK_WIDGET(gtk_builder_get_object(builder, "r2_perc"));
  r3_perc = GTK_WIDGET(gtk_builder_get_object(builder, "r3_perc"));
  r4_perc = GTK_WIDGET(gtk_builder_get_object(builder, "r4_perc"));

  r1_sd = GTK_WIDGET(gtk_builder_get_object(builder, "r1_std"));
  r2_sd = GTK_WIDGET(gtk_builder_get_object(builder, "r2_std"));
  r3_sd = GTK_WIDGET(gtk_builder_get_object(builder, "r3_std"));
  r4_sd = GTK_WIDGET(gtk_builder_get_object(builder, "r4_std"));

  // elevation table

  r1_mean_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r1_mean_elev"));
  r2_mean_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r2_mean_elev"));
  r3_mean_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r3_mean_elev"));
  r4_mean_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r4_mean_elev"));

  r1_perc_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r1_perc_elev"));
  r2_perc_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r2_perc_elev"));
  r3_perc_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r3_perc_elev"));
  r4_perc_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r4_perc_elev"));

  r1_sd_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r1_std_elev"));
  r2_sd_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r2_std_elev"));
  r3_sd_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r3_std_elev"));
  r4_sd_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r4_std_elev"));

  r1_ratio_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r1_ratio_elev"));
  r2_ratio_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r2_ratio_elev"));
  r3_ratio_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r3_ratio_elev"));
  r4_ratio_elev = GTK_WIDGET(gtk_builder_get_object(builder, "r4_ratio_elev"));

  /***********************    table porh    *************************/

  r1_rf = GTK_WIDGET(gtk_builder_get_object(builder, "r1_rf"));
  r2_rf = GTK_WIDGET(gtk_builder_get_object(builder, "r2_rf"));
  r3_rf = GTK_WIDGET(gtk_builder_get_object(builder, "r3_rf"));
  r4_rf = GTK_WIDGET(gtk_builder_get_object(builder, "r4_rf"));

  r1_bz = GTK_WIDGET(gtk_builder_get_object(builder, "r1_bz"));
  r2_bz = GTK_WIDGET(gtk_builder_get_object(builder, "r2_bz"));
  r3_bz = GTK_WIDGET(gtk_builder_get_object(builder, "r3_bz"));
  r4_bz = GTK_WIDGET(gtk_builder_get_object(builder, "r4_bz"));

  r1_pf = GTK_WIDGET(gtk_builder_get_object(builder, "r1_pf"));
  r2_pf = GTK_WIDGET(gtk_builder_get_object(builder, "r2_pf"));
  r3_pf = GTK_WIDGET(gtk_builder_get_object(builder, "r3_pf"));
  r4_pf = GTK_WIDGET(gtk_builder_get_object(builder, "r4_pf"));

  /*
    r1_tm = GTK_WIDGET(gtk_builder_get_object(builder, "r1_tm"));
    r2_tm = GTK_WIDGET(gtk_builder_get_object(builder, "r2_tm"));
    r3_tm = GTK_WIDGET(gtk_builder_get_object(builder, "r3_tm"));
    r4_tm = GTK_WIDGET(gtk_builder_get_object(builder, "r4_tm"));

    r1_th = GTK_WIDGET(gtk_builder_get_object(builder, "r1_th"));
    r2_th = GTK_WIDGET(gtk_builder_get_object(builder, "r2_th"));
    r3_th = GTK_WIDGET(gtk_builder_get_object(builder, "r3_th"));
    r4_th = GTK_WIDGET(gtk_builder_get_object(builder, "r4_th"));

    r1_tb = GTK_WIDGET(gtk_builder_get_object(builder, "r1_tb"));
    r2_tb = GTK_WIDGET(gtk_builder_get_object(builder, "r2_tb"));
    r3_tb = GTK_WIDGET(gtk_builder_get_object(builder, "r3_tb"));
    r4_tb = GTK_WIDGET(gtk_builder_get_object(builder, "r4_tb"));
  */

  r1_rf2bz = GTK_WIDGET(gtk_builder_get_object(builder, "r1_rf2bz"));
  r2_rf2bz = GTK_WIDGET(gtk_builder_get_object(builder, "r2_rf2bz"));
  r3_rf2bz = GTK_WIDGET(gtk_builder_get_object(builder, "r3_rf2bz"));
  r4_rf2bz = GTK_WIDGET(gtk_builder_get_object(builder, "r4_rf2bz"));

  r1_bz2pf = GTK_WIDGET(gtk_builder_get_object(builder, "r1_bz2pf"));
  r2_bz2pf = GTK_WIDGET(gtk_builder_get_object(builder, "r2_bz2pf"));
  r3_bz2pf = GTK_WIDGET(gtk_builder_get_object(builder, "r3_bz2pf"));
  r4_bz2pf = GTK_WIDGET(gtk_builder_get_object(builder, "r4_bz2pf"));

  r1_rf2pf = GTK_WIDGET(gtk_builder_get_object(builder, "r1_rf2pf"));
  r2_rf2pf = GTK_WIDGET(gtk_builder_get_object(builder, "r2_rf2pf"));
  r3_rf2pf = GTK_WIDGET(gtk_builder_get_object(builder, "r3_rf2pf"));
  r4_rf2pf = GTK_WIDGET(gtk_builder_get_object(builder, "r4_rf2pf"));

  intro_animation = GTK_WIDGET(gtk_builder_get_object(builder, "intro_animation"));

  scale_widgetu = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 400, 10);
  gtk_scale_set_value_pos(GTK_SCALE(scale_widgetu), GTK_POS_BOTTOM);
  gtk_scale_add_mark(GTK_SCALE(scale_widgetu), 0, GTK_POS_TOP, "0");
  gtk_scale_add_mark(GTK_SCALE(scale_widgetu), 100, GTK_POS_TOP, "100");
  gtk_scale_add_mark(GTK_SCALE(scale_widgetu), 200, GTK_POS_TOP, "200");
  gtk_scale_add_mark(GTK_SCALE(scale_widgetu), 300, GTK_POS_TOP, "300");
  gtk_scale_add_mark(GTK_SCALE(scale_widgetu), 400, GTK_POS_TOP, "400");
  gtk_scale_set_digits(GTK_SCALE(scale_widgetu), 0);
  gtk_range_set_value(GTK_RANGE(scale_widgetu), 120);

  scale_widgetl = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 200, 10);
  gtk_scale_set_value_pos(GTK_SCALE(scale_widgetl), GTK_POS_BOTTOM);
  gtk_scale_add_mark(GTK_SCALE(scale_widgetl), 0, GTK_POS_TOP, "0");
  gtk_scale_add_mark(GTK_SCALE(scale_widgetl), 50, GTK_POS_TOP, "50");
  gtk_scale_add_mark(GTK_SCALE(scale_widgetl), 100, GTK_POS_TOP, "100");
  gtk_scale_add_mark(GTK_SCALE(scale_widgetl), 150, GTK_POS_TOP, "150");
  gtk_scale_add_mark(GTK_SCALE(scale_widgetl), 200, GTK_POS_TOP, "200");
  gtk_scale_set_digits(GTK_SCALE(scale_widgetl), 0);
  gtk_range_set_value(GTK_RANGE(scale_widgetl), 30);

  g_signal_connect(scale_widgetl, "value-changed", G_CALLBACK(on_scale_value_changed_lower), NULL);
  g_signal_connect(scale_widgetu, "value-changed", G_CALLBACK(on_scale_value_changed_upper), NULL);
  GtkStyleContext *u_scale = gtk_widget_get_style_context(scale_widgetu);
  GtkStyleContext *l_scale = gtk_widget_get_style_context(scale_widgetl);
  gtk_style_context_add_class(u_scale, "u_scale");
  gtk_style_context_add_class(l_scale, "l_scale");

  GtkWidget *scale_heading = gtk_label_new("Adjust ColorMap");
  GtkStyleContext *scale_head = gtk_widget_get_style_context(scale_heading);
  gtk_style_context_add_class(scale_head, "scale_head");

  // Pack the scale into the window
  cmap_scale_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_box_pack_start(GTK_BOX(cmap_scale_box), scale_heading, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(cmap_scale_box), scale_widgetl, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(cmap_scale_box), scale_widgetu, TRUE, TRUE, 10);
  gtk_widget_show_all(cmap_scale_box);
  gtk_widget_set_size_request(cmap_scale_box, 334, 10);
  gtk_fixed_put(GTK_FIXED(fixed), cmap_scale_box, 670, 780);

  /*******************     history      ***********************/
  calendar = GTK_WIDGET(gtk_builder_get_object(builder, "calendar"));
  cal_box = GTK_WIDGET(gtk_builder_get_object(builder, "cal_box"));
  test_date_from = GTK_WIDGET(gtk_builder_get_object(builder, "test_date_from"));
  test_date_to = GTK_WIDGET(gtk_builder_get_object(builder, "test_date_to"));

  char gif[100];
  sprintf(gif, "%scode/ui_data/Angiocam_intro.gif", WORKING_DIR);
  animation = gdk_pixbuf_animation_new_from_file(gif, NULL);

  gtk_image_set_from_animation(GTK_IMAGE(intro_animation), animation);

#ifdef sql_database
  tree_view = create_tree_view();
  // Create a list store model
  list_store = gtk_list_store_new(COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
  // Set the model for the tree view
  gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(list_store));

  fill_list();

  // Connect the "row-activated" signal of the tree view to the callback function
  g_signal_connect(tree_view, "row-activated", G_CALLBACK(on_row_activated), NULL);

  // Add the tree view to a scrollable container
  GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
  int window_width = 1200;
  gtk_widget_set_size_request(scrolled_window, window_width, 400);

  gtk_fixed_put(GTK_FIXED(fixed4), scrolled_window, (960 - window_width / 2), 550);
  // gtk_container_add(GTK_CONTAINER(fixed4), scrolled_window);

  context_scrolled_window = gtk_widget_get_style_context(tree_view);
  gtk_style_context_add_class(context_scrolled_window, "scrolll_window");

  gtk_builder_add_callback_symbol(builder, "onclick_find", G_CALLBACK(onclick_find));
  /*
  gtk_builder_add_callback_symbol(builder, "onclickfind_enter", G_CALLBACK(onclickfind_enter));

  id_find = GTK_WIDGET(gtk_builder_get_object(builder, "id_find"));
  g_signal_connect(id_find, "enter-notify-event", G_CALLBACK(onclickfind_enter), NULL);
  //gtk_builder_add_callback_symbol(builder, "onclick_find", G_CALLBACK(onclick_find));
  */

#endif

  init_graph();

  //  g_timeout_add(500, (GSourceFunc)draw_calib_roi, draw_ptr);

  gtk_builder_add_callback_symbol(builder, "ontogglepreviewlsci", G_CALLBACK(ontogglepreviewlsci));
  gtk_builder_add_callback_symbol(builder, "onclick_calibrate", G_CALLBACK(onclick_calibrate));
  gtk_builder_add_callback_symbol(builder, "onclickmainevent", G_CALLBACK(onclickmainevent));
  gtk_builder_add_callback_symbol(builder, "switch_elevation", G_CALLBACK(switch_elevation));

  gtk_builder_add_callback_symbol(builder, "onclick_report_gen", G_CALLBACK(onclick_report_gen));

  gtk_builder_add_callback_symbol(builder, "on_click_r1", G_CALLBACK(on_click_r1));
  gtk_builder_add_callback_symbol(builder, "on_click_r2", G_CALLBACK(on_click_r2));
  gtk_builder_add_callback_symbol(builder, "on_click_r3", G_CALLBACK(on_click_r3));
  gtk_builder_add_callback_symbol(builder, "on_click_r4", G_CALLBACK(on_click_r4));

  gtk_builder_add_callback_symbol(builder, "exposure_increase", G_CALLBACK(exposure_increase));
  gtk_builder_add_callback_symbol(builder, "exposure_decrease", G_CALLBACK(exposure_decrease));

  gtk_builder_add_callback_symbol(builder, "zoom_in", G_CALLBACK(zoom_in));
  gtk_builder_add_callback_symbol(builder, "zoom_out", G_CALLBACK(zoom_out));
  gtk_builder_add_callback_symbol(builder, "onclick_bp_connect", G_CALLBACK(onclick_bp_connect));

  gtk_builder_add_callback_symbol(builder, "onclick_free_select", G_CALLBACK(onclick_free_select));
  gtk_builder_add_callback_symbol(builder, "onclick_rect_select", G_CALLBACK(onclick_rect_select));
  gtk_builder_add_callback_symbol(builder, "onclick_circle_select", G_CALLBACK(onclick_circle_select));
  gtk_builder_add_callback_symbol(builder, "show_roi_clicked", G_CALLBACK(show_roi_clicked));
  gtk_builder_add_callback_symbol(builder, "on_back_home_clicked", G_CALLBACK(on_back_home_clicked));

  gtk_builder_add_callback_symbol(builder, "mouse_press_callback", G_CALLBACK(mouse_press_callback));
  gtk_builder_add_callback_symbol(builder, "mouse_press_callback_normal", G_CALLBACK(mouse_press_callback_normal));

  /*********************    Page 1 button callbacks   **********************/

  gtk_builder_add_callback_symbol(builder, "onclick_history", G_CALLBACK(onclick_history));
  gtk_builder_add_callback_symbol(builder, "onclick_new_test", G_CALLBACK(onclick_new_test));

  /*********************  page 2    callbacks   **************************/
  gtk_builder_add_callback_symbol(builder, "onclick_perfusion", G_CALLBACK(onclick_perfusion));
  gtk_builder_add_callback_symbol(builder, "onclick_elevation", G_CALLBACK(onclick_elevation));
  gtk_builder_add_callback_symbol(builder, "onclick_porh", G_CALLBACK(onclick_porh));
  gtk_builder_add_callback_symbol(builder, "onclick_home", G_CALLBACK(onclick_home));
  gtk_builder_add_callback_symbol(builder, "onclick_save", G_CALLBACK(onclick_save));

  gtk_builder_add_callback_symbol(builder, "calender_dropped_from", G_CALLBACK(calender_dropped_from));
  gtk_builder_add_callback_symbol(builder, "calender_dropped_to", G_CALLBACK(calender_dropped_to));
  gtk_builder_add_callback_symbol(builder, "date_selected", G_CALLBACK(date_selected));

  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(window1), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(window2), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(history_window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_builder_connect_signals(builder, NULL);

  GdkScreen *screen = gdk_screen_get_default();
  GtkCssProvider *provider = gtk_css_provider_new();
  char style_name[500];
  sprintf(style_name, "%scode/style.css", WORKING_DIR);
  gtk_css_provider_load_from_path(provider, style_name, NULL);
  gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  gtk_widget_show(window1);
}

void sigsegv_handler(int signal)
{
  printf("Segmentation fault occurred!\n");
#ifdef bluetooth_raspi
  sen("mpc_off"); // turning off the main pointing lasers and camera
#endif
  exit(1); // Terminate the program
}

int main(int argc, char *argv[])
{
  // special_case = 1;
  /* Initialize gtk+*/
  gtk_init(&argc, &argv);
  signal(SIGSEGV, sigsegv_handler); // to handle the segmentation fault
  if (gpu_init() == -1)
  {
    g_print("GPU is not detected!\n");
    exit(1);
  }

  GtkApplication *app;
  int status;

  app = gtk_application_new("com.angiocam.myapp", G_APPLICATION_FLAGS_NONE);

  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  gtk_main();

#ifdef bluetooth_raspi
  sen("mpc_off"); // turning off the main lasers
#endif

  g_clear_object(&stream);
  g_clear_object(&gcamera);
  if (x != NULL)
  {
    g_free(x);
  }
  if (init_pdf == 0)
  {
    init_pdf = 1;
    HPDF_Free(pdf);
  }

  g_print("\nApp closed\n");
  return 0;
}
