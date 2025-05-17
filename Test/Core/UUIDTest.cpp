#include "UUID.hpp"
#include "Entity/IEntity.hpp"
#include <gtest/gtest.h>

using namespace MEngine;

TEST(UUIDTest, GenerateUUID)
{
    UUIDGenerator uuidGen;
    UUID uuid1 = uuidGen();
    UUID uuid2 = uuidGen();
    EXPECT_NE(uuid1, uuid2);
}
TEST(UUIDTest, GenerateZeroUUID)
{
    UUID uuid = UUID();
    EXPECT_EQ(uuid.ToString(), "00000000-0000-0000-0000-000000000000");
}
TEST(UUIDTest, UUIDToString)
{
    UUIDGenerator uuidGen;
    UUID uuid = uuidGen();
    std::string uuidStr = uuid.ToString();
    std::cout << "UUID: " << uuidStr << std::endl;
    EXPECT_EQ(uuidStr.size(), 36); // UUID string representation is 36 characters long
}

TEST(UUIDTest, UUIDFromString)
{
    UUIDGenerator uuidGen;
    UUID uuid = uuidGen();
    std::string uuidStr = uuid.ToString();
    UUID parsedUUID = UUID(uuidStr);
    EXPECT_EQ(uuid, parsedUUID);
}
TEST(UUIDTest, UUIDFormat)
{
    UUIDGenerator uuidGen;
    UUID uuid1 = uuidGen();
}