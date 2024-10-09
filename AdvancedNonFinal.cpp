#include <iostream>
#include <vector>
#include <string>
#include "json.hpp"
#include <fstream>
#include <ctime> // for date functionality
#include <algorithm>

using json = nlohmann::json;

using namespace std;

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

class Storage {
protected:
    std::vector<Ingredient> ingredients;

public:
    virtual void addIngredient(const Ingredient& ingredient) {
        for (auto& ing : ingredients) {
            if (ing.getName() == ingredient.getName()) {
                ing.setQuantity(ing.getQuantity() + ingredient.getQuantity());
                return;
            }
        }
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

    void expiringSoon() const {
        time_t now = time(0);
        tm *ltm = localtime(&now);

        for (const auto& ingredient : ingredients) {
            if (!ingredient.getExpirationDate().empty()) {
                tm exp = {};
                strptime(ingredient.getExpirationDate().c_str(), "%Y-%m-%d", &exp);
                time_t exp_time = mktime(&exp);

                if (difftime(exp_time, now) <= 5 * 24 * 60 * 60) {
                    std::cout << ingredient.getName() << " is expiring in less than 5 days.\n";
                }
            }
        }
    }
};

class Pantry : public Storage {
public:
    void addIngredient(const Ingredient& ingredient) override {
        Storage::addIngredient(ingredient);
        std::cout << ingredient.getName() << " added to Pantry.\n";
    }

    void runningLow() const {
        for (const auto& ingredient : ingredients) {
            if (ingredient.getQuantity() < 2) {
                std::cout << ingredient.getName() << " is running low.\n";
            }
        }
    }
};

class Recipe {
private:
    std::string recipeName;
    std::vector<std::pair<std::string, std::string>> requiredIngredients;
    std::vector<std::pair<std::string, std::string>> condiments;
    std::vector<std::string> steps;
    std::string category;

public:
    Recipe(std::string name, 
           std::vector<std::pair<std::string, std::string>> ingredients, 
           std::vector<std::pair<std::string, std::string>> condiments, 
           std::vector<std::string> steps, std::string type)
        : recipeName(name), requiredIngredients(ingredients), condiments(condiments), steps(steps), category(type) {}

    std::string getRecipeName() const {
        return recipeName;
    }

    std::string getType() const {
        return category;
    }

    bool canMakeRecipe(const std::vector<Ingredient>& userIngredients, std::vector<std::string>& missingIngredients) const {
        bool hasAllMainIngredients = true;

        for (const auto& reqIngredient : requiredIngredients) {
            bool found = false;
            for (const auto& userIngredient : userIngredients) {
                std::string recipeIngredientName = reqIngredient.first;
                std::string userIngredientName = userIngredient.getName();
                std::transform(recipeIngredientName.begin(), recipeIngredientName.end(), recipeIngredientName.begin(), ::tolower);
                std::transform(userIngredientName.begin(), userIngredientName.end(), userIngredientName.begin(), ::tolower);

                if (userIngredientName == recipeIngredientName) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                missingIngredients.push_back(reqIngredient.first);
                hasAllMainIngredients = false;
            }
        }

        return true;
    }

    std::vector<std::pair<std::string, std::string>> getRequiredIngredients() const {
        return requiredIngredients;
    }

    std::vector<std::pair<std::string, std::string>> getCondiments() const {
        return condiments;
    }

    std::vector<std::string> getSteps() const {
        return steps;
    }
};

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
        string category = recipeData["category"];
        vector<pair<string, string>> ingredients;
        vector<pair<string, string>> condiments;
        vector<string> steps;

        for (const auto& ingredient : recipeData["ingredients"]) {
            string name = ingredient["name"];
            string quantity = ingredient["quantity"];
            string unit = ingredient["unit"];
            ingredients.push_back({ name, quantity + " " + unit });
        }

        for (const auto& condiment : recipeData["condiments"]) {
            string name = condiment["name"];
            string quantity = condiment["quantity"];
            string unit = condiment["unit"];
            condiments.push_back({ name, quantity + " " + unit });
        }

        for (const auto& step : recipeData["steps"]) {
            steps.push_back(step);
        }

