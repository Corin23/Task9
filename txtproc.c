#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
                                  void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if (!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

int *readURL(const char *URL) {
  CURL *curl_handle;
  CURLcode res;
  int c;
  int *retArrCurl = malloc(128 * sizeof(int));
  struct MemoryStruct chunk;

  chunk.memory = malloc(1);
  chunk.size = 0;

  curl_global_init(CURL_GLOBAL_ALL);

  curl_handle = curl_easy_init();

  curl_easy_setopt(curl_handle, CURLOPT_URL, URL);

  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  res = curl_easy_perform(curl_handle);

  if (res != CURLE_OK) {
    // fprintf(stderr, "curl_easy_perform() failed: %s\n",
    //       curl_easy_strerror(res));
    retArrCurl = NULL;
  } else {
    for (int i = 0; i < chunk.size; i++) {
      c = chunk.memory[i];
      retArrCurl[c]++;
    }
  }

  curl_easy_cleanup(curl_handle);

  free(chunk.memory);

  curl_global_cleanup();

  return retArrCurl;
}

static int *countChars(const char *input) {
  int c;
  int *retArr = malloc(128 * sizeof(int));

  FILE *file;
  file = fopen(input, "r");

  if (file != NULL) {
    while ((c = getc(file)) != EOF) {
      retArr[c]++;
    }
    fclose(file);
  }

  else if (readURL(input) != NULL) {
    retArr = readURL(input);
  } else {
    for (int i = 0; i < strlen(input); i++) {
      c = input[i];
      retArr[c]++;
    }
  }
  return retArr;
  free(retArr);
}

static float getPercentage(int *input1_arr, int *input2_arr) {
  int counter = 0, length = 0;
  float result;

  for (int i = 0; i < 128; i++) {
    if ((input1_arr[i] && input2_arr[i]) != 0) {
      counter += input1_arr[i];
    }
    length += input1_arr[i];
  }

  return result = (float)counter / length * 100;
}

int main(int argc, char *argv[]) {
  float result = 1;
  int *input1, *input2;

  if (argc < 3) {
    printf("You need to enter two arguments: input1 and input2\n");
    exit(EXIT_FAILURE);
  }

  if (argc > 3) {
    printf("Too many arguments! You only need 2: input1 and input2.\n");
    exit(EXIT_FAILURE);
  }

  input1 = countChars(argv[1]);
  input2 = countChars(argv[2]);

  printf("%s %s\n", argv[1], argv[2]);

  for (int i = 0; i < 128; i++) {
    printf("input1: %d %d\n", i, input1[i]);
  }
  for (int i = 0; i < 128; i++) {
    printf("input2: %d %d\n", i, input2[i]);
  }
  result = getPercentage(input1, input2);
  printf("%.2f\n", result);
}