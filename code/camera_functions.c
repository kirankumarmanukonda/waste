
#include "camera_functions.h"


/****************       CAMERA Initialization     *********************/

ArvCamera *gcamera;
ArvStream *stream;

/******      initalize camera     **************/
void free_data()
{
  /**************           Freed the pointers that are globally declared   ***************/

  free(buffer_rgb);
  free(Z_val);
}
void create_stream()
{
  GError *error = NULL;
  stream = arv_camera_create_stream(gcamera, NULL, NULL, &error);

  if (ARV_IS_STREAM(stream))
  {
    size_t payload;
    int i;
    /* Retrieve the payload size for buffer creation */
    payload = arv_camera_get_payload(gcamera, &error);
    if (error == NULL)
    {
      /* Insert some buffers in the stream buffer pool */
      for (i = 0; i < 5; i++)
        arv_stream_push_buffer(stream, arv_buffer_new(payload, NULL));
    }
  }
}

int assign_ip_camera()
{
  // Define the network interface names for both cases
  const char *interface1 = "eno1";
  const char *interface2 = "enp2s0";

  // Use a command buffer to store the command
  char command[100];

  // Check if "eno1" exists as a network interface
  if (system("ifconfig eno1 2>/dev/null") == 0)
  {
    // Interface "eno1" exists, so use it
    snprintf(command, sizeof(command), "sudo ifconfig %s 169.254.118.246 netmask 255.255.255.0 up", interface1);
  }
  else if (system("ifconfig enp2s0 2>/dev/null") == 0)
  {

    // Interface "enp2s0" exists, so use it
    snprintf(command, sizeof(command), "sudo ifconfig %s 169.254.118.246 netmask 255.255.255.0 up", interface2);
  }
  else
  {
    // None of the interfaces exist
    printf("Neither %s nor %s interface found.\n", interface1, interface2);
    return 1;
  }

  // Execute the chosen command
  int result = system(command);

  return result;
}

float beta, SG;
int fps, window_size;
float exposure_tim = 10;
char exp_tim_string[10];

int camerainit()
{
  system("sudo systemctl restart NetworkManager");
  assign_ip_camera();
  // system("sudo ifconfig eno1 169.254.118.246 netmask 255.255.255.0 up");
  GError *error = NULL;
  /* Connect to the first available camera */
  gcamera = arv_camera_new(NULL, &error);
  ArvAcquisitionMode cam_mode = ARV_ACQUISITION_MODE_CONTINUOUS;
  ArvDevice *device = arv_camera_get_device(gcamera);
  arv_device_set_string_feature_value(device, "LineSelector", "Line2", NULL);
  arv_device_set_string_feature_value(device, "LineMode", "Output", NULL);
  // arv_device_set_string_feature_value(device, "LineSource","ExposureActive",NULL);
  if (ARV_IS_CAMERA(gcamera))
  {
    printf("Found camera '%s'\n", arv_camera_get_model_name(gcamera, NULL));
    // printf ("hearbeat = %d\n", arv_camera_get_integer (gcamera, "GevHeartbeatTimeout", NULL));
    // digitalWrite(laser,HIGH);
    // arv_camera_set_integer (gcamera, "GevHeartbeatTimeout", 10000, NULL);
    arv_camera_set_frame_rate(gcamera, fps, NULL);
    arv_camera_set_gain(gcamera, 1.0, &error); // gain of the camera
    arv_camera_set_exposure_time(gcamera, exposureTime, &error);
    arv_camera_set_acquisition_mode(gcamera, cam_mode, NULL);
    arv_camera_set_pixel_format(gcamera, ARV_PIXEL_FORMAT_MONO_10, &error);

    printf("formatt of the image %x\n", arv_camera_get_pixel_format(gcamera, &error));
    create_stream();
  }
  else
  {
    show_dialog("Camera Not Found......!");
    g_print("Camera Not Found......!");
    return 0;
  }
  return 1;
}

// Function to save the configuration to the .config file
void saveConfig(const char *filename, int exposureTime, int fps, int window, float rois[4][4], float beta, float SG)
{
  FILE *configFile = fopen(filename, "w");
  if (configFile == NULL)
  {
    perror("Error opening file");
    return;
  }

  fprintf(configFile, "[CameraSettings]\n");
  fprintf(configFile, "ExposureTime=%d\n", exposureTime);
  fprintf(configFile, "FPS=%d\n", fps);
  fprintf(configFile, "Window=%d\n", window);
  fprintf(configFile, "ROI1_XPress=%d\n", rois[0][0]);
  fprintf(configFile, "ROI1_XRel=%d\n", rois[0][1]);
  fprintf(configFile, "ROI1_YPres=%d\n", rois[0][2]);
  fprintf(configFile, "ROI1_YRel=%d\n", rois[0][3]);
  fprintf(configFile, "ROI2_XPress=%d\n", rois[1][0]);
  fprintf(configFile, "ROI2_XRel=%d\n", rois[1][1]);
  fprintf(configFile, "ROI2_YPres=%d\n", rois[1][2]);
  fprintf(configFile, "ROI2_YRel=%d\n", rois[1][3]);
  fprintf(configFile, "Beta=%f\n", beta);
  fprintf(configFile, "SG=%f\n", SG);

  fclose(configFile);
}

