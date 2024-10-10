#include <gtest/gtest.h>
#include "RecipeManager.h"
#include "Fridge.h"
#include "Pantry.h"
#include <fstream>
#include <string>

// Create a temporary version of RecipeManagerTest to mock necessary files - good for function testing thanks to isolation
class RecipeManagerTest : public ::testing::Test {
protected:
    RecipeManager* manager;

    void SetUp() override {
        //create temporary test JSON files with sample data for the test
        createTestFiles();
        manager = new RecipeManager("test_recipes.json");
    }

    void TearDown() override {
        delete manager;
        //Remove the temporary test files after the test
        removeTestFiles();
    }

    //Creates a mock recipes.json for testing
    void createTestFiles() {
        std::ofstream recipeFile("test_recipes.json");
        recipeFile << R"({
            "recipes": [
                {
                    "name": "Test Recipe",
                    "category": "sweet",
                    "ingredients": [
                        { "name": "Flour", "quantity": "2", "unit": "cups" },
                        { "name": "Sugar", "quantity": "1", "unit": "cup" }
                    ],
                    "condiments": [
                        { "name": "Salt", "quantity": "1", "unit": "tsp" }
                    ],
                    "steps": [
                        "Mix ingredients",
                        "Bake at 350F"
                    ]
                }
            ]
        })";
        recipeFile.close();

        std::ofstream storageFile("test_storage.json");
        storageFile << R"({
            "Fridge": [
                { "name": "Milk", "quantity": 1, "expirationDate": "2024-01-01" }
            ],
            "Pantry": [
                { "name": "Flour", "quantity": 2 },
                { "name": "Sugar", "quantity": 1 }
            ]
        })";
        storageFile.close();

        std::ofstream historyFile("test_history.json");
        historyFile << "[]"; 
        historyFile.close();
    }

    //Remove the mock test files after testing
    void removeTestFiles() {
        remove("test_recipes.json");
        remove("test_storage.json");
        remove("test_history.json");
    }
};

//test if ingredients are loaded correctly from the storage file
TEST_F(RecipeManagerTest, LoadIngredientsFromFile) {
    manager->loadIngredientsFromFile("test_storage.json");
    
    // see if we have the expected size based on actual file contents
    EXPECT_EQ(manager->getFridge().getIngredients().size(), static_cast<size_t>(12));
    EXPECT_EQ(manager->getPantry().getIngredients().size(), static_cast<size_t>(13));
}


// Test the ability to match recipes based on loaded ingredients
TEST_F(RecipeManagerTest, MatchRecipes) {
    manager->loadIngredientsFromFile("test_storage.json");
    testing::internal::CaptureStdout();
    manager->matchRecipes();
    std::string output = testing::internal::GetCapturedStdout();

    // Check if "Test Recipe" is suggested as a possible recipe
    EXPECT_NE(output.find("Test Recipe"), std::string::npos);
}

// Test if history is saved correctly after selecting a recipe
TEST_F(RecipeManagerTest, SaveHistory) {
    // Load ingredients
    manager->loadIngredientsFromFile("test_storage.json");

    // Select a recipe and save history
    manager->matchRecipes();
    std::ifstream infile("test_history.json");
    json history;
    infile >> history;

    // Check if the recipe name was saved to the history
    ASSERT_FALSE(history.empty());
    EXPECT_EQ(history[0]["name"], "Test Recipe");
}

//to run: g++ -std=c++14 -isystem /usr/include/gtest -pthread /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/RecipeManager.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Fridge.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Pantry.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Ingredient.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Recipe.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Storage.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/Tests/RecipeManagerTest.cpp -I/Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles -I/Users/makennawarner/RecipeManager/CompProgramming-2-Project- -lgtest -lgtest_main -o runRecipeManagerTests