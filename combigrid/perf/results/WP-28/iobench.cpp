#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
using Clock = std::chrono::steady_clock;
static double secs(Clock::time_point a) { return std::chrono::duration<double>(Clock::now() - a).count(); }
int main(int argc, char** argv) {
  std::string dir = argv[1];
  for (size_t recBytes : {56, 176}) {
    for (size_t n : {100000, 1000000}) {
      for (int sync = 0; sync < 2; sync++) {
        std::string path = dir + "/bench.bin";
        int fd = open(path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
        std::vector<char> buf(recBytes * 128 * 64, 1);  // 64 chunks per write call
        size_t total = recBytes * n;
        auto t = Clock::now();
        for (size_t off = 0; off < total; off += buf.size()) {
          size_t len = std::min(buf.size(), total - off);
          if (write(fd, buf.data(), len) != (ssize_t)len) abort();
        }
        char hdr[64] = {};
        pwrite(fd, hdr, 64, 0);
        if (sync) fdatasync(fd);
        double s = secs(t);
        close(fd);
        printf("bulk  rec=%3zuB n=%7zu sync=%d: %8.1f ms  %6.1f ns/rec  %7.0f MB/s\n", recBytes, n, sync,
               s * 1e3, s * 1e9 / n, total / s / 1e6);
        unlink(path.c_str());
        sleep(1);
      }
    }
  }
  // small flushes: 1000 op calls each appending k records + header update
  for (size_t k : {0, 10, 1000}) {
    for (int sync = 0; sync < 2; sync++) {
      std::string path = dir + "/bench.bin";
      int fd = open(path.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0644);
      std::vector<char> buf(56 * k + 1, 1);
      int calls = sync ? 200 : 2000;
      auto t = Clock::now();
      for (int c = 0; c < calls; c++) {
        if (k) { write(fd, buf.data(), 56 * k); char hdr[8] = {}; pwrite(fd, hdr, 8, 0); if (sync) fdatasync(fd); }
      }
      double s = secs(t);
      close(fd); unlink(path.c_str());
      printf("small k=%4zu sync=%d: %8.2f us/call\n", k, sync, s * 1e6 / calls);
    }
  }
  // temp-file creation
  {
    auto t = Clock::now();
    for (int i = 0; i < 1000; i++) { int fd = open(dir.c_str(), O_TMPFILE | O_RDWR, 0600); if (fd < 0) { perror("O_TMPFILE"); break; } close(fd); }
    printf("O_TMPFILE open+close: %.2f us\n", secs(t) * 1e3);
    t = Clock::now();
    for (int i = 0; i < 1000; i++) { std::string p = dir + "/sgXXXXXX"; int fd = mkstemp(p.data()); unlink(p.c_str()); close(fd); }
    printf("mkstemp+unlink+close: %.2f us\n", secs(t) * 1e3);
    t = Clock::now();
    std::string path = dir + "/lock.bin"; int fd = open(path.c_str(), O_CREAT|O_RDWR, 0644);
    for (int i = 0; i < 1000; i++) { flock(fd, LOCK_EX|LOCK_NB); flock(fd, LOCK_UN);} 
    printf("flock pair: %.2f us\n", secs(t) * 1e3); close(fd); unlink(path.c_str());
  }
}
