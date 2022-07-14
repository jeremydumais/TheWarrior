#include "editTextureFormController.hpp"
#include "textureDTO.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

std::vector<std::string> getAllTextureNamesSample() { return { "tex1", "tex2"}; };
TextureDTO getTextureDTOSample1() { return { "tex1", "tex1.png", 256, 512, 16, 32}; };

class EditTextureFormControllerNullOriginalSample : public ::testing::Test
{
public:
    EditTextureFormControllerNullOriginalSample()
        : nameSamples(getAllTextureNamesSample()),
          controller(nullptr, nameSamples)
    {
    }
    std::vector<std::string> nameSamples;
    EditTextureFormController controller;
};

TEST(EditTextureFormController_getOriginalTexture, ReturnTextureDTO)
{
    EditTextureFormController controller(std::unique_ptr<TextureDTO>(new TextureDTO {
                                            "tex1", "tex1.png", 256, 512, 16, 32}),
                                         getAllTextureNamesSample());
    auto actual = controller.getOriginalTexture();
    ASSERT_EQ("tex1", actual.name);   
    ASSERT_EQ("tex1.png", actual.filename);   
    ASSERT_EQ(256, actual.width);   
    ASSERT_EQ(512, actual.height);   
    ASSERT_EQ(16, actual.tileWidth);   
    ASSERT_EQ(32, actual.tileHeight);   
}

TEST_F(EditTextureFormControllerNullOriginalSample, validateTextureOperation_WithNoTextureInfoProvided_ReturnFalse)
{
    ASSERT_FALSE(controller.validateTextureOperation(nullptr));
    ASSERT_EQ("textureInfo must be provided.", controller.getLastError());
}

TEST_F(EditTextureFormControllerNullOriginalSample, validateTextureOperation_WithNameAreadyTaken_ReturnFalse)
{
    auto texToAdd = std::make_unique<TextureDTO>(getTextureDTOSample1());
    ASSERT_FALSE(controller.validateTextureOperation(std::move(texToAdd)));
    ASSERT_EQ("The name tex1 is already used.", controller.getLastError());
}

TEST_F(EditTextureFormControllerNullOriginalSample, validateTextureOperation_WithNameInCapsAreadyTaken_ReturnFalse)
{
    auto texToAdd = std::make_unique<TextureDTO>(getTextureDTOSample1());
    texToAdd->name = "TEX1";
    ASSERT_FALSE(controller.validateTextureOperation(std::move(texToAdd)));
    ASSERT_EQ("The name TEX1 is already used.", controller.getLastError());
}

TEST_F(EditTextureFormControllerNullOriginalSample, validateTextureOperation_WithNameInCapsAndSpacesAreadyTaken_ReturnFalse)
{
    auto texToAdd = std::make_unique<TextureDTO>(getTextureDTOSample1());
    texToAdd->name = "  TEX1  ";
    ASSERT_FALSE(controller.validateTextureOperation(std::move(texToAdd)));
    ASSERT_EQ("The name   TEX1   is already used.", controller.getLastError());
}

TEST_F(EditTextureFormControllerNullOriginalSample, validateTextureOperation_WithNameNotAreadyTaken_ReturnTrue)
{
    auto texToAdd = std::make_unique<TextureDTO>(getTextureDTOSample1());
    texToAdd->name = "tex3";
    ASSERT_TRUE(controller.validateTextureOperation(std::move(texToAdd)));
}

TEST(EditTextureFormController_validateTextureOperation, WithOriginalAndSameName_ReturnTrue)
{
    auto textureNames = getAllTextureNamesSample();
    EditTextureFormController controller(std::unique_ptr<TextureDTO>(new TextureDTO {
                                            "tex1", "tex1.png", 256, 512, 16, 32}),
                                         textureNames);
    auto texToUpdate = std::make_unique<TextureDTO>(getTextureDTOSample1());                                    
    ASSERT_TRUE(controller.validateTextureOperation(std::move(texToUpdate)));
}

