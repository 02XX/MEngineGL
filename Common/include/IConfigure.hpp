#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;
namespace MEngine
{
class IConfigure
{
  public:
    virtual ~IConfigure() = default;
    virtual const Json &GetJson() const = 0;
};
} // namespace MEngine
