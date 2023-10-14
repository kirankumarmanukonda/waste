#include "calculations.h"

void reset_table_values()
{
  mean_d_h = 0, mean_d_w = 0, mean_d_2 = 0, mean_d_3 = 0, mean_calib_SG = 0, mean_calib_beta = 0;
  sum_d_h = 0, sum_d_w = 0, sum_d_2 = 0, sum_d_3 = 0, sum_calib_SG = 0, sum_calib_beta = 0;

  sum_d_norm_h = 0, sum_d_norm_w = 0, sum_d_norm_2 = 0, sum_d_norm_3 = 0;

  perc_d_h = 0, perc_d_w = 0, perc_d_2 = 0, perc_d_3 = 0;
  std_dev_d_h = 0, std_dev_d_w = 0, std_dev_d_2 = 0, std_dev_d_3 = 0;
  pre_bas_lin_h = 0, pre_bas_lin_w, pre_bas_lin_2, pre_bas_lin_3;
  post_bas_lin_h = 100, post_bas_lin_w = 1000, post_bas_lin_2 = 1000, post_bas_lin_3 = 1000;

  tm_h = 0, tm_w = 0, tm_2 = 0, tm_3 = 0;
  th_h = 0, th_w = 0, th_2 = 0, th_3 = 0;
  tb_h = 0, tb_w = 0, tb_2 = 0, tb_3 = 0;
  med_d_h = 0, med_d_w = 0, med_d_2 = 0, med_d_3 = 0;
  min_d_h = 1000, min_d_w = 1000, min_d_2 = 1000, min_d_3 = 1000;
  max_d_h = 0, max_d_w = 0, max_d_2 = 0, max_d_3 = 0;
}

