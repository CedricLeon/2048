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

    //auto cos = [](double a)->double {return std::cos(a); };
    //auto sin = [](double a)->double {return std::sin(a); };
    //auto tan = [](double a)->double {return std::tan(a); };
    //auto pi = [](double a)->double {return M_PI; };

    set.add(*(new Instructions::LambdaInstruction<double, double>(minus)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(add)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(mult)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(div)));
    set.add(*(new Instructions::LambdaInstruction<double, double>(max)));
    set.add(*(new Instructions::LambdaInstruction<double>(exp)));
    set.add(*(new Instructions::LambdaInstruction<double>(ln)));
    set.add(*(new Instructions::MultByConstant<double>()));     // <double, float>

    //set.add(*(new Instructions::LambdaInstruction<double>(cos)));
    //set.add(*(new Instructions::LambdaInstruction<double>(sin)));
    //set.add(*(new Instructions::LambdaInstruction<double>(tan)));
    //set.add(*(new Instructions::LambdaInstruction<double>(pi)));

    // Initialisation
    Learn::LearningParameters params;
    File::ParametersParser::loadParametersFromJson(ROOT_DIR "/params.json", params);

    // Instantiate the LearningEnvironment
    // (Z : 90), Down (S : 83), Right (D : 68) or Left (Q : 81)
    game2048 gameLE({90, 83, 68, 81});

    std::cout << "Number of threads: " << std::thread::hardware_concurrency() << std::endl;

    // Instantiate and init the learning agent
    Learn::ParallelLearningAgent la(gameLE, set, params);
    la.init();

    const TPG::TPGVertex* bestRoot = NULL;

    // Start a thread for controlling the loop
#ifndef NO_CONSOLE_CONTROL
    // Console
    std::atomic<bool> exitProgram = true; // (set to false by other thread)
    std::atomic<bool> toggleDisplay = true;
    std::atomic<bool> doDisplay = false;
    std::atomic<uint64_t> generation = 0;

    //std::thread threadDisplay(std::ref(exitProgram), std::ref(toggleDisplay), std::ref(doDisplay), &bestRoot, std::ref(set), std::ref(gameLE), std::ref(params), std::ref(generation));

    while (exitProgram); // Wait for other thread to print key info.
#else
    std::atomic<bool> exitProgram = false; // (set to false by other thread)
	std::atomic<bool> toggleDisplay = false;
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

#ifndef NO_CONSOLE_CONTROL
        generation = i;
        if (toggleDisplay && !exitProgram)
        {
            bestRoot = la.getBestRoot().first;
            doDisplay = true;
            while (doDisplay && !exitProgram);
        }
#endif
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
    //threadDisplay.join();
#endif

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
