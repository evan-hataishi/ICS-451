#include <stdio.h>
#include <string.h>

int main()
{
  unsigned char buffer[1500];
  printf("Length: %ld\n", strlen(buffer));
  FILE * fileptr;
  FILE * fp;
  fp = fopen("res.jpg", "wb");

  fileptr = fopen("image.jpg", "rb");

  while (fread(buffer, 1, 1500, fileptr) > 0) {
    //printf("Length: %ld\n", strlen(buffer));
    if (fwrite(buffer, 1, 1500, fp) != 1500) {
      printf("error\n");
    }
    memset(buffer, 0, 1500);
  }

  fclose(fileptr);
  fclose(fp);
}
