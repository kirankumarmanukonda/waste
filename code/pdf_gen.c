#include "pdf_gen.h"



void Page_headings(const char *study_type, char *name, char *age, char *gender, char *pat_id, char *pat_clin)
{
  /* Print the title of the page (with positioning center). */
  /*
  def_font = HPDF_GetFont(pdf, "Courier-Bold", NULL);
  HPDF_Page_SetFontAndSize(page[pg_num], def_font, 30);

  tw = HPDF_Page_TextWidth(page[pg_num], page_title);
  HPDF_Page_BeginText(page[pg_num]);
  HPDF_Page_TextOut(page[pg_num], (width - tw) / 2, height - 50, page_title);
  HPDF_Page_EndText(page[pg_num]);
  * */

  char pat_duration[15];
  sprintf(pat_duration, "%s seconds", floatToString(elapsed_time));

  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  char date[20], time[20];
  strftime(date, sizeof(date), "%d-%m-%Y", tm);
  strftime(time, sizeof(time), "%H:%M:%S", tm);

  HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
  HPDF_Font fontB = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);

  /* output subtitle. */
  HPDF_Page_BeginText(page[pg_num]);

  HPDF_Page_SetFontAndSize(page[pg_num], fontB, 10);
  HPDF_Page_TextOut(page[pg_num], 50, height - 80, "Patient ID  :");
  HPDF_Page_TextOut(page[pg_num], 108, height - 80, pat_id);
  HPDF_Page_TextOut(page[pg_num], 50, height - 100, "Name       :");
  HPDF_Page_TextOut(page[pg_num], 105, height - 100, name);
  HPDF_Page_TextOut(page[pg_num], 50, height - 120, "Ref. Doctor :");
  HPDF_Page_TextOut(page[pg_num], 115, height - 120, pat_doc);
  HPDF_Page_TextOut(page[pg_num], 50, height - 140, "Image Size :");
  HPDF_Page_TextOut(page[pg_num], 105, height - 140, "   1920 X 1200");

  HPDF_Page_TextOut(page[pg_num], 250, height - 80, "Age      :");
  HPDF_Page_TextOut(page[pg_num], 295, height - 80, age);
  HPDF_Page_TextOut(page[pg_num], 250, height - 100, "Gender :");
  HPDF_Page_TextOut(page[pg_num], 295, height - 100, gender);
  HPDF_Page_TextOut(page[pg_num], 250, height - 120, "Frame Rate :");
  HPDF_Page_TextOut(page[pg_num], 320, height - 120, intToString(image_no / elapsed_time));
  HPDF_Page_TextOut(page[pg_num], 250, height - 140, "No. Images :");
  HPDF_Page_TextOut(page[pg_num], 320, height - 140, intToString(image_no));

  HPDF_Page_TextOut(page[pg_num], 400, height - 80, "Report Date :");
  HPDF_Page_TextOut(page[pg_num], 480, height - 80, date);
  HPDF_Page_TextOut(page[pg_num], 400, height - 100, "Report Time :");
  HPDF_Page_TextOut(page[pg_num], 480, height - 100, time);
  HPDF_Page_TextOut(page[pg_num], 400, height - 120, "Duration    :");
  HPDF_Page_TextOut(page[pg_num], 480, height - 120, pat_duration);

  char test_num[30];
  sprintf(test_num, "Test   :    %03d", report_num);
  HPDF_Page_SetFontAndSize(page[pg_num], fontB, 11);
  HPDF_Page_TextOut(page[pg_num], 50, height - 170, test_num);

  HPDF_Page_SetRGBFill(page[pg_num], 0.05, 0.1, 0.8);
  const char *study = study_type;
  def_font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
  HPDF_Page_SetFontAndSize(page[pg_num], def_font, 15);
  st = HPDF_Page_TextWidth(page[pg_num], study);
  HPDF_Page_TextOut(page[pg_num], (width - st) / 2, height - 170, study);
  HPDF_Page_SetRGBFill(page[pg_num], 0, 0, 0);
  HPDF_Page_EndText(page[pg_num]);

  HPDF_Page_SetLineWidth(page[pg_num], 1);
  HPDF_Page_SetRGBStroke(page[pg_num], 0.05, 0.1, 0.8);
  HPDF_Page_MoveTo(page[pg_num], (width - st) / 2 - 5, height - 172);
  HPDF_Page_LineTo(page[pg_num], (width - st) / 2 - 5 + HPDF_Page_TextWidth(page[pg_num], study) + 5, height - 172);
  HPDF_Page_Stroke(page[pg_num]);

  HPDF_Page_BeginText(page[pg_num]);
  HPDF_Font fontimghed = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
  if (!control_porh)
  {
    HPDF_Page_SetFontAndSize(page[pg_num], fontimghed, 12);
    HPDF_Page_TextOut(page[pg_num], width - 195, height - 230, "PERFUSION IMAGE");
    HPDF_Page_TextOut(page[pg_num], width - 185, height - 480, "NORMAL IMAGE");
  }
  else
  {
    HPDF_Page_SetFontAndSize(page[pg_num], fontimghed, 12);
    HPDF_Page_TextOut(page[pg_num], width - 345, height - 200, "PERFUSION IMAGE");
    HPDF_Page_TextOut(page[pg_num], width - 345, height - 515, "NORMAL IMAGE");
  }
  HPDF_Page_EndText(page[pg_num]);

  HPDF_Page_SetRGBStroke(page[pg_num], 0, 0, 0);
  HPDF_Page_SetLineWidth(page[pg_num], 1.5);
  HPDF_Page_Rectangle(page[pg_num], 40, height - 180, 525, 170);
  HPDF_Page_Stroke(page[pg_num]);
}

