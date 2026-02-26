
namespace sgpp {
namespace combigrid {
namespace tools {

int ceil(const int a, const int b) { return a / b + (a % b != 0); }

unsigned int ceil(const unsigned int a, const unsigned int b) { return a / b + (a % b != 0); }

unsigned long ceil(const unsigned long a, const unsigned long b) { return a / b + (a % b != 0); }

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp