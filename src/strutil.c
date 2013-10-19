int chunks(char* data, const char delim, int chunkSize, void (*f)(char*, int)) {
  char* begin = data;
  char* cur = data;

  while (*begin != '\0') {
    int count = 0;
    for (cur = begin; *cur != '\0' && count < chunkSize; ++cur) {
      if (*cur == delim)
        count++;
    }
    if (*cur == '\0')
      count++; // Last item.

    f(begin, count);
    begin = cur;
  }
}
