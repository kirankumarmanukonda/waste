# 0 "camera_functions.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "camera_functions.c"
# 1 "camera_functions.h" 1


# 1 "common.h" 1
# 4 "camera_functions.h" 2







extern rgb *buffer_rgb;
extern float *Z_val;


int camerainit();
void free_data();
int assign_ip_camera();
void create_stream();
void saveConfig(const char *filename, int exposureTime, int fps, int window, float rois[4][4], float beta, float SG);
void loadConfig(const char *filename, int *exposureTime, int *fps, int *window, float rois[4][4], float *beta, float *SG);


int calib_flag = 0, calib_device = 0;
int from_calib_button = 0;
# 2 "camera_functions.c" 2




ArvCamera *gcamera;
ArvStream *stream;


void free_data()
{


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

    payload = arv_camera_get_payload(gcamera, &error);
    if (error == NULL)
    {

      for (i = 0; i < 5; i++)
        arv_stream_push_buffer(stream, arv_buffer_new(payload, NULL));
    }
  }
}

int assign_ip_camera()
{

  const char *interface1 = "eno1";
  const char *interface2 = "enp2s0";


  char command[100];


  if (system("ifconfig eno1 2>/dev/null") == 0)
  {

    snprintf(command, sizeof(command), "sudo ifconfig %s 169.254.118.246 netmask 255.255.255.0 up", interface1);
  }
  else if (system("ifconfig enp2s0 2>/dev/null") == 0)
  {


    snprintf(command, sizeof(command), "sudo ifconfig %s 169.254.118.246 netmask 255.255.255.0 up", interface2);
  }
  else
  {

    printf("Neither %s nor %s interface found.\n", interface1, interface2);
    return 1;
  }


  int result = system(command);

  return result;
}

float beta, SG;
int exposureTime, fps, window_size;
float exposure_tim = 10;
char exp_tim_string[10];

int camerainit()
{
  system("sudo systemctl restart NetworkManager");
  assign_ip_camera();

  GError *error = NULL;

  gcamera = arv_camera_new(NULL, &error);
  ArvAcquisitionMode cam_mode = ARV_ACQUISITION_MODE_CONTINUOUS;
  ArvDevice *device = arv_camera_get_device(gcamera);
  arv_device_set_string_feature_value(device, "LineSelector", "Line2", NULL);
  arv_device_set_string_feature_value(device, "LineMode", "Output", NULL);

  if (ARV_IS_CAMERA(gcamera))
  {
    printf("Found camera '%s'\n", arv_camera_get_model_name(gcamera, NULL));



    arv_camera_set_frame_rate(gcamera, fps, NULL);
    arv_camera_set_gain(gcamera, 1.0, &error);
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


void loadConfig(const char *filename, int *exposureTime, int *fps, int *window, float rois[4][4], float *beta, float *SG)
{
  FILE *configFile = fopen(filename, "r");
  calib_device = 1;

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