void plot_graph(HPDF_Page page, float *x_values, float *y_values, int num_points, float max_x, float max_y, float mov_x, float mov_y, float width, float height)
{
  max_y = (max_y == 0) ? 1 : max_y;
  HPDF_REAL graph_width = width;
  HPDF_REAL graph_height = height;
  HPDF_REAL x_margin = mov_x;
  HPDF_REAL y_margin = mov_y;
  HPDF_REAL x_scale = (graph_width - 2 * x_margin) / max_x;
  HPDF_REAL y_scale = (graph_height - 2 * x_margin) / max_y;

  // grid
  int x_axis_shift = 100, y_axis_shift = 20;
  HPDF_Page_SetLineWidth(page, 0.3);
  HPDF_Page_SetRGBStroke(page, 0.8, 0.8, 0.8);
  for (float y = y_margin; y < y_margin + graph_height - y_axis_shift; y += 10)
  {
    HPDF_Page_MoveTo(page, x_margin, y);
    HPDF_Page_LineTo(page, x_margin + graph_width - x_axis_shift, y);
  }
  HPDF_Page_Stroke(page);

  for (float x = x_margin; x < x_margin + graph_width - x_axis_shift; x += 10)
  {
    HPDF_Page_MoveTo(page, x, y_margin);
    HPDF_Page_LineTo(page, x, y_margin + graph_height - y_axis_shift);
  }
  HPDF_Page_Stroke(page);

  HPDF_Page_SetLineWidth(page, 0.5);

  HPDF_Page_SetRGBStroke(page, 0, 0, 0);
  // Draw x-axis
  HPDF_Page_MoveTo(page, x_margin, y_margin);
  HPDF_Page_LineTo(page, x_margin + graph_width - x_axis_shift, y_margin);
  HPDF_Page_Stroke(page);

  // Draw y-axis
  HPDF_Page_MoveTo(page, x_margin, y_margin);
  HPDF_Page_LineTo(page, x_margin, y_margin + graph_height - y_axis_shift);
  HPDF_Page_Stroke(page);

  if (control_porh)
  {
    HPDF_Page_MoveTo(page, x_margin + graph_width - x_axis_shift, y_margin);
    HPDF_Page_LineTo(page, x_margin + graph_width - x_axis_shift, y_margin + graph_height - y_axis_shift);
    HPDF_Page_Stroke(page);
  }

  // Calculate the arrowhead coordinates
  HPDF_REAL arrow_x = x_margin + graph_width - x_axis_shift - 5;
  HPDF_REAL arrow_y1 = y_margin + 3;
  HPDF_REAL arrow_y2 = y_margin - 3;

  // Draw the arrowhead for x
  HPDF_Page_MoveTo(page, arrow_x, arrow_y1);
  HPDF_Page_LineTo(page, x_margin + graph_width - x_axis_shift, y_margin);
  HPDF_Page_LineTo(page, arrow_x, arrow_y2);
  HPDF_Page_Stroke(page);

  arrow_x = y_margin + graph_height - y_axis_shift - 5;
  arrow_y1 = x_margin + 3;
  arrow_y2 = x_margin - 3;

  // Draw the arrowhead for y
  HPDF_Page_MoveTo(page, arrow_y1, arrow_x);
  HPDF_Page_LineTo(page, x_margin, y_margin + graph_height - y_axis_shift);
  HPDF_Page_LineTo(page, arrow_y2, arrow_x);
  HPDF_Page_Stroke(page);

  if (control_porh)
  {
    arrow_x = y_margin + graph_height - y_axis_shift - 5;
    arrow_y1 = x_margin + graph_width - x_axis_shift + 3;
    arrow_y2 = x_margin + graph_width - x_axis_shift - 3;

    HPDF_Page_MoveTo(page, arrow_y1, arrow_x);
    HPDF_Page_LineTo(page, x_margin + graph_width - x_axis_shift, y_margin + graph_height - y_axis_shift);
    HPDF_Page_LineTo(page, arrow_y2, arrow_x);
    HPDF_Page_Stroke(page);
  }

  // Add labels for x-axis scale
  HPDF_Page_BeginText(page);
  HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);

  HPDF_Page_SetFontAndSize(page, font, 8);
  for (float i = 0; i <= max_x; i += max_x / 5.0)
  {
    char label[10];
    sprintf(label, "%.1f", i);
    HPDF_Page_TextOut(page, x_margin + i * x_scale, y_margin - 20, label);
  }
  HPDF_Page_EndText(page);

  // Add labels for y-axis scale
  HPDF_Page_BeginText(page);
  HPDF_Page_SetFontAndSize(page, font, 8);
  for (float i = 0; i <= max_y + (max_y / 5.0); i += max_y / 5.0)
  {
    char label[10];
    sprintf(label, "%.0f", i);
    HPDF_Page_TextOut(page, x_margin - 15, y_margin + i * y_scale - 5, label);
  }
  HPDF_Page_EndText(page);

  HPDF_Page_BeginText(page);
  HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica-Bold", NULL), 5);
  HPDF_Page_TextOut(page, width - 100, 2 * height - 50, "ROI 1 (Ref)");
  HPDF_Page_TextOut(page, width - 100, 2 * height - 60, "ROI 2");
  HPDF_Page_TextOut(page, width - 100, 2 * height - 70, "ROI 3");
  HPDF_Page_TextOut(page, width - 100, 2 * height - 80, "ROI 4");
  if (control_porh)
  {
    HPDF_Page_TextOut(page, width - 100, 2 * height - 90, "BP Graph");
  }
  HPDF_Page_EndText(page);

  HPDF_Page_SetLineWidth(page, 0.7);

  if (control_porh)
  {
    HPDF_Page_SetRGBStroke(page, 0, 0, 1);
    HPDF_Page_MoveTo(page, width - 110, 2 * height - 87);
    HPDF_Page_LineTo(page, width - 120, 2 * height - 87);
    HPDF_Page_Stroke(page);
  }

  HPDF_Page_SetRGBStroke(page, 0, 0, 1);
  HPDF_Page_MoveTo(page, width - 110, 2 * height - 47);
  HPDF_Page_LineTo(page, width - 120, 2 * height - 47);
  HPDF_Page_Stroke(page);

  HPDF_Page_SetRGBStroke(page, 1, 0, 0.5);
  HPDF_Page_MoveTo(page, width - 110, 2 * height - 57);
  HPDF_Page_LineTo(page, width - 120, 2 * height - 57);
  HPDF_Page_Stroke(page);

  HPDF_Page_SetRGBStroke(page, 0, 1, 0);
  HPDF_Page_MoveTo(page, width - 110, 2 * height - 67);
  HPDF_Page_LineTo(page, width - 120, 2 * height - 67);
  HPDF_Page_Stroke(page);

  HPDF_Page_SetRGBStroke(page, 0.4, 0.2, 0.0);
  HPDF_Page_MoveTo(page, width - 110, 2 * height - 77);
  HPDF_Page_LineTo(page, width - 120, 2 * height - 77);
  HPDF_Page_Stroke(page);

  //   /* add labels for the axes */
  HPDF_Page_BeginText(page);
  HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica-Bold", NULL), 6);
  HPDF_Page_TextOut(page, graph_width - x_margin - 10, y_margin - 10, "Time(s)");
  HPDF_Page_TextOut(page, x_margin - 15, graph_height + y_margin - 20, " PU ");

  if (control_porh)
  {
    HPDF_Page_TextOut(page, x_margin + graph_width - x_axis_shift, y_margin + graph_height - 10, "mm/Hg");
  }
  HPDF_Page_EndText(page);

  HPDF_Page_SetLineWidth(page, 0.5);
  HPDF_Page_SetRGBStroke(page, 0, 0, 1); // Blue
  num_points = num_points - 1;
  // Move to the first point
  HPDF_Page_MoveTo(page, x_margin + x_values[0] * x_scale, y_margin + y_values[0] * y_scale);
  // Draw lines connecting the points
  for (int i = 0; i < num_points - 1; i++)
  {
    HPDF_Page_LineTo(page, x_margin + x_values[i] * x_scale, y_margin + y_values[i] * y_scale);
  }
  HPDF_Page_Stroke(page);

  HPDF_Page_SetRGBStroke(page, 1, 0, 0.5); // orange
  // Move to the first point
  HPDF_Page_MoveTo(page, x_margin + x_values[0] * x_scale, y_margin + points2_norm[0] * y_scale);
  // Draw lines connecting the points
  for (int i = 0; i < num_points - 1; i++)
  {
    HPDF_Page_LineTo(page, x_margin + x_values[i] * x_scale, y_margin + points2_norm[i] * y_scale);
  }
  HPDF_Page_Stroke(page);

  HPDF_Page_SetRGBStroke(page, 0, 1, 0); // green
  // Move to the first point
  HPDF_Page_MoveTo(page, x_margin + x_values[0] * x_scale, y_margin + points3_norm[0] * y_scale);
  // Draw lines connecting the points
  for (int i = 0; i < num_points - 1; i++)
  {
    HPDF_Page_LineTo(page, x_margin + x_values[i] * x_scale, y_margin + points3_norm[i] * y_scale);
  }
  HPDF_Page_Stroke(page);

  HPDF_Page_SetRGBStroke(page, 0.4, 0.2, 0.0); // brown
  // Move to the first point
  HPDF_Page_MoveTo(page, x_margin + x_values[0] * x_scale, y_margin + points3_norm[0] * y_scale);
  // Draw lines connecting the points
  for (int i = 0; i < num_points - 1; i++)
  {
    HPDF_Page_LineTo(page, x_margin + x_values[i] * x_scale, y_margin + points4_norm[i] * y_scale);
  }
  HPDF_Page_Stroke(page);

  // bp graph
  if (control_porh)
  {
    HPDF_Page_SetRGBStroke(page, 0, 0, 1); //
    // Move to the first point
    HPDF_Page_MoveTo(page, x_margin + x_values[0] * x_scale, y_margin + bp_points[0] * y_scale);
    // Draw lines connecting the points
    for (int i = 0; i < num_points - 1; i++)
    {
      HPDF_Page_LineTo(page, x_margin + x_values[i] * x_scale, y_margin + bp_points[i] * y_scale);
    }
    HPDF_Page_Stroke(page);
  }
}