        recipes.emplace_back(recipeName, ingredients, condiments, steps, category);
    }

    return recipes;
}

class RecipeManager {
private:
    Fridge fridge;
    Pantry pantry;
    std::vector<Recipe> recipes;

    void saveHistory(const Recipe& recipe) {
        std::ifstream infile("history.json");
        json history;

        // If the file is empty or doesn't exist, start a new array
        if (infile.is_open() && infile.peek() != std::ifstream::traits_type::eof()) {
            infile >> history;
        }
        infile.close();

        // If the history is not an array, reset it
        if (!history.is_array()) {
            history = json::array();
        }

        // Append the new recipe name to the history
        json j;
        j["name"] = recipe.getRecipeName();
        history.push_back(j);

        // Write the updated history back to the file
        std::ofstream outfile("history.json");
        if (outfile.is_open()) {
            outfile << history.dump(4);  // Pretty print with indent of 4 spaces
            outfile.close();
        }
    }

public:
    RecipeManager(const std::string& recipeFilename) {
        recipes = loadRecipesFromJSON(recipeFilename);
        loadIngredientsFromFile("storage.json");
    }

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

    void collectIngredients() {
        while (true) {
            std::string name;
            int quantity;
            std::cout << "Please enter the ingredient name (or type 'done' to finish): ";
            std::cin.ignore();
            std::getline(std::cin, name);
            if (name == "done") break;

            std::cout << "Enter the quantity of " << name << ": ";
            std::cin >> quantity;
            Ingredient newIngredient(name, quantity);

            std::string storageLocation;
            std::cout << "Is the ingredient stored in the (F)ridge or (P)antry? ";
            std::cin >> storageLocation;

            if (storageLocation == "F" || storageLocation == "f") {
                std::string expirationDate;
                std::cout << "Enter the expiration date (YYYY-MM-DD): ";
                std::cin >> expirationDate;
                newIngredient = Ingredient(name, quantity, expirationDate);
                fridge.addIngredient(newIngredient);
            } else if (storageLocation == "P" || storageLocation == "p") {
                pantry.addIngredient(newIngredient);
            } else {
                std::cout << "Invalid option. Please choose (F)ridge or (P)antry.\n";
                continue;
            }

            saveIngredientsToFile("storage.json");
        }
    }