// Function to read the configuration from the .config file
void loadConfig(const char *filename, int *exposureTime, int *fps, int *window, float rois[4][4], float *beta, float *SG)
{
  FILE *configFile = fopen(filename, "r");
  calib_device = 1;
  // Check if the file was opened successfully
  if (configFile == NULL)
  {
    printf("Error opening the file.");
    show_dialog("Calibrate the Device.....!");
    calib_device = 0;
  }

  char line[100];
  while (fgets(line, sizeof(line), configFile))
  {
    if (line[0] == '[')
    {
      // Ignore section headers
      continue;
    }

    char key[50];
    int intValue;
    float floatValue;
    if (sscanf(line, "%49[^=]=%d", key, &intValue) == 2)
    {
      if (strcmp(key, "ExposureTime") == 0)
      {
        *exposureTime = intValue;
      }
      else if (strcmp(key, "FPS") == 0)
      {
        *fps = intValue;
      }
      else if (strcmp(key, "Window") == 0)
      {
        *window = intValue;
      }
      else if (strcmp(key, "ROI1_XPress") == 0)
      {
        rois[0][0] = intValue;
      }
      else if (strcmp(key, "ROI1_XRel") == 0)
      {
        rois[0][1] = intValue;
      }
      else if (strcmp(key, "ROI1_YPres") == 0)
      {
        rois[0][2] = intValue;
      }
      else if (strcmp(key, "ROI1_YRel") == 0)
      {
        rois[0][3] = intValue;
      }
      else if (strcmp(key, "ROI2_XPress") == 0)
      {
        rois[1][0] = intValue;
      }
      else if (strcmp(key, "ROI2_XRel") == 0)
      {
        rois[1][1] = intValue;
      }
      else if (strcmp(key, "ROI2_YPres") == 0)
      {
        rois[1][2] = intValue;
      }
      else if (strcmp(key, "ROI2_YRel") == 0)
      {
        rois[1][3] = intValue;
      }
    }
    if (sscanf(line, "%49[^=]=%f", key, &floatValue) == 2)
    {
      if (strcmp(key, "Beta") == 0)
      {
        *beta = floatValue;
      }
      else if (strcmp(key, "SG") == 0)
      {
        *SG = floatValue;
      }
    }
  }

  fclose(configFile);
}