void print_table1()
{

#ifdef table
  /*********************  table   **************/
  cal_data();
  /*
  char mean[5],med[5],max[5],min[5];
  sprintf(mean, "%d",mean_d_h);
  sprintf(med, "%d",med_d_h);
  sprintf(min, "%d",min_d_h);
  sprintf(max, "%d",max_d_h);
  */
  /* Table data */
  const char *data[5][6] =
      {
          {"RoI No", " Area ", " Mean ", "Std dev", "   %", "Ratio"},
          {"1", floatToString(roi_area[0]), intToString(mean_d_norm_h), intToString(std_dev_d_h), "Ref", floatToString(r1_elev_ratio)},
          {"2", floatToString(roi_area[1]), intToString(mean_d_norm_w), intToString(std_dev_d_w), intToString(cal_percentage(mean_d_norm_h, mean_d_norm_w)), floatToString(r2_elev_ratio)},
          {"3", floatToString(roi_area[2]), intToString(mean_d_norm_2), intToString(std_dev_d_2), intToString(cal_percentage(mean_d_norm_h, mean_d_norm_2)), floatToString(r3_elev_ratio)},
          {"4", floatToString(roi_area[3]), intToString(mean_d_norm_3), intToString(std_dev_d_3), intToString(cal_percentage(mean_d_norm_h, mean_d_norm_3)), floatToString(r4_elev_ratio)}
          //{"1", mean, mean, "Ref", med, min, max}
      };

  /* Table dimensions */
  float row_height = 20;
  float col_width = 45;

  /* Table position */
  float x = 70;
  float y = 260;

  HPDF_Page_SetLineWidth(page[pg_num], 1);
  HPDF_Page_SetRGBStroke(page[pg_num], 1, 0, 1);
  int table_cols;
  if (switch_elevation_flag)
  {
    table_cols = 6;
    x = x - 30;
  }
  else
  {
    table_cols = 5;
  }
  /* Draw table */
  for (int i = 0; i < 5; i++)
  {
    for (int j = 0; j < table_cols; j++)
    {
      if (i == 0)
      {
        HPDF_Font fontt = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
        HPDF_Page_SetFontAndSize(page[pg_num], fontt, 10);
        HPDF_Page_Rectangle(page[pg_num], x + (j * col_width) - 13, y - (i * row_height) - 2, col_width, row_height);
        HPDF_Page_Stroke(page[pg_num]);

        HPDF_Page_BeginText(page[pg_num]);
        HPDF_Page_TextOut(page[pg_num], x + (j * col_width) - 7, y - (i * row_height), data[i][j]);
        HPDF_Page_EndText(page[pg_num]);

        fontt = HPDF_GetFont(pdf, "Helvetica", NULL);
        HPDF_Page_SetFontAndSize(page[pg_num], fontt, 10);
      }
      else
      {
        HPDF_Page_Rectangle(page[pg_num], x + (j * col_width) - 13, y - (i * row_height) - 2, col_width, row_height);
        HPDF_Page_Stroke(page[pg_num]);
        HPDF_Page_SetRGBFill(page[pg_num], 0.0, 0.0, 0.0);
        if (j == 0 && i > 0)
        {
          if (i == 1)
            HPDF_Page_SetRGBFill(page[pg_num], 0.0, 0.0, 1.0);
          else if (i == 2)
            HPDF_Page_SetRGBFill(page[pg_num], 1.0, 0.0, 0.0);
          else if (i == 3)
            HPDF_Page_SetRGBFill(page[pg_num], 0.0, 1.0, 0.0);
          else
            HPDF_Page_SetRGBFill(page[pg_num], 0.4, 0.2, 0.0);
        }
        HPDF_Page_BeginText(page[pg_num]);
        HPDF_Page_TextOut(page[pg_num], x + (j * col_width) + 1, y - (i * row_height) + 2, data[i][j]);
        HPDF_Page_EndText(page[pg_num]);
      }
    }
  }

  /***********************    Table END       *******************/
#endif
}

