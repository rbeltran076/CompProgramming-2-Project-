#include <gtest/gtest.h>
#include "Storage.h"
#include "Ingredient.h"

class StorageTest : public ::testing::Test {
protected:
    Storage storage;
};

// Test case to verify that adding a new ingredient works as expected.
TEST_F(StorageTest, AddIngredient) {
    Ingredient sugar("Sugar", 2, "2024-12-31");
    storage.addIngredient(sugar);

    ASSERT_EQ(storage.getIngredients().size(), 1); // Verify that the storage contains exactly 1 ingredient.
    EXPECT_EQ(storage.getIngredients()[0].getName(), "Sugar");
    EXPECT_EQ(storage.getIngredients()[0].getQuantity(), 2);
    EXPECT_EQ(storage.getIngredients()[0].getExpirationDate(), "2024-12-31");
}

// Test case to verify that adding an existing ingredient updates its quantity.
TEST_F(StorageTest, AddExistingIngredientUpdatesQuantity) {
    Ingredient sugar("Sugar", 2, "2024-12-31");
    storage.addIngredient(sugar);

    Ingredient moreSugar("Sugar", 3, ""); //add more sugar (same ingredient) with a different quantity 
    storage.addIngredient(moreSugar);

    ASSERT_EQ(storage.getIngredients().size(), 1); // Storage should still have only 1 entry (ingredient was only modified)
    EXPECT_EQ(storage.getIngredients()[0].getQuantity(), 5);  // Quantity should be updated to 5
}

TEST_F(StorageTest, ConvertToJson) {
    Ingredient sugar("Sugar", 2, "2024-12-31");
    storage.addIngredient(sugar);

    json j = storage.toJSON();
    EXPECT_EQ(j[0]["name"], "Sugar");
    EXPECT_EQ(j[0]["quantity"], 2);
    EXPECT_EQ(j[0]["expirationDate"], "2024-12-31");
}

TEST_F(StorageTest, ConvertFromJson) {
    json j = R"([
        {"name": "Sugar", "quantity": 2, "expirationDate": "2024-12-31"},
        {"name": "Flour", "quantity": 1, "expirationDate": "2024-10-10"}
    ])"_json;

    storage.fromJSON(j);

    ASSERT_EQ(storage.getIngredients().size(), 2);
    EXPECT_EQ(storage.getIngredients()[0].getName(), "Sugar");
    EXPECT_EQ(storage.getIngredients()[1].getName(), "Flour");
}

//to run: g++ -std=c++14 -isystem /usr/include/gtest -pthread /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Ingredient.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles/Storage.cpp /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/Tests/StorageTest.cpp -I/Users/makennawarner/RecipeManager/CompProgramming-2-Project-/HeaderFiles -I/Users/makennawarner/RecipeManager/CompProgramming-2-Project-/ -lgtest -lgtest_main -o runTests
//./runTests