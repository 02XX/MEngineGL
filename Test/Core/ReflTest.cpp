#include "gtest/gtest.h"
#include <entt/core/fwd.hpp>
#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>
#include <gtest/gtest.h>
#include <memory>
#include <refl.hpp>
#include <string>
#include <tuple>

/*
    名称（英文）	名称（中文）	说明
refl	顶级命名空间	refl-cpp的顶级命名空间，包含核心命名空间、类及函数
attr	内置属性命名空间	定义内置属性的命名空间
usage	属性约束命名空间	包含适用于refl-cpp属性的约束条件
descriptor	反射描述符命名空间	包含基础反射原语及操作这些原语的函数
member	成员类型标记命名空间	包含表示不同类型可反射成员的标签类型
runtime	运行时工具命名空间	包含可能产生运行时开销的工具（如代理、调试、调用等）
trait	类型特性命名空间	提供与refl-cpp相关的类型级操作
util	工具类型命名空间	包含处理工具类型的实用函数和类型
any	通用属性基类	继承此类的属性类型仅可用于REFL_TYPE、REFL_FIELD或REFL_FUNC之一
field	字段属性基类	继承此类的属性类型仅可用于REFL_FIELD()
function	函数属性基类	继承此类的属性类型仅可用于REFL_FUNC()
member	成员属性基类	继承此类的属性类型仅可用于REFL_FUNC或REFL_FIELD
type	类型属性基类	继承此类的属性类型仅可用于REFL_TYPE()
base_types	基类型指定类	用于指定目标类型的基类型
debug	调试显示类	指定类型在调试上下文中的显示方式
property	属性装饰类	用于标记作为属性的函数
field_descriptor	字段描述符	表示反射的字段
function_descriptor	函数描述符	表示反射的函数
member_descriptor_base	成员描述符基类	成员描述符的基类型
type_descriptor	类型描述符	表示反射的类型
proxy	代理对象类	具有与目标反射函数和字段相同静态接口的代理对象
append	类型追加类	将类型追加到列表中
as_tuple	元组转换类	提供成员类型定义，将模板参数转换为std::tuple特化版本
as_type_list	类型列表转换类	提供成员类型定义，将模板参数转换为type_list
concat	列表连接类	将多个类型列表连接在一起
contains	类型包含检查类	检查类型列表中是否包含指定类型
contains_base	基类包含检查类	检查类型列表中是否包含派生自指定类型的类型
contains_instance	模板实例包含检查类	检查类型列表中是否包含指定模板的实例
filter	类型过滤类	根据谓词模板过滤类型列表
get	类型获取类	提供成员类型定义，获取类型列表中指定位置的类型
index_of	类型索引类	返回类型在列表中的索引（不存在时返回-1）
index_of_base	基类索引类	返回派生自指定类型的类型索引（不存在时返回-1）
index_of_instance	模板实例索引类	返回模板实例类型的索引（不存在时返回-1）
is_container	容器检查类	检查类型是否支持.begin()和.end()操作
is_descriptor	描述符检查类	检测类型是否为refl-cpp描述符
is_field	字段描述符检查类	检测类型是否为字段描述符
is_function	函数描述符检查类	检测类型是否为函数描述符
is_instance	模板实例检查类	检测类型是否为模板特化
is_instance_of	模板派生检查类	检测类型是否为指定模板的实例
is_member	成员描述符检查类	检测类型是否为成员描述符
is_property	属性标记检查类	检查类型是否被标记为属性
is_proxy	代理检查类	检测类型是否为代理对象
is_reflectable	可反射检查类	检查类型是否具有反射元数据
is_type	类型描述符检查类	检测类型是否为type_descriptor
map	类型映射类	根据谓词模板转换类型列表
prepend	类型前置类	将类型插入到列表开头
remove_qualifiers	限定符移除类	移除类型的所有引用和cv限定符
reverse	列表反转类	反转类型列表顺序
skip	类型跳过类	跳过类型列表前N个类型
unique	唯一列表生成类	创建仅包含源列表非重复元素的新列表
const_string	编译时字符串类	表示编译期字符串
type_list	类型列表类	表示编译期类型列表（通过可变模板参数提供）*/
using namespace entt::literals;
struct Editable : refl::attr::usage::field
{
    bool isEditable = true;
    constexpr Editable(bool isEditable = true) noexcept : isEditable(isEditable)
    {
    }
};
struct MyCustomData
{
    const char *name;
    int value;
};

