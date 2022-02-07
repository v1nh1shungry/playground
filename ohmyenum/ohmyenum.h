#ifndef OHMYENUM_H
#define OHMYENUM_H

#include <concepts>
#include <string_view>
#include <limits>
#include <array>
#include <cassert>
#include <optional>
#include <iostream>

namespace ohmyenum {

    namespace detail {

        /**
         * The default range of enum value
         * @details
         *  The enum value must be greater than or equal to ENUM_RANGE_MIN,
         *  and less than or equal to ENUM_RANGE_MAX.
         *  By default, ENUM_RANGE_MIN = -128, ENUM_RANGE_MAX = 128.
         *  That is, enum value must be in [-128, 127].
         */
        inline constexpr int ENUM_RANGE_MIN = -128;
        inline constexpr int ENUM_RANGE_MAX = 127;

        /**
         * The pure enum type concept
         * @attention No cv-qualifier, no reference.
         */
        template<class T>
        concept enumerable = std::is_enum_v<T> && std::is_same_v<T, std::decay_t<T>>;

        /**
         * A helper function that get enum value name or enum type name from the string
         * @return If the name is valid, return the name, or return empty string_view.
         * @details
         *  This function is used by enum_type_name and enum_value_name. And it supports
         *  only ASCII.
         */
        constexpr std::string_view pretty_name(std::string_view name) noexcept {
            for (auto i = name.size(); 0 < i; --i) {
                char c = name[i - 1];
                if (!(('a' <= c && c <= 'z')
                      || ('A' <= c && c <= 'Z')
                      || ('0' <= c && c <= '9')
                      || ('_' == c))) {
                    name.remove_prefix(i);
                    break;
                }
            }

            if (!name.empty()) {
                if (char c = name.front(); (('a' <= c && c <= 'z')
                                            || ('A' <= c && c <= 'Z')
                                            || ('_' == c))) {
                    return name;
                }
            }

            return {};
        }

        /**
         * Get the type name of the enum type
         * @details
         *  This works by using the compiler extension. The macro will be replaced by
         *  a human-readable function signature by the compiler. And the template arguments
         *  will also be contained in this macro. So we can use it to get the enum type name.
         * @attention
         *  If the enum type doesn't own a name, that is, an anonymous enum type, we will
         *  get an empty string_view. If you're confused, see what the macro will be if we
         *  pass an anonymous enum into it. \n
         *  The return type of this function must be auto, even if we know it is std::string_view.
         */
        template<enumerable E>
        constexpr auto enum_type_name() noexcept {
#  if defined(__clang__) || defined(__GNUC__)
            constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#  elif defined(_MSC_VER)
            constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#  endif
            return name;
        }

        template<enumerable E>
        inline constexpr auto enum_type_name_v = enum_type_name<E>();

        /**
         * Get the name of the enum value
         * @details
         *  It works just like enum_type_name, check it.
         */
        template<enumerable E, E e>
        constexpr auto enum_value_name() noexcept {
#  if defined(__clang__) || defined(__GNUC__)
            constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#  elif defined(_MSC_VER)
            constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#  endif
            return name;
        }

        template<enumerable E, E e>
        inline constexpr auto enum_value_name_v = enum_value_name<E, e>();

        /**
         * Check if the enum value is valid
         * @details
         *  It works by using enum_value_name. The function enum_value_name returns
         *  empty string_view if the enum_value is invalid.
         */
        template<enumerable E, E e>
        constexpr bool is_valid_enum() noexcept {
            return !enum_value_name<E, e>().empty();
        }

        template<enumerable E, E e>
        inline constexpr bool is_valid_enum_v = is_valid_enum<E, e>();

        /**
         * A helper function helps enum_values to create an enum value
         * @details
         *  For reasons that this function constructs an enum by MIN + i instead of just i,
         *  check the function enum_values.
         */
        template<enumerable E, int MIN>
        requires (!std::is_same_v<std::underlying_type_t<E>, bool> || MIN == 0)
        constexpr E enum_value(std::size_t i) noexcept {
            return static_cast<E>(static_cast<int>(i) + MIN);
        }

