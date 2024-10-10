#include <gtest/gtest.h>
#include "Pantry.h"
#include "Ingredient.h"

class PantryTest : public ::testing::Test {
protected:
    Pantry pantry;
};

TEST_F(PantryTest, AddIngredientToPantry) {
    Ingredient sugar("Sugar", 1, "");
    pantry.addIngredient(sugar);

    ASSERT_EQ(pantry.getIngredients().size(), 1);
    EXPECT_EQ(pantry.getIngredients()[0].getName(), "Sugar");
    EXPECT_EQ(pantry.getIngredients()[0].getQuantity(), 1);
}

TEST_F(PantryTest, RunningLow) {
    Ingredient salt("Salt", 1, "");
    pantry.addIngredient(salt);
    testing::internal::CaptureStdout(); //capture the output to test it
    pantry.runningLow();
    std::string output = testing::internal::GetCapturedStdout(); //output that was printed to stdout during the test.
    // Check if the output contains the message "Salt is running low.".
    // The find() function returns the position of the substring if it's found, or std::string::npos if not.
    EXPECT_NE(output.find("Salt is running low."), std::string::npos);
}

//to run: g++ -std=c++14 -isystem /usr/include/gtest -pthread /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Ingredient.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Storage.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Pantry.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/Tests/PantryTest.cpp -I/Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles -lgtest -lgtest_main -o runTests