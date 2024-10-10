#include <gtest/gtest.h>
#include "Fridge.h"
#include "Ingredient.h"

class FridgeTest : public ::testing::Test {
protected:
    Fridge fridge;
};

TEST_F(FridgeTest, AddIngredientToFridge) {
    Ingredient milk("Milk", 1, "2024-01-15");
    fridge.addIngredient(milk);

    ASSERT_EQ(fridge.getIngredients().size(), 1);
    EXPECT_EQ(fridge.getIngredients()[0].getName(), "Milk");
    EXPECT_EQ(fridge.getIngredients()[0].getQuantity(), 1);
}

TEST_F(FridgeTest, ExpiringSoon) {
    // Add an ingredient expiring soon
    Ingredient cheese("Cheese", 1, "2024-01-05");
    fridge.addIngredient(cheese);

    testing::internal::CaptureStdout(); //Capture the output printed by the fridge.expiringSoon() function.
    //Redirect the standard output (stdout) to a stringstream so we can test the output.
    fridge.expiringSoon();
    std::string output = testing::internal::GetCapturedStdout(); // retrieve the stdout output

   //find the position where the message starts, it should not return std::string::npos (meaning the message was found).
    EXPECT_NE(output.find("Cheese is expiring in less than 5 days."), std::string::npos);

}

//to run: g++ -std=c++14 -isystem /usr/include/gtest -pthread /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Ingredient.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Storage.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Fridge.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/Tests/FridgeTest.cpp -I/Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles -lgtest -lgtest_main -o runTests