        /**
         * Compare the two integers may be from two different integral type
         * @details
         *  This function is mostly picked from magic_enum.
         *  See https://github.com/Neargye/magic_enum/blob/master/include/magic_enum.hpp#L298
         */
        template<std::integral L, std::integral R>
        constexpr bool less_than(L lhs, R rhs) noexcept {
            if constexpr (std::is_signed_v<L> == std::is_signed_v<R>) {
                // If same signedness (both signed or both unsigned).
                return lhs < rhs;
            } else if constexpr (std::is_same_v<L, bool>) { // bool special case
                return static_cast<R>(lhs) < rhs;
            } else if constexpr (std::is_same_v<R, bool>) { // bool special case
                return lhs < static_cast<L>(rhs);
            } else if constexpr (std::is_signed_v<R>) {
                // If 'right' is negative, then result is 'false', otherwise cast & compare.
                return rhs > 0 && lhs < static_cast<std::make_unsigned_t<R>>(rhs);
            } else {
                // If 'left' is negative, then result is 'true', otherwise cast & compare.
                return lhs < 0 || static_cast<std::make_unsigned_t<L>>(lhs) < rhs;
            }
        }

        /**
         * Pick the maximum between ENUM_RANGE_MIN and the minimum of the underlying type of
         * the enum type
         */
        template<enumerable E>
        constexpr int reflected_min() noexcept {
            using U = std::underlying_type_t<E>;

            constexpr auto underlying_limit = (std::numeric_limits<U>::min)();
            return less_than(ENUM_RANGE_MIN, underlying_limit) ? underlying_limit : ENUM_RANGE_MIN;
        }

        /**
         * Pick the minimum between ENUM_RANGE_MAX and the maximum of the underlying type of
         * the enum type
         */
        template<enumerable E>
        constexpr int reflected_max() noexcept {
            using U = std::underlying_type_t<E>;

            constexpr auto underlying_limit = (std::numeric_limits<U>::max)();
            return less_than(ENUM_RANGE_MAX, underlying_limit) ? ENUM_RANGE_MAX : underlying_limit;
        }

        template<enumerable E>
        inline constexpr int reflected_min_v = reflected_min<E>();

        template<enumerable E>
        inline constexpr int reflected_max_v = reflected_max<E>();

        /**
         * A helper function that helps enum_values to count the valid enum value
         */
        template<std::size_t N>
        constexpr std::size_t enum_count(const bool (&valid)[N]) noexcept {
            std::size_t count = 0;
            for (auto &v: valid) {
                if (v) {
                    ++count;
                }
            }
            return count;
        }

        /**
         * Get all the valid enum value for the enum type
         * @details
         *  The index_sequence is used to construct a bool array contains the validation
         *  of enum value in [MIN, MIN + sizeof...(I)]. Check another enum_values function
         *  for more details.
         */
        template<enumerable E, int MIN, std::size_t ...I>
        constexpr auto enum_values(std::index_sequence<I...>) noexcept {
            constexpr bool valid[sizeof...(I)] = {is_valid_enum_v<E, enum_value<E, MIN>(I)>...};
            constexpr auto count = enum_count(valid);

            if constexpr (count == 0) {
                return std::array<E, 0>{};
            } else {
                std::array<E, count> res;
                for (std::size_t i = 0, v = 0; v < count; ++i) {
                    if (valid[i]) {
                        res[v++] = enum_value<E, MIN>(i);
                    }
                }
                return res;
            }
        }

        /**
         * A wrapper that drives another enum_values function to get all the valid enum values
         * @details
         *  It first gets the possible range of the enum type. Then it drives another
         *  enum_values function above to check the validation of all the values in the range
         *  to get all the valid enum values.
         */
        template<enumerable E>
        constexpr auto enum_values() noexcept {
            constexpr int min = reflected_min_v<E>;
            constexpr int max = reflected_max_v<E>;
            constexpr int range = max - min + 1;
            return enum_values<E, min>(std::make_index_sequence<range>{});
        }

        template<enumerable E>
        inline constexpr auto enum_values_v = enum_values<E>();

        template<enumerable E>
        inline constexpr auto enum_count_v = enum_values_v<E>.size();

        /**
         * The minimum value of the valid enum values
         */
        template<enumerable E>
        inline constexpr auto enum_min_v =
                0 < enum_count_v<E> ? static_cast<std::underlying_type_t<E>>(enum_values_v<E>.front())
                                    : std::underlying_type_t<E>{0};

