#include <gtest/gtest.h>
#include "RecipeManager.h"
#include "Fridge.h"
#include "Pantry.h"
#include "json.hpp"
#include <fstream>
#include <string>


//test fixture class for testing the integration of RecipeManager class and involved .json files
class RecipeManagerTest:public ::testing::Test {
protected:
    RecipeManager* manager;

    void SetUp() override {
        manager= new RecipeManager("test_recipes.json");
    }
    void TearDown() override {
        delete manager;
    }
};


/*
//test case to load ingredients from json file
TEST_F(RecipeManagerTest, LoadIngredientsFromJson) {
    manager->loadIngredientsFromFile("storage.json");
    EXPECT_EQ(manager->getFridge().getIngredients().size(), expectedFridgeCount);
    EXPECT_EQ(manager->getPantry().getIngredients().size(), expectedPantryCount);
}
*/

// test case to save ingredients correctly to JSON fridge
TEST_F(RecipeManagerTest, SaveFridgeIngredientsToJson) {
    manager->getFridge().addIngredient("Potato", 2);
    manager->loadIngredientsFromFile("storage.json");

    RecipeManager newManager("test_recipes.json");  //new instance to verify data
    newManager.saveIngredientsToFile("storage.json"); //loads to check

    EXPECT_EQ(newManager.getFridge().getIngredients().size(), 1);
    EXPECT_TRUE(newManager.getFridge().hasIngredient("Potato"));
}

// test case to save ingredients correctly to JSON pantry
TEST_F(RecipeManagerTest, SavePantryIngredientsToJson) {
    manager->getPantry().addIngredient("Rice", 1);
    manager->loadIngredientsFromFile("storage.json");

    RecipeManager newManager("test_recipes.json");  //new instance to verify data
    newManager.saveIngredientsToFile("storage.json"); //loads to check

    EXPECT_EQ(newManager.getPantry().getIngredients().size(), 1);
    EXPECT_TRUE(newManager.getPantry().hasIngredient("Rice"));
}


//test case for matching recipes based on inputted ingredients --> tests that the proper recipe is printed/outputted
TEST_F(RecipeManagerTest, MatchRecipes) {
    manager->loadIngredientsFromFile("storage.json");
    testing::internal::CaptureStdout();
    manager->matchRecipes();
    std::string output = testing::internal::GetCapturedStdout();


    //based on ingredients, checks for specific recipe suggested
    EXPECT_NE(output.find("French Toast"), std::string::npos);
    //went ahead and added all necessary ingredients for French Toast, so should find this
}


// test case for ensuring the history is saved correctly after selecting a recipe
TEST_F(RecipeManagerTest, SaveHistory) {
    manager->loadIngredientsFromFile("storage.json");
    manager->matchRecipes();

    std::ifstream infile("history.json");
    nlohmann::json history;
    infile >> history;

    ASSERT_FALSE(history.empty());
    EXPECT_EQ(history[0]["name"], "French Toast");
    //french toast will be added to history for the first time during this test
}

// test case that recipe can be found based on avaliable ingredients in storage
TEST_F(RecipeManagerTest, RecipeFoundBasedOnStorageJson) {
    manager->loadIngredientsFromFile("storage.json");
    std::vector<std::string> foundRecipes = manager->findRecipesByIngredients();
    EXPECT_FALSE(foundRecipes.empty());
    EXPECT_EQ(foundRecipes[0], "French Toast");
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}




// test cases needed
// 1. storage.json contains all inputted ingredients, their quantities, and expiration dates after being inputted
// 2. need to ensure that out of the storage.json file, if we check the avaliable ingredients with the recipes.json file, which contains recipes and their ingredients, that a recipe will be found where all ingredients and their quantities can be found in our storage.json
// 3. need to ensure that after a recipe from recipe.json is selected, that the recipe is stored in history.json along with the date of prepartion