   void matchRecipes() {
    std::string recipeType;
    std::cout << "Do you want a (S)weet or (Sa)vory recipe? ";
    std::cin >> recipeType;

    // Convert the recipeType input to lowercase
    std::transform(recipeType.begin(), recipeType.end(), recipeType.begin(), ::tolower);

    std::vector<Ingredient> allIngredients = fridge.getIngredients();
    allIngredients.insert(allIngredients.end(), pantry.getIngredients().begin(), pantry.getIngredients().end());

    std::vector<std::string> possibleRecipes;
    std::vector<const Recipe*> matchingRecipes;

    for (const auto& recipe : recipes) {
        // Convert recipe category to lowercase to ensure case-insensitive comparison
        std::string category = recipe.getType();
        std::transform(category.begin(), category.end(), category.begin(), ::tolower);

        // Check if the recipe matches the desired type (sweet or savory)
        if ((recipeType == "s" && category == "sweet") || (recipeType == "sa" && category == "savory")) {
            std::vector<std::string> missingIngredients;
            if (recipe.canMakeRecipe(allIngredients, missingIngredients)) {
                possibleRecipes.push_back(recipe.getRecipeName());
                matchingRecipes.push_back(&recipe);
            } else if (!missingIngredients.empty()) {
                std::cout << "You are missing the following ingredients for " << recipe.getRecipeName() << ": ";
                for (const auto& ingredient : missingIngredients) {
                    std::cout << ingredient << " ";
                }
                std::cout << "\n";
            }
        }
    }

    if (possibleRecipes.empty()) {
        std::cout << "Sorry, you cannot make any recipes with the ingredients you have.\n";
    } else {
        std::cout << "Based on your ingredients, you can make the following recipes:\n";
        for (size_t i = 0; i < possibleRecipes.size(); ++i) {
            std::cout << i + 1 << ". " << possibleRecipes[i] << "\n";
        }

        int choice;
        std::cout << "Enter the number of the recipe you want to see in full: ";
        std::cin >> choice;

        if (choice > 0 && choice <= matchingRecipes.size()) {
            displayFullRecipe(*matchingRecipes[choice - 1]);
            saveHistory(*matchingRecipes[choice - 1]);
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
}

    void displayFullRecipe(const Recipe& recipe) {
        std::cout << "Recipe: " << recipe.getRecipeName() << "\n";
        std::cout << "Ingredients:\n";
        for (const auto& ingredient : recipe.getRequiredIngredients()) {
            std::cout << "- " << ingredient.first << ": " << ingredient.second << "\n";
        }

        std::cout << "Steps:\n";
        for (const auto& step : recipe.getSteps()) {
            std::cout << "- " << step << "\n";
        }
    }

    void saveIngredientsToFile(const std::string& filename) {
        json j;
        j["Fridge"] = fridge.toJSON();
        j["Pantry"] = pantry.toJSON();

        std::ofstream file(filename);
        if (file.is_open()) {
            file << j.dump(4);
            file.close();
            std::cout << "Ingredients saved to " << filename << "\n";
        } else {
            std::cerr << "Unable to open file " << filename << "\n";
        }
    }

    void menu() {
        int option;
        do {
            std::cout << "What would you like to do?\n";
            std::cout << "1. Add ingredients\n2. Generate recipes\n3. View recipe history\n4. Check notifications (expiring soon and running low)\n5. Exit\n";
            std::cin >> option;

            switch (option) {
                case 1:
                    collectIngredients();
                    break;
                case 2:
                    matchRecipes();
                    break;
                case 3:
                    viewRecipeHistory();
                    break;
                case 4:
                    fridge.expiringSoon();
                    pantry.runningLow();
                    break;
                case 5:
                    std::cout << "Goodbye!\n";
                    break;
                default:
                    std::cout << "Invalid option. Please try again.\n";
                    break;
            }
        } while (option != 5);
    }

    void viewRecipeHistory() {
        std::ifstream file("history.json");
        
        if (!file.is_open()) {
            std::cerr << "History file does not exist. Initializing history.\n";
            // Create an empty history file if it doesn't exist
            std::ofstream outfile("history.json");
            outfile << "[]";  // Empty array
            outfile.close();
            return;
        }

        json history;
        try {
            file >> history;
        } catch (json::parse_error& e) {
            std::cerr << "Error parsing history file: " << e.what() << "\n";
            std::cerr << "Resetting history to an empty array.\n";
            std::ofstream outfile("history.json");
            outfile << "[]";  // Reset to empty array
            outfile.close();
            return;
        }

        if (!history.is_array()) {
            std::cerr << "History format is incorrect, resetting history to an empty array.\n";
            std::ofstream outfile("history.json");
            outfile << "[]";  // Reset to empty array
            outfile.close();
            return;
        }

        if (history.empty()) {
            std::cout << "No recipe history found.\n";
            return;
        }

        std::cout << "Recipe History:\n";
        for (size_t i = 0; i < history.size(); ++i) {
            if (history[i].contains("name") && history[i]["name"].is_string()) {
                std::cout << i + 1 << ". " << history[i]["name"] << "\n";
            } else {
                std::cout << i + 1 << ". Invalid entry in history.\n";
            }
        }

        int choice;
        std::cout << "Enter the number of the recipe to view details or 0 to go back to the main menu: ";
        std::cin >> choice;

        if (choice > 0 && choice <= history.size()) {
            std::string selectedRecipeName = history[choice - 1]["name"];
            for (const auto& recipe : recipes) {
                if (recipe.getRecipeName() == selectedRecipeName) {
                    displayFullRecipe(recipe);
                    break;
                }
            }
        } else if (choice == 0) {
            std::cout << "Returning to the main menu.\n";
        } else {
            std::cout << "Invalid choice. Returning to the main menu.\n";
        }

        file.close();
    }
};

int main() {
    std::string recipeFilename = "recipes.json";
    RecipeManager manager(recipeFilename);

    manager.menu();

    return 0;
}
