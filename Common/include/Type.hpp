#pragma once

#include <concepts>
namespace MEngine
{
template <typename T, typename Func>
concept GetterFunc = std::invocable<Func, const T &> && std::same_as<std::invoke_result_t<Func, const T &>, T>;
template <typename T, typename Func>
concept SetterFunc = std::invocable<Func, T &>;

template <typename T> class Property
{
  private:
    T mValue;
    

  public:
    template <GetterFunc<T> TGetter, SetterFunc<T> TSetter>
    Property(TGetter getter, TSetter setter) : mGetter(getter), mSetter(setter)
    {
    }
};

} // namespace MEngine