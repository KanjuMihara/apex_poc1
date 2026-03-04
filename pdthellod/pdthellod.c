#include <android/log.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define LOG_TAG "pdthellod"

static const char* kFileAPath = "/apex/com.oem.pdthellod/etc/fileA";

int main() {
  while (1) {
    int fd = open(kFileAPath, O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
      __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                          "open(%s) failed: errno=%d (%s)", kFileAPath, errno,
                          strerror(errno));
      sleep(5);
      continue;
    }

    char buf[64];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n < 0) {
      __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                          "read(%s) failed: errno=%d (%s)", kFileAPath, errno,
                          strerror(errno));
    } else {
      buf[n] = '\0';
      __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "read(%s) ok: %s",
                          kFileAPath, buf);
    }

    close(fd);
    sleep(5);
  }
  return 0;
}
