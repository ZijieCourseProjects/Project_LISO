//
// Created by Eric Zhao on 13/4/2022.
//

#include <stdlib.h>
#include <stdio.h>
#include "../include/util.h"
#include "string.h"

//get file name from filepath

char *get_filename(char *filepath) {
  char *filename = strrchr(filepath, '/');
  if (filename == NULL) {
    filename = filepath;
  } else {
    filename++;
  }
  return filename;
}

char *itoa(unsigned num) {
  char *str = malloc(sizeof(char) * 10);
  sprintf(str, "%u", num);
  return str;
}

int getFileType(char *filePath) {
  char *fileName = get_filename(filePath);
  char *extension = strrchr(fileName, '.');
  if (extension == NULL) {
    return 0;
  }
  extension++;
  if (strcmp(extension, "c") == 0) {
    return 1;
  } else if (strcmp(extension, "cpp") == 0) {
    return 2;
  } else if (strcmp(extension, "java") == 0) {
    return 3;
  } else if (strcmp(extension, "py") == 0) {
    return 0;
  } else if (strcmp(extension, "txt") == 0) {
    return 5;
  } else if (strcmp(extension, "html") == 0) {
    return 6;
  } else if (strcmp(extension, "css") == 0) {
    return 7;
  } else if (strcmp(extension, "js") == 0) {
    return 8;
  } else if (strcmp(extension, "php") == 0) {
    return 9;
  }
  return -1;
}

char *my_strcat(char *dest, char *src) {
  if(src == NULL) {
    return dest;
  }
  return strcat(dest, src);
}
