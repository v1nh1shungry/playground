#include <utility>

template <class Left, class Right> struct merge;
template <auto... Ls, auto... Rs>
struct merge<std::index_sequence<Ls...>, std::index_sequence<Rs...>>
    : std::type_identity<std::index_sequence<Ls..., Rs...>> {};

template <template <auto> class F, class Seq> struct filter;
template <template <auto> class F>
struct filter<F, std::index_sequence<>>
    : std::type_identity<std::index_sequence<>> {};
template <template <auto> class F, auto Head, auto... Tail>
struct filter<F, std::index_sequence<Head, Tail...>>
    : merge<std::conditional_t<F<Head>::value, std::index_sequence<Head>,
                               std::index_sequence<>>,
            typename filter<F, std::index_sequence<Tail...>>::type> {};

template <class Seq> struct quick_sort;
template <>
struct quick_sort<std::index_sequence<>>
    : std::type_identity<std::index_sequence<>> {};
template <auto Head, auto... Tail>
struct quick_sort<std::index_sequence<Head, Tail...>> {
  template <auto N> using less_equal = std::bool_constant<(N <= Head)>;
  template <auto N> using greater_than = std::bool_constant<(N > Head)>;
  using type = typename merge<
      typename quick_sort<typename filter<
          less_equal, std::index_sequence<Tail...>>::type>::type,
      typename merge<
          std::index_sequence<Head>,
          typename filter<greater_than,
                          std::index_sequence<Tail...>>::type>::type>::type;
};

static_assert(std::is_same_v<quick_sort<std::index_sequence<3, 2, 1, 4>>::type, std::index_sequence<1, 2, 3, 4>>);