void print_table2(float x, float y)
{

#ifdef table
  /*********************  table   **************/

  const char *data[5][8] =
      {
          {"RoI No", "Area", "RF", "BZ", "PF", "BZ:RF%", "PF:BZ%", "PF:RF%"},
          {"1", floatToString(roi_area[0]), intToString(pre_bas_lin_h), intToString(min_d_h), intToString(max_d_h), intToString(cal_percentage(pre_bas_lin_h, min_d_h)), intToString(cal_percentage(min_d_h, max_d_h)), intToString(cal_percentage(pre_bas_lin_h, max_d_h))},
          {"2", floatToString(roi_area[1]), intToString(pre_bas_lin_w), intToString(min_d_w), intToString(max_d_w), intToString(cal_percentage(pre_bas_lin_w, min_d_w)), intToString(cal_percentage(min_d_w, max_d_w)), intToString(cal_percentage(pre_bas_lin_w, max_d_w))},
          {"3", floatToString(roi_area[2]), intToString(pre_bas_lin_2), intToString(min_d_2), intToString(max_d_2), intToString(cal_percentage(pre_bas_lin_2, min_d_2)), intToString(cal_percentage(min_d_2, max_d_2)), intToString(cal_percentage(pre_bas_lin_2, max_d_2))},
          {"4", floatToString(roi_area[3]), intToString(pre_bas_lin_3), intToString(min_d_3), intToString(max_d_3), intToString(cal_percentage(pre_bas_lin_3, min_d_3)), intToString(cal_percentage(min_d_3, max_d_3)), intToString(cal_percentage(pre_bas_lin_3, max_d_3))}};

  /* Table dimensions */
  float row_height = 20;
  float col_width = 43;

  /* Table position */

  HPDF_Page_SetLineWidth(page[pg_num], 1);
  HPDF_Page_SetRGBStroke(page[pg_num], 1, 0.5, 0.5);

  /* Draw table */
  for (int i = 0; i < 5; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (i == 0)
      {
        HPDF_Font fontt = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
        HPDF_Page_SetFontAndSize(page[pg_num], fontt, 10);
      }
      else
      {
        HPDF_Font fontt = HPDF_GetFont(pdf, "Helvetica", NULL);
        HPDF_Page_SetFontAndSize(page[pg_num], fontt, 10);
      }
      HPDF_Page_Rectangle(page[pg_num], x + (j * col_width), y - (i * row_height), col_width, row_height);
      HPDF_Page_Stroke(page[pg_num]);

      HPDF_Page_BeginText(page[pg_num]);

      const char *text = data[i][j];
      HPDF_REAL text_width = HPDF_Page_TextWidth(page[pg_num], text);

      HPDF_Page_SetRGBFill(page[pg_num], 0.0, 0.0, 0.0);
      if (j == 0 && i > 0)
      {
        if (i == 1)
          HPDF_Page_SetRGBFill(page[pg_num], 0.0, 0.0, 1.0);
        else if (i == 2)
          HPDF_Page_SetRGBFill(page[pg_num], 1.0, 0.0, 0.0);
        else if (i == 3)
          HPDF_Page_SetRGBFill(page[pg_num], 0.0, 1.0, 0.0);
        else
          HPDF_Page_SetRGBFill(page[pg_num], 0.4, 0.2, 0.0);
      }

      HPDF_REAL text_x = x + (j * col_width) + (col_width - text_width) / 2;
      HPDF_REAL text_y = y - (i * row_height) + (row_height / 2);

      HPDF_Page_TextOut(page[pg_num], text_x, text_y, text);
      HPDF_Page_EndText(page[pg_num]);
    }
  }

  HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);

  /* output subtitle. */
  HPDF_Page_BeginText(page[pg_num]);

  int shortforms_loc = 348;
  HPDF_Page_SetFontAndSize(page[pg_num], font, 8);
  HPDF_Page_TextOut(page[pg_num], x + shortforms_loc, y + 15, "Area = Area of ROI in cm²");
  HPDF_Page_TextOut(page[pg_num], x + shortforms_loc, y, "RF = Rest Flow");
  HPDF_Page_TextOut(page[pg_num], x + shortforms_loc, y - 15, "BZ = Biological Zero");
  HPDF_Page_TextOut(page[pg_num], x + shortforms_loc, y - 30, "PF = Peak Flow");
  HPDF_Page_TextOut(page[pg_num], x + shortforms_loc, y - 45, "BZ : RF % = Percentage BZ to RF");
  HPDF_Page_TextOut(page[pg_num], x + shortforms_loc, y - 60, "PF : BZ% = Percentage PF to BZ");
  HPDF_Page_TextOut(page[pg_num], x + shortforms_loc, y - 75, "PF : RF% = Percentage PF to RF");

  HPDF_Page_EndText(page[pg_num]);
  /***********************    Table END       *******************/
