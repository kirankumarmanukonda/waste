#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "main.h"

extern int preview_flag;
extern int reanalyse_start;
extern int window_size;
extern int std_max;
extern int std_min;
extern float *avg_Z;


/********************LUT Colorspce*************************/

uint8_t lookup_table[256][3] =
    {
        {0, 1, 43},
        {0, 2, 47},
        {0, 3, 50},
        {0, 4, 53},
        {0, 5, 56},
        {0, 5, 60},
        {0, 6, 63},
        {0, 7, 66},
        {0, 8, 70},
        {0, 9, 73},
        {0, 10, 76},
        {0, 11, 80},
        {0, 12, 83},
        {0, 13, 86},
        {0, 14, 89},
        {0, 15, 93},
        {0, 16, 96},
        {0, 17, 99},
        {0, 18, 103},
        {0, 19, 106},
        {0, 19, 109},
        {0, 20, 113},
        {0, 21, 116},
        {0, 22, 119},
        {0, 23, 122},
        {0, 24, 126},
        {0, 25, 129},
        {0, 26, 132},
        {0, 27, 136},
        {0, 28, 139},
        {0, 29, 142},
        {0, 30, 145},
        {0, 31, 149},
        {0, 32, 152},
        {0, 33, 155},
        {0, 33, 159},
        {0, 34, 162},
        {0, 35, 165},
        {0, 36, 169},
        {0, 37, 172},
        {0, 38, 175},
        {0, 39, 178},
        {0, 40, 182},
        {0, 41, 185},
        {0, 42, 188},
        {0, 43, 192},
        {0, 44, 195},
        {0, 45, 198},
        {0, 46, 202},
        {0, 47, 205},
        {0, 47, 208},
        {0, 48, 211},
        {0, 49, 215},
        {0, 50, 218},
        {0, 51, 221},
        {0, 52, 225},
        {0, 53, 228},
        {0, 54, 231},
        {0, 55, 234},
        {0, 56, 238},
        {0, 57, 241},
        {0, 58, 244},
        {0, 59, 248},
        {0, 60, 251},
        {0, 61, 253},
        {0, 64, 251},
        {0, 67, 249},
        {0, 70, 247},
        {0, 73, 245},
        {0, 76, 244},
        {0, 79, 242},
        {0, 83, 240},
        {0, 86, 238},
        {0, 89, 236},
        {0, 92, 234},
        {0, 95, 232},
        {0, 98, 230},
        {0, 101, 229},
        {0, 104, 227},
        {0, 107, 225},
        {0, 110, 223},
        {0, 113, 221},
        {0, 116, 219},
        {0, 119, 217},
        {0, 122, 215},
        {0, 125, 214},
        {0, 129, 212},
        {0, 132, 210},
        {0, 135, 208},
        {0, 138, 206},
        {0, 141, 204},
        {0, 144, 202},
        {0, 147, 200},
        {0, 150, 199},
        {0, 153, 197},
        {0, 156, 195},
        {0, 159, 193},
        {0, 162, 191},
        {0, 165, 189},
        {0, 168, 187},
        {0, 171, 185},
        {0, 175, 183},
        {0, 178, 182},
        {0, 181, 180},
        {0, 184, 178},
        {0, 187, 176},
        {0, 190, 174},
        {0, 193, 172},
        {0, 196, 170},
        {0, 199, 168},
        {0, 202, 167},
        {0, 205, 165},
        {0, 208, 163},
        {0, 211, 161},
        {0, 214, 159},
        {0, 217, 157},
        {0, 221, 155},
        {0, 224, 153},
        {0, 227, 152},
        {0, 230, 150},
        {0, 233, 148},
        {0, 236, 146},
        {0, 239, 144},
        {0, 242, 142},
        {0, 245, 140},
        {0, 248, 138},
        {0, 251, 136},
        {0, 254, 135},
        {2, 255, 133},
        {6, 255, 131},
        {10, 255, 128},
        {14, 255, 126},
        {18, 255, 124},
        {22, 255, 122},
        {26, 255, 120},
        {30, 255, 118},
        {34, 255, 116},
        {38, 255, 114},
        {42, 255, 112},
        {46, 255, 110},
        {50, 255, 107},
        {54, 255, 105},
        {58, 255, 103},
        {62, 255, 101},
        {66, 255, 99},
        {70, 255, 97},
        {74, 255, 95},
        {78, 255, 93},
        {82, 255, 91},
        {85, 255, 89},
        {89, 255, 87},
        {93, 255, 84},
        {97, 255, 82},
        {101, 255, 80},
        {105, 255, 78},
        {109, 255, 76},
        {113, 255, 74},
        {117, 255, 72},
        {121, 255, 70},
        {125, 255, 68},
        {129, 255, 66},
        {133, 255, 63},
        {137, 255, 61},
        {141, 255, 59},
        {145, 255, 57},
        {149, 255, 55},
        {153, 255, 53},
        {157, 255, 51},
        {161, 255, 49},
        {165, 255, 47},
        {169, 255, 45},
        {173, 255, 42},
        {177, 255, 40},
        {181, 255, 38},
        {185, 255, 36},
        {189, 255, 34},
        {193, 255, 32},
        {197, 255, 30},
        {201, 255, 28},
        {205, 255, 26},
        {209, 255, 24},
        {213, 255, 21},
        {217, 255, 19},
        {221, 255, 17},
        {225, 255, 15},
        {229, 255, 13},
        {233, 255, 11},
        {237, 255, 9},
        {241, 255, 7},
        {245, 255, 5},
        {248, 255, 3},
        {252, 255, 1},
        {253, 253, 0},
        {254, 250, 1},
        {254, 246, 1},
        {254, 243, 1},
        {254, 239, 2},
        {254, 235, 2},
        {254, 232, 2},
        {254, 228, 3},
        {254, 225, 3},
        {254, 221, 3},
        {254, 218, 4},
        {254, 214, 4},
        {254, 211, 4},
        {254, 207, 5},
        {254, 204, 5},
        {254, 200, 6},
        {254, 196, 6},
        {254, 193, 6},
        {254, 189, 7},
        {254, 186, 7},
        {254, 182, 7},
        {254, 179, 8},
        {254, 175, 8},
        {254, 172, 8},
        {254, 168, 9},
        {254, 165, 9},
        {254, 161, 9},
        {254, 158, 10},
        {255, 154, 10},
        {255, 150, 10},
        {255, 147, 11},
        {255, 143, 11},
        {255, 140, 12},
        {255, 136, 12},
        {255, 133, 12},
        {255, 129, 13},
        {255, 126, 13},
        {255, 122, 13},
        {255, 119, 14},
        {255, 115, 14},
        {255, 112, 14},
        {255, 108, 15},
        {255, 104, 15},
        {255, 101, 15},
        {255, 97, 16},
        {255, 94, 16},
        {255, 90, 16},
        {255, 87, 17},
        {255, 83, 17},
        {255, 80, 17},
        {255, 76, 18},
        {255, 73, 18},
        {255, 69, 19},
        {255, 66, 19},
        {255, 62, 19},
        {255, 58, 20},
        {255, 55, 20},
        {255, 51, 20},
        {255, 48, 21},
        {255, 44, 21},
        {255, 41, 21},
        {255, 37, 22},
        {255, 34, 22},
        {255, 30, 22}};

