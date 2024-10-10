#include <gtest/gtest.h>
#include "Ingredient.h" 

//Unit tests for the Ingredient class: IngredientTest Suite

TEST(IngredientTest, DefaultConstructorDoesNotThrow) { 
    EXPECT_NO_THROW({
        Ingredient ingredient; // Expect the default constructor not to output anything
    });
}

TEST(IngredientTest, ParameterizedConstructor) {
    Ingredient ingredient("flour", 2, "2024-10-15"); // Creating an Ingredient with name "flour" and quantity 2 and an expiration date of 2024-10-15
    EXPECT_EQ(ingredient.getName(), "flour");
    EXPECT_EQ(ingredient.getQuantity(), 2);
    EXPECT_EQ(ingredient.getExpirationDate(), "2024-10-15");
}

TEST(IngredientTest, ToJsonConversion) {
    Ingredient ingredient("sugar", 1, "2024-10-15"); // Create an Ingredient object "sugar" with quantity 1
    json j = ingredient.toJSON(); // Convert the object to JSON
    EXPECT_EQ(j["name"], "sugar");
    EXPECT_EQ(j["quantity"], 1);
    EXPECT_EQ(j["expirationDate"], "2024-10-15");
}

TEST(IngredientTest, FromJsonConversion) {
    json j = {{"name", "butter"}, {"quantity", 3}, {"expirationDate", "2024-10-15"}}; // JSON object representing "butter" with quantity 3
    Ingredient ingredient = Ingredient::fromJSON(j); // Create an Ingredient object from the JSON object
    EXPECT_EQ(ingredient.getName(), "butter");
    EXPECT_EQ(ingredient.getQuantity(), 3);
    EXPECT_EQ(ingredient.getExpirationDate(), "2024-10-15");
}

//to run:
// g++ -std=c++14 -isystem /usr/include/gtest -pthread /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Ingredient.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/Tests/IngredientTest.cpp -I/Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles -I/Users/makennawarner/RecipeManager/CompProgramming-2-Project-/ -lgtest -lgtest_main -o runTests
//./runTests