struct Serializable : refl::attr::usage::member
{
};
struct Table : refl::attr::usage::type
{
    int type = 1;

    constexpr Table(int type) noexcept : type(type)
    {
    }
};
REFL_AUTO(type(Editable));
struct Student
{
  private:
    std::string name;

  public:
    int id;
    bool isMale;
    void setName(const std::string &name)
    {
        this->name = name;
    }
    std::string getName() const
    {
        return name;
    }
    Student(int id, std::string name, bool isMale) : id(id), isMale(isMale), name(name)
    {
        GTEST_LOG_(INFO) << "Student constructor";
    }
};

class Animal
{
  public:
    std::string name;
    Animal()
    {
        name = "Animal";
    }
    virtual ~Animal() = default;
    virtual void Speak() const
    {
        GTEST_LOG_(INFO) << "Animal speaks";
    }
};
class Dog : public Animal
{
  public:
    Dog()
    {
        name = "Dog";
    }
    virtual ~Dog() = default;
    virtual void Speak() const override
    {
        GTEST_LOG_(INFO) << "Dog barks";
    }
};
class Cat : public Animal
{
  public:
    Cat()
    {
        name = "Cat";
    }
    virtual ~Cat() = default;
    virtual void Speak() const override
    {
        GTEST_LOG_(INFO) << "Cat meows";
    }
};
struct DisplayName
{
    const char *name;
    constexpr DisplayName(const char *name) noexcept : name(name)
    {
    }
};
void registerMeta()
{
    entt::meta<Student>()
        .type("Player"_hs)
        .data<&Student::setName, &Student::getName>("name"_hs)
        .custom<MyCustomData>(MyCustomData{"草年末", 42})
        .data<&Student::id>("id"_hs);
    entt::meta<Animal>().type("Animal"_hs).data<&Animal::name>("name"_hs);
    entt::meta<Dog>()
        .type("Dog"_hs)
        .base<Animal>()
        .custom<DisplayName>(DisplayName{"hhhhhh"})
        .data<&Animal::name>("name"_hs);
    entt::meta<Cat>().type("Cat"_hs).base<Animal>();
}
class ReflTest : public ::testing::Test
{
  public:
  protected:
    void SetUp() override
    {
        registerMeta();
    }

    void TearDown() override
    {
    }
};
class EnttMetaTest : public ::testing::Test
{
  public:
  protected:
    void SetUp() override
    {
        registerMeta();
    }

    void TearDown() override
    {
    }
};
// TEST_F(ReflTest, refl)
// {
//     auto reflEntity = refl::reflect(student);
//     GTEST_LOG_(INFO) << "reflEntity.name: " << reflEntity.name;
//     auto fields = reflEntity.members;
//     auto firstField = refl::util::find_first(fields, [](auto field) { return field.name == "id"; });
//     refl::util::for_each(fields, [&](auto field) {
//         GTEST_LOG_(INFO) << "name1: " << field.name;
//         auto name = refl::descriptor::get_name(field);
//         GTEST_LOG_(INFO) << "name: " << name;
//         auto displayName = refl::descriptor::get_display_name(field);
//         GTEST_LOG_(INFO) << "displayName: " << displayName;
//         auto attr = refl::descriptor::has_attribute<Editable>(field);
//         GTEST_LOG_(INFO) << "attr: " << attr;
//     });
//     auto attr = reflEntity.attributes;
//     GTEST_LOG_(INFO) << "attr.size: " << std::tuple_size<decltype(attr)>::value;
// }
// TEST_F(EnttMetaTest, basic)
// {
//     registerMeta();
//     auto metaStudent = entt::forward_as_meta(student);
//     GTEST_LOG_(INFO) << "class: " << metaStudent.type().info().name();
//     auto data = metaStudent.type().data();

