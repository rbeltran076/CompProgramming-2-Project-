#include <gtest/gtest.h>
#include "Recipe.h"
#include "Ingredient.h"

class RecipeTest : public ::testing::Test {
protected:
    Recipe* recipe; //pointer to a Recipe object that will be used in each test case.

    //the SetUp() function runs before each test case for initialization purposes. Here, we initialize a Cake object with two ingredients: sugar and flour
    void SetUp() override {
        std::vector<std::pair<std::string, std::string>> ingredients = { {"Flour", "1 cup"}, {"Sugar", "2 tbsp"} };
        std::vector<std::pair<std::string, std::string>> condiments = { {"Salt", "1 tsp"} };
        std::vector<std::string> steps = { "Mix ingredients", "Bake at 350F" };
        recipe = new Recipe("Cake", ingredients, condiments, steps, "Dessert");
    }

    //runs after each test case
    void TearDown() override {
        delete recipe;
    }
};

TEST_F(RecipeTest, GetRecipeName) {
    EXPECT_EQ(recipe->getRecipeName(), "Cake");
}

TEST_F(RecipeTest, GetType) {
    EXPECT_EQ(recipe->getType(), "Dessert");
}

TEST_F(RecipeTest, CanMakeRecipe) {
    std::vector<Ingredient> availableIngredients = { Ingredient("Flour", 1, ""), Ingredient("Sugar", 2, "") };
    std::vector<std::string> missingIngredients;
    EXPECT_TRUE(recipe->canMakeRecipe(availableIngredients, missingIngredients)); //expect that the recipe can be made with the available ingredients.
}

TEST_F(RecipeTest, MissingIngredients) {
    std::vector<Ingredient> availableIngredients = { Ingredient("Flour", 1, "") };
    std::vector<std::string> missingIngredients;
    EXPECT_FALSE(recipe->canMakeRecipe(availableIngredients, missingIngredients)); //expect that the ingredient cannot be made as sugar is missing
    EXPECT_EQ(missingIngredients.size(), 1);
    EXPECT_EQ(missingIngredients[0], "Sugar");
}

TEST_F(RecipeTest, GetRequiredIngredients) {
    auto ingredients = recipe->getRequiredIngredients();
    EXPECT_EQ(ingredients.size(), 2);
    EXPECT_EQ(ingredients[0].first, "Flour");
    EXPECT_EQ(ingredients[1].first, "Sugar");
}

TEST_F(RecipeTest, GetSteps) {
    auto steps = recipe->getSteps();
    EXPECT_EQ(steps.size(), 2);
    EXPECT_EQ(steps[0], "Mix ingredients");
    EXPECT_EQ(steps[1], "Bake at 350F");
}