int gpu_init()
{
  int deviceCount = 0;
  cudaError_t error_id = cudaGetDeviceCount(&deviceCount);

  if (error_id != cudaSuccess)
  {
    printf("cudaGetDeviceCount returned %d\n-> %s\n",
           (int)error_id, cudaGetErrorString(error_id));
    printf("Result = FAIL\n");
    return -1;
  }
  if (deviceCount == 0)
  {
    printf("There are no available device(s) that support CUDA\n");
    return -1;
  }
  else
  {
    printf("Detected %d CUDA Capable device(s)\n", deviceCount);
  }
  int dev = 0;
  cudaSetDevice(dev);
  cudaDeviceProp deviceProp;
  cudaGetDeviceProperties(&deviceProp, dev);
  printf("Device %d: \"%s\"\n", dev, deviceProp.name);

  return 0;
}

void saveImageArrayAsBinary(const char *filename, unsigned char *imageArray, size_t arraySize)
{
  FILE *file = fopen(filename, "wb");

  if (file == NULL)
  {
    perror("Error opening file");
    return;
  }

  size_t elements_written = fwrite(imageArray, sizeof(unsigned char), arraySize, file);

  if (elements_written != arraySize)
  {
    perror("Error writing image array");
  }

  fclose(file);
}

/*********************************************************/