#endif
}

void rect_roi_draw_pdf(float x_pos, float y_pos, float resize_w, float resize_h, float image_height)
{
  float rgb_colors[4][3] =
      {
          {0.0, 0.0, 1.0}, // Case 0: Blue
          {1.0, 0.0, 0.5}, // Case 1: Reddish-Purple
          {0.0, 1.0, 0.0}, // Case 2: Green
          {0.4, 0.2, 0.0}  // Case 3: Brown
      };
  for (int i = 0; i < 4; i++)
  {
    HPDF_Page_SetRGBStroke(page[pg_num], rgb_colors[i][0], rgb_colors[i][1], rgb_colors[i][2]);
    if (control_rect_roi[i])
    {
      float roi_height = abs(rois[i][3] - rois[i][2]) / resize_h;
      float roi_width = abs(rois[i][1] - rois[i][0]) / resize_w;
      HPDF_Page_Rectangle(page[pg_num], x_pos + rois[i][0] / resize_w, y_pos + image_height - roi_height - rois[i][2] / resize_h, roi_width, roi_height);
    }
    else
    {
      HPDF_Page_Circle(page[pg_num], x_pos + circle_rois[i][0] / resize_w, y_pos + image_height - circle_rois[i][1] / resize_h, circle_rois[i][2] / sqrt(resize_w * resize_h));
      // cairo_arc(cr, circle_rois[1][0] / perf_scale_x, circle_rois[1][1] / perf_scale_y, circle_rois[1][2] / sqrt(perf_scale_x * perf_scale_y), 0, 2 * M_PI);
    }
    HPDF_Page_Stroke(page[pg_num]);
  }
}
void free_roi_draw_pdf(float x_pos, float y_pos, float resize_w, float resize_h, float image_height)
{
  char *colors[] = {"Blue", "Red", "Green", "Brown"};
  for (int i = 0; i < 4; i++)
  {
    if (control_roi[i] == 1)
    {
      free_roi_color[i] = colors[i];
    }
  }
  for (int i = 0; i < free_roi_num; i++)
  {

    if (!strcmp(free_roi_color[i], "Blue"))
    {
      HPDF_Page_SetRGBStroke(page[pg_num], 0, 0, 1);
    }
    else if (!strcmp(free_roi_color[i], "Red"))
    {
      HPDF_Page_SetRGBStroke(page[pg_num], 1, 0, 0.5);
    }
    else if (!strcmp(free_roi_color[i], "Green"))
    {
      HPDF_Page_SetRGBStroke(page[pg_num], 0, 1, 0);
    }
    else if (!strcmp(free_roi_color[i], "Brown"))
    {
      HPDF_Page_SetRGBStroke(page[pg_num], 0.4, 0.2, 0.0);
    }

    HPDF_Page_MoveTo(page[pg_num], x_pos + polygon[i][0].x / resize_w, y_pos + image_height - polygon[i][0].y / resize_h);
    for (int j = 0; j < roi_vertexes[i]; j++)
    {
      HPDF_Page_LineTo(page[pg_num], x_pos + polygon[i][j].x / resize_w, y_pos + image_height - polygon[i][j].y / resize_h);

      // g_print("acc %.0f %.0f %d\n ", polygon[i][j].x / perf_scale_x, polygon[i][j].y / perf_scale_y, roi_vertexes[i]);
    }
    HPDF_Page_LineTo(page[pg_num], x_pos + polygon[i][0].x / resize_w, y_pos + image_height - polygon[i][0].y / resize_h);
    HPDF_Page_Stroke(page[pg_num]);
  }
}


