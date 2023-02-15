#ifndef FORMAT_HXX
#define FORMAT_HXX

#include <algorithm> // std::find
#include <string_view> // std::string_view
#include <string> // std::string
#include <sstream> // std::ostringstream

template<typename... Ts>
std::string format(std::string_view str_view, Ts... values)
{
    std::ostringstream oss;
    auto format_single = [&oss, &str_view] (auto value)
        {
            std::size_t open_bracket_pos = str_view.find('{');
            if (open_bracket_pos == std::string::npos)
            {
                return;
            }
            else
            {
                std::size_t close_bracket_pos = str_view.find('}', open_bracket_pos + 1);
                if (close_bracket_pos == std::string::npos) 
                { 
                    return;
                }
                else
                {
                    oss << str_view.substr(0, open_bracket_pos) << value;
                    str_view = str_view.substr(close_bracket_pos + 1);
                }
            }
        };
    (format_single(values),...);
    oss << str_view;
    return oss.str();
}

#endif // FORMAT_HXX