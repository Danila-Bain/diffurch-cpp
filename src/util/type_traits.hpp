#pragma once
#include <tuple>
#include <type_traits>

template <typename T, template <typename...> typename U>
struct is_kind_of : std::false_type {};
template <typename... TArgs, template <typename...> typename U>
struct is_kind_of<U<TArgs...>, U> : std::true_type {};
template <typename T, template <typename...> typename U>
inline constexpr bool is_kind_of_v = is_kind_of<T, U>::value;

template <template <typename> typename Condition, std::size_t I = 0,
          typename... Ts>
constexpr auto filter_tuple(const std::tuple<Ts...> &tup) {
  if constexpr (I == sizeof...(Ts))
    return std::tuple<>{};
  else {
    auto filtered_tail = filter_tuple<Condition, I + 1>(tup);
    if constexpr (Condition<
                      std::tuple_element_t<I, std::tuple<Ts...>>>::value) {
      return std::tuple_cat(std::make_tuple(std::get<I>(tup)), filtered_tail);
    } else {
      return filtered_tail;
    }
  }
}

/*template <typename T> struct Boo {};*/
/**/
/*template <typename T> using is_kind_of_Boo = is_kind_of<T, Boo>;*/
/**/
/*static_assert(std::tuple_size_v<decltype(std::make_tuple(Boo<int>(), 4,*/
/*                                                         Boo<double>()))> ==
 * 3);*/
/*static_assert(std::tuple_size_v<decltype(filter_tuple<is_kind_of_Boo>(*/
/*                  std::make_tuple(Boo<int>(), 4, Boo<double>())))> == 2);*/
/**/
/*template <template <typename...> typename EventType, std::size_t I = 0,*/
/*          typename... Ts>*/
/*constexpr auto filter_event_tuple(const std::tuple<Ts...> &tup) {*/
/*  if constexpr (I == sizeof...(Ts)) {*/
/*    return std::tuple<>{}; // Base case: empty tuple when no types satisfy
 * the*/
/*                           // condition*/
/*  } else {*/
/*    if constexpr (is_kind_of_v<*/
/*                      typename std::tuple_element_t<I,
 * std::tuple<Ts...>>::type,*/
/*                      EventType>) {*/
/*      auto filtered_tail = filter_event_tuple<EventType, I + 1>(*/
/*          tup); // Recursive call for the rest of the tuple*/
/*      return std::tuple_cat(std::make_tuple(std::get<I>(tup)),
 * filtered_tail);*/
/*    } else {*/
/*      return filter_event_tuple<EventType, I + 1>(tup); // Skip this element
 * and continue*/
/*    }*/
/*  }*/
/*}*/
