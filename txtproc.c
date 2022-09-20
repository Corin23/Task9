#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *IS_URL = "https";
const char *IS_FILE = "txt";

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

static void countURL(int *urlbuf, const char *URL) {
  CURL *curl_handle;
  CURLcode res;
  int c;
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
    //     curl_easy_strerror(res));
    urlbuf = NULL;
  } else {
    for (int i = 0; i < chunk.size; i++) {
      c = chunk.memory[i];
      if (c >= 0 && c<= 128)
        urlbuf[c]++;
    }
  }

  curl_easy_cleanup(curl_handle);

  free(chunk.memory);

  curl_global_cleanup();
}

static void countFile(int *filebuf, const char *input) {
   int c;

  FILE *file;
  file = fopen(input, "r");

  if (!file) {
    printf("Couldn't open the file!");
    exit(EXIT_FAILURE);
  }
  
  while ((c = fgetc(file)) != EOF) {
    if(c >= 0 && c <= 128){
      filebuf[c]++;
      }
    }
    
  fclose(file);
    
}

static void countChars(int *strbuf, const char *input) {
   int ch;
   int c = 0;

  while((ch = input[c]) != '\0'){
    strbuf[ch]++;
    c++;
  }
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
  static float result = 1;
  static int input1[128] = {0}, input2[128] = {0};
  const char *firstarg, *secondarg, *exten1, *exten2;

  if (argc < 3) {
    printf("You need to enter two arguments: input1 and input2\n");
    exit(EXIT_FAILURE);
  }

  if (argc > 3) {
    printf("Too many arguments! You only need 2: input1 and input2.\n");
    exit(EXIT_FAILURE);
  }

  firstarg = argv[1];
  secondarg = argv[2];

  exten1 = &firstarg[strlen(firstarg) - 3];
  exten2 = &secondarg[strlen(secondarg) - 3];

  if (strncmp(IS_URL, firstarg, 5) == 0){
    countURL(input1, firstarg);
  } else if (strncmp(IS_FILE, exten1, 3) == 0) {
    countFile(input1, firstarg);
  } else {
    countChars(input1, firstarg);
  }

  if (strncmp(IS_URL, secondarg, 5) == 0) {
    countURL(input2, secondarg);
  } else if (strncmp(IS_FILE, exten2, 3) == 0) {
    countFile(input2, secondarg);
  } else {
    countChars(input2, secondarg);
  }

  result = getPercentage(input1, input2);
  printf("%.2f\n", result);
}
