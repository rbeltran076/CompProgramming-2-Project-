#include <gtest/gtest.h>
#include <string>
#include "json.hpp"
 // for JSON support, you can include this if you're using nlohmann json

using json = nlohmann::json;  // Alias for convenience

// Ingredient class definition
class Ingredient {
private:
    std::string name;
    int quantity;
    std::string expirationDate;

public:
    Ingredient() {}
    Ingredient(std::string n, int q, std::string exp = "") : name(n), quantity(q), expirationDate(exp) {}

    std::string getName() const { return name; }
    int getQuantity() const { return quantity; }
    std::string getExpirationDate() const { return expirationDate; }

    void setQuantity(int q) { quantity = q; }

    json toJSON() const {
        return { {"name", name}, {"quantity", quantity}, {"expirationDate", expirationDate} };
    }

    static Ingredient fromJSON(const json& j) {
        return Ingredient(j["name"], j["quantity"], j.value("expirationDate", ""));
    }
};

// Unit tests for the Ingredient class
TEST(IngredientTest, DefaultConstructorDoesNotThrow) { 
    EXPECT_NO_THROW({
        Ingredient ingredient; // Expect the default constructor not to throw
    });
}


TEST(IngredientTest, ParameterizedConstructor) {
    Ingredient ingredient("flour", 2); // Creating an Ingredient with name "flour" and quantity 2
    EXPECT_EQ(ingredient.getName(), "flour");
    EXPECT_EQ(ingredient.getQuantity(), 2);
}

TEST(IngredientTest, ToJsonConversion) {
    Ingredient ingredient("sugar", 1); // Create an Ingredient object "sugar" with quantity 1
    json j = ingredient.toJSON(); // Convert the object to JSON
    EXPECT_EQ(j["name"], "sugar");
    EXPECT_EQ(j["quantity"], 1);
}

TEST(IngredientTest, FromJsonConversion) {
    json j = {{"name", "butter"}, {"quantity", 3}}; // JSON object representing "butter" with quantity 3
    Ingredient ingredient = Ingredient::fromJSON(j); // Create an Ingredient object from the JSON object
    EXPECT_EQ(ingredient.getName(), "butter");
    EXPECT_EQ(ingredient.getQuantity(), 3);
}

// Main function for running all the tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

//to run: 
//g++ -std=c++17 -I /usr/include/gtest -I /Users/makennawarner/RecipeManager/CompProgramming-2-Project- -pthread /Users/makennawarner/RecipeManager/CompProgramming-2-Project-/Tests/SimplifiedIngredientTest.cpp -lgtest -lgtest_main -o IngredientTest
//./IngredientTest