TEST(EditTextureFormController_validateTextureOperation, WithOriginalAndSameNameAndSpaces_ReturnTrue)
{
    auto textureNames = getAllTextureNamesSample();
    EditTextureFormController controller(std::unique_ptr<TextureDTO>(new TextureDTO {
                                            "tex1", "tex1.png", 256, 512, 16, 32}),
                                         textureNames);
    auto texToUpdate = std::make_unique<TextureDTO>(getTextureDTOSample1());                                   
    texToUpdate->name = "  tex1  ";
    ASSERT_TRUE(controller.validateTextureOperation(std::move(texToUpdate)));
}

TEST(EditTextureFormController_validateTextureOperation, WithOriginalAndSameNameAndCaps_ReturnTrue)
{
    auto textureNames = getAllTextureNamesSample();
    EditTextureFormController controller(std::unique_ptr<TextureDTO>(new TextureDTO {
                                            "tex1", "tex1.png", 256, 512, 16, 32}),
                                         textureNames);
    auto texToUpdate = std::make_unique<TextureDTO>(getTextureDTOSample1());                                   
    texToUpdate->name = "  TEX1  ";                                        
    ASSERT_TRUE(controller.validateTextureOperation(std::move(texToUpdate)));
}

TEST(EditTextureFormController_validateTextureOperation, WithOriginalAndDifferentNameNotTaken_ReturnTrue)
{
    auto textureNames = getAllTextureNamesSample();
    EditTextureFormController controller(std::unique_ptr<TextureDTO>(new TextureDTO {
                                            "tex1", "tex1.png", 256, 512, 16, 32}),
                                         textureNames);
    auto texToUpdate = std::make_unique<TextureDTO>(getTextureDTOSample1());                                   
    texToUpdate->name = "tex3";                                        
    ASSERT_TRUE(controller.validateTextureOperation(std::move(texToUpdate)));
}

TEST(EditTextureFormController_validateTextureOperation, WithOriginalAndDifferentNameTaken_ReturnFalse)
{
    auto textureNames = getAllTextureNamesSample();
    EditTextureFormController controller(std::unique_ptr<TextureDTO>(new TextureDTO {
                                            "tex1", "tex1.png", 256, 512, 16, 32}),
                                         textureNames);
    auto texToUpdate = std::make_unique<TextureDTO>(getTextureDTOSample1());                                   
    texToUpdate->name = "tex2";                                        
    ASSERT_FALSE(controller.validateTextureOperation(std::move(texToUpdate)));
    ASSERT_EQ("The name tex2 is already used.", controller.getLastError());
}

TEST(EditTextureFormController_validateTextureOperation, WithOriginalAndDifferentNameTakenAndSpaces_ReturnFalse)
{
    auto textureNames = getAllTextureNamesSample();
    EditTextureFormController controller(std::unique_ptr<TextureDTO>(new TextureDTO {
                                            "tex1", "tex1.png", 256, 512, 16, 32}),
                                         textureNames);
    auto texToUpdate = std::make_unique<TextureDTO>(getTextureDTOSample1());                                   
    texToUpdate->name = "  tex2  ";                                        
    ASSERT_FALSE(controller.validateTextureOperation(std::move(texToUpdate)));
    ASSERT_EQ("The name   tex2   is already used.", controller.getLastError());
}

TEST(EditTextureFormController_validateTextureOperation, WithOriginalAndDifferentNameTakenAndSpacesAndCaps_ReturnFalse)
{
    auto textureNames = getAllTextureNamesSample();
    EditTextureFormController controller(std::unique_ptr<TextureDTO>(new TextureDTO {
                                            "tex1", "tex1.png", 256, 512, 16, 32}),
                                         textureNames);
    auto texToUpdate = std::make_unique<TextureDTO>(getTextureDTOSample1());                                   
    texToUpdate->name = "  TEX2  ";                                        
    ASSERT_FALSE(controller.validateTextureOperation(std::move(texToUpdate)));
    ASSERT_EQ("The name   TEX2   is already used.", controller.getLastError());
}