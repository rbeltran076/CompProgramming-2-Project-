#include <iostream> 
#include <vector>
#include <string>
#include "json.hpp"
#include <fstream> //for json functionality

//note: to get "json.hpp" run 
//curl -LO https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp 
//in your terminal.

using json = nlohmann::json; //to use this library, you must update c++ with: 
//g++ -std=c++11 RecipeManager.cpp -o RecipeManager
//then run: ./RecipeManager
//VERY IMPORTANT (see above)

using namespace std;

//notice use of Encapsulation and Ingredient() constructor to allocate resources
class Ingredient {
private:
    std::string name;
    int quantity;  // Quantity is now an integer (no units)
public:
    Ingredient() {}
    Ingredient(std::string n, int q) : name(n), quantity(q) {}

    std::string getName() const { return name; }
    int getQuantity() const { return quantity; }

    json toJSON() const {
        return { {"name", name}, {"quantity", quantity} };
    }

    static Ingredient fromJSON(const json& j) {
        return Ingredient(j["name"], j["quantity"]);
    }
};

class Storage {
protected:
    std::vector<Ingredient> ingredients;
public:
    virtual void addIngredient(const Ingredient& ingredient) {
        ingredients.push_back(ingredient);
    }

    const std::vector<Ingredient>& getIngredients() const {
        return ingredients;
    }

    json toJSON() const {
        json ingredientArray = json::array();
        for (const auto& ingredient : ingredients) {
            ingredientArray.push_back(ingredient.toJSON());
        }
        return ingredientArray;
    }

    void fromJSON(const json& j) {
        for (const auto& item : j) {
            ingredients.push_back(Ingredient::fromJSON(item));
        }
    }
};

class Fridge : public Storage {
public:
    void addIngredient(const Ingredient& ingredient) override {
        Storage::addIngredient(ingredient);
        std::cout << ingredient.getName() << " added to Fridge.\n";
    }
};

class Pantry : public Storage {
public:
    void addIngredient(const Ingredient& ingredient) override {
        Storage::addIngredient(ingredient);
        std::cout << ingredient.getName() << " added to Pantry.\n";
    }
};

class Recipe {
private:
    std::string recipeName;
    std::vector<std::pair<std::string, std::string>> requiredIngredients;  // Use string for both ingredient name and quantity/unit
    std::vector<std::pair<std::string, std::string>> condiments;           // Condiments (optional)
    std::vector<std::string> steps;  // Steps for the recipe

public:
    // Updated constructor to accept steps
    Recipe(std::string name, 
           std::vector<std::pair<std::string, std::string>> ingredients, 
           std::vector<std::pair<std::string, std::string>> condiments, 
           std::vector<std::string> steps)
        : recipeName(name), requiredIngredients(ingredients), condiments(condiments), steps(steps) {}

    // Get the recipe name
    std::string getRecipeName() const {
        return recipeName;
    }

    // Function to match user-provided ingredients and condiments with the recipe
    bool canMakeRecipe(const std::vector<Ingredient>& userIngredients, std::vector<std::string>& missingIngredients) const {
        bool canMake = true;

        // Check for required ingredients
        for (const auto& reqIngredient : requiredIngredients) {
            bool found = false;
            for (const auto& userIngredient : userIngredients) {
                // Check if the user has the required ingredient
                if (userIngredient.getName() == reqIngredient.first) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                missingIngredients.push_back(reqIngredient.first);
                canMake = false;
            }
        }

        return canMake;  // True if all required ingredients are found
    }

    // Get the required ingredients
    std::vector<std::pair<std::string, std::string>> getRequiredIngredients() const {
        return requiredIngredients;
    }

    // Get the condiments
    std::vector<std::pair<std::string, std::string>> getCondiments() const {
        return condiments;
    }

    // Get the steps of the recipe
    std::vector<std::string> getSteps() const {
        return steps;
    }
};

//Function to load recipes from a JSON file
vector<Recipe> loadRecipesFromJSON(const string& filename) {
    vector<Recipe> recipes;

    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cerr << "Could not open the file: " << filename << endl;
        return recipes;
    }

    json j;
    inputFile >> j;

    for (const auto& recipeData : j["recipes"]) {
        string recipeName = recipeData["name"];
        vector<pair<string, string>> ingredients;
        vector<pair<string, string>> condiments;
        vector<string> steps;  // Add a vector to store steps

        // Parse ingredients (name, quantity, unit)
        for (const auto& ingredient : recipeData["ingredients"]) {
            string name = ingredient["name"];
            string quantity = ingredient["quantity"];
            string unit = ingredient["unit"];
            ingredients.push_back({ name, quantity + " " + unit });
        }

        // Parse condiments (name, quantity, unit)
        for (const auto& condiment : recipeData["condiments"]) {
            string name = condiment["name"];
            string quantity = condiment["quantity"];
            string unit = condiment["unit"];
            condiments.push_back({ name, quantity + " " + unit });
        }

        // Parse steps (array of strings)
        for (const auto& step : recipeData["steps"]) {
            steps.push_back(step);
        }

        // Add the recipe to the list of recipes (assuming Recipe class takes steps)
        recipes.emplace_back(recipeName, ingredients, condiments, steps);
    }

    return recipes;
}

class RecipeManager {
private:
    Fridge fridge;
    Pantry pantry;
    std::vector<Recipe> recipes;

public:
    // Constructor to load recipes from a file and ingredients from storage.json
    RecipeManager(const std::string& recipeFilename) {
        // Load the recipes from the specified recipe JSON file
        recipes = loadRecipesFromJSON(recipeFilename);

        // Load the ingredients from the storage JSON file
        loadIngredientsFromFile("storage.json");
    }