/********	for the time axis	**********/
int timer_flag = 0;
struct timeval start_time, end_time;
struct timeval test_start_time, test_end_time;
double map(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/******		capture images *****/
ArvBuffer *buffer;
char *buffer_data;

GError *error = NULL;


int create_dir = 0, max_time = 0;
int missing = 0;
int camera(GtkWidget *widget)
{
  if (elapsed_time <= max_time)
  {

    if (!ARV_IS_CAMERA(gcamera))
    {
      // camerainit();
      g_print("cam failed\n");
    }
    else
    {
      /* Don't destroy the buffer, but put it back into the buffer pool */
      buffer = arv_stream_pop_buffer(stream);

      arv_stream_push_buffer(stream, buffer);

      if (ARV_IS_BUFFER(buffer) && arv_buffer_get_status(buffer) == ARV_BUFFER_STATUS_SUCCESS)
      {
        size_t buffer_size;
        buffer_data = (char *)arv_buffer_get_data(buffer, &buffer_size);

#ifdef img_save

        if (!calib_flag)
        {

          sprintf(reportname, "%scontrast_%d.bin", image_location, image_no);
          FILE *pgmimg;
          pgmimg = fopen(reportname, "wb");
          fwrite(buffer_data, buffer_size, 1, pgmimg);
          fclose(pgmimg);

          image_no++;
        }
#endif

        gpu_lsci((uint16_t *)buffer_data);
        // g_print("%d\n", image_no);

        pixbuf_big = gdk_pixbuf_new_from_data((guchar *)buffer_rgb, GDK_COLORSPACE_RGB, FALSE, 8, 1920, 1200, 1920 * sizeof(rgb), NULL, NULL);
        // free(buffer_rgb);
        // memset(&buffer_rgb, 0, sizeof(buffer_rgb));
        // resize image
        GdkPixbuf *pixbuf = gdk_pixbuf_scale_simple(pixbuf_big, 900, 500, GDK_INTERP_BILINEAR);
        // gdk_pixbuf_apply_lut(pixbuf, lookup_table, has_alpha);

        gtk_image_set_from_pixbuf(GTK_IMAGE(lsciimage), pixbuf);

        g_object_unref(pixbuf);
        g_object_unref(pixbuf_big);
        //  roi(widget);
        /*
        while (gtk_events_pending())
          gtk_main_iteration();
          */
        if (timer_flag == 0)
        {
          gettimeofday(&start_time, NULL);
          timer_flag = 1;
        }
        else
        {
          gettimeofday(&end_time, NULL);
          elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                         (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
        }

        cal_perfusion();

        free_data();
        // printf("blue %f \n",perfusionh);
        // printf("red %f \n",perfusionw);

        // printf("gren%d %d %d %d %f \n",rois[2][0],rois[2][1],rois[2][2],rois[2][3],perfusion2);

        g_timeout_add(1, (GSourceFunc)timer_callbackp, (gpointer)chartp);
        if (control_porh)
        {
          g_timeout_add(1, (GSourceFunc)timer_callbackbp, (gpointer)chartbp);
        }

        while (gtk_events_pending())
          gtk_main_iteration();

        /*
                  gettimeofday(&test_end_time, NULL);
                  float timing = (test_end_time.tv_sec - test_start_time.tv_sec) +
                                   (test_end_time.tv_usec - test_start_time.tv_usec) / 1000000.0;
                  printf("The time taken by the gpu is :%f\n",timing);
        */
        /******************************************************/
      }
      else
      {
        missing++;
        // g_print("%d\n", missing);
        //  printf("Entering Buffer Error State\n");
        //   i--;
        // g_print("fail buffer ");
      }
      // }
    }

    return TRUE;
  }
  else
  {

    // g_clear_object(&stream);
    g_source_remove(camera_id);
    // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mainevent), TRUE);
    return FALSE;
  }
}

// #define fixed_roi

int img = 1, reanalyse_flag = 1;
int PAUSE = 1, reanalyse_completed = 0; // PAUSE will be zero if click pause
int reanalyse(GtkWidget *widget, gpointer image)
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
    g_source_remove(idsim);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), FALSE);
    // draw_roi();
    // draw_free_roi();
  }
  else
  {
    float max = 0, min = 255;

    uint16_t buffer[size];

    GdkPixbuf *pixbuf;

    if (img < image_no)
    {
      sprintf(reportname, "%scontrast_%d.bin", image_location, img);
      img++;

      FILE *ptr;
      ptr = fopen(reportname, "rb");
      fread(buffer, sizeof(buffer), 1, ptr);
      if (!ptr)
      {
        printf("Unable to open file!");
        // return 1;
      }
      fclose(ptr);

      gpu_lsci(buffer);

      /*
            sprintf(reportname, "%sraw_images/tif/contrast_%d.tif", dirbase, image_no);
      TIFF* tiffimg;
      tiffimg = TIFFOpen(reportname, "w");
      TIFFSetField(tiffimg, TIFFTAG_IMAGEWIDTH, 1920); // Set the image width
      TIFFSetField(tiffimg, TIFFTAG_IMAGELENGTH, 1200); // Set the image height
      TIFFSetField(tiffimg, TIFFTAG_SAMPLESPERPIXEL, 1); // Set the number of samples per pixel
      TIFFSetField(tiffimg, TIFFTAG_BITSPERSAMPLE, 10); // Set the bit depth per sample
      TIFFSetField(tiffimg, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK); // Set photometric interpretation as black-is-zero
      TIFFSetField(tiffimg, TIFFTAG_COMPRESSION, COMPRESSION_NONE); // Set compression method to none
      TIFFSetField(tiffimg, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG); // Set planar configuration as contiguous
      TIFFWriteRawStrip(tiffimg, 0, buffer, sizeof(buffer)); // Write the image data to the TIFF file
      TIFFClose(tiffimg);
      */

      pixbuf = gdk_pixbuf_new_from_data((guchar *)buffer_rgb, GDK_COLORSPACE_RGB, FALSE, 8, 1920, 1200, 1920 * sizeof(rgb), NULL, NULL);

      // resize image
      pixbuf = gdk_pixbuf_scale_simple(pixbuf, 900, 500, GDK_INTERP_BILINEAR);
      // gdk_pixbuf_apply_lut(pixbuf, lookup_table, has_alpha);
      gtk_image_set_from_pixbuf(GTK_IMAGE(lsciimage), pixbuf);

      // roi(widget);

      bp_val = bp_points[img];
      cal_perfusion();

      // if (timer_flag == 0)
      // {
      //   gettimeofday(&start_time, NULL);
      //   timer_flag = 1;
      // }
      // else
      // {
      //   gettimeofday(&end_time, NULL);
      //   elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
      //                  (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
      // }

      free_data();
      g_object_unref(pixbuf);

      g_timeout_add(1, (GSourceFunc)timer_callbackp, (gpointer)chartp);
      if (control_porh)
      {
        g_timeout_add(1, (GSourceFunc)timer_callbackbp, (gpointer)chartbp);
      }
      /******************************************************/

      // free(buffer_rgb);
    }
    else if (img == image_no)
    {
      reanalyse_flag = 1;
      img = 1;
      g_source_remove(idsim);
      // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), FALSE);
      gtk_button_set_label(GTK_BUTTON(widget), "Reanalyse");

      if (control_porh)
      {
        cal_porh_data();
        gtk_widget_hide(normalbox);
        gtk_widget_show(roi_table_porh);
      }
      else
      {
        cal_data();
        show_avg_perf_img(0);
      }
      report();
      g_timeout_add(500, (GSourceFunc)draw_roi, draw_ptr);
      g_timeout_add(500, (GSourceFunc)draw_free_roi, draw_ptr);

      report_num++;
      reanalyse_completed = 1;
    }
    // g_object_unref(pixbuf);
  }
  return PAUSE;
}

int p_create_dir = 0;
int main_create_dir = 1;
int history_flag = 0;
int back_button = 0;
void prev_lsci()
{
  ArvBuffer *buffer;
  char *buffer_data;

  GError *error = NULL;
  int img = 0;

  int tempp, k = 0;

  // g_clear_object(&stream);
  // create_stream();
  if (!ARV_IS_CAMERA(gcamera))
  {
    camerainit();
  }

  if (ARV_IS_STREAM(stream))
  {
    int i = 0;
    if (error == NULL)
      /* Start the acquisition */
      arv_camera_start_acquisition(gcamera, &error);

    if (error == NULL)
    {
      // #pragma omp parallel for //1150 = 30 s

      for (int i = 0; i < 5; i++)
      {
        buffer = arv_stream_pop_buffer(stream);
        arv_stream_push_buffer(stream, buffer);
      }
      arv_camera_stop_acquisition(gcamera, &error);

      if (ARV_IS_BUFFER(buffer) && arv_buffer_get_status(buffer) == ARV_BUFFER_STATUS_SUCCESS)
      {

        size_t buffer_size;
        buffer_data = (char *)arv_buffer_get_data(buffer, &buffer_size);

#ifdef img_save
        if (!calib_flag)
        {
          sprintf(reportname, "%scontrast_%d.bin", image_location_preview, image_no);
          FILE *pgmimg;
          pgmimg = fopen(reportname, "wb");
          fwrite(buffer_data, buffer_size, 1, pgmimg);
          fclose(pgmimg);
        }
#endif

        gpu_lsci((uint16_t *)buffer_data);
        // g_print("%d\n", image_no);

        pixbuf_big = gdk_pixbuf_new_from_data((guchar *)buffer_rgb, GDK_COLORSPACE_RGB, FALSE, 8, 1920, 1200, 1920 * sizeof(rgb), NULL, NULL);
        // free(buffer_rgb);
        // memset(&buffer_rgb, 0, sizeof(buffer_rgb));
        // resize image
        GdkPixbuf *pixbuf = gdk_pixbuf_scale_simple(pixbuf_big, 900, 500, GDK_INTERP_BILINEAR);
        // gdk_pixbuf_apply_lut(pixbuf, lookup_table, has_alpha);

        gtk_image_set_from_pixbuf(GTK_IMAGE(lsciimage), pixbuf);

        if (!calib_flag)
        {
          char img_file[200];
          sprintf(img_file, "%slsci.png", image_location_preview);
          if (!gdk_pixbuf_save(pixbuf_big, img_file, "png", &error, NULL))
          {
            printf("Unable to save image: %s\n", error->message);
          }
        }
        g_object_unref(pixbuf);
        // roi(widget);
        /*
        while (gtk_events_pending())
          gtk_main_iteration();
          */

        free_data();

        while (gtk_events_pending())
          gtk_main_iteration();
      }
      else
      {
        // printf("Entering Buffer Error State\n");
        //  i--;
      }
    }

    /* Don't because again we need to intialize Destroy the stream object */
    // g_clear_object(&stream);
  }

  while (gtk_events_pending())
    gtk_main_iteration();
}

int take_img()
{
#ifdef bluetooth_raspi
  if (!ARV_IS_CAMERA(gcamera))
  {
    sen("prev_init");
  }
  else
  {
    sen("preview");
  }

  if (!calib_flag)
  {
    char color_photo[300];
    sprintf(color_photo, "%snormal_pic.jpg", image_location_preview);
    rec_img(color_photo);

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(color_photo, NULL);
    pixbuf = gdk_pixbuf_scale_simple(pixbuf, 600, 334, GDK_INTERP_BILINEAR);
    gtk_image_set_from_pixbuf(GTK_IMAGE(normalimg), pixbuf);

    // draw_roi();
    while (gtk_events_pending())
      gtk_main_iteration();
    g_object_unref(pixbuf);
  }
#endif

  return FALSE;
}
int roipoints_x[size];
int roipoints_y[size];
int roipoints_cnt = 0;

