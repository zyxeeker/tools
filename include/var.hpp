/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-05-27 10:33:57
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-05-29 18:11:04
 * @Description: 
 */

#ifndef VAR_HPP_
#define VAR_HPP_

#include <tuple>
#include <atomic>
#include <string>
#include <utility>

#include <nlohmann/json.hpp>

namespace tools {
namespace var {

class Base {
 public:
  virtual ~Base() = default;
  /**
   * 解释Json对象
   */
  virtual void FromJson(const nlohmann::json& json) = 0;
  /**
   * 转换为Json对象
   */
  virtual nlohmann::json ToJson() = 0;
  /**
   * 序列化, 调用ToJson进行转换, 再通过库函数转换为字符串
   * @return 返回Json字符串
   */
  virtual std::string ToJsonString() {
    return nlohmann::to_string(ToJson());
  }
  /**
   * 反序列化, 调用库进行解析, 再通过调用FromJson进行转换
   * @param json_str Json字符串
   * @throw 解析失败抛出nlohmann::json::parse_error
   * @return 返回是否成功反序列化
   */
  virtual void FromJsonString(const std::string& json_str) {
    FromJson(nlohmann::json::parse(json_str));
  }
};

/**
 * var::Base的监听模板类基类
 */
class ObserverBaseObj {
 public:
  /**
   * 监听回调子模块
   */
  class Listener {
   public:
    using Ptr = std::shared_ptr<Listener>;
    using WPtr = std::weak_ptr<Listener>;
    using Id = size_t;
    using Callback = std::function<void(const var::Base*, const var::Base*)>;
   public:
    explicit Listener(Id id, Callback cb)
        : id_(id), cb_(std::move(cb)) {}
    ~Listener() = default;
    /**
     * 重写()操作符用于在更新时调用所存储的回调
     * @param old_val 旧值
     * @param new_val 新值
     */
    inline void operator()(const var::Base* old_val, const var::Base* new_val) {
      if (cb_) cb_(old_val, new_val);
    }
    /**
     * 返回自身Id
     */
    inline Id id() const { return id_; }
   private:
    // 监听者id
    Id id_;
    // 所储存的回调用于更新时调用
    Callback cb_;
  };
 public:
  explicit ObserverBaseObj(std::shared_ptr<var::Base> var)
      : var_(std::move(var)) {}
  virtual ~ObserverBaseObj() = default;
  /**
   * 将Json对象转换接口作为虚函数的原因如下:
   * 1. 用于对接配置管理器的Json值更新, 需要继承的子类去确定对象并实现
   * 2. 用于其他调用者可能会传入Json值进行更新
   * @param json json值
   */
  virtual void Update(const nlohmann::json& json) = 0;
 protected:
  /**
   * 添加监听者回调
   * @param id 监听者ID
   * @param cb 回调
   */
  inline void AddListener(const Listener::Ptr& listener) {
    listeners_.insert({listener->id(), listener});
  }
  /**
   * 移除指定的监听者回调
   * @param id 监听者ID
   */
  inline void RemoveListener(const Listener::Ptr& listener) {
    listeners_.erase(listener->id());
  }
  /**
   * 通知所有已注册回调进行更新
   * @param new_val 新值
   */
  inline void NotifyChange(const var::Base* new_val) {
    for (auto& i : listeners_) {
      if (i.second.expired()) continue;
      (*i.second.lock())(var_.get(), new_val);
    }
  }
  /**
   * 返回保存的var
   */
  inline std::shared_ptr<var::Base> var() const {
    return var_;
  }
 private:
  // 存储以var::Base为基类的值
  std::shared_ptr<var::Base> var_;
  // 存储监听者回调对象
  std::map<Listener::Id, Listener::WPtr> listeners_;
};

/**
 * 适用于var::Base的监听者模板类设计
 */
template<class T>
class ObserverObj : protected ObserverBaseObj {
 public:
  template <typename ...Args>
  explicit ObserverObj(Args&&... args)
      : ObserverBaseObj(std::make_shared<T>(std::forward<Args>(args)...)) {}
  explicit ObserverObj(const T& val)
      : ObserverBaseObj(std::make_shared<T>(val)) {}
  ~ObserverObj() = default;
  /**
   * 用于对应类型值进行更新
   */
  void Update(const T& val) {
    auto base = dynamic_cast<const var::Base*>(&val);
    if (!base) return;
    UpdateImpl(base);
  }
  /**
   * 使用Json对象来进行更新
   */
  void Update(const nlohmann::json& json) override {
    auto _val = std::dynamic_pointer_cast<var::Base>(std::make_shared<T>());
    _val->FromJson(json);
    UpdateImpl(_val.get());
  }
  /**
   * 注册回调函数, 用于普通函数
   * @return 返回监听对象指针, 交给调用者控制生命周期
   */
  inline Listener::Ptr RegisterListener(void (*func)(const T*, const T*)) {
    auto cb = [func](const var::Base* old_val, const var::Base* new_val) {
      (*func)(dynamic_cast<const T*>(old_val), dynamic_cast<const T*>(new_val));
    };
    return RegisterListenerImpl(cb);
  }
  /**
   * 注册回调函数, 用于类成员函数
   * @return 返回监听对象指针, 交给调用者控制生命周期
   */
  template <class Class>
  inline Listener::Ptr RegisterListener(void (Class::*func)(const T*, const T*), Class *c) {
    auto cb = [c, func](const var::Base* old_val, const var::Base* new_val) {
      (c->*func)(dynamic_cast<const T*>(old_val), dynamic_cast<const T*>(new_val));
    };
    return RegisterListenerImpl(cb);
  }
  /**
   * 反注册监听回调
   * @param listener 监听回调对象
   */
  inline void UnregisterListener(const Listener::Ptr& listener) {
    this->RemoveListener(listener);
  }
  /**
   * 重写=操作符, 便于调用
   */
  ObserverObj<T>& operator=(const T& val) {
    Update(val);
    return *this;
  }
  ObserverObj<T>& operator=(const nlohmann::json& json) {
    Update(json);
    return *this;
  }
 private:
  /**
   * 通知发生回调, 并赋予新值
   */
  void UpdateImpl(const var::Base* val) {
    this->NotifyChange(val);
    *(std::dynamic_pointer_cast<T>(this->var()).get()) = *(dynamic_cast<const T*>(val));
  }
  /**
   * 注册监听实现
   */
  Listener::Ptr RegisterListenerImpl(const Listener::Callback& cb) {
    static std::atomic_uint64_t id = 0;
    auto ptr = std::make_shared<Listener>(id++, cb);
    this->AddListener(ptr);
    return ptr;
  }
};

} // var
} // tools

#endif //VAR_HPP_
