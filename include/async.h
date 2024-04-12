/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-10 18:42:26
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-12 18:08:38
 * @Description: 
 */

#ifndef ASYNC_H_
#define ASYNC_H_

#include <memory>
#include <string>
#include <future>
#include <functional>
#include <utility>

namespace tools {
namespace async {

class Thread {
  using Func = std::function<void()>;
  struct Obj {
    std::string name;
    Func func;
  };
 public:
#if _WIN32
  using Id = uint64_t;
#elif __unix__
  using Id = pid_t;
#endif
 public:
  template<typename Func, typename ...Args>
  Thread(const std::string &name, Func &func, Args &&... args)
      : Thread({name, [&] { func(std::forward<Args>(args)...); }}) {}
  template<class Class, typename ...Args>
  Thread(const std::string &name, void (Class::*func)(Args...), Class *c, Args &&... args)
      : Thread({name, [&] { (c->*func)(std::forward<Args>(args)...); }}) {}
  ~Thread();

  void Join();
  void Detach();
  bool IsRunning();
  Id GetId();
  std::string GetName();

 private:
  explicit Thread(const struct Obj &obj);
  class Impl;
  std::unique_ptr<Impl> impl_;
};

class Task {
  class ObjImpl {
   public:
    using Ptr = std::shared_ptr<ObjImpl>;
    using Func = std::function<void()>;
    ObjImpl() = default;
    ~ObjImpl() = default;
    inline const Func &func() const { return func_; }
    inline void set_func(Func &&func) { func_ = std::forward<Func>(func); }
   private:
    Func func_;
  };

 public:
  template<typename T>
  class Obj : public ObjImpl {
   public:
    explicit Obj(std::string name, std::packaged_task<T()> &&pkg)
        : name_(std::move(name)),
          pkg_(std::forward<std::packaged_task<T()> >(pkg)) {
      set_func([&] { (pkg_)(); });
    }
    ~Obj() = default;
   private:
    const std::string name_;
    std::packaged_task<T()> pkg_;
  };

 public:
  template<typename Func, typename ...Args>
  static auto Create(const std::string &name, Func &func, Args &&... args) {
    using Ret = decltype(func(args...));
    auto pkg = std::make_shared<Obj<Ret> >(
        name,
        std::packaged_task<Ret()>([&] { return func(std::forward<Args>(args)...); }));
    PushTask(std::dynamic_pointer_cast<ObjImpl>(pkg));
    return std::weak_ptr<Obj<Ret> >(pkg);
  }
  template<typename Ret, class Class, typename ...Args>
  static auto Create(const std::string &name, Ret Class::*func, Class *c, Args &&... args) {
    using _Ret = decltype((c->*func)(args...));
    auto pkg = std::make_shared<Obj<_Ret> >(
        name,
        std::packaged_task<_Ret()>([&] { return (c->*func)(std::forward<Args>(args)...); }));
    PushTask(std::dynamic_pointer_cast<ObjImpl>(pkg));
    return std::weak_ptr<Obj<_Ret> >(pkg);
  }
 private:
  static void PushTask(const ObjImpl::Ptr& obj) {
    // TODO: Impl
  }
};

} // async
} // tools

#endif //ASYNC_H_