void draw_points(GtkWidget *event_box, gpointer data)
{

  cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(event_box));
  // Set line color and width
  cairo_set_source_rgb(cr, 1, 0, 0); // Black color
  cairo_set_line_width(cr, 2);

  /*/

  // Start drawing lines
  cairo_move_to(cr, roipoints_x[0]/perf_scale_x, roipoints_y[0]/perf_scale_y);
  for (int i = 0; i < roipoints_cnt; i++) {
      cairo_line_to(cr, roipoints_x[i]/perf_scale_x, roipoints_y[i]/perf_scale_y);
      cairo_move_to(cr, roipoints_x[i]/perf_scale_x, roipoints_y[i]/perf_scale_y);
  }
  //cairo_close_path(cr);

  // Stroke the path to draw lines
  cairo_stroke(cr);

  cairo_destroy(cr);
*/
  double point_size = 3.0;

  // Draw the point using a rectangle
  for (int i = 0; i < roipoints_cnt; i++)
  {

    cairo_rectangle(cr, roipoints_x[i] / perf_scale_x - point_size / 2, roipoints_y[i] / perf_scale_y - point_size / 2, point_size, point_size);
  }

  cairo_fill(cr);
  cairo_destroy(cr);
}

void put_lsciimg()
{
  preview_flag = 1;
  char image_name[200];

  sprintf(image_name, "%sprev/lsci.png", current_working_dir);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_name, NULL);
  if (GDK_IS_PIXBUF(pixbuf))
  {
    pixbuf = gdk_pixbuf_scale_simple(pixbuf, 900, 500, GDK_INTERP_BILINEAR);
    gtk_image_set_from_pixbuf(GTK_IMAGE(lsciimage), pixbuf);

    g_object_unref(pixbuf);
  }
  else
  {
    show_dialog("No LSCI Data available");
  }

  // while (gtk_events_pending())
  //   gtk_main_iteration();
  preview_flag = 0;
}
int put_img()
{
  char image_name[300];
  sprintf(image_name, "%sprev/normal_pic.jpg", current_working_dir);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_name, NULL);
  if (GDK_IS_PIXBUF(pixbuf))
  {

    // resize image
    pixbuf = gdk_pixbuf_scale_simple(pixbuf, 600, 334, GDK_INTERP_BILINEAR);
    gtk_image_set_from_pixbuf(GTK_IMAGE(normalimg), pixbuf);
    g_object_unref(pixbuf);
  }
  else
  {
    gtk_image_set_from_icon_name(GTK_IMAGE(normalimg), "gtk-dialog-warning", GTK_ICON_SIZE_DIALOG);
  }
  while (gtk_events_pending())
    gtk_main_iteration();
}



