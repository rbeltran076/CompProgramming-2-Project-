#include <iostream> 
#include <vector>
#include <string>


using namespace std;

//Note that running this file alone produces an error as "ingredient" is not 
//defined here. This file is meant to store the ingredients class test code.
//The main() function below designed to ensure proper functionality of adding
//ingredients and quantities.

int main() {
   vector<Ingredient> ingredients;
  
   while (true) {
       Ingredient ingredient;
       if (ingredient.getQuantity() == -1) break;  // Ends the loop when "done" is entered
       ingredients.push_back(ingredient);
   }


   for (const auto& ingredient : ingredients) {
       cout << "Ingredient: " << ingredient.getName() << ", Quantity: " << ingredient.getQuantity() << endl;
       if (ingredient.getQuantity() < 2 && ingredient.getQuantity() > 0) {
           ingredient.isLowQuantity();
       }
   }


   return 0;
}
