// dispatch

template <typename T, T t> struct dispatch_helper;

template <typename T, typename... Args>
using member_callback_t = int (T::*)(void *, Args...);

//

template <typename T, typename ... Args, member_callback_t<T, Args...> target_func>
struct dispatch_helper<member_callback_t<T, Args...>, target_func> {
  static int dispatch(void * L) {
    return 0;
  }
};

#define DISPATCH(F) &dispatch_helper<decltype(F), (F)>::dispatch

//

struct test {
  int callback_one(void *, int) {
    return 0;
  }
};

typedef int (*desired_sig_t)(void *);

int main() {
  desired_sig_t ptr{DISPATCH(&test::callback_one)};
}
