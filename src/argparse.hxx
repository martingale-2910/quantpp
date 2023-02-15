#ifndef ARGPARSE_HXX
#define ARGPARSE_HXX

#include <algorithm> // std::find
#include <string_view> // std::string_view
#include <string> // std::string
#include <sstream> // std::ostringstream
#include <vector> // std::vector
#include <unordered_map> // std::unordered_map

// TODO: Add --seed cmd line opt + add handling of --right cmd line opt
static const std::vector<std::string> options{"--rate", "--vol", "--spot", "--strike", "--ttm", "--right", "--npaths", "--nsteps"};

std::unordered_map<std::string, std::string> parse_cmd_options(std::vector<std::string> const & args, std::vector<std::string> const & options)
{
    auto cmd_opt_map = std::unordered_map<std::string, std::string>();
    for(auto it = args.begin(); it != args.end(); ++it)
    {
        bool opt_is_present = std::find(options.begin(), options.end(), (*it)) != options.end();
        if (opt_is_present)
        {   
            auto val_it = next(it);
            bool val_is_not_opt = std::find(options.begin(), options.end(), (*val_it)) == options.end();
            if (val_is_not_opt && val_it != args.end() && !(*val_it).empty())
            {
                cmd_opt_map.emplace(*it, *(val_it));
                it = val_it;
            }
        };
    }
    return cmd_opt_map;
}

std::string parse_cmd_option(std::vector<std::string> const & args, std::string const & option)
{
    auto option_it = std::find(args.begin(), args.end(), option);
    if (option_it != args.end() && (++option_it) != args.end()) // we check that the iterator or the next one isnt the end
    {
        return (*option_it);
    }
    return "";
};

bool cmd_option_exists(std::vector<std::string> const & args, std::string const & option)
{
    return (std::find(args.begin(), args.end(), option) != args.end());
};

#endif // ARGPARSE_HXX