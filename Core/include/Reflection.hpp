// #pragma once
// #include <cstddef>
// #include <functional>
// #include <string>
// #include <typeinfo>
// #include <unordered_map>

// namespace MEngine
// {
// struct FieldInfo
// {
//     std::size_t id;
//     std::string name;
//     std::type_info const *type;
//     size_t offset;
// };
// struct MethodInfo
// {
//     std::size_t id;
//     std::string name;
//     void *functionPtr;
// };
// struct ClassInfo
// {
//     std::size_t id;
//     std::string name;
//     std::type_info const *type;
//     std::vector<FieldInfo> fields;
//     std::vector<MethodInfo> methods;
//     ClassInfo *baseClass;
//     std::function<void *(void *)> constructor;
// };
// class Reflection
// {
//   private:
//     static std::unordered_map<int, ClassInfo> mClasses;

//   public:
//     static void RegisterClass(const ClassInfo &classInfo)
//     {
//         mClasses[classInfo.id] = classInfo;
//     }
//     static ClassInfo &GetClass(int id)
//     {
//         auto it = mClasses.find(id);
//         if (it != mClasses.end())
//         {
//             return it->second;
//         }
//         throw std::runtime_error("Class not found");
//     }
//     static void UnregisterClass(int id)
//     {
//         mClasses.erase(id);
//     }
// };

// } // namespace MEngine