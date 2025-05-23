#include "AssetDatabase.hpp"
#include "Asset/Asset.hpp"
#include "Asset/PBRMaterial.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <thread>
using namespace MEngine::Editor;
using namespace MEngine;
TEST(AssetDatabaseTest, ImportAsset_Png_GenerateMeta)
{
    auto pngPath = std::filesystem::current_path() / "Test" / "Data" / "test.png";
    AssetDatabase::ImportAsset(pngPath);
    auto metaPath = pngPath;
    metaPath += ".meta";
    EXPECT_TRUE(std::filesystem::exists(metaPath));
    AssetDatabase::ImportAsset(pngPath);
}
TEST(AssetDatabaseTest, CreateAsset_Material_Create)
{
    auto material = std::make_shared<MEngine::PBRMaterial>();
    material->AlbedoTextureID = UUIDGenerator()();
    material->NormalTextureID = UUIDGenerator()();
    material->ARMTextureID = UUIDGenerator()();
    material->Parameters.albedo = glm::vec3(1.0f, 0.0f, 0.0f);
    material->Parameters.metallic = 0.5f;
    material->Parameters.roughness = 0.5f;
    material->Parameters.ao = 0.5f;
    material->PipelineType = PipelineType::ForwardOpaquePBR;
    auto path = std::filesystem::current_path() / "Test" / "Data" / "test.mat";
    AssetDatabase::CreateAsset<PBRMaterial>(material, path);
    EXPECT_TRUE(std::filesystem::exists(path));
    EXPECT_TRUE(std::filesystem::exists(path.string() + ".meta"));
}
TEST(AssetDatabaseTest, Refresh)
{
    auto duration = std::chrono::seconds(20);
    auto start = std::chrono::steady_clock::now();
    std::jthread t([&]() {
        auto path = std::filesystem::current_path() / "Test" / "Data";
        AssetDatabase::RegisterAssetDirectory(path);
        while (std::chrono::steady_clock::now() - start < duration)
        {
            AssetDatabase::Refresh();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            GTEST_LOG_(INFO) << "Refreshing asset database...";
        }
        AssetDatabase::UnregisterAssetDirectory(path);
        GTEST_LOG_(INFO) << "Done refreshing asset database...";
    });
}