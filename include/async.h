/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-10 18:42:26
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-16 16:19:32
 * @Description: 
 */

#ifndef ASYNC_H_
#define ASYNC_H_

#if _WIN32
#include <Windows.h>
#elif __unix__
#include <sys/syscall.h>
#endif

#include <chrono>
#include <memory>
#include <queue>
#include <memory>
#include <string>
#include <future>
#include <thread>
#include <functional>
#include <utility>

namespace tools {
namespace async {

#if !_HAS_CXX20
// TODO: Impl More...
class Semaphore {
 public:
  explicit Semaphore(size_t count) : count_(count) {}
  void signal() {
    std::unique_lock<std::mutex> lk(mutex_);
    ++count_;
    if (count_ <= 0)
      cv_.notify_one();
  }
  void wait() {
    std::unique_lock<std::mutex> lk(mutex_);
    --count_;
    if (count_ < 0)
      cv_.wait(lk);
  }

 private:
  std::mutex mutex_;
  std::condition_variable cv_;
  size_t count_;
};
#endif // !CXX20

class Thread {
 public:
#if _WIN32
  using Id = uint64_t;
#elif __unix__
  using Id = pid_t;
#endif
  using Func = std::function<void()>;

  struct Helper {
    inline static bool GetCurThreadId(Id& id) {
#if _WIN32
      id = static_cast<Id>(::GetCurrentThreadId());
#elif __unix__
      id = static_cast<Id>(syscall(SYS_gettid));
#endif
      return true;
    }

    inline static void SetCurThreadName(const std::string &name) {
#if _WIN32
      // TODO: ?
#elif __unix__
      pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
#endif
    }
  };

 public:
  template<typename Func, typename ...Args>
  Thread(const std::string& name, Func* func, Args&&... args)
      : Thread(name, [func, &args...]{ (*func)(std::forward<Args>(args)...); }) {}
  template<class Class, typename ...Args>
  Thread(const std::string& name, void (Class::*func)(Args...), Class *c, Args &&... args)
      : Thread(name, [c, func, &args...]{ (c->*func)(std::forward<Args>(args)...); }) {}
  Thread(Thread&& th) noexcept { *this = std::forward<Thread>(th); }
  Thread(const Thread& th) = delete;
  ~Thread() = default;

  void Join() {
    if (thread_.joinable())
      thread_.join();
  }
  void Detach() {
    if (thread_.joinable())
      thread_.detach();
  }
  Thread& operator=(Thread&& th) noexcept {
    func_ = std::move(th.func_);
    name_ = std::move(th.name_);
    id_ = th.id_;
    running_ = th.running_;
    thread_ = std::move(th.thread_);
    return *this;
  }

  bool running() const { return running_; }
  inline Id id() const { return id_; }
  inline const std::string& name() const { return name_; }

 private:
  template <typename Func>
  explicit Thread(std::string name, Func&& func)
      : name_(std::move(name)),
        func_(std::forward<Func>(func)),
        id_(0),
        running_(false) {
    thread_ = std::thread(&Thread::Run, this);
    sem_.wait();
  }
  void Run() {
    Helper::SetCurThreadName(name_);
    Helper::GetCurThreadId(id_);
    running_ = true;
    sem_.signal();
    if (func_) func_();
    running_ = false;
    id_ = 0;
  }
 private:
  std::function<void()> func_;
  std::string name_;
  Id id_{0};
  bool running_{false};
  std::thread thread_;
  Semaphore sem_{0};
};

///// TASK
class TaskBase {
 public:
  using Ptr = std::shared_ptr<TaskBase>;
  using Func = std::function<void()>;
  struct Item {
    using Ptr = std::shared_ptr<Item>;
    using WPtr = std::weak_ptr<Item>;
    Func func;
  };
  TaskBase() = default;
  ~TaskBase() = default;
 protected:
  Item::Ptr item_;
};

template <typename T>
class Task : protected TaskBase {
 public:
  template <typename Func, typename ...Args>
  Task(const std::string& name, Func* func, Args&&... args)
      : Task(name, [func, &args...]{ return (*func)(std::forward<Args>(args)...); }) {}
  template <class Class, typename ...Args>
  Task(const std::string& name, T (Class::*func)(Args...), Class *c, Args &&... args)
      : Task(name, [c, func, &args...]{ return (c->*func)(std::forward<Args>(args)...); }) {}
  inline T& Wait() const { return future_.get(); }
  inline T Wait() { return future_.get(); }
  template <class Rep, class Period>
  bool WaitFor(std::chrono::duration<Rep, Period>& time, T& result) const {
    if (future_.wait_for(time) != std::future_status::ready) {
      return false;
    }
    result = future_.get();
    return true;
  }
 private:
  template <typename Func>
  explicit Task(std::string name, Func&& func)
      : name_(std::move(name)),
        pkg_(std::forward<Func>(func)),
        future_(pkg_.get_future().share()) {
    this->item_ = std::make_shared<Item>(Item{ [&]{ pkg_(); }});
  }
 private:
  std::string name_;
  std::packaged_task<T()> pkg_;
  std::shared_future<T> future_;
};

template <>
class Task<void> : protected TaskBase {
 public:
  template <typename Func, typename ...Args>
  Task(const std::string& name, Func* func, Args&&... args)
      : Task(name, [func, &args...]{ return (*func)(std::forward<Args>(args)...); }) {}
  template <class Class, typename ...Args>
  Task(const std::string& name, void (Class::*func)(Args...), Class *c, Args &&... args)
      : Task(name, [c, func, &args...]{ return (c->*func)(std::forward<Args>(args)...); }) {}
  inline void Wait() { return future_.get(); }
  template <class Rep, class Period>
  bool WaitFor(std::chrono::duration<Rep, Period>& time) const {
    if (future_.wait_for(time) != std::future_status::ready) {
      return false;
    }
    future_.get();
    return true;
  }
 private:
  template <typename Func>
  explicit Task(std::string name, Func&& func)
      : name_(std::move(name)),
        pkg_(std::forward<Func>(func)),
        future_(pkg_.get_future().share()) {
    this->item_ = std::make_shared<Item>(Item{ [&]{ pkg_(); }});
  }
 private:
  std::string name_;
  std::packaged_task<void()> pkg_;
  std::shared_future<void> future_;
};

template <typename Func, typename ...Args>
auto MakeTask(const std::string&& name, Func&& func, Args&&... args) {
  using Ret = decltype((*func)(args...));
  return Task<Ret>{ name, func, std::forward<Args>(args)... };
}

template <typename Ret, class Class, typename ...Args>
auto MakeTask(const std::string&& name, Ret (Class::*func)(Args...), Class *c, Args &&... args) {
  return Task<Ret>{ name, func, c, std::forward<Args>(args)... };
}

class PoolBase {
 public:
  PoolBase(std::string name, size_t size)
      : name_(std::move(name)), size_(size), started_(false) {}
  ~PoolBase() = default;