float actual_distance_x = 19.5, actual_distance_y = 12; // in cm
void finding_roi_area()
{
  float scale_image_x = actual_distance_x / wi;
  float scale_image_y = actual_distance_y / h;

  for (int i = 0; i < 4; i++)
  {
    if (control_roi[i])
    {
      // Polygon Area
      // polygon[free_roi_num][vertex].x = press_x * perf_scale_x;
      // for (int j = 0; j < roi_vertexes[free_roi_num]; j++)
      // {

      // }
    }
    else
    {
      if (control_rect_roi[i])
      {
        // rectangle Area = L X B
        roi_area[i] = (abs(rois[i][0] - rois[i][1]) * scale_image_x) * (abs(rois[i][2] - rois[i][3]) * scale_image_y);
      }
      else
      {
        // circle Area = pi X r^2
        roi_area[i] = M_PI * (circle_rois[i][2] * scale_image_x) * (circle_rois[i][2] * scale_image_y);
        //g_print("Area circle %f", roi_area[i]);
      }
    }
  }
}
void cal_data_porh()
{
  reset_table_values();
  finding_roi_area();
  float rising_start = 0, dipping_stop = 0;
  float start_peak_h = 0, end_peak_h = 0, start_peak_w = 0, end_peak_w = 0, start_peak_2 = 0, end_peak_2 = 0, start_peak_3 = 0, end_peak_3 = 0;
  float time_x_h = 0, time_x_w = 0, time_x_2 = 0, time_x_3 = 0;
  float time_b_h = 0, time_b_w = 0, time_b_2 = 0, time_b_3 = 0;
  int pre_occl = 1, baselin_cnt = 0, baselin_cnt_post = 0;
  int sum_d_h_bas_line = 0, sum_d_w_bas_line = 0, sum_d_2_bas_line = 0, sum_d_3_bas_line = 0;
  int sum_d_h_bas_line_post = 0, sum_d_w_bas_line_post = 0, sum_d_2_bas_line_post = 0, sum_d_3_bas_line_post = 0;

  // min_d_h = 1000; min_d_w = 1000; min_d_h = 1000; min_d_w = 1000; min_d_2 = 1000; min_d_3 = 1000;
  // max_d_h = 0; max_d_w = 0; max_d_h = 0; min_d_w = 0; max_d_2 = 0; max_d_3 = 0;

  float sum_bzh = 0, sum_bzw = 0, sum_bz2 = 0, sum_bz3 = 0;
  int sum_bz_inc = 0;
  float upto_val = 180;
  int exe_once = 1;
  int flag = 1;
  int max_index_h = -1, max_index_w = -1, max_index_2 = -1, max_index_3 = -1;
  for (int i = 0; i < count; i++)
  {

    int temp_max = findMax(points_norm[i], points2_norm[i], points3_norm[i], points4_norm[i], bp_points[i]);
    if (temp_max > max_y_val_graph)
    {
      max_y_val_graph = temp_max;
    }

    // finding the pre occlusion base line
    if (bp_points[i] <= 20 && time_axis[i] <= 10)
    {
      sum_d_h_bas_line = sum_d_h_bas_line + points_norm[i];
      sum_d_w_bas_line = sum_d_w_bas_line + points2_norm[i];
      sum_d_2_bas_line = sum_d_2_bas_line + points3_norm[i];
      sum_d_3_bas_line = sum_d_3_bas_line + points4_norm[i];

      baselin_cnt++;
    }

    // finding the post occlusion base line
    if (bp_points[i] <= 60 && time_axis[i] >= 50)
    {
      /*
      if(points[i] < post_bas_lin_h)
      {
        post_bas_lin_h = points[i];
      }

      if(points2[i] < post_bas_lin_w)
      {
        post_bas_lin_w = points2[i];
      }

      if(points3[i] < post_bas_lin_2)
      {
        post_bas_lin_2 = points3[i];
      }

      if(points4[i] < post_bas_lin_3)
      {
        post_bas_lin_3 = points4[i];
      }
*/
      sum_d_h_bas_line_post = sum_d_h_bas_line_post + points_norm[i];
      sum_d_w_bas_line_post = sum_d_w_bas_line_post + points2_norm[i];
      sum_d_2_bas_line_post = sum_d_2_bas_line_post + points3_norm[i];
      sum_d_3_bas_line_post = sum_d_3_bas_line_post + points4_norm[i];

      baselin_cnt_post++;
      // g_print("pob%d ",baselin_cnt_post);
    }

    if (bp_points[i] >= 180 && time_axis[i] < upto_val && time_axis[i] > 15)
    {
      if (exe_once)
      {
        exe_once = 0;
        upto_val = time_axis[i] + 10;
      }
      sum_bzh = sum_bzh + points_norm[i];
      sum_bzw = sum_bzw + points2_norm[i];
      sum_bz2 = sum_bz2 + points3_norm[i];
      sum_bz3 = sum_bz3 + points4_norm[i];
      sum_bz_inc++;
    }

    if (time_axis[i] > upto_val + 10 && time_axis[i] < upto_val + 30)
    {
      if (points_norm[i] > max_d_h && bp_points[i] <= 60)
      {
        // max_d_h = points[i];
        if (check_proper(points_norm, i))
        {
          max_d_h = points_norm[i];
          end_peak_h = time_axis[i];
          max_index_h = i;
        }
      }

      if (points2_norm[i] > max_d_w && bp_points[i] <= 60)
      {
        if (check_proper(points2_norm, i))
        {
          max_d_w = points2_norm[i];
          end_peak_w = time_axis[i];
          max_index_w = i;
        }
      }

      if (points3_norm[i] > max_d_2 && bp_points[i] <= 60)
      {

        if (check_proper(points3_norm, i))
        {
          max_d_2 = points3_norm[i];
          end_peak_2 = time_axis[i];
          max_index_2 = i;
        }
      }

      if (points4_norm[i] > max_d_3 && bp_points[i] <= 60)
      {

        if (check_proper(points4_norm, i))
        {
          max_d_3 = points4_norm[i];
          end_peak_3 = time_axis[i];
          max_index_3 = i;
        }
      }
    }
    if (flag && bp_points[i] > 120)
    {
      // flag =0;
      if (bp_points[i + 1] < bp_points[i] && bp_points[i + 3] < bp_points[i])
      {
        rising_start = time_axis[i];
        // g_print("time start pushing %f\n",rising_start);
      }
    }
  }

  if (baselin_cnt != 0)
  {
    pre_bas_lin_h = (float)sum_d_h_bas_line / baselin_cnt;
    pre_bas_lin_w = (float)sum_d_w_bas_line / baselin_cnt;
    pre_bas_lin_2 = (float)sum_d_2_bas_line / baselin_cnt;
    pre_bas_lin_3 = (float)sum_d_3_bas_line / baselin_cnt;
  }

  if (baselin_cnt_post != 0)
  {
    post_bas_lin_h = (float)sum_d_h_bas_line_post / baselin_cnt_post;
    post_bas_lin_w = (float)sum_d_w_bas_line_post / baselin_cnt_post;
    post_bas_lin_2 = (float)sum_d_2_bas_line_post / baselin_cnt_post;
    post_bas_lin_3 = (float)sum_d_3_bas_line_post / baselin_cnt_post;
  }

  if (sum_bz_inc != 0)
  {
    min_d_h = (float)sum_bzh / sum_bz_inc;
    min_d_w = (float)sum_bzw / sum_bz_inc;
    min_d_2 = (float)sum_bz2 / sum_bz_inc;
    min_d_3 = (float)sum_bz3 / sum_bz_inc;
  }

  float x_h = (pre_bas_lin_h + max_d_h) / 2;
  float x_w = (pre_bas_lin_w + max_d_w) / 2;
  float x_2 = (pre_bas_lin_2 + max_d_2) / 2;
  float x_3 = (pre_bas_lin_3 + max_d_3) / 2;

  /*
    float minDiff_h = INT_MAX, minDiff_w = INT_MAX, minDiff_2 = INT_MAX, minDiff_3 = INT_MAX;
    float threshold_h, threshold_w, threshold_2, threshold_3;
    for (int i = 0; i < count; i++)
    {
      float diff_h = abs(points[i] - x_h); // Calculate the absolute difference
      if (diff_h < minDiff_h && time_axis[i] > end_peak_h && time_axis[i] < end_peak_h + 10)
      {
        minDiff_h = diff_h;
        threshold_h = points[i];
      }

      float diff_w = abs(points2[i] - x_w); // Calculate the absolute difference
      if (diff_w < minDiff_w && time_axis[i] > end_peak_w && time_axis[i] < end_peak_w + 10)
      {
        minDiff_w = diff_w;
        threshold_w = points2[i];
      }

      float diff_2 = abs(points3[i] - x_2); // Calculate the absolute difference
      if (diff_2 < minDiff_2 && time_axis[i] > end_peak_2 && time_axis[i] < end_peak_2 + 10)
      {
        minDiff_2 = diff_2;
        threshold_2 = points3[i];
      }

      float diff_3 = abs(points4[i] - x_3); // Calculate the absolute difference
      if (diff_3 < minDiff_3 && time_axis[i] > end_peak_3 && time_axis[i] < end_peak_3 + 10)
      {
        minDiff_3 = diff_3;
        threshold_3 = points4[i];
      }
    }

    int t1 = 1, t2 = 1, t3 = 1, t4 = 1, t1b = 1, t2b = 1, t3b = 1, t4b = 1;
    int threshold = 3;

    for (int i = 0; i < count; i++)
    {
      // finding the half (TH)

      if (points[i] == threshold_h && t1 && bp_points[i] <= 60 && time_axis[i] > end_peak_h)
      {
        time_x_h = time_axis[i];
        t1 = 0;
      }
      if (points2[i] == threshold_w && t2 && bp_points[i] <= 60 && time_axis[i] > end_peak_w)
      {
        time_x_w = time_axis[i];
        t2 = 0;
      }
      if (points3[i] == threshold_2 && t3 && bp_points[i] <= 60 && time_axis[i] > end_peak_2)
      {
        time_x_2 = time_axis[i];
        t3 = 0;
      }
      if (points4[i] == threshold_3 && t4 && bp_points[i] <= 60 && time_axis[i] > end_peak_3)
      {
        time_x_3 = time_axis[i];
        t4 = 0;
      }

      // finding the tb (TB)
      if (points[i] < pre_bas_lin_h + threshold && points[i] > pre_bas_lin_h - threshold && t1b && bp_points[i] <= 60 && time_axis[i] > end_peak_h)
      {
        time_b_h = time_axis[i];
        t1b = 0;
      }
      if (points2[i] < pre_bas_lin_w + threshold && points2[i] > pre_bas_lin_w - threshold && t2b && bp_points[i] <= 60 && time_axis[i] > end_peak_w)
      {
        time_b_w = time_axis[i];
        t2b = 0;
      }
      if (points3[i] < pre_bas_lin_2 + threshold && points3[i] > pre_bas_lin_2 - threshold && t3b && bp_points[i] <= 60 && time_axis[i] > end_peak_2)
      {
        time_b_2 = time_axis[i];
        t3b = 0;
      }
      if (points4[i] < pre_bas_lin_3 + threshold && points4[i] > pre_bas_lin_3 - threshold && t4b && bp_points[i] <= 60 && time_axis[i] > end_peak_3)
      {
        time_b_3 = time_axis[i];
        t4b = 0;
      }
    }
    tm_h = end_peak_h - rising_start;
    tm_w = end_peak_w - rising_start;
    tm_2 = end_peak_2 - rising_start;
    tm_3 = end_peak_3 - rising_start;

    th_h = time_x_h - rising_start;
    th_w = time_x_w - rising_start;
    th_2 = time_x_2 - rising_start;
    th_3 = time_x_3 - rising_start;

    tb_h = time_b_h - rising_start;
    tb_w = time_b_w - rising_start;
    tb_2 = time_b_2 - rising_start;
    tb_3 = time_b_3 - rising_start;
  */
  // g_print("tm %f endpk %f\n", tm_h, end_peak_h);
}