void savefullwindow(GtkWidget *widget)
{
  GtkAllocation allocation_w;
  GError *error;
  GFile *file;
  GOutputStream *stream;
  GdkPixbuf *pixbuf;
  // printf("Entered Saveplot\n");
  sprintf(filename, "%s/Report_%s.pgm", current_working_dir, pat_name);
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

int data_receive_flag = 0, data_receive_flag_bp = 0;
int reanalyse_start = 0;
/*******************Chart PM*******************/
static gboolean timer_callbackp(GtkWidget *chartp)
{
  if (data_receive_flag)
  {
    data_receive_flag = 0;
    if (calib_flag)
    {
      yp[count] = perfusionh;
    }
    else
    {
      yp[count] = perfusionh_norm;
    }
    points[count] = perfusionh;
    points_raw[count] = (int)perfusionh;

    points_norm[count] = (int)perfusionh_norm;

    points_calib_SG[count] = (float)calib_SG;
    points_calib_beta[count] = (float)calib_beta;

    if (reanalyse_start)
      elapsed_time = time_axis[count];
    else
      time_axis[count] = elapsed_time;
    xp[count] = elapsed_time;
    slope_xyseries_update_data(SLOPE_XYSERIES(seriesp), xp, yp, count);

    if (calib_flag)
    {
      y[count] = (int)perfusionw;
    }
    else
    {
      y[count] = (int)perfusionw_norm;
    }

    points2[count] = perfusionw;
    points2_raw[count] = (int)perfusionw;

    points2_norm[count] = (int)perfusionw_norm;

    x[count] = elapsed_time;
    slope_xyseries_update_data(SLOPE_XYSERIES(series), x, y, count);

    if (calib_flag)
    {
      y2[count] = perfusion2;
    }
    else
    {
      y2[count] = perfusion2_norm;
    }

    points3[count] = perfusion2;
    points3_raw[count] = (int)perfusion2;

    points3_norm[count] = perfusion2_norm;

    x2[count] = elapsed_time;
    slope_xyseries_update_data(SLOPE_XYSERIES(series2), x2, y2, count);

    if (calib_flag)
    {
      y3[count] = perfusion3;
    }
    else
    {
      y3[count] = perfusion3_norm;
    }
    points4[count] = perfusion3;
    points4_raw[count] = (int)perfusion3;

    points4_norm[count] = perfusion3_norm;

    x3[count] = elapsed_time;
    slope_xyseries_update_data(SLOPE_XYSERIES(series3), x3, y3, count);

    slope_view_redraw(SLOPE_VIEW(chartp));
    if (control_porh)
    {
      slope_view_redraw(SLOPE_VIEW(chartp_porh));
    }
    count++;

    // g_print("%f %f %f %f     %f %f %f %f\n", yp, y, y2, y3, perfusionh_norm, perfusionw_norm, perfusion2_norm, perfusion3_norm);
  }

  return TRUE;
}
/********************************************/


/*******************Chart BP*******************/
char bp_str[10];
static gboolean timer_callbackbp(GtkWidget *chartbp)
{
  if (data_receive_flag_bp)
  {
    data_receive_flag_bp = 0;
    // static long count = 0;
    ybp[count] = bp_val;
    bp_points[count] = bp_val;
    xbp[count] = elapsed_time;

    slope_xyseries_update_data(SLOPE_XYSERIES(seriesbp), xbp, ybp, count);
    sprintf(bp_str, "%d", bp_val);
    gtk_label_set_text(GTK_LABEL(bp_prompt_val), bp_str);
    slope_view_redraw(SLOPE_VIEW(chartbp));
    // countbp++;
  }
  return TRUE;
}
/********************************************/


int check_noise_img(float x[comp_num], float per_val)
{
  if (abs(x[0] - per_val) <= 20 && abs(x[1] - per_val) <= 20)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

void filter_data()
{
  if (at_start)
  {
    at_start = 0;
    p_h[0] = perfusionh;
    p_h[1] = perfusionh;

    p_w[0] = perfusionw;
    p_w[1] = perfusionw;

    p_2[0] = perfusion2;
    p_2[1] = perfusion2;

    p_3[0] = perfusion3;
    p_3[1] = perfusion3;
  }
  else
  {
    p_h[0] = p_h[1];
    p_h[1] = perfusionh;

    p_w[0] = p_w[1];
    p_w[1] = perfusionw;

    p_2[0] = p_2[1];
    p_2[1] = perfusion2;

    p_3[0] = p_3[1];
    p_3[1] = perfusion3;

    if (check_noise_img(p_h, perfusionh))
    {
      perfusionh = p_h[0];
      p_h[1] = p_h[0];
      // g_print("noise ");
    }

    if (check_noise_img(p_w, perfusionw))
    {
      perfusionh = p_w[0];
    }
    if (check_noise_img(p_2, perfusion2))
    {
      perfusionh = p_2[0];
    }
    if (check_noise_img(p_3, perfusion3))
    {
      perfusionh = p_3[0];
    }
  }
}

int start_mov_avg = 1;

float *mov_average(float value[4])
{
  float static mov_window[4][5] =
      {
          {value[0], value[0], value[0], value[0], value[0]},
          {value[1], value[1], value[1], value[1], value[1]},
          {value[2], value[2], value[2], value[2], value[2]},
          {value[3], value[3], value[3], value[3], value[3]},
      };
  if (start_mov_avg)
  {
    start_mov_avg = 0;
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 5; j++)
      {
        mov_window[i][j] = value[i];
      }
    }
  }
  // else
  // {
  float sum = 0;
  for (int j = 0; j < 4; j++)
  {
    sum = 0;
    for (int i = 0; i < 5 - 1; i++)
    {
      sum = sum + mov_window[j][i];
      mov_window[j][i] = mov_window[j][i + 1];
    }
    mov_window[j][4] = value[j];
    value[j] = (sum + value[j]) / 5;
  }

  return value;

  //}
}

int std_max = 400;
int std_min = 0;

float standardize(float val)
{
  // Adjust the range of values into the 0 to 400
  // val = (val < 0) ? std_min : (val > std_max) ? std_max : val;
  if (val < std_min)
  {
    val = std_min;
  }
  return val;
}

unsigned char *image_buffer;