  bool Init() {
    std::lock_guard<std::mutex> lk(mutex_);
    if (started_) return false;
    started_ = true;
    workers_.clear();
    for (auto i = 0; i < size_; i++) {
      workers_.emplace_back(new Thread(name_ + std::to_string(i), &PoolBase::Worker, this));
    }
    return true;
  }

  void Finalized() {
    {
      std::lock_guard<std::mutex> l(mutex_);
      if (!started_) return;
      started_ = false;
      cv_.notify_all();
    }

    for (auto &i : workers_) {
      i->Join();
    }
  }

  inline const std::string& name() const { return name_; }
  inline size_t size() const { return size_; }

 protected:
  virtual void Worker() = 0;

 protected:
  bool started_;
  std::mutex mutex_;
  std::condition_variable cv_;

 private:
  std::string name_;
  size_t size_;
  std::vector<std::shared_ptr<Thread> > workers_;
};

template<typename T>
class ThreadPool : protected PoolBase {
 public:
  explicit ThreadPool(const std::string& name, size_t size,
                      const std::function<void(const T&)>& func)
      : PoolBase(name, size),
        func_(func) {
    this->Init();
  }
  ~ThreadPool() { this->Finalized(); }

  void Add(const T& data) {
    std::lock_guard<std::mutex> lk(this->mutex_);
    if (!this->started_) return;
    queue_.push(data);
    this->cv_.notify_one();
  }

 private:
  void Worker() override {
    while (started_) {
      T data;
      {
        std::unique_lock<std::mutex> lk(this->mutex_);
        this->cv_.wait(lk, [&]{ return !(this->started_ && queue_.empty()); });
        if (!this->started_) return;
        data = queue_.front();
        queue_.pop();
      }
      if (func_) func_(data);
    }
  }

 private:
  std::function<void(const T&)> func_;
  std::queue<T> queue_;
};

class TaskPool : protected PoolBase {
  class TaskHelper : protected TaskBase {
   public:
    TaskHelper() = default;
    ~TaskHelper() = default;
    inline const Item::Ptr& item() const { return this->item_; }
  };
 public:
  explicit TaskPool(const std::string& name, size_t size = 3)
      : PoolBase(name, size) {
    this->Init();
  }
  ~TaskPool() { this->Finalized(); }

  template <typename T>
  void Add(Task<T>* task) {
    std::lock_guard<std::mutex> lk(this->mutex_);
    if (!this->started_) return;
    auto _task = reinterpret_cast<TaskHelper*>(task);
    tasks_.push(_task->item());
    this->cv_.notify_one();
  }

 private:
  void Worker() override {
    TaskBase::Item::WPtr task;
    {
      std::unique_lock<std::mutex> lk(this->mutex_);
      cv_.wait(lk, [&]{ return !(this->started_ && tasks_.empty()); });
      if (!this->started_) return;
      task = tasks_.front();
      tasks_.pop();
    }
    if (!task.expired()) {
      auto task_ptr = task.lock();
      if (task_ptr->func)
        (task_ptr->func)();
    }
  }
 private:
  std::queue<TaskBase::Item::WPtr> tasks_;
};

} // async
} // tools

#endif //ASYNC_H_
