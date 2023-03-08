#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>



namespace common
{

template <class T, typename... A>
class GenericFactory
{
public:
    using CtorFunc = std::function<typename std::unique_ptr<typename std::remove_pointer_t<T>>(A...)>;


    static auto proto()
    {
        return [](A... a) { return std::make_unique<T>(a...); };
    }

    GenericFactory()  = default;
    ~GenericFactory() = default;

    /**
     * \brief register a prototype
     * \example:
     * GenericFactory<X> f;
     * f.registerPrototype("kitty", GenericFactory<X>::proto());
     */
    bool registerPrototype(std::string_view key, CtorFunc&& constructor_function)
    {
        auto ret = registeredConstructors.emplace(key, std::move(constructor_function));
        return ret.second;
    }

    /**
     * \brief	Gets module constructor by name
     * \param	type_name	Name of the type you want to build.
     * \return	The constructor of that type.
     */
    const CtorFunc& getPrototype(std::string_view type_name) const
    {
        return registeredConstructors.at(std::string(type_name));
    }

    /**
     * \brief construct an instance of the registered type
     * \param key
     */
    auto make(std::string_view key, A... a) const
    {
        return getPrototype(key)(a...);
    }

private:
    std::map<std::string, CtorFunc, std::less<>> registeredConstructors; ///< Mapping of key to constructors
};


} // namespace common