int r1_mean_val = 0, r2_mean_val = 0, r3_mean_val = 0, r4_mean_val = 0;

void cal_data_perf()
{
  reset_table_values();
  // min_d_h = points[0];

  finding_roi_area();

  for (int i = 0; i < count; i++)
  {
    sum_d_h = sum_d_h + points[i];
    sum_d_w = sum_d_w + points2[i];
    sum_d_2 = sum_d_2 + points3[i];
    sum_d_3 = sum_d_3 + points4[i];

    if (calib_flag)
    {
      sum_calib_SG = sum_calib_SG + points_calib_SG[i];
      sum_calib_beta = sum_calib_beta + points_calib_beta[i];
    }

    sum_d_norm_h = sum_d_norm_h + points_norm[i];
    sum_d_norm_w = sum_d_norm_w + points2_norm[i];
    sum_d_norm_2 = sum_d_norm_2 + points3_norm[i];
    sum_d_norm_3 = sum_d_norm_3 + points4_norm[i];

    int temp_max = findMax(points_norm[i], points2_norm[i], points3_norm[i], points4_norm[i], bp_points[i]);

    if (temp_max > max_y_val_graph)
    {
      max_y_val_graph = temp_max;
    }

    /*
    if (points_norm[i] < min_d_h)
    {
      min_d_h = points_norm[i];
    }
    if (points_norm[i] > max_d_h)
    {
      max_d_h = points_norm[i];
    }

    if (points2_norm[i] < min_d_w)
    {
      min_d_w = points2_norm[i];
    }
    if (points2_norm[i] > max_d_w)
    {
      max_d_w = points2_norm[i];
    }

    if (points3_norm[i] < min_d_2)
    {
      min_d_2 = points3_norm[i];
    }
    if (points3_norm[i] > max_d_2)
    {
      max_d_2 = points3_norm[i];
    }

    if (points4_norm[i] < min_d_3)
    {
      min_d_3 = points4_norm[i];
    }
    if (points4_norm[i] > max_d_3)
    {
      max_d_3 = points4_norm[i];
    }
    */
  }

  if (count != 0)
  {
    mean_d_h = (float)sum_d_h / count;
    mean_d_w = (float)sum_d_w / count;
    mean_d_2 = (float)sum_d_2 / count;
    mean_d_3 = (float)sum_d_3 / count;

    if (calib_flag)
    {
      mean_calib_SG = (float)sum_calib_SG / count;
      mean_calib_beta = (float)sum_calib_beta / count;
    }

    sum_d_h = 0;
    sum_d_w = 0;
    sum_d_2 = 0;
    sum_d_3 = 0;

    mean_d_norm_h = (float)sum_d_norm_h / count;
    mean_d_norm_w = (float)sum_d_norm_w / count;
    mean_d_norm_2 = (float)sum_d_norm_2 / count;
    mean_d_norm_3 = (float)sum_d_norm_3 / count;

    // g_print("Signal gain %f ",(float)sum_d_norm_2 / count);
    sum_d_h = 0;
    sum_d_w = 0;
    sum_d_2 = 0;
    sum_d_3 = 0;

    for (int i = 0; i < count; i++)
    {
      sum_d_h = sum_d_h + (points_norm[i] - mean_d_norm_h) * (points_norm[i] - mean_d_norm_h);
      sum_d_w = sum_d_w + (points2_norm[i] - mean_d_norm_w) * (points2_norm[i] - mean_d_norm_w);
      sum_d_2 = sum_d_2 + (points3_norm[i] - mean_d_norm_2) * (points3_norm[i] - mean_d_norm_2);
      sum_d_3 = sum_d_3 + (points4_norm[i] - mean_d_norm_3) * (points4_norm[i] - mean_d_norm_3);
    }
    std_dev_d_h = sqrt((float)sum_d_norm_h / count);
    std_dev_d_w = sqrt((float)sum_d_norm_w / count);
    std_dev_d_2 = sqrt((float)sum_d_norm_2 / count);
    std_dev_d_3 = sqrt((float)sum_d_norm_3 / count);

    sum_d_h = 0;
    sum_d_w = 0;
    sum_d_2 = 0;
    sum_d_3 = 0;

    if (mean_d_norm_h != 0)
      r1_elev_ratio = (float)r1_mean_val / mean_d_norm_h;
    if (mean_d_norm_w != 0)
      r2_elev_ratio = (float)r2_mean_val / mean_d_norm_w;
    if (mean_d_norm_2 != 0)
      r3_elev_ratio = (float)r3_mean_val / mean_d_norm_2;
    if (mean_d_norm_3 != 0)
      r4_elev_ratio = (float)r4_mean_val / mean_d_norm_3;
  }

  perc_d_h = 0;
  perc_d_w = cal_percentage(mean_d_norm_h, mean_d_norm_w);
  perc_d_2 = cal_percentage(mean_d_norm_h, mean_d_norm_2);
  perc_d_3 = cal_percentage(mean_d_norm_h, mean_d_norm_3);

  // g_print("%d \n",cal_percentage(mean_calib_beta, mean_calib_SG));
}