        /**
         * The maximum value of the valid enum values
         */
        template<enumerable E>
        inline constexpr auto enum_max_v =
                0 < enum_count_v<E> ? static_cast<std::underlying_type_t<E>>(enum_values_v<E>.back())
                                    : std::underlying_type_t<E>{0};

        template<enumerable E, std::size_t ...I>
        constexpr auto enum_names(std::index_sequence<I...>) noexcept {
            return std::array<std::string_view, sizeof...(I)>{{enum_value_name_v<E, enum_values_v<E>[I]>...}};
        }

        template<enumerable E>
        inline constexpr auto enum_names_v = enum_names<E>(std::make_index_sequence<enum_count_v<E>>{});

        template<enumerable E, std::size_t ...I>
        constexpr auto enum_entries(std::index_sequence<I...>) noexcept {
            return std::array<std::pair<E, std::string_view>, sizeof...(I)>{
                    {{enum_values_v<E>[I], enum_names_v<E>[I]}...}};
        }

        template<enumerable E>
        inline constexpr auto enum_entries_v = enum_entries<E>(std::make_index_sequence<enum_count_v<E>>{});

        /**
         * Check if the type T is a scoped enum
         * @details
         *  A scoped enum is an enum type like this: \n
         *      enum class Color { RED, GREEN, BLUE }; \n
         *  It works because of the fact that the scoped enum cannot be converted to integer normally.
         *  Also this meta function accepts not only enum type, you can put any types in it.
         */
        template<class T>
        struct is_scoped_enum : std::false_type {
        };

        template<enumerable E>
        struct is_scoped_enum<E> : std::bool_constant<!std::is_convertible_v<E, std::underlying_type_t<E>>> {
        };

        template<class T>
        struct is_unscoped_enum : std::false_type {
        };

        template<enumerable E>
        struct is_unscoped_enum<E> : std::bool_constant<std::is_convertible_v<E, std::underlying_type_t<E>>> {
        };

        template<enumerable E>
        struct underlying_type : std::underlying_type<E> {
        };

    } // namespace ohmyenum::detail

    /**
     * The enum concept
     * @attention
     *  This concept is different from detail::enumerable. The cv-qualifier and reference is allowed.
     */
    template<class T>
    concept enumerable = detail::enumerable<std::decay_t<T>>;

    /**
     * Check if the type T is a scoped enum type
     */
    template<class T>
    struct is_scoped_enum : detail::is_scoped_enum<std::decay_t<T>> {
    };

    template<class T>
    inline constexpr auto is_scoped_enum_v = is_scoped_enum<T>::value;

    /**
     * Check if the type T is an unscoped enum type
     */
    template<class T>
    struct is_unscoped_enum : detail::is_unscoped_enum<std::decay_t<T>> {
    };

    template<class T>
    inline constexpr auto is_unscoped_enum_v = is_unscoped_enum<T>::value;

    /**
     * Get the underlying type of the enum type
     */
    template<enumerable E>
    struct underlying_type : detail::underlying_type<std::decay_t<E>> {
    };

    template<enumerable E>
    using underlying_type_t = typename underlying_type<E>::type;

    /**
     * Get the name of the enum type
     * @attention
     *  If the enum type doesn't own a name, that is, an anonymous enum type,
     *  the compilation will panic
     */
    template<enumerable E>
    requires (detail::enum_type_name_v<std::decay_t<E>>.size() != 0)
    constexpr auto enum_type_name() noexcept {
        return detail::enum_type_name_v<std::decay_t<E>>;
    }

    /**
     * Get the count of the enum values
     */
    template<enumerable E>
    constexpr auto enum_count() noexcept {
        return detail::enum_count_v<std::decay_t<E>>;
    }

    /**
     * Returns enum value at specified index.
     * @details
     *  The index is ordered by the value of the enum value.
     */
    template<enumerable E>
    constexpr auto enum_value(std::size_t index) noexcept {
        assert(index < enum_count<E>());
        return detail::enum_values_v<std::decay_t<E>>[index];
    }

    /**
     * Get std::array with enum values, sorted by enum value.
     */
    template<enumerable E>
    constexpr auto enum_values() noexcept {
        return detail::enum_values_v<std::decay_t<E>>;
    }

