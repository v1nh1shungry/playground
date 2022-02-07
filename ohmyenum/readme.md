# Introduction

`ohmyenum` is a [magic_enum](https://github.com/Neargye/magic_enum.git)-like static reflection library for enums for learning purpose, with fewer APIs and implemented with C++20.

* `enum_cast` obtains enum value from string or integer.
* `enum_value` returns enum value at specified index.
* `enum_values` obtains enum value sequence.
* `enum_count` returns number of enum values.
* `enum_integer` obtains integer value from enum value.
* `enum_name` returns name from enum value.
* `enum_names` obtains string enum name sequence.
* `enum_entries` obtains pair (value enum, string enum name) sequence.
* `enum_index` obtains index in enum value sequence from enum value.
* `enum_contains` checks whether enum contains enumerator with such value.
* `enum_type_name` returns name of enum type.
* `is_unscoped_enum` checks whether type is an [Unscoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Unscoped_enumeration).
* `is_scoped_enum` checks whether type is an [Scoped enumeration](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations).
* `ostream_operators` ostream operators for enums.

# Attention

I have to admit that I have picked some codes from `magic_enum`. And I have no intention of plagiarizing magic_ enum. It is just for learning purpose.

# How to achieve this

If you can read Chinese, I wrote an article about this. Check it out: [magic_enum：C++ enum静态反射库](https://zhuanlan.zhihu.com/p/464758750).

Or `ohmyenum` is well commented and pretty simple, you can go ahead and check out the codes.

# Acknowledge

Great thanks to `magic_enum` for providing us such a beautiful library.
