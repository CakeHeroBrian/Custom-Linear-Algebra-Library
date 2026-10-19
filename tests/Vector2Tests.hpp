#pragma once

#include "../include/vectors/Vector2/f32Vector2.hpp"
#include "../include/vectors/Vector2/f64Vector2.hpp"

#include "../include/vectors/Vector2/i8Vector2.hpp"
#include "../include/vectors/Vector2/i16Vector2.hpp"
#include "../include/vectors/Vector2/i32Vector2.hpp"
#include "../include/vectors/Vector2/i64Vector2.hpp"

#include "../include/vectors/Vector2/u8Vector2.hpp"
#include "../include/vectors/Vector2/u16Vector2.hpp"
#include "../include/vectors/Vector2/u32Vector2.hpp"
#include "../include/vectors/Vector2/u64Vector2.hpp"

#include <cstdint>
#include <limits>
#include <print>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace CLAL {
	namespace Vector2Tests {
		namespace Private {
			inline int g_total = 0;
			inline int g_failed = 0;
			inline const char* g_currentType = "";

#define CLAL_CHECK(...) do { \
				++::CLAL::Vector2Tests::Private::g_total; \
				if (!(__VA_ARGS__)) { \
					++::CLAL::Vector2Tests::Private::g_failed; \
					std::println("  FAIL [{}] line {}: {}", \
						::CLAL::Vector2Tests::Private::g_currentType, __LINE__, #__VA_ARGS__); \
				} \
			} while (0)                                                                   \


			template <typename V>
			using ElementT = std::remove_cvref_t<decltype(std::declval<V&>()[0])>;

			template <typename V>
			V make(auto x, auto y) {
				using T = ElementT<V>;
				return V(static_cast<T>(x), static_cast<T>(y));
			}

			template <typename F>
			bool throwsOutOfRange(F&& f) {
				try {
					f();
				}
				catch (const std::out_of_range&) {
					return true;
				}
				catch (...) {
					return false;
				}

				return false;
			}

			template <typename V>
			void testConstruction() {
				using T = ElementT<V>;

				V defaultConstructor;
				CLAL_CHECK(defaultConstructor[0] == T(0) && defaultConstructor[1] == T(0));

				V filledConstructor(T(4));
				CLAL_CHECK(filledConstructor[0] == T(4) && filledConstructor[1] == T(4));

				V perservedOrder(T(1), T(2));
				CLAL_CHECK(perservedOrder[0] == T(1));
				CLAL_CHECK(perservedOrder[1] == T(2));

				V converted(1, 2);
				CLAL_CHECK(converted[0] == T(1), converted[1] == T(2));

				// extended
				CLAL::Vector<T, 3> vector3(perservedOrder, T(3));
				CLAL_CHECK(vector3[0] == T(1) && vector3[1] == T(2) && vector3[2] == T(3));

				CLAL::Vector<T, 4> vector4(perservedOrder, T(3), T(4));
				CLAL_CHECK(vector4[0] == T(1) && vector4[1] == T(2) && vector4[2] == T(3) && vector4[3] == T(4));

				// copy / move
				V copy(perservedOrder);
				CLAL_CHECK(copy == perservedOrder);
				V move(std::move(copy));
				CLAL_CHECK(move == perservedOrder);
				V assigned;
				assigned = perservedOrder;
				CLAL_CHECK(assigned == perservedOrder);
				V moveAssign;
				moveAssign = std::move(assigned);
				CLAL_CHECK(moveAssign == perservedOrder);
			}

			// indexing
			template <typename V>
			void testIndexing() {
				using T = ElementT<V>;

				V vector = make<V>(1, 2);
				vector[0] = T(9);
				vector[1] = T(8);
				CLAL_CHECK(vector[0] == T(9) && vector[1] == T(8));

				const V constVector = make<V>(3, 4);
				CLAL_CHECK(constVector[0] == T(3) && constVector[1] == T(4));

				CLAL_CHECK(throwsOutOfRange([&] {(void)vector[2]; }));
				CLAL_CHECK(throwsOutOfRange([&] {(void)vector[3]; }));
				CLAL_CHECK(throwsOutOfRange([&] {(void)vector[~size_t(0)]; }));
				CLAL_CHECK(throwsOutOfRange([&] {(void)constVector[2]; }));
				CLAL_CHECK(!throwsOutOfRange([&] {(void)vector[1]; }));
			}

			// compound assignment
			template <typename V>
			void testCompound() {
				using T = ElementT<V>;

				V a = make<V>(1, 2);
				V b = make<V>(5, 7);

				// add
				V c = b;
				c += a;
				CLAL_CHECK(c == make<V>(6, 9));

				c = b;
				c += T(1);
				CLAL_CHECK(c == make<V>(6, 8));

				// subtract
				c = b;
				c -= a;
				CLAL_CHECK(c == make<V>(4, 5));

				c = b;
				c -= T(1);
				CLAL_CHECK(c == make<V>(4, 6));

				// multiply
				c = b;
				c *= a;
				CLAL_CHECK(c == make<V>(5, 14));

				c = b;
				c *= T(2);
				CLAL_CHECK(c == make<V>(10, 14));

				// divide
				c = make<V>(8, 6);
				c /= make<V>(2, 3);
				CLAL_CHECK(c == make<V>(4, 2));

				c = make<V>(8, 6);
				c /= T(2);
				CLAL_CHECK(c == make<V>(4, 3));

				// chaining
				c = a;
				(c += b) += b;
				CLAL_CHECK(c == make<V>(11, 16));

				V& ref = (c += a);
				CLAL_CHECK(&ref == &c);

				// self operations
				c = a;
				c += c;
				CLAL_CHECK(c == make<V>(2, 4));

				c = a;
				c *= c;
				CLAL_CHECK(c == make<V>(1, 4));

				c = b;
				c -= c;
				CLAL_CHECK(c == make<V>(0, 0));
			}

			// binary arithmetic
			template <typename V>
			void testBinary() {
				using T = ElementT<V>;

				const V a = make<V>(1, 2);
				const V b = make<V>(5, 7);

				// basic operations
				CLAL_CHECK(a + b == make<V>(6, 9));
				CLAL_CHECK(b - a == make<V>(4, 5));
				CLAL_CHECK(a * b == make<V>(5, 14));
				CLAL_CHECK(make<V>(8, 6) / make<V>(2, 3) == make<V>(4, 2));

				// right scalar
				CLAL_CHECK(a + T(3) == make<V>(4, 5));
				CLAL_CHECK(b - T(1) == make<V>(4, 6));
				CLAL_CHECK(a * T(3) == make<V>(3, 6));
				CLAL_CHECK(make<V>(8, 6) / T(2) == make<V>(4, 3));

				// left scalar
				CLAL_CHECK(T(3) + a == make<V>(4, 5));
				CLAL_CHECK(T(10) - a == make<V>(9, 8));
				CLAL_CHECK(T(3) * a == make<V>(3, 6));

				// order
				CLAL_CHECK(a + b == b + a);
				CLAL_CHECK(T(10) - a != a - T(10));

				// operands dont modify
				CLAL_CHECK(a == make<V>(1, 2));
				CLAL_CHECK(b == make<V>(5, 7));

				// components are independent
				CLAL_CHECK((a + b)[0] == T(6) && (a + b)[1] == T(9));
			}

			// unary minus
			template <typename V>
			void testUnaryMinus() {
				using T = ElementT<V>;

				if constexpr (std::is_signed_v<T>) {
					const V a = make<V>(1, -2);
					CLAL_CHECK(-a == make<V>(-1, 2));
					CLAL_CHECK(-(-a) == a);
					CLAL_CHECK(a == make<V>(1, -2));
				}
			}

			// comparison
			template <typename V>
			void testComparison() {
				const V a = make<V>(1, 2);

				CLAL_CHECK(a == make<V>(1, 2));
				CLAL_CHECK(!(a != make<V>(1, 2)));
				CLAL_CHECK(a != make<V>(1, 3));
				CLAL_CHECK(a != make<V>(2, 2));

				CLAL_CHECK(a < make<V>(1, 3));
				CLAL_CHECK(a < make<V>(2, 0));
				CLAL_CHECK(a <= make<V>(1, 2));
				CLAL_CHECK(a >= make<V>(1, 2));
				CLAL_CHECK(make<V>(2, 0) > a);
				CLAL_CHECK(!(a < a));
				CLAL_CHECK(!(a > a));
			}

			// integer behaviour
			template <typename V>
			void testIntegerBehaviour() {
				using T = ElementT<V>;

				if constexpr (std::is_integral_v<T>) {
					CLAL_CHECK(make<V>(7, 9) / T(2) == make<V>(3, 4));
					if constexpr (std::is_signed_v<T>) {
						CLAL_CHECK(make<V>(-7, -9) / T(2) == make<V>(-3, -4));
					}

					if constexpr (std::is_unsigned_v<T>) {
						constexpr T maxValue = std::numeric_limits<T>::max();
						CLAL_CHECK(V(T(0)) - T(1) == V(maxValue));
						CLAL_CHECK(V(T(0)) - V(T(1)) == V(maxValue));
						CLAL_CHECK(V(maxValue) + T(1) == V(T(0)));
					}
				}
			}

			// bitwise, integer on ly
			template <typename V>
			void testBitwise() {
				using T = ElementT<V>;

				if constexpr (std::is_integral_v<T>) {
					const V a = make<V>(0b1100, 0b1010);
					const V b = make<V>(0b1010, 0b0110);

					CLAL_CHECK((a & b) == make<V>(0b1000, 0b0010));
					CLAL_CHECK((a | b) == make<V>(0b1110, 0b1110));
					CLAL_CHECK((a ^ b) == make<V>(0b0110, 0b1100));
					CLAL_CHECK((a << 1) == make<V>(0b11000, 0b10100));
					CLAL_CHECK((a >> 1) == make<V>(0b0110, 0b0101));
					CLAL_CHECK((a >> 0) == a);
					CLAL_CHECK((~V(T(0))) == V(static_cast<T>(~T(0))));
					CLAL_CHECK(~(~a) == a);

					// compound forms
					V c = a; c &= b;
					CLAL_CHECK(c == make<V>(0b1000, 0b0010));
					c = a; c |= b;
					CLAL_CHECK(c == make<V>(0b1110, 0b1110));
					c = a; c ^= b;
					CLAL_CHECK(c == make<V>(0b0110, 0b1100));
					c = a; c <<= 1;
					CLAL_CHECK(c == make<V>(0b11000, 0b10100));
					c = a; c >>= 1;
					CLAL_CHECK(c == make<V>(0b0110, 0b0101));

					// x ^ x == 0, x & ~x == 0, x | ~x == all ones
					CLAL_CHECK((a ^ a) == V(T(0)));
					CLAL_CHECK((a & ~a) == V(T(0)));
					CLAL_CHECK((a | ~a) == V(static_cast<T>(~T(0))));

					// 8-bit types promote to int during the shift, then narrow back
					if constexpr (sizeof(T) == 1 && std::is_unsigned_v<T>) {
						CLAL_CHECK((V(T(200), T(1)) << 1) == V(T(144), T(2)));
					}

					// arithmetic shift of negatives
					if constexpr (std::is_signed_v<T>) {
						CLAL_CHECK((make<V>(-8, -4) >> 1) == make<V>(-4, -2));
					}
				}
			}

			// floating point
			template <typename V>
			void testFloatBehaviour() {
				using T = ElementT<V>;

				if constexpr (std::is_floating_point_v<T>) {
					CLAL_CHECK(make<V>(7, 9) / T(2) == V(T(3.5), T(4.5)));

					constexpr T infinity = std::numeric_limits<T>::infinity();
					constexpr T nan = std::numeric_limits<T>::quiet_NaN();

					V z = V(T(1)) / V(T(0));
					CLAL_CHECK(z[0] == infinity && z[1] == infinity);

					V n = V(T(0)) / V(T(0));
					CLAL_CHECK(n[0] != n[0] && n[1] != n[1]);

					V withNan(nan, T(1));
					CLAL_CHECK(withNan != withNan);
					CLAL_CHECK(!(withNan == withNan));

					CLAL_CHECK(V(T(-0.0)) == V(T(0.0)));
				}
			}

			// casting
			template <typename V>
			void testCasting() {
				using T = ElementT<V>;

				V a = make<V>(1, 2);
				CLAL::Vector<double, 2> asDouble = a;
				CLAL_CHECK(asDouble[0] == 1.0 && asDouble[1] == 2.0);

				V back = static_cast<V>(asDouble);
				CLAL_CHECK(back == a);

				CLAL::Vector<int, 2> asInt = a;
				CLAL_CHECK(asInt[0] == 1 && asInt[1] == 2);
			}

			inline void testCastingSpecific() {
				CLAL::Vector<float, 2> asFloat(-1.7f, 2.9f);
				CLAL::Vector<int, 2> asInt = asFloat;
				CLAL_CHECK(asInt[0] == -1 && asInt[1] == 2);

				CLAL::Vector<int, 2> wide(300, -1);
				CLAL::Vector<int8_t, 2> narrow = wide;
				CLAL_CHECK(narrow[0] == 44 && narrow[1] == -1);

				asInt = CLAL::Vector<int, 2>(3, -4);
				asFloat = CLAL::Vector<float, 2>(asInt);
				CLAL_CHECK(asFloat[0] == 3.0f && asFloat[1] == -4.0f);
			}

			template <typename V>
			void runAll(const char* name) {
				g_currentType = name;
				int failedBefore = g_failed;

				testConstruction<V>();
				testIndexing<V>();
				testCompound<V>();
				testBinary<V>();
				testUnaryMinus<V>();
				testComparison<V>();
				testIntegerBehaviour<V>();
				testBitwise<V>();
				testFloatBehaviour<V>();
				testCasting<V>();

				std::println("{:<12} {}", name, g_failed == failedBefore ? "ok" : "FAILED");
			}
		}

		inline bool tests() {
			using namespace Private;

			g_total = 0;
			g_failed = 0;

			runAll<CLAL::f32Vector2>("f32Vector2");
			runAll<CLAL::f64Vector2>("f64Vector2");

			runAll<CLAL::i8Vector2>("i8Vector2");
			runAll<CLAL::i16Vector2>("i16Vector2");
			runAll<CLAL::i32Vector2>("i32Vector2");
			runAll<CLAL::i64Vector2>("i64Vector2");

			runAll<CLAL::u8Vector2>("u8Vector2");
			runAll<CLAL::u16Vector2>("u16Vector2");
			runAll<CLAL::u32Vector2>("u32Vector2");
			runAll<CLAL::u64Vector2>("u64Vector2");

			g_currentType = "cast";
			testCastingSpecific();

			std::println("{} checks, {} failed", g_total, g_failed);
			return g_failed == 0;
		}
	}

#undef CLAL_CHECK
}