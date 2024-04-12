/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-11 10:33:36
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-12 18:08:30
 * @Description: 
 */

#if _WIN32
#include <Windows.h>
#elif __unix__
#include <sys/syscall.h>
#endif

#include <thread>
#include <async.h>

using namespace tools::async;

class Thread::Impl {
 public:
  explicit Impl(const Obj &obj)
      : func_(obj.func),
        name_(obj.name), id_(0),
        running_(false) {
    thread_ = std::thread(&Impl::Run, this);
  }
  ~Impl() = default;

  void Run() {
    SetName(name_);
    GetId(id_);
    running_ = true;
    if (func_) func_();
    running_ = false;
    id_ = 0;
  }

  inline static bool GetId(Id& id) {
#if _WIN32
    id = static_cast<Id>(::GetCurrentThreadId());
#elif __unix__
    id = static_cast<Id>(syscall(SYS_gettid));
#endif
    return true;
  }

  inline static void SetName(const std::string &name) {
#if _WIN32
    // TODO: ?
#elif __unix__
    pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
#endif
  }

  inline void Join() {
    if (thread_.joinable())
      thread_.join();
  }
  inline void Detach() {
    if (thread_.joinable())
      thread_.detach();
  }

  inline const std::string &name() const {
    return name_;
  }
  inline Id id() const {
    return id_;
  }
  inline bool running() const {
    return running_;
  }

 private:
  const std::function<void()> func_;
  const std::string name_;
  Id id_;
  bool running_;
  std::thread thread_;
};

Thread::Thread(const struct Obj &obj)
    : impl_(new Impl(obj)) {}

Thread::~Thread() = default;

void Thread::Join() {
  impl_->Join();
}

void Thread::Detach() {
  impl_->Detach();
}

bool Thread::IsRunning() {
  return impl_->running();
}

Thread::Id Thread::GetId() {
  return impl_->id();
}

std::string Thread::GetName() {
  return impl_->name();
}