    /**
     * Get name from static storage enum variable.
     * @attention
     *  The compilation will panic if the enum value is invalid.
     */
    template<auto e>
    requires enumerable<std::decay_t<decltype(e)>>
             && detail::is_valid_enum_v<std::decay_t<decltype(e)>, e>
    constexpr auto enum_name() noexcept {
        return detail::enum_value_name_v<std::decay_t<decltype(e)>, e>;
    }

    /**
     * Get name from enum value.
     * @attention
     *  If the enum value is invalid, it will returns an empty string_view.
     */
    template<enumerable E>
    constexpr auto enum_name(E value) noexcept {
        using D = std::decay_t<E>;
        using U = underlying_type_t<E>;
        const auto v = static_cast<U>(value);
        const auto min = detail::enum_min_v<D>;
        const auto max = detail::enum_max_v<D>;
        if (min <= v && v <= max) {
            return detail::enum_names_v<E>[static_cast<std::size_t>(v - min)];
        }
        return std::string_view{};
    }

    /**
     * Returns std::array with names, sorted by enum value.
     */
    template<enumerable E>
    constexpr auto enum_names() noexcept {
        return detail::enum_names_v<std::decay_t<E>>;
    }

    /**
     * Returns std::array with pairs (value, name), sorted by enum value.
     */
    template<enumerable E>
    constexpr auto enum_entries() noexcept {
        return detail::enum_entries_v<std::decay_t<E>>;
    }

    /**
     * Obtains enum value from integer value.
     * @return optional with enum value.
     */
    template<enumerable E>
    constexpr std::optional<std::decay_t<E>> enum_cast(underlying_type_t<E> value) noexcept {
        using D = std::decay_t<E>;
        constexpr auto min = detail::enum_min_v<D>;
        constexpr auto max = detail::enum_max_v<D>;
        if (min <= value && value <= max) {
            return {static_cast<D>(value)};
        }
        return {};
    }

    /**
     * Obtains enum value from name.
     * @return optional with enum value
     */
    template<enumerable E>
    constexpr std::optional<std::decay_t<E>> enum_cast(std::string_view name) noexcept {
        constexpr auto values = enum_values<E>();
        for (std::size_t i = enum_count<E>(); 0 < i; --i) {
            if (name == enum_name(values[i - 1])) {
                return {values[i - 1]};
            }
        }
        return {};
    }

    /**
     * @return integer value from enum value.
     */
    template<enumerable E>
    constexpr underlying_type_t<E> enum_integer(E value) noexcept {
        return static_cast<underlying_type_t<E>>(value);
    }

    /**
     * Obtains index in enum values from enum value.
     * @return optional with index.
     */
    template<enumerable E>
    constexpr std::optional<std::size_t> enum_index(E value) noexcept {
        using D = std::decay_t<E>;
        using U = underlying_type_t<E>;
        const auto v = static_cast<U>(value);
        constexpr auto min = detail::enum_min_v<D>;
        constexpr auto max = detail::enum_max_v<D>;
        if (min <= v && v <= max) {
            return {static_cast<std::size_t>(v - min)};
        }
        return {};
    }

    /**
     * Checks whether enum contains enumerator with such enum value.
     */
    template<enumerable E>
    constexpr bool enum_contains(E value) noexcept {
        return enum_cast<E>(static_cast<std::decay_t<E>>(static_cast<underlying_type_t<E>>(value))).has_value();
    }

    /**
     * Checks whether enum contains enumerator with such integer value.
     */
    template<enumerable E>
    constexpr bool enum_contains(underlying_type_t<E> value) noexcept {
        return enum_cast<E>(value).has_value();
    }

    /**
     * Checks whether enum contains enumerator with such name.
     */
    template<enumerable E>
    constexpr bool enum_contains(std::string_view name) noexcept {
        return enum_cast<E>(name).has_value();
    }

    namespace ostream_operators {

        template<class Char, class Traits, enumerable E>
        std::basic_ostream<Char, Traits> &operator<<(std::basic_ostream<Char, Traits> &os, E value) {
            if (enum_contains<E>(value)) {
                os << enum_name(value);
            }
            return os;
        }

        template<class Char, class Traits, enumerable E>
        std::basic_ostream<Char, Traits> &operator<<(std::basic_ostream<Char, Traits> &os, std::optional<E> value) {
            return value.has_value() ? (os << value.value()) : os;
        }

    } // namespace ostream_operators

} // namespace ohmyenum

#endif // !OHMYENUM_H