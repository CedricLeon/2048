#include <iostream>
#include <cstdlib>
#include <ctime>

#define _USE_MATH_DEFINES // To get M_PI

#include <math.h>
#include <numeric>
#include <thread>
#include <atomic>
#include <chrono>
#include <inttypes.h>

#include <gegelati.h>

#include "../include/game2048.h"

#ifndef NB_GENERATIONS
#define NB_GENERATIONS 1200
#endif

void getKey(std::atomic<bool>& exit) {
    std::cout << std::endl;
    std::cout << "Press `q` then [Enter] to exit." << std::endl;
    std::cout.flush();

    exit = false;

    while (!exit) {
        char c;
        std::cin >> c;
        switch (c) {
        case 'q':
        case 'Q':
            exit = true;
            break;
        default:
            printf("Invalid key '%c' pressed.", c);
            std::cout.flush();
        }
    }

    printf("Program will terminate at the end of next generation.\n");
    std::cout.flush();
}

// TPG training
int main() {

    std::cout << "Start 2048 application." << std::endl;

    // Create the instruction set for programs
    Instructions::Set set;
    auto minus = [](double a, double b)->double {return a - b; };
    auto add = [](double a, double b)->double {return a + b; };
    auto mult = [](double a, double b)->double {return a * b; };
    auto div = [](double a, double b)->double {return a / b; };
    auto max = [](double a, double b)->double {return std::max(a, b); };
    auto ln = [](double a)->double {return std::log(a); };
    auto exp = [](double a)->double {return std::exp(a); };

    auto minus_int = [](int a, int b)->int {return a - b; };
    auto add_int = [](int a, int b)->int {return a + b; };
    auto mult_int = [](int a, int b)->int {return a * b; };
    //auto div_int = [](int a, int b)->int {return a / b; };
    auto max_int = [](int a, int b)->int {return std::max(a, b); };
    auto ln_int = [](int a)->int {return std::log(a); };
    auto exp_int = [](int a)->int {return std::exp(a); };

    set.add(*(new Instructions::LambdaInstruction<double, double>(minus)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(add)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(mult)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(div)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(max)));
    set.add(*(new Instructions::LambdaInstruction<double>(exp)));
    set.add(*(new Instructions::LambdaInstruction<double>(ln)));

    set.add(*(new Instructions::LambdaInstruction<int, int>(minus_int)));
    set.add(*(new Instructions::LambdaInstruction<int, int>(add_int)));
    set.add(*(new Instructions::LambdaInstruction<int, int>(mult_int)));
    //set.add(*(new Instructions::LambdaInstruction<int, int>(div_int)));
    set.add(*(new Instructions::LambdaInstruction<int, int>(max_int)));
    set.add(*(new Instructions::LambdaInstruction<int>(exp_int)));
    set.add(*(new Instructions::LambdaInstruction<int>(ln_int)));

    // Initialisation
    Learn::LearningParameters params;
    File::ParametersParser::loadParametersFromJson(ROOT_DIR "/params.json", params);

    // Instantiate the LearningEnvironment
    // (Z : 90), Right (D : 68), Down (S : 83) or Left (Q : 81)
    game2048 gameLE({90, 68, 83, 81});

    std::cout << "Number of threads: " << std::thread::hardware_concurrency() << std::endl;

    // Instantiate and init the learning agent
    Learn::ParallelLearningAgent la(gameLE, set, params);
    //Learn::LearningAgent la(gameLE, set, params);
    la.init();

    const TPG::TPGVertex* bestRoot = NULL;

    // Start a thread for controlling the loop
#ifndef NO_CONSOLE_CONTROL
    // Console
    std::atomic<bool> exitProgram = true; // (set to false by other thread)
    std::atomic<uint64_t> generation = 0;

    std::thread threadKeyboard(getKey, std::ref(exitProgram));

    while (exitProgram); // Wait for other thread to print key info.
#else
    std::atomic<bool> exitProgram = false;
#endif

    // Basic logger
    Log::LABasicLogger basicLogger(la);

    // Create an exporter for all graphs
    File::TPGGraphDotExporter dotExporter("out_0000.dot", la.getTPGGraph());

    // Logging best policy stat.
    std::ofstream stats;
    stats.open("bestPolicyStats.md");
    Log::LAPolicyStatsLogger policyStatsLogger(la, stats);

    for (int i = 0; i < NB_GENERATIONS && !exitProgram; i++)
    {
        char buff[13];
        sprintf(buff, "out_%04d.dot", i);
        dotExporter.setNewFilePath(buff);
        dotExporter.print();

        la.trainOneGeneration(i);
    }

    // Keep best policy
    la.keepBestPolicy();
    dotExporter.setNewFilePath("out_best.dot");
    dotExporter.print();

    TPG::PolicyStats ps;
    ps.setEnvironment(la.getTPGGraph().getEnvironment());
    ps.analyzePolicy(la.getBestRoot().first);
    std::ofstream bestStats;
    bestStats.open("out_best_stats.md");
    bestStats << ps;
    bestStats.close();
    stats.close();

    // cleanup
    for (unsigned int i = 0; i < set.getNbInstructions(); i++)
        delete (&set.getInstruction(i));

#ifndef NO_CONSOLE_CONTROL
    // Exit the thread
    std::cout << "Exiting program, press a key then [enter] to exit if nothing happens.";
    threadKeyboard.join();
#endif

    return 0;
}



// Playing as the user
/*int main() {
    // reset random's seed ... ?
    srand(time(0));

    // Initialisation

    game2048 game({90, 68, 83, 81});
    //initGame(); // Done in default contructor
    game.printUI();
    char command;

    // Main loop which break if user lost, won or enter the leave char (L or l)
    while (true)
    {
        std::cout << "Choose a direction : ";
        std::cin >> command;
        if((command == 76 || command == 108) || !game.move(command)) // ('l' or 'L' to leave) or (lost or win)
        {
            std::cout << "Leaving the Game" << std::endl;
            break;
        }
        game.printUI();
    }
    return 0;
}*/
