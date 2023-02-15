#include <iostream> // std::cout, std::endl
#include <chrono> // std::chrono::high_resolution_clock
#include <vector> // std::vector
#include <unordered_map> // std::unordered_map

#include "argparse.hxx"
#include "format.hxx"
#include "model.hxx"
#include "option.hxx"
#include "quantmc.hxx"

int main(int argc, char** argv)
{
    // PARSE CMD LINE ARGS
    std::vector<std::string> args(argv + 1, argv + argc); // We're not interested in the value of argc
    std::unordered_map<std::string, std::string> cmd_option_map = parse_cmd_options(args, options);
    if (cmd_option_map.size())
    {
        std::cout << "Using following cmd line args:\n";
    }
    for (auto const& [k, v] : cmd_option_map)
    {
        std::cout << format("\t{} => {}\n", k, v);
    };

    // INITIALIZE MODEL, OPTION, MC SETTINGS
    double r = cmd_option_map.count("--rate") > 0 ? std::stod(cmd_option_map["--rate"]) : 0.05;
    double vol = cmd_option_map.count("--vol") > 0 ? std::stod(cmd_option_map["--vol"]) : 0.2;
    auto model = BS(r, vol);

    double k = cmd_option_map.count("--strike") > 0 ? std::stod(cmd_option_map["--strike"]) : 110;
    double t = cmd_option_map.count("--ttm") > 0 ? std::stod(cmd_option_map["--ttm"]) : 1.0;

    auto callopt = EuropeanCall(k, t);
    auto putopt = EuropeanPut(k, t);

    double s0 = cmd_option_map.count("--spot") > 0 ? std::stod(cmd_option_map["--spot"]) : 100;
    uint npaths = cmd_option_map.count("--npaths") > 0 ? std::stoul(cmd_option_map["--npaths"]) : 10000;
    uint nsteps = cmd_option_map.count("--nsteps") > 0 ? std::stoul(cmd_option_map["--nsteps"]) : 360;

    // COMPUTE CALL & PUT MC VALUES
    std::cout << "Using seed " << seed << std::endl;
    auto time1 = std::chrono::high_resolution_clock::now().time_since_epoch();
    double val1 = compute_mc_price(model, callopt, s0, npaths, nsteps);
    auto time2 = std::chrono::high_resolution_clock::now().time_since_epoch();
    ulong duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count();
    double val2 = compute_mc_price(model, putopt, s0, npaths, nsteps);
    auto time3 = std::chrono::high_resolution_clock::now().time_since_epoch();
    ulong duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(time3 - time2).count();

    // PRINT RESULTS
    std::cout << format("[CALL] MC price = {}, pricing duration = {}ms\n", val1, duration1);
    std::cout << format("[PUT] MC price = {}, pricing duration = {}ms\n", val2, duration2);
    std::cout << duration2;

    return 0;
};