    // Collect ingredients from the user and save them into storage
    void collectIngredients() {
        while (true) {
            std::string name;
            int quantity;
            std::cout << "Please enter the ingredient name (or type 'done' to finish): ";
            std::cin >> name;
            if (name == "done") break;

            std::cout << "Enter the quantity of " << name << ": ";
            std::cin >> quantity;
            Ingredient newIngredient(name, quantity);

            std::string storageLocation;
            std::cout << "Is the ingredient stored in the (F)ridge or (P)antry? ";
            std::cin >> storageLocation;

            if (storageLocation == "F" || storageLocation == "f") {
                fridge.addIngredient(newIngredient);
            } else if (storageLocation == "P" || storageLocation == "p") {
                pantry.addIngredient(newIngredient);
            } else {
                std::cout << "Invalid option. Please choose (F)ridge or (P)antry.\n";
            }

            // Save updated storage to JSON after adding new ingredients
            saveIngredientsToFile("storage.json");
        }
    }

    // Save the ingredients in Fridge and Pantry to a JSON file
    void saveIngredientsToFile(const std::string& filename) {
        json j;
        j["Fridge"] = fridge.toJSON();
        j["Pantry"] = pantry.toJSON();

        std::ofstream file(filename);
        if (file.is_open()) {
            file << j.dump(4);  // Pretty print with 4-space indentation
            file.close();
            std::cout << "Ingredients saved to " << filename << "\n";
        } else {
            std::cerr << "Unable to open file " << filename << "\n";
        }
    }

    // Load ingredients from the JSON file into Fridge and Pantry
    void loadIngredientsFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << filename << "\n";
            return;
        }

        json j;
        file >> j;

        if (j.contains("Fridge")) {
            fridge.fromJSON(j["Fridge"]);
        }

        if (j.contains("Pantry")) {
            pantry.fromJSON(j["Pantry"]);
        }

        file.close();
        std::cout << "Ingredients loaded from " << filename << "\n";
    }

    // Display stored ingredients from both fridge and pantry
    void displayStoredIngredients() const {
        std::cout << "Fridge Ingredients:\n";
        for (const auto& ingredient : fridge.getIngredients()) {
            std::cout << "- " << ingredient.getName() << ": " << ingredient.getQuantity() << "\n";
        }

        std::cout << "Pantry Ingredients:\n";
        for (const auto& ingredient : pantry.getIngredients()) {
            std::cout << "- " << ingredient.getName() << ": " << ingredient.getQuantity() << "\n";
        }
    }

// Function to display the full recipe details
void displayFullRecipe(const Recipe& recipe) {
    std::cout << "Recipe: " << recipe.getRecipeName() << "\n";
    
    // Display ingredients
    std::cout << "Ingredients:\n";
    for (const auto& ingredient : recipe.getRequiredIngredients()) {
        std::cout << "- " << ingredient.first << ": " << ingredient.second << "\n";
    }

    // Display condiments
    std::cout << "Condiments:\n";
    for (const auto& condiment : recipe.getCondiments()) {
        std::cout << "- " << condiment.first << ": " << condiment.second << "\n";
    }

    // Display steps using the getSteps() method
    std::cout << "Steps:\n";
    for (const auto& step : recipe.getSteps()) {  // Using getSteps() method
        std::cout << "- " << step << "\n";
    }
}

// Match ingredients to recipes and print results
void matchRecipes() {
    // Combine ingredients from fridge and pantry
    std::vector<Ingredient> allIngredients = fridge.getIngredients();
    allIngredients.insert(allIngredients.end(), pantry.getIngredients().begin(), pantry.getIngredients().end());

    std::vector<std::string> possibleRecipes;
    std::vector<const Recipe*> matchingRecipes;

    for (const auto& recipe : recipes) {
        std::vector<std::string> missingIngredients;
        if (recipe.canMakeRecipe(allIngredients, missingIngredients)) {
            possibleRecipes.push_back(recipe.getRecipeName());
            matchingRecipes.push_back(&recipe); // Store the pointer to the matching recipe
        } else if (!missingIngredients.empty()) {
            std::cout << "You are missing the following ingredients for " << recipe.getRecipeName() << ": ";
            for (const auto& ingredient : missingIngredients) {
                std::cout << ingredient << " ";
            }
            std::cout << "\n";
        }
    }

    // Print out possible recipes
    if (possibleRecipes.empty()) {
        std::cout << "Sorry, you cannot make any recipes with the ingredients you have.\n";
    } else {
        std::cout << "Based on your ingredients, you can make the following recipes:\n";
        for (size_t i = 0; i < possibleRecipes.size(); ++i) {
            std::cout << i + 1 << ". " << possibleRecipes[i] << "\n";
        }

        // Ask the user which recipe they want to see in full
        int choice;
        std::cout << "Enter the number of the recipe you want to see in full: ";
        std::cin >> choice;

        if (choice > 0 && choice <= matchingRecipes.size()) {
            // Show the full recipe details
            displayFullRecipe(*matchingRecipes[choice - 1]);
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
}
};

int main() {
    // Initialize RecipeManager with recipes from the JSON file
    RecipeManager recipeManager("recipes.json");

    // Step 1: Load previously saved ingredients from storage.json
    recipeManager.loadIngredientsFromFile("storage.json");

    // Step 2: Collect ingredients from user
    recipeManager.collectIngredients();

    // Step 3: Display stored ingredients (optional but useful to see current fridge and pantry contents)
    recipeManager.displayStoredIngredients();

    // Step 4: Match recipes based on available ingredients and allow the user to select one to view
    recipeManager.matchRecipes();

    return 0;
}
