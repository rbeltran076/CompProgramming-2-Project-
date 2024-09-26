#include <iostream> 
#include <vector>
#include <string>

using namespace std;

//notice use of Encapsulation and Ingredient() constructor to allocate resources
class Ingredient {
    private:
        string name;
        int quantity;
    public:
        Ingredient(){
            cout << "Please enter the ingredient name (or type 'done' to finish): "; 
            cin >> name;
            if (name != "done"){
                cout << "Enter the quantity you have of " << name << ": ";
                cin >> quantity;
                }
            else {
                quantity = -1; // i set this to -1 bc if quantity == 0, "ingredient is running low will print"
            }
        }

        string getName() const {   //this will be used in the getRecipes class
            return name;
        }

        int getQuantity() const {   //this will be used in the getRecipes class
            return quantity;
        }

        //decreaseQuantity() will be called when the user confirms they will make a certain recipe
        //in the RecipeManger class
        void decreaseQuantity() {
            quantity--;
        }

        //isLowQuantity() will print a "notification" if the quantity of an ingredient is running low
        //function will be called when -1 < quantity < 2
        void isLowQuantity() {
            cout << "The quantity of " << name << " is running low!";
        }


};