rgb *buffer_rgb;
float *Z_val;
float *Z_val_norm;
float *Z_val_prev;
// #define k
#define averaging
// #define averaging_val
float *Z_avg, *Z_sum0, *Z_sum1;
float *Z_avg_val, *Z_sum0_val, *Z_sum1_val;

void free_avg()
{
  if (Z_avg != NULL)
  {
    free(Z_avg);
    free(Z_sum1);
    free(Z_sum0);
  }
}

__global__ void fillZeroPaddedArray(uint16_t *buffer, float *zimg, int w)
{
  int half_w = w / 2;
  int i = blockIdx.x * blockDim.x + threadIdx.x;

  int j = blockIdx.y * blockDim.y + threadIdx.y;

  if (i < h + w - 1 && j < wi + w - 1)
  {
    if ((i >= half_w && j >= half_w) && (i < h + half_w && j < wi + half_w))
    {
      zimg[i * (wi + w - 1) + j] = ((float)buffer[(i - half_w) * wi + (j - half_w)] != 0) ? (float)buffer[(i - half_w) * wi + (j - half_w)] : 1;
    }
    else
    {
      zimg[i * (wi + w - 1) + j] = (float)1;
    }
  }
}

__global__ void lsci_kernel(float *zimg, float *Z, int w)
{
  int half_w = w / 2;
  float s = 0;
  float m = 0.0;
  float sd = 0.0;
  float SD = 0.0;
  // float coherence_factor = 3.0;

  // for calculating the coherence_factor
  // int finding_mean = 1;
#define solving
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  int j = blockIdx.y * blockDim.y + threadIdx.y;

  if ((i >= half_w && i < h + half_w) && (j >= half_w && j < wi + half_w))
  {
    // printf("hello");
    s = 0;

    for (int iter_i = -half_w; iter_i <= half_w; iter_i++)
    {
      for (int iter_j = -half_w; iter_j <= half_w; iter_j++)
      {
        s = s + zimg[(i + iter_i) * (wi + w - 1) + (j + iter_j)];
      }
    }

    m = s / (w * w);
    SD = 0;

    for (int iter_i = -half_w; iter_i <= half_w; iter_i++)
    {
      for (int iter_j = -half_w; iter_j <= half_w; iter_j++)
      {
        SD = SD + (zimg[(i + iter_i) * (wi + w - 1) + (j + iter_j)] - m) * (zimg[(i + iter_i) * (wi + w - 1) + (j + iter_j)] - m);
      }
    }

    sd = SD / (w * w);

    sd = sqrt(sd);

    // calculating the mean and standard deviation for the coherence_factor
    // if(finding_mean && i>600 && j>960 && i<650 && j< 1000)
    // {
    //   finding_mean = 0;
    //   //printf("The mean and Std Dev : %f %f\n",m, sd);
    //   printf("The k value %f\n",sd/m);
    // }

    Z[(i - half_w) * (wi) + (j - half_w)] = (sd / m);

    // Z[(i - half_w) * (wi) + (j - half_w)] = (sd / m);

    // Z[(i - half_w) * (wi) + (j - half_w)] = sqrt(0.02849*0.02849* (0.2904 + 1)*(1/57.1375 - 1/90.78) + Z[(i - half_w) * (wi) + (j - half_w)]*Z[(i - half_w) * (wi) + (j - half_w)]);
    // Z[(i - half_w) * (wi) + (j - half_w)] = sqrt(0.0181*0.0181* (22.349 + 1)*(1/57.1 - 1/41.91) + Z[(i - half_w) * (wi) + (j - half_w)]*Z[(i - half_w) * (wi) + (j - half_w)]);

    // Z[(i - half_w) * (wi) + (j - half_w)] = sqrt(0.02849*0.02849* (0.2904 + 1)*(1/57.1375) + Z[(i - half_w) * (wi) + (j - half_w)]*Z[(i - half_w) * (wi) + (j - half_w)]);

    // 3.3 *
    // Z[(i - half_w) * (wi) + (j - half_w)] = sqrt(0.0181*0.0181* (22.349 + 1)*(1/41.91) + Z[(i - half_w) * (wi) + (j - half_w)]*Z[(i - half_w) * (wi) + (j - half_w)]);

    // 1/255 - 1/zimg[(i) * (wi) + (j)]
  }
}