void report()
{
  HPDF_Image image1, image2, cmap_horizontal, cmap_vertical, logo;
  // strcpy(fname, argv[0]);

  if (init_pdf)
  {
    pdf = HPDF_New(error_handler, NULL);
    init_pdf = 0;
    if (!pdf)
    {
      printf("error: cannot create PdfDoc object\n");
    }
    if (HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL) != HPDF_OK)
    {
      printf("Failed to set compression mode.\n");
      HPDF_Free(pdf);
    }
  }

  if (setjmp(env))
  {
    HPDF_Free(pdf);
  }

  /* Add a new page object. */
  page[pg_num] = HPDF_AddPage(pdf);
  height = HPDF_Page_GetHeight(page[pg_num]);
  width = HPDF_Page_GetWidth(page[pg_num]);

  // HPDF_AddPageLabel(pdf, pg_num, HPDF_PAGE_NUM_STYLE_DECIMAL, 0, "");

  char color[300];
  sprintf(color, "%sprev/lsci.png", current_working_dir);

  image1 = HPDF_LoadPngImageFromFile(pdf, color);

// image1 = HPDF_LoadPngImageFromFile(pdf, "contrast.png");
#ifdef bluetooth_raspi
  char image_name[300];
  sprintf(image_name, "%sprev/normal_pic.jpg", current_working_dir);

  image2 = HPDF_LoadJpegImageFromFile(pdf, image_name);
