#include <iostream>
#include <cstdlib>
#include <ctime>

/*#include <numeric>
#include <thread>
#include <atomic>
#include <chrono>
#include <inttypes.h>*/

#include <gegelati.h>
#define _USE_MATH_DEFINES // To get M_PI

class MultByConstParam;

#include <cmath>

#include "../include/game2048.h"

// TPG training
int main() {

    std::cout << "Start 2048 application." << std::endl;

    // Create the instruction set for programs
    /*Instructions::Set set;
    auto minus = [](double a, double b)->double {return a - b; };
    auto add = [](double a, double b)->double {return a + b; };
    auto mult = [](double a, double b)->double {return a * b; };
    auto div = [](double a, double b)->double {return a / b; };
    auto max = [](double a, double b)->double {return std::max(a, b); };
    auto ln = [](double a)->double {return std::log(a); };
    auto exp = [](double a)->double {return std::exp(a); };
    auto cos = [](double a)->double {return std::cos(a); };
    auto sin = [](double a)->double {return std::sin(a); };
    auto tan = [](double a)->double {return std::tan(a); };
    auto pi = [](double a)->double {return M_PI; };

    set.add(*(new Instructions::LambdaInstruction<double, double>(minus)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(add)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(mult)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(div)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(max)));
    set.add(*(new Instructions::LambdaInstruction<double>(exp)));
    set.add(*(new Instructions::LambdaInstruction<double>(ln)));
    set.add(*(new Instructions::LambdaInstruction<double>(cos)));
    set.add(*(new Instructions::LambdaInstruction<double>(sin)));
    set.add(*(new Instructions::LambdaInstruction<double>(tan)));
    set.add(*(new Instructions::MultByConstant<double>()));     // float ?
    set.add(*(new Instructions::LambdaInstruction<double>(pi)));*/

    // Initialisation

    auto game = new game2048();
    //initGame(); // Done in default contructor
    game->printUI();
    char command;

    // Main loop which break if user lost, won or enter the leave char (L or l)
    while (true)
    {
        std::cout << "Choose a direction : ";
        std::cin >> command;
        if((command == 76 || command == 108) || !game->move(command)) // ('l' or 'L' to leave) or (lost or win)
        {
            std::cout << "Leaving the Game" << std::endl;
            break;
        }
        game->printUI();
    }
    delete game;
    return 0;
}



// Playing as the user
/*int main() {
    // reset random's seed ... ?
    srand(time(0));

    // Initialisation

    auto game = new game2048();
    //initGame(); // Done in default contructor
    game->printUI();
    char command;

    // Main loop which break if user lost, won or enter the leave char (L or l)
    while (true)
    {
        std::cout << "Choose a direction : ";
        std::cin >> command;
        if((command == 76 || command == 108) || !game->move(command)) // ('l' or 'L' to leave) or (lost or win)
        {
            std::cout << "Leaving the Game" << std::endl;
            break;
        }
        game->printUI();
    }
    delete game;
    return 0;
}*/
