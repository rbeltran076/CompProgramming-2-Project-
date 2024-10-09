#include <gtest/gtest.h>
#include "RecipeManager.cpp"  

//Unit tests for the Ingredient class: IngredientTest Suite

TEST(IngredientTest, DefaultConstructor) { 
    Ingredient ingredient; //calls the ingredient default constructor Ingredient() {} & creates an Ingredient object with default values.
    EXPECT_EQ(ingredient.getName(), ""); //we expect the default constructor to set Name to an empty string
    EXPECT_EQ(ingredient.getQuantity(), 0); //we expect the default constructor to set quantity to 0
}

TEST(IngredientTest, ParameterizedConstructor) {
    Ingredient ingredient("flour", 2); //when we input ""flour", 2" into the ingredient constructor, we expect:
    EXPECT_EQ(ingredient.getName(), "flour");
    EXPECT_EQ(ingredient.getQuantity(), 2);
}

TEST(IngredientTest, ToJsonConversion) {
    Ingredient ingredient("sugar", 1); //creates an ingredient object called "sugar" with quantity 1
    json j = ingredient.toJSON(); //uses the toJSON() method to convert the object into a JSON representation
    EXPECT_EQ(j["name"], "sugar");
    EXPECT_EQ(j["quantity"], 1);
}

TEST(IngredientTest, FromJsonConversion) {
    json j = {{"name", "butter"}, {"quantity", 3}}; //fromJSON() takes a json format; here, we create a JSON object
    Ingredient ingredient = Ingredient::fromJSON(j); //create an ingredient object from the JSON object 
    EXPECT_EQ(ingredient.getName(), "butter");
    EXPECT_EQ(ingredient.getQuantity(), 3);
}
