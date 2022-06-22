#ifndef SRCS_UTILS_RESULT_HPP
#define SRCS_UTILS_RESULT_HPP

template <typename T>
struct Result {
  bool is_err_;
  T    object_;

  Result(bool is_err = true, const T &object = T())
      : is_err_(is_err)
      , object_(object) {}
};

template <typename T>
struct Error : public Result<T> {
  Error() {}
};

template <typename T>
struct Ok : public Result<T> {
  Ok(const T &object)
      : Result<T>(false, object) {}
};

#endif /* SRCS_UTILS_RESULT_HPP */
