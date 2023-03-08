#pragma once
#include <regex>
#include <string>


class Strings
{
public:
    static bool matches(const std::string& s, const std::string& pattern)
    {
        std::regex pattern_regex(pattern);
        return std::regex_search(std::string(s), pattern_regex);
    }

    static std::string wildcardToRegex(const std::string& s)
    {
        std::string res = std::regex_replace(s, std::regex("\\."), "\\.");
        res             = std::regex_replace(res, std::regex("\\?"), ".");

        return std::regex_replace(res, std::regex("\\*"), ".*");
    }
    static bool matchesWildCard(const std::string& s, const std::string& wildcardPattern, bool caseSensitive = true)
    {
        return std::regex_match(s, std::regex(wildcardToRegex(wildcardPattern)));
    }

    static std::vector<std::string_view> tokenize(
        std::string_view       str,
        const std::string_view delims = " ")
    {


        std::vector<std::string_view> output;
        size_t                        first = 0;

        while (first < str.size())
        {
            const auto second = str.find_first_of(delims, first);

            if (first != second)
                output.emplace_back(str.substr(first, second - first));

            if (second == std::string_view::npos)
                break;

            first = second + 1;
        }

        return output;
    }

    // TODO: requires(ContainerType a) or // template <template <typename... Args> class Container, typename, typename... Types>

    template <typename T>
    static std::string concatWithSeparator(const T& c, std::string_view separator)
    {
        std::string result;
        for (auto it = c.begin(); it != c.end(); ++it)
        {
            result += delegateToString(*it);
            if (std::next(it) != c.end())
            {
                result += separator;
            }
        }
        return result;
    }

private:
    // delegate to "to_string" as needed, arithmetic should cover int/float types
    template <class T>
        requires std::is_arithmetic_v<T>
    static std::string delegateToString(const T& t)
    {
        return std::to_string(t);
    }
    //implicit conversion
    template <class T>
        requires(! std::is_fundamental_v<T>)
    static std::string delegateToString(const T& s)
    {
        return (s);
    }
};