#endif
  // #define solve_pdf

  cmap_horizontal = HPDF_LoadPngImageFromFile(pdf, "ui_data/colorbar_horizontal.png");
  cmap_vertical = HPDF_LoadPngImageFromFile(pdf, "ui_data/colorbar_vertical.png");
  logo = HPDF_LoadPngImageFromFile(pdf, "ui_data/angiocam.png");

  HPDF_Page_DrawImage(page[pg_num], logo, 150, 780, HPDF_Image_GetWidth(logo) / 1.5, HPDF_Image_GetHeight(logo) / 1.5); // 784

  int resize_w, resize_h;
  int x_pos_cmap, y_pos_cmap; // color map
  int x_pos_nimg, y_pos_nimg; // normal image
                              // int resize_w = 10, resize_h = 10;
  if (!control_porh)
  {
    resize_w = 8;
    resize_h = 8;
    x_pos_cmap = 325;
    y_pos_cmap = 325 + 120; // color map
    x_pos_nimg = 325;
    y_pos_nimg = 80 + 120; // normal image
    plot_graph(page[pg_num], time_axis, points_norm, image_no, elapsed_time, max_y_val_graph, 50, 320, 350, 330);
    if (switch_elevation_flag)
    {
      Page_headings("Elevation Study", pat_name, pat_age, pat_gender, pat_id, pat_clin);
      control_reports[report_num] = 11;
    }
    else
    {
      Page_headings("Perfusion Study", pat_name, pat_age, pat_gender, pat_id, pat_clin);
      if (!elev)
      {
        control_reports[report_num] = 0;
      }
      else
      {
        control_reports[report_num] = 10;
      }
    }
    print_table1();
  }
  else
  {
    resize_w = 4;
    resize_h = 4;
    x_pos_cmap = 50;
    y_pos_cmap = 340; // color map
    x_pos_nimg = 70;
    y_pos_nimg = 20; // normal image
    Page_headings("PORH Study", pat_name, pat_age, pat_gender, pat_id, pat_clin);
    control_reports[report_num] = 2;
  }

#ifdef bluetooth_raspi
  HPDF_Page_DrawImage(page[pg_num], image2, x_pos_nimg, y_pos_nimg, HPDF_Image_GetWidth(image1) / resize_w, HPDF_Image_GetHeight(image1) / resize_h); // same size as image1
#endif
  HPDF_Page_DrawImage(page[pg_num], image1, x_pos_cmap, y_pos_cmap, HPDF_Image_GetWidth(image1) / resize_w, HPDF_Image_GetHeight(image1) / resize_h);

  if (control_porh)
  {
    int gap = 10;
    HPDF_Page_DrawImage(page[pg_num], cmap_vertical, gap + x_pos_cmap + HPDF_Image_GetWidth(image1) / resize_w, y_pos_cmap, 5, HPDF_Image_GetHeight(image1) / resize_h);

    /*********************  Scale for the color map   ************/
    HPDF_Page_BeginText(page[pg_num]);
    HPDF_Page_SetFontAndSize(page[pg_num], HPDF_GetFont(pdf, "Helvetica", NULL), 5);
    int inc_val = (cmap_max_val - cmap_min_val) / 10;
    float spacing = 0;
    for (float i = cmap_min_val; i <= cmap_max_val; i = i + inc_val)
    {
      char scl[5];
      sprintf(scl, "%d", (int)i);
      HPDF_Page_TextOut(page[pg_num], 2 * gap + x_pos_cmap + HPDF_Image_GetWidth(image1) / resize_w, y_pos_cmap + spacing * HPDF_Image_GetHeight(image1) / resize_h - 5, scl);
      spacing = spacing + 0.1;
    }
    HPDF_Page_EndText(page[pg_num]);
  }
  else
  {
    HPDF_Page_DrawImage(page[pg_num], cmap_horizontal, x_pos_cmap, 425, HPDF_Image_GetWidth(image1) / resize_w, 5);
    /*********************  Scale for the color map   ************/
    HPDF_Page_BeginText(page[pg_num]);
    HPDF_Page_SetFontAndSize(page[pg_num], HPDF_GetFont(pdf, "Helvetica", NULL), 5);
    int inc_val = (cmap_max_val - cmap_min_val) / 10;
    float spacing = 0;
    for (float i = cmap_min_val; i <= cmap_max_val; i = i + inc_val)
    {
      char scl[5];
      sprintf(scl, "%d", (int)i); // g_print("%d\n",(int)i);
      HPDF_Page_TextOut(page[pg_num], x_pos_cmap + spacing * HPDF_Image_GetWidth(image1) / resize_w - 5, y_pos_cmap - 30, scl);
      spacing = spacing + 0.1;
    }
    HPDF_Page_EndText(page[pg_num]);
  }

  rect_roi_draw_pdf(x_pos_cmap, y_pos_cmap, resize_w, resize_h, HPDF_Image_GetHeight(image1) / resize_h);
  free_roi_draw_pdf(x_pos_cmap, y_pos_cmap, resize_w, resize_h, HPDF_Image_GetHeight(image1) / resize_h);

#ifdef normal_img_roi
  rect_roi_draw_pdf(x_pos_nimg, y_pos_nimg, resize_w, resize_h, roi_scale);
  free_roi_draw_pdf(x_pos_nimg, y_pos_nimg, resize_w, resize_h, roi_scale);
#endif

  sprintf(rep_name, "%s%s_report.pdf", dirbase, pat_name);
  pg_num++;

  if (control_porh)
  {
    page[pg_num] = HPDF_AddPage(pdf);
    HPDF_Image logo = HPDF_LoadPngImageFromFile(pdf, "ui_data/angiocam.png");
    HPDF_Page_DrawImage(page[pg_num], logo, 150, 780, HPDF_Image_GetWidth(logo) / 1.5, HPDF_Image_GetHeight(logo) / 1.5);

    height = HPDF_Page_GetHeight(page[pg_num]);
    width = HPDF_Page_GetWidth(page[pg_num]);
    plot_graph(page[pg_num], time_axis, points, image_no, elapsed_time, max_y_val_graph, 80, 380, 550, 370);
    print_table2(80, 270);

    // g_print("h and wi %f %f", height, width);

    pg_num++;
  }
}

void remove_cliniciation(HPDF_Page page, double x, double y, double width, double height)
{
  HPDF_Page_SetRGBFill(page, 1.0, 1.0, 1.0); // Set fill color to white
  HPDF_Page_Rectangle(page, x, y, width, height);
  HPDF_Page_Fill(page);
}

void final_report(int last)
{
  // check the report exists or not
  struct stat buffer;
  int rep = 0;
  while (stat(rep_name, &buffer) == 0)
  {
    rep++;
    printf("File exists at %s\n", rep_name);
    sprintf(rep_name, "%s%s_report%d.pdf", dirbase, pat_name, rep);
  }

/**************   Remove the extra cliniciation notes   ***************/
#ifdef report_bug
  if (control_porh)
  {
    for (int i = 0; i < pg_num; i = i + 2)
    {
      remove_cliniciation(page[i + 1], 50 - 2, 25, 500 + 4, 150);
    }
  }
  else
  {
    for (int i = 0; i < report_num; i++)
    {
      remove_cliniciation(page[i], 50 - 2, 25, 500 + 4, 150);
    }
  }
#endif

  /**************     Adding cliniciation Notes to report *****************/
  HPDF_Page_SetRGBFill(page[last], 0, 0, 0);
  HPDF_Font font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
  HPDF_Page_SetFontAndSize(page[last], font, 12);
  HPDF_Page_BeginText(page[last]);
  HPDF_Page_TextOut(page[last], 50, 160, "BRIEF EXAMINATION NOTES:");
  HPDF_Page_EndText(page[last]);
  // def_font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
  // HPDF_Page_SetFontAndSize(page[last], def_font, 12);

  const char *text = pat_clin;
  // sprintf(text, "%s", pat_clin);
  HPDF_Page_SetFontAndSize(page[last], font, 10);
  HPDF_REAL left = 60;
  HPDF_REAL top = 140;   // HPDF_Page_GetHeight(page) - 100;
  HPDF_REAL right = 550; // HPDF_Page_GetWidth(page) - 50;
  HPDF_REAL bottom = 30; // HPDF_Page_GetHeight(page) - 150;

  HPDF_REAL textWidth = HPDF_Page_TextWidth(page[last], text);
  HPDF_UINT textLength = (HPDF_UINT)strlen(text);

  HPDF_Page_BeginText(page[last]);
  HPDF_Page_TextRect(page[last], left, top, right, bottom, text, HPDF_TALIGN_LEFT, &textLength);
  HPDF_Page_EndText(page[last]);

  HPDF_Page_SetRGBStroke(page[last], 0, 0, 0);
  HPDF_Page_SetLineWidth(page[last], 1);
  HPDF_Page_Rectangle(page[last], 50, 40, 500, 110);
  HPDF_Page_Stroke(page[last]);

  HPDF_Page_BeginText(page[last]);
  HPDF_Page_TextOut(page[last], 500, 3, "Signature");
  HPDF_Page_EndText(page[last]);

  HPDF_SaveToFile(pdf, rep_name);

  /* clean up */
  // HPDF_Free(pdf);
  printf("Report Generated Successfully...\n");

  /************   store the perfusion data in file     **************/
  // char perf[400];
  // sprintf(perf, "%sperf_values.txt", dirbase);

  // FILE *fp_time;
  // fp_time = fopen(perf, "w");
  // for (int i = 0; i < image_no; i++)
  // {
  //   fprintf(fp_time, "%f ", points[i]);
  // }
  // fclose(fp_time);
}

/**************************************************************/