int special_case = 0;
int previousValue = 0, consider = 0;
void cal_perfusion()
{

  /***************      Perfusion       *********************/
  /*
    perfusion_sum = 0;
    pixels = 0;

    rect_roi_cal(580, 680, 460, 560);


    calib_beta = perfusion_sum / pixels;
    calib_beta = SG *( 1/(calib_beta*calib_beta * beta*beta) - 1);
    // beta
    // calib_beta = 1/calib_beta;

    perfusion_sum = 0;
    pixels = 0;

    rect_roi_cal(870, 970, 460, 560);


    calib_SG = perfusion_sum / pixels;
    calib_SG = SG * (1/(calib_SG*calib_SG * beta*beta)  - 1);
    //signal gain
    // float sg = 1/(calib_SG*calib_SG * calib_beta*calib_beta) - 1;
    // calib_SG = 250/sg;

  */

  int maxh = 0, maxw = 0, max2 = 0, max3 = 0;

  perfusion_sum = 0;
  perfusion_sum_norm = 0;
  pixels = 0;

  if (special_case)
  {
    // int counting_lines = 0;
    // //      // #pragma omp parallel for
    // for (int y = 0; y < h; y++)
    // {
    //   for (int x = 0; x < wi; x++)
    //   {
    //     counting_lines = 0;
    //     for (int row = x; row < wi; row++)
    //     {
    //       int newValue = image_buffer[y * wi + row];
    //       // g_print("%d ",newValue);
    //       if (newValue != previousValue)
    //       {
    //         if ((previousValue == 0) && (newValue == 255))
    //         {
    //           // printf("Variable changed from 0 to 1.\n");
    //           // consider = 0;
    //           consider++;
    //         }
    //         else if ((previousValue == 255) && (newValue == 0))
    //         {
    //           // printf("Variable changed from 1 to 0.\n");
    //           // printf("consider\n");
    //           // consider = 1;
    //           consider++;
    //         }
    //         previousValue = newValue;
    //       }

    //       if (consider == 2)
    //       {
    //         consider = 0;
    //         counting_lines++;
    //       }
    //     }
    //     if(counting_lines % 2)
    //     {
    //       perfusion_sum = perfusion_sum + Z_val[(y) * (wi) + (x)];
    //       perfusion_sum_norm = perfusion_sum_norm + Z_val_norm[(y) * (wi) + (x)];
    //       pixels++;
    //     }
    //   }
    // }

    for (int y = 0; y < h; y++)
    {
      for (int x = 0; x < wi; x++)
      {
        if (image_buffer[(y) * (wi) + (x)] == 1)
        {
          perfusion_sum = perfusion_sum + Z_val[(y) * (wi) + (x)];
          pixels++;
        }
      }
    }

    perfusionh = perfusion_sum / pixels;
    perfusionh_norm = (1 / (perfusionh * perfusionh * beta * beta) - 1) * SG;
    perfusionh = 1 / (perfusionh * perfusionh);
    perfusionh_norm = standardize(perfusionh_norm);
  }

  else
  {

    // g_print("%d %d %d %d rois\n",control_rect_roi[0], control_rect_roi[1], control_rect_roi[2], control_rect_roi[3]);

    free_roi_num = 0;
    if (control_roi[0])
    {
      free_roi_cal();
      free_roi_num++;
    }
    else
    {
      if (control_rect_roi[0])
        rect_roi_cal(rois[0][0], rois[0][1], rois[0][2], rois[0][3]);
      else
        circle_roi_cal(circle_rois[0][0], circle_rois[0][1], circle_rois[0][2]);
    }
    perfusionh = perfusion_sum / pixels;
    if (calib_flag)
    {
      calib_beta = 1 / (perfusionh);
      perfusionh = 1 / (perfusionh * perfusionh);
    }
    else
    {
      perfusionh_norm = (1 / (perfusionh * perfusionh * beta * beta) - 1) * SG;
      // perfusionh_norm = (1/(perfusionh * beta) - 1) * SG;
      perfusionh_norm = standardize(perfusionh_norm);
      perfusionh = 1 / (perfusionh * perfusionh);
    }

    // g_print("Roi1 %d %d %d %d\n",rois[0][0], rois[0][1], rois[0][2], rois[0][3]);

    maxh = max_value;
    max_value = 0;
    // g_print("perfusion_sum pixels %f \n",100/sqrt(perfusionh));

    pixels = 0, perfusion_sum = 0;
    perfusion_sum_norm = 0;

    if (control_roi[1])
    {
      // free_roi_num = 0;
      free_roi_cal();
      // g_print("two \n");    // free_roi_num = 0;
      free_roi_num++;
    }
    else
    {
      if (control_rect_roi[1])
        rect_roi_cal(rois[1][0], rois[1][1], rois[1][2], rois[1][3]);
      else
        circle_roi_cal(circle_rois[1][0], circle_rois[1][1], circle_rois[1][2]);
    }
    perfusionw = perfusion_sum / pixels;

    if (calib_flag)
    {
      float sg = 1 / (perfusionw * perfusionw * calib_beta * calib_beta) - 1;

      // float sg = 1/(perfusionw * calib_beta) - 1;

      calib_SG = std_max / sg;

      perfusionw = 1 / (perfusionw * perfusionw);
    }
    else
    {
      perfusionw_norm = (1 / (perfusionw * perfusionw * beta * beta) - 1) * SG;
      // perfusionw_norm = (1/(perfusionw * beta) - 1) * SG;

      perfusionw_norm = standardize(perfusionw_norm);
      perfusionw = 1 / (perfusionw * perfusionw);
    }

    // g_print("Roi2 %d %d %d %d\n",rois[1][0], rois[1][1], rois[1][2], rois[1][3]);

    // perfusionw_raw

    maxw = max_value;
    max_value = 0;
    // g_print("perfusion_sum pixels %f %f\n", perfusion_sum, pixels);
    pixels = 0, perfusion_sum = 0;
    perfusion_sum_norm = 0;

    if (control_roi[2])
    {
      free_roi_cal();
      free_roi_num++;
    }
    else
    {
      if (control_rect_roi[2])
        rect_roi_cal(rois[2][0], rois[2][1], rois[2][2], rois[2][3]);
      else
        circle_roi_cal(circle_rois[2][0], circle_rois[2][1], circle_rois[2][2]);
    }
    perfusion2 = perfusion_sum / pixels;

    if (calib_flag)
    {
      perfusion2 = 1 / (perfusion2 * perfusion2);
    }
    else
    {
      perfusion2_norm = (1 / (perfusion2 * perfusion2 * beta * beta) - 1) * SG;
      // perfusion2_norm = (1/(perfusion2 * beta) - 1) * SG;
      perfusion2_norm = standardize(perfusion2_norm);
      perfusion2 = 1 / (perfusion2 * perfusion2);
    }

    max2 = max_value;
    max_value = 0;
    // g_print("perfusion_sum pixels %f %f\n", perfusion_sum, pixels);
    pixels = 0, perfusion_sum = 0;
    perfusion_sum_norm = 0;

    if (control_roi[3])
    {
      free_roi_cal();
      free_roi_num++;
    }
    else
    {
      if (control_rect_roi[3])
        rect_roi_cal(rois[3][0], rois[3][1], rois[3][2], rois[3][3]);
      else
        circle_roi_cal(circle_rois[3][0], circle_rois[3][1], circle_rois[3][2]);
    }

    perfusion3 = perfusion_sum / pixels;

    if (calib_flag)
    {
      perfusion3 = 1 / (perfusion3 * perfusion3);
    }
    else
    {
      perfusion3_norm = (1 / (perfusion3 * perfusion3 * beta * beta) - 1) * SG;
      // perfusion3_norm = (1/(perfusion3 * beta) - 1) * SG;
      perfusion3_norm = standardize(perfusion3_norm);
      perfusion3 = 1 / (perfusion3 * perfusion3);
    }

    // perfusion3_norm  = perfusion3*beta * 0.09633;

    max3 = max_value;

    /********************         max normalization          ******************/

    // max_value = findMax(maxh, maxw, max2, max3);

    // int multiplier = 100;
    // perfusion3 = multiplier*perfusion3/max_value;
    // perfusion2 = multiplier*perfusion2/max_value;
    // perfusionw = multiplier*perfusionw/max_value;
    // perfusionh = multiplier*perfusionh/max_value;

    // g_print("perfusion_sum pixels %f %f\n\n", perfusion_sum, pixels);
    pixels = 0, perfusion_sum = 0;
    perfusion_sum_norm = 0;

    max_value = 0;
  }
  // g_print("perf %f %f %f %f\n", perfusionh, perfusionw, perfusion2, perfusion3);
  //  for(int i=0; i<4; i++)
  //  {
  //   g_print("free roi flags : %d\n",control_roi[i]);
  //  }
  //  g_print("its end total free roi is : %d\n",free_roi_num);
  if (isinf(perfusionh) || isnan(perfusionh))
    perfusionh = 0;
  if (isinf(perfusionw) || isnan(perfusionw))
    perfusionw = 0;
  if (isinf(perfusion2) || isnan(perfusion2))
    perfusion2 = 0;
  if (isinf(perfusion3) || isnan(perfusion3))
    perfusion3 = 0;

  if (isinf(perfusionh_norm) || isnan(perfusionh_norm))
    perfusionh_norm = 0;
  if (isinf(perfusionw_norm) || isnan(perfusionw_norm))
    perfusionw_norm = 0;
  if (isinf(perfusion2_norm) || isnan(perfusion2_norm))
    perfusion2_norm = 0;
  if (isinf(perfusion3_norm) || isnan(perfusion3_norm))
    perfusion3_norm = 0;
  // filter_data();

  float x[4] = {perfusionh, perfusionw, perfusion2, perfusion3};
  float *perf_values = mov_average(x);
  perfusionh = *(perf_values);
  perfusionw = *(perf_values + 1);
  perfusion2 = *(perf_values + 2);
  perfusion3 = *(perf_values + 3);

  data_receive_flag = 1;
  data_receive_flag_bp = 1;

  // aver++;
  //  //g_print("%f %f %f", perfusionh, perfusionw, perfusion2);
  //  perfu_sum_h = perfu_sum_h + perfusionh;
  //  perfu_sum_w = perfu_sum_w + perfusionw;
  //  perfu_sum_2 = perfu_sum_2 + perfusion2;
  //  perfu_sum_3 = perfu_sum_3 + perfusion3;
  //  if(aver%4 == 0)
  //  {
  //    perfusionh = (float) perfu_sum_h/4;
  //    perfusionw = (float) perfu_sum_w/4;
  //    perfusion2 = (float) perfu_sum_2/4;
  //    perfusion3 = (float) perfu_sum_3/4;
  //    data_receive_flag = 1;
  //    data_receive_flag_bp = 1;
  //    perfu_sum_h = 0;
  //    perfu_sum_w = 0;
  //    perfu_sum_2 = 0;
  //    perfu_sum_3 = 0;
  //  }
  // g_print("Rois size %d %d %d %d\n", rois[0][0], rois[0][1], rois[0][2], rois[0][3]);
}
