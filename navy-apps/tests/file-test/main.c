#include <assert.h>
#include <stdio.h>

int main() {
  FILE *fp = fopen("/share/files/num", "r+");
  assert(fp);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  assert(size == 5000);

  fseek(fp, 500 * 5, SEEK_SET);
  printf("After fseek to 2500, ftell = %ld\n", ftell(fp));
  
  // Test raw reading
  char buffer[20];
  size_t bytes_read = fread(buffer, 1, 10, fp);
  printf("fread returned %u bytes\n", (unsigned)bytes_read);
  if (bytes_read > 0) {
    buffer[bytes_read] = 0;
    printf("Raw data: '");
    for (size_t j = 0; j < bytes_read; j++) {
      if (buffer[j] >= 32 && buffer[j] <= 126) printf("%c", buffer[j]);
      else printf("\\x%02x", (unsigned char)buffer[j]);
    }
    printf("'\n");
  }
  
  fseek(fp, 500 * 5, SEEK_SET);
  int i, n;
  for (i = 500; i < 1000; i++) {
    int result = fscanf(fp, "%d", &n);
    printf("fscanf result=%d, n=%d, expected=%d\n", result, n, i + 1);
    if (i >= 502) break;  // Only check first few
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i++) {
    fprintf(fp, "%4d\n", i + 1 + 1000);
  }

  for (i = 500; i < 1000; i++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1 + 1000);
  }

  fclose(fp);

  printf("PASS!!!\n");

  return 0;
}