//     for (auto &&[id, data] : data)
//     {
//         GTEST_LOG_(INFO) << "id: " << id;
//         auto custom = data.custom();
//         if (auto *ddd = static_cast<std::string *>(custom))
//         {
//             GTEST_LOG_(INFO) << "1111111111111111";
//         }
//         auto value = data.get(metaStudent);
//         if (value.type() == entt::resolve<std::string>())
//         {
//             auto v = value.cast<std::string>();
//             GTEST_LOG_(INFO) << "value: " << v;
//         }
//     }
// }
// TEST_F(EnttMetaTest, base)
// {
//     registerMeta();
//     std::shared_ptr<Animal> animal = std::make_shared<Dog>();
//     auto metaAny = entt::forward_as_meta(*animal);
//     GTEST_LOG_(INFO) << "class: " << metaAny.type().info().name();
//     auto data = metaAny.type().data();

//     for (auto &&[id, data] : data)
//     {

//         GTEST_LOG_(INFO) << "id: " << id;
//         auto value = data.get(metaAny);
//         if (value.type() == entt::resolve<std::string>())
//         {
//             auto v = value.cast<std::string>();
//             GTEST_LOG_(INFO) << "value: " << v;
//         }
//     }
// }
// TEST_F(EnttMetaTest, modify)
// {
//     registerMeta();
//     std::shared_ptr<Animal> animal = std::make_shared<Dog>();
//     auto metaType = entt::resolve<Dog>();
//     std::string newType = "Goose";
//     for (auto &&[id, data] : metaType.data())
//     {
//         auto value = data.get(*std::dynamic_pointer_cast<Dog>(animal));
//         GTEST_LOG_(INFO) << "value type: " << value.type().info().name();
//         if (value.type() == entt::resolve<std::string>())
//         {
//             auto v = value.cast<std::string>();
//             bool success = data.set(*std::dynamic_pointer_cast<Dog>(animal), newType);
//             GTEST_LOG_(INFO) << "set success: " << success;
//             GTEST_LOG_(INFO) << "value: " << v;
//         }
//     }
//     EXPECT_EQ(animal->name, newType);
//     GTEST_LOG_(INFO) << "modify name: " << animal->name;
// }
// TEST_F(EnttMetaTest, modify3)
// {
//     registerMeta();
//     Dog animal;
//     auto metaType = entt::resolve<Dog>();
//     std::string newType = "Goose";
//     for (auto &&[id, data] : metaType.data())
//     {
//         auto value = data.get(animal);
//         GTEST_LOG_(INFO) << "value type: " << value.type().info().name();
//         if (value.type() == entt::resolve<std::string>())
//         {
//             auto v = value.cast<std::string>();
//             bool success = data.set(animal, newType);
//             GTEST_LOG_(INFO) << "set success: " << success;
//             GTEST_LOG_(INFO) << "value: " << v;
//         }
//     }
//     EXPECT_EQ(animal.name, newType);
//     GTEST_LOG_(INFO) << "modify name: " << animal.name;
// }
// TEST_F(EnttMetaTest, modify2)
// {
//     registerMeta();
//     Student student{123, "John Doe", true};
//     auto metaType = entt::resolve<Student>();
//     std::string newName = "Tom Smith";
//     int newId = 321;
//     for (auto &&[id, data] : metaType.data())
//     {
//         auto value = data.get(student);
//         GTEST_LOG_(INFO) << "value type: " << value.type().info().name();
//         if (value.type() == entt::resolve<std::string>())
//         {
//             bool success = data.set(student, newName);
//             GTEST_LOG_(INFO) << "set success: " << success;
//             auto v = value.cast<std::string>();
//             GTEST_LOG_(INFO) << "value: " << v;
//         }
//         else if (value.type() == entt::resolve<int>())
//         {
//             auto v = value.cast<int>();
//             data.set(student, newId);
//             GTEST_LOG_(INFO) << "value: " << v;
//         }
//     }
//     EXPECT_EQ(student.getName(), newName);
//     EXPECT_EQ(student.id, newId);
// }
TEST_F(EnttMetaTest, ABC)
{
    std::shared_ptr<Animal> animal = std::make_shared<Dog>();
    auto metaType = entt::resolve<Dog>();
    auto custom = metaType.custom();
    if (auto ddd = static_cast<DisplayName *>(custom))
    {
        GTEST_LOG_(INFO) << ddd->name;
    }
    auto base = metaType.base();
    for (auto &&[id, data] : base)
    {
    }
}