#define headache
extern float beta;
extern float SG;
extern int calib_flag;
__global__ void normalize_invert(float *Z, float *Z_val, float *avg_Z, float beta, float SG, int calib_flag, int std_max, int std_min)
{
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  int j = blockIdx.y * blockDim.y + threadIdx.y;
  // printf("sg and beta %f %f",SG, beta);

  if (i < h && j < wi)
  {

    Z_val[i * wi + j] = Z[i * wi + j];

    Z[i * wi + j] = Z[i * wi + j] * Z[i * wi + j];
    // if(isnan(Z[i * wi + j]) || isinf(Z[i * wi + j]) || Z[i * wi + j] == 0)
    // {
    //     Z[i * wi + j] = Z[i * wi + j - 1];
    // }
    // printf("%.1f", Z[i * wi + j]);

    Z[i * wi + j] = (1 / (Z[i * wi + j] * beta * beta) - 1) * SG;
    // Z[i * wi + j] = (1/(Z[i * wi + j] * beta) - 1) * SG;

    if (Z[i * wi + j] < 1)
    {
      Z[i * wi + j] = 1;
    }
    else if (Z[i * wi + j] > std_max)
    {
      Z[i * wi + j] = std_max;
    }
    Z[i * wi + j] = Z[i * wi + j] / std_max;

    avg_Z[i * wi + j] = avg_Z[i * wi + j] + Z[i * wi + j];
  }
}

#ifdef averaging
__global__ void sum_Z(float *Z, float *Z_avg, float *Z_sum0, float *Z_sum1)
{
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  int j = blockIdx.y * blockDim.y + threadIdx.y;

  if (i < h && j < wi)
  {
    Z_avg[i * wi + j] = (Z[i * wi + j] + Z_sum0[i * wi + j] + Z_sum1[i * wi + j]) / 3;
    // if(Z_avg[i * wi + j]<0.8)
    // {
    //   printf("%0.1f ",Z_avg[i * wi + j]);
    // }
    Z_sum1[i * wi + j] = Z_sum0[i * wi + j];
    Z_sum0[i * wi + j] = Z[i * wi + j];
  }
}

__global__ void sum_Z_val(float *Z_val, float *Z_avg_val, float *Z_sum0_val, float *Z_sum1_val)
{
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  int j = blockIdx.y * blockDim.y + threadIdx.y;

  if (i < h && j < wi)
  {
    Z_avg_val[i * wi + j] = (Z_val[i * wi + j] + Z_sum0_val[i * wi + j] + Z_sum1_val[i * wi + j]) / 3;
    // if(Z_avg[i * wi + j]<0.8)
    // {
    //   printf("%0.1f ",Z_avg[i * wi + j]);
    // }
    Z_sum1_val[i * wi + j] = Z_sum0_val[i * wi + j];
    Z_sum0_val[i * wi + j] = Z_val[i * wi + j];
  }
}

#endif

__global__ void grayscale_to_rgb_kernel(float *Z, uint8_t *lookup_table, rgb *buffer_rgb, int std_max, float lower_threshold, float upper_threshold)
{
  float range_min = (float)lower_threshold / std_max; // 0.88; cmap_min_val 0.875
  float range_max = (float)upper_threshold / std_max; // 0.97; cmap_max_val 0.96
  float range = range_max - range_min;
  float rangeinv = 1 / range;

  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int idy = blockIdx.y * blockDim.y + threadIdx.y;
  if (idx < h && idy < wi)
  {
    float grey = (Z[idx * wi + idy] > range_min) ? ((Z[idx * wi + idy] < range_max) ? ((Z[idx * wi + idy] - range_min) * rangeinv * 255) : 255) : 0;

    Z[idx * wi + idy] = grey;
    // float grey = (Z[idx * wi + idy] > 0) ? ((Z[idx * wi + idy] < 250) ? (Z[idx * wi + idy] ) : 255) : 0;

    buffer_rgb[idx * wi + idy].b = lookup_table[(int)grey * 3 + 2];
    buffer_rgb[idx * wi + idy].g = lookup_table[(int)grey * 3 + 1];
    buffer_rgb[idx * wi + idy].r = lookup_table[(int)grey * 3];
  }
}

__global__ void compute_avg_Z(float *Z, uint8_t *lookup_table, rgb *buffer_rgb, int num_imgs, int std_max, float lower_threshold, float upper_threshold)
{
  float range_min = (float)lower_threshold / std_max; // 0.88; cmap_min_val 0.875
  float range_max = (float)upper_threshold / std_max; // 0.97; cmap_max_val 0.96
  float range = range_max - range_min;
  float rangeinv = 1 / range;

  float temp;
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int idy = blockIdx.y * blockDim.y + threadIdx.y;
  if (idx < h && idy < wi)
  {
    temp = Z[idx * wi + idy] / num_imgs;
    float grey = (temp > range_min) ? ((temp < range_max) ? ((temp - range_min) * rangeinv * 255) : 255) : 0;

    buffer_rgb[idx * wi + idy].b = lookup_table[(int)grey * 3 + 2];
    buffer_rgb[idx * wi + idy].g = lookup_table[(int)grey * 3 + 1];
    buffer_rgb[idx * wi + idy].r = lookup_table[(int)grey * 3];
  }
}
int dynamic_alloc_once = 1;
void gpu_lsci(uint16_t buffer[BUFFER_SIZE])
{

  float *zimg;
  float *Z;
  int w = window_size; // window size
  int half_w = floor(w / 2);

  Z = (float *)malloc((wi) * (h) * sizeof(float));

  Z_val = (float *)malloc((wi) * (h) * sizeof(float));

  zimg = (float *)malloc((wi + w - 1) * (h + w - 1) * sizeof(float));

  // final_Z = (float *)malloc((wi + 4) * (h + 4) * sizeof(float));

  uint16_t *dev_buffer;
  float *dev_zimg;
  cudaMalloc((void **)&dev_buffer, sizeof(uint16_t) * h * wi);
  cudaMalloc((void **)&dev_zimg, sizeof(float) * (h + w - 1) * (wi + w - 1));

  // Copy input array from host to device
  cudaMemcpy(dev_buffer, buffer, sizeof(uint16_t) * h * wi, cudaMemcpyHostToDevice);

  // Define number of threads and blocks for the kernel
  dim3 threadsPerBlock(5, 5);
  dim3 numBlocks((h + w - 1 + threadsPerBlock.x - 1) / threadsPerBlock.x, (wi + w - 1 + threadsPerBlock.y - 1) / threadsPerBlock.y);

  // Call kernel function
  fillZeroPaddedArray<<<numBlocks, threadsPerBlock>>>(dev_buffer, dev_zimg, w);

  // Wait for kernel to finish
  cudaDeviceSynchronize();

  // Copy zero-padded array from device to host
  cudaMemcpy(zimg, dev_zimg, sizeof(float) * (h + w - 1) * (wi + w - 1), cudaMemcpyDeviceToHost);

  // Free memory on the GPU
  cudaFree(dev_buffer);

  // getPGM(zimg);

  float *d_Z, *d_avg_Z;

  cudaMalloc((void **)&d_Z, sizeof(float) * h * wi);
  cudaMalloc((void **)&d_avg_Z, sizeof(float) * h * wi);
  cudaMemcpy(dev_zimg, zimg, sizeof(float) * (h + w - 1) * (wi + w - 1), cudaMemcpyHostToDevice);

  dim3 threadsBlock(5, 5);
  dim3 num_Blocks((h + threadsBlock.x - 1) / threadsBlock.x, (wi + threadsBlock.y - 1) / threadsBlock.y);

  lsci_kernel<<<num_Blocks, threadsBlock>>>(dev_zimg, d_Z, w);

  cudaMemcpy(Z, d_Z, sizeof(float) * h * wi, cudaMemcpyDeviceToHost);

  cudaFree(dev_zimg);
  free(zimg);

  float *d_Z_val;
  cudaMalloc((void **)&d_Z_val, sizeof(float) * h * wi);

  // Copy input data from host to device
  cudaMemcpy(d_Z, Z, h * wi * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(d_avg_Z, avg_Z, h * wi * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(d_Z_val, Z_val, h * wi * sizeof(float), cudaMemcpyHostToDevice);

  // Launch kernel with appropriate grid and block size
  dim3 blockDim(32, 32);
  dim3 gridDim((h + blockDim.x - 1) / blockDim.x, (wi + blockDim.y - 1) / blockDim.y);
  normalize_invert<<<gridDim, blockDim>>>(d_Z, d_Z_val, d_avg_Z, beta, SG, calib_flag, std_max, std_min);

  // Copy output data from device to host
  cudaMemcpy(Z, d_Z, h * wi * sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(Z_val, d_Z_val, h * wi * sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(avg_Z, d_avg_Z, h * wi * sizeof(float), cudaMemcpyDeviceToHost);

  cudaFree(d_avg_Z);
#ifdef averaging
  // if(reanalyse_start && colormap_calib<1)
  // {
  //   dynamic_alloc_once = 1;
  // }

  if (preview_flag != 0)
  {
    float *d_Z_avg;
    float *d_Z_sum0;
    float *d_Z_sum1;
    cudaMalloc((void **)&d_Z_avg, sizeof(float) * h * wi);
    cudaMalloc((void **)&d_Z_sum0, sizeof(float) * h * wi);
    cudaMalloc((void **)&d_Z_sum1, sizeof(float) * h * wi);

    // Copy input data from host to device
    cudaMemcpy(d_Z, Z, h * wi * sizeof(float), cudaMemcpyHostToDevice);

    if (dynamic_alloc_once)
    {
      free_avg();
      Z_avg = (float *)malloc((wi) * (h) * sizeof(float));
      Z_sum0 = (float *)malloc((wi) * (h) * sizeof(float));
      Z_sum1 = (float *)malloc((wi) * (h) * sizeof(float));

      cudaMemcpy(d_Z_avg, Z, h * wi * sizeof(float), cudaMemcpyHostToDevice);
      cudaMemcpy(d_Z_sum0, Z, h * wi * sizeof(float), cudaMemcpyHostToDevice);
      cudaMemcpy(d_Z_sum1, Z, h * wi * sizeof(float), cudaMemcpyHostToDevice);
      dynamic_alloc_once = 0;
    }
    // Launch kernel with appropriate grid and block size
    else
    {
      cudaMemcpy(d_Z_avg, Z_avg, h * wi * sizeof(float), cudaMemcpyHostToDevice);
      cudaMemcpy(d_Z_sum0, Z_sum0, h * wi * sizeof(float), cudaMemcpyHostToDevice);
      cudaMemcpy(d_Z_sum1, Z_sum1, h * wi * sizeof(float), cudaMemcpyHostToDevice);
    }

    // printf("%f valskjfd\n",Z_avg[(700) * (wi) + (100)]);
    sum_Z<<<gridDim, blockDim>>>(d_Z, d_Z_avg, d_Z_sum0, d_Z_sum1);

    // Copy output data from device to host

    // cudaMemcpy(Z_avg, d_Z_avg, h * wi * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(Z, d_Z_avg, h * wi * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(Z_sum0, d_Z_sum0, h * wi * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(Z_sum1, d_Z_sum1, h * wi * sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_Z_avg);
    cudaFree(d_Z_sum0);
    cudaFree(d_Z_sum1);
  }
  else
  {
    // dynamic_alloc_once = 1;
  }
#endif

#ifdef averaging_val
  float *d_Z_avg_val;
  float *d_Z_sum0_val;
  float *d_Z_sum1_val;
  cudaMalloc((void **)&d_Z_avg_val, sizeof(float) * h * wi);
  cudaMalloc((void **)&d_Z_sum0_val, sizeof(float) * h * wi);
  cudaMalloc((void **)&d_Z_sum1_val, sizeof(float) * h * wi);

  // Copy input data from host to device
  cudaMemcpy(d_Z_val, Z_val, h * wi * sizeof(float), cudaMemcpyHostToDevice);

  if (dynamic_alloc_once)
  {
    Z_avg_val = (float *)malloc((wi) * (h) * sizeof(float));
    Z_sum0_val = (float *)malloc((wi) * (h) * sizeof(float));
    Z_sum1_val = (float *)malloc((wi) * (h) * sizeof(float));

    cudaMemcpy(d_Z_avg_val, Z_val, h * wi * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Z_sum0_val, Z_val, h * wi * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Z_sum1_val, Z_val, h * wi * sizeof(float), cudaMemcpyHostToDevice);
    dynamic_alloc_once = 0;
  }
  // Launch kernel with appropriate grid and block size
  else
  {
    cudaMemcpy(d_Z_avg_val, Z_avg_val, h * wi * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Z_sum0_val, Z_sum0_val, h * wi * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Z_sum1_val, Z_sum1_val, h * wi * sizeof(float), cudaMemcpyHostToDevice);
  }

  // printf("%f valskjfd\n",Z_avg[(700) * (wi) + (100)]);
  sum_Z<<<gridDim, blockDim>>>(d_Z_val, d_Z_avg_val, d_Z_sum0_val, d_Z_sum1_val);

  // Copy output data from device to host

  // cudaMemcpy(Z_avg, d_Z_avg, h * wi * sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(Z_val, d_Z_avg_val, h * wi * sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(Z_sum0_val, d_Z_sum0_val, h * wi * sizeof(float), cudaMemcpyDeviceToHost);
  cudaMemcpy(Z_sum1_val, d_Z_sum1_val, h * wi * sizeof(float), cudaMemcpyDeviceToHost);

  cudaFree(d_Z_avg_val);
  cudaFree(d_Z_sum0_val);
  cudaFree(d_Z_sum1_val);
#endif
  cudaFree(d_Z_val);

#ifdef headache
  // else
  // {
  //   printf("%f %f\n",cmap_min_val, cmap_max_val);
  // }

  uint8_t *d_lookup_table;
  rgb *d_buffer_rgb;

  buffer_rgb = (rgb *)malloc(wi * h * sizeof(rgb));
  cudaMalloc(&d_lookup_table, 256 * 3 * sizeof(uint8_t));
  cudaMalloc(&d_buffer_rgb, h * wi * sizeof(rgb));

  cudaMemcpy(d_Z, Z, h * wi * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(d_lookup_table, lookup_table, 256 * 3 * sizeof(uint8_t), cudaMemcpyHostToDevice);

  dim3 block_size(32, 32);
  dim3 grid_size((h + block_size.x - 1) / block_size.x, (wi + block_size.y - 1) / block_size.y);
  grayscale_to_rgb_kernel<<<grid_size, block_size>>>(d_Z, d_lookup_table, d_buffer_rgb, std_max, cmap_min_val, cmap_max_val);
  cudaMemcpy(buffer_rgb, d_buffer_rgb, h * wi * sizeof(rgb), cudaMemcpyDeviceToHost);
  cudaMemcpy(Z, d_Z, h * wi * sizeof(float), cudaMemcpyDeviceToHost);

  if (preview_flag)
  {

    // unsigned char imageArray[1920 * 1200];
    //   // Fill imageArray with your data using loops...
    //   for (int y = 0; y < 1200; y++) {
    //       for (int x = 0; x < 1920; x++) {
    //           // Calculate the index of the current pixel
    //           int index = y * 1920 + x;
    //           // Assign a value to the pixel
    //           imageArray[index] = (unsigned char)Z[y * wi + x]; // Example pixel value calculation
    //       }
    //   }
    //   // Calculate the total size of the image array
    //   size_t arraySize = 1920 * 1200 * sizeof(unsigned char);
    //   // Save the image array as a binary file
    //   saveImageArrayAsBinary("image_array.bin", imageArray, arraySize);
  }

  cudaFree(d_Z);
  cudaFree(d_lookup_table);
  cudaFree(d_buffer_rgb);
#endif
  free(Z);
}

void colormap_avg_img(int num_imgs)
{
  uint8_t *d_lookup_table;
  rgb *d_buffer_rgb;
  float *d_avg_Z;

  cudaMalloc((void **)&d_avg_Z, sizeof(float) * h * wi);

  buffer_rgb = (rgb *)malloc(wi * h * sizeof(rgb));
  cudaMalloc(&d_lookup_table, 256 * 3 * sizeof(uint8_t));
  cudaMalloc(&d_buffer_rgb, h * wi * sizeof(rgb));

  cudaMemcpy(d_avg_Z, avg_Z, h * wi * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(d_lookup_table, lookup_table, 256 * 3 * sizeof(uint8_t), cudaMemcpyHostToDevice);

  dim3 block_size(32, 32);
  dim3 grid_size((h + block_size.x - 1) / block_size.x, (wi + block_size.y - 1) / block_size.y);
  compute_avg_Z<<<grid_size, block_size>>>(d_avg_Z, d_lookup_table, d_buffer_rgb, num_imgs, std_max, cmap_min_val, cmap_max_val);
  cudaMemcpy(buffer_rgb, d_buffer_rgb, h * wi * sizeof(rgb), cudaMemcpyDeviceToHost);
  cudaMemcpy(avg_Z, d_avg_Z, h * wi * sizeof(float), cudaMemcpyDeviceToHost);

  cudaFree(d_avg_Z);
  cudaFree(d_lookup_table);
  cudaFree(d_buffer_rgb);
}