#pragma once

#include "../include/vectors/Vector4/f32Vector4.hpp"
#include "../include/vectors/Vector4/f64Vector4.hpp"

#include "../include/vectors/Vector4/i8Vector4.hpp"
#include "../include/vectors/Vector4/i16Vector4.hpp"
#include "../include/vectors/Vector4/i32Vector4.hpp"
#include "../include/vectors/Vector4/i64Vector4.hpp"

#include "../include/vectors/Vector4/u8Vector4.hpp"
#include "../include/vectors/Vector4/u16Vector4.hpp"
#include "../include/vectors/Vector4/u32Vector4.hpp"
#include "../include/vectors/Vector4/u64Vector4.hpp"

#include <cstdint>
#include <limits>
#include <print>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace CLAL {
	namespace Vector4Tests {
		namespace Private {
			inline int g_total = 0;
			inline int g_failed = 0;
			inline const char* g_currentType = "";

#define CLAL_CHECK(...) do { \
				++::CLAL::Vector4Tests::Private::g_total; \
				if (!(__VA_ARGS__)) { \
					++::CLAL::Vector4Tests::Private::g_failed; \
					std::println("  FAIL [{}] line {}: {}", \
						::CLAL::Vector4Tests::Private::g_currentType, __LINE__, #__VA_ARGS__); \
				} \
			} while (0)


			template <typename V>
			using ElementT = std::remove_cvref_t<decltype(std::declval<V&>()[0])>;

			template <typename V>
			V make(auto x, auto y, auto z, auto w) {
				using T = ElementT<V>;
				return V(static_cast<T>(x), static_cast<T>(y), static_cast<T>(z), static_cast<T>(w));
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
				CLAL_CHECK(defaultConstructor[0] == T(0) && defaultConstructor[1] == T(0) && defaultConstructor[2] == T(0) && defaultConstructor[3] == T(0));

				V filledConstructor(T(4));
				CLAL_CHECK(filledConstructor[0] == T(4) && filledConstructor[1] == T(4) && filledConstructor[2] == T(4) && filledConstructor[3] == T(4));

				V perservedOrder(T(1), T(2), T(3), T(4));
				CLAL_CHECK(perservedOrder[0] == T(1));
				CLAL_CHECK(perservedOrder[1] == T(2));
				CLAL_CHECK(perservedOrder[2] == T(3));
				CLAL_CHECK(perservedOrder[3] == T(4));

				V converted(1, 2, 3, 4);
				CLAL_CHECK(converted[0] == T(1) && converted[1] == T(2) && converted[2] == T(3) && converted[3] == T(4));

				// extended
				CLAL::Vector<T, 5> vector5(perservedOrder, T(5));
				CLAL_CHECK(vector5[0] == T(1) && vector5[1] == T(2) && vector5[2] == T(3) && vector5[3] == T(4) && vector5[4] == T(5));

				CLAL::Vector<T, 6> vector6(perservedOrder, T(5), T(6));
				CLAL_CHECK(vector6[0] == T(1) && vector6[1] == T(2) && vector6[2] == T(3) && vector6[3] == T(4) && vector6[4] == T(5) && vector6[5] == T(6));

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

				V vector = make<V>(1, 2, 3, 4);
				vector[0] = T(9);
				vector[1] = T(8);
				vector[2] = T(7);
				vector[3] = T(6);
				CLAL_CHECK(vector[0] == T(9) && vector[1] == T(8) && vector[2] == T(7) && vector[3] == T(6));

				const V constVector = make<V>(3, 4, 5, 6);
				CLAL_CHECK(constVector[0] == T(3) && constVector[1] == T(4) && constVector[2] == T(5) && constVector[3] == T(6));

				CLAL_CHECK(throwsOutOfRange([&] {(void)vector[4]; }));
				CLAL_CHECK(throwsOutOfRange([&] {(void)vector[5]; }));
				CLAL_CHECK(throwsOutOfRange([&] {(void)vector[~size_t(0)]; }));
				CLAL_CHECK(throwsOutOfRange([&] {(void)constVector[4]; }));
				CLAL_CHECK(!throwsOutOfRange([&] {(void)vector[1]; }));
			}

			// compound assignment
			template <typename V>
			void testCompound() {
				using T = ElementT<V>;

				V a = make<V>(1, 2, 3, 4);
				V b = make<V>(5, 7, 9, 11);

				// add
				V c = b;
				c += a;
				CLAL_CHECK(c == make<V>(6, 9, 12, 15));

				c = b;
				c += T(1);
				CLAL_CHECK(c == make<V>(6, 8, 10, 12));

				// subtract
				c = b;
				c -= a;
				CLAL_CHECK(c == make<V>(4, 5, 6, 7));

				c = b;
				c -= T(1);
				CLAL_CHECK(c == make<V>(4, 6, 8, 10));

				// multiply
				c = b;
				c *= a;
				CLAL_CHECK(c == make<V>(5, 14, 27, 44));

				c = b;
				c *= T(2);
				CLAL_CHECK(c == make<V>(10, 14, 18, 22));

				// divide
				c = make<V>(8, 6, 4, 2);
				c /= make<V>(2, 3, 4, 2);
				CLAL_CHECK(c == make<V>(4, 2, 1, 1));

				c = make<V>(8, 6, 4, 2);
				c /= T(2);
				CLAL_CHECK(c == make<V>(4, 3, 2, 1));

				// chaining
				c = a;
				(c += b) += b;
				CLAL_CHECK(c == make<V>(11, 16, 21, 26));

				V& ref = (c += a);
				CLAL_CHECK(&ref == &c);

				// self operations
				c = a;
				c += c;
				CLAL_CHECK(c == make<V>(2, 4, 6, 8));

				c = a;
				c *= c;
				CLAL_CHECK(c == make<V>(1, 4, 9, 16));

				c = b;
				c -= c;
				CLAL_CHECK(c == make<V>(0, 0, 0, 0));
			}

			// binary arithmetic
			template <typename V>
			void testBinary() {
				using T = ElementT<V>;

				const V a = make<V>(1, 2, 3, 4);
				const V b = make<V>(5, 7, 9, 11);

				// basic operations
				CLAL_CHECK(a + b == make<V>(6, 9, 12, 15));
				CLAL_CHECK(b - a == make<V>(4, 5, 6, 7));
				CLAL_CHECK(a * b == make<V>(5, 14, 27, 44));
				CLAL_CHECK(make<V>(8, 6, 4, 2) / make<V>(2, 3, 4, 2) == make<V>(4, 2, 1, 1));

				// right scalar
				CLAL_CHECK(a + T(3) == make<V>(4, 5, 6, 7));
				CLAL_CHECK(b - T(1) == make<V>(4, 6, 8, 10));
				CLAL_CHECK(a * T(3) == make<V>(3, 6, 9, 12));
				CLAL_CHECK(make<V>(8, 6, 4, 2) / T(2) == make<V>(4, 3, 2, 1));

				// left scalar
				CLAL_CHECK(T(3) + a == make<V>(4, 5, 6, 7));
				CLAL_CHECK(T(10) - a == make<V>(9, 8, 7, 6));
				CLAL_CHECK(T(3) * a == make<V>(3, 6, 9, 12));

				// order
				CLAL_CHECK(a + b == b + a);
				CLAL_CHECK(T(10) - a != a - T(10));

				// operands dont modify
				CLAL_CHECK(a == make<V>(1, 2, 3, 4));
				CLAL_CHECK(b == make<V>(5, 7, 9, 11));

				// components are independent
				CLAL_CHECK((a + b)[0] == T(6) && (a + b)[1] == T(9) && (a + b)[2] == T(12) && (a + b)[3] == T(15));
			}

			// unary minus
			template <typename V>
			void testUnaryMinus() {
				using T = ElementT<V>;

				if constexpr (std::is_signed_v<T>) {
					const V a = make<V>(1, -2, 3, -4);
					CLAL_CHECK(-a == make<V>(-1, 2, -3, 4));
					CLAL_CHECK(-(-a) == a);
					CLAL_CHECK(a == make<V>(1, -2, 3, -4));
				}
			}

			// comparison
			template <typename V>
			void testComparison() {
				const V a = make<V>(1, 2, 3, 4);

				CLAL_CHECK(a == make<V>(1, 2, 3, 4));
				CLAL_CHECK(!(a != make<V>(1, 2, 3, 4)));
				CLAL_CHECK(a != make<V>(1, 3, 2, 5));
				CLAL_CHECK(a != make<V>(2, 2, 2, 2));

				CLAL_CHECK(a < make<V>(1, 3, 3, 3));
				CLAL_CHECK(a < make<V>(2, 0, 3, 4));
				CLAL_CHECK(a <= make<V>(1, 2, 3, 4));
				CLAL_CHECK(a >= make<V>(1, 2, 3, 4));
				CLAL_CHECK(make<V>(2, 0, 3, 4) > a);
				CLAL_CHECK(!(a < a));
				CLAL_CHECK(!(a > a));
			}

			// integer behaviour
			template <typename V>
			void testIntegerBehaviour() {
				using T = ElementT<V>;

				if constexpr (std::is_integral_v<T>) {
					CLAL_CHECK(make<V>(7, 9, 11, 13) / T(2) == make<V>(3, 4, 5, 6));
					if constexpr (std::is_signed_v<T>) {
						CLAL_CHECK(make<V>(-7, -9, -11, -13) / T(2) == make<V>(-3, -4, -5, -6));
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
					const V a = make<V>(0b1100, 0b1010, 0b0101, 0b1111);
					const V b = make<V>(0b1010, 0b0110, 0b1100, 0b0011);

					CLAL_CHECK((a & b) == make<V>(0b1000, 0b0010, 0b0100, 0b0011));
					CLAL_CHECK((a | b) == make<V>(0b1110, 0b1110, 0b1101, 0b1111));
					CLAL_CHECK((a ^ b) == make<V>(0b0110, 0b1100, 0b1001, 0b1100));
					CLAL_CHECK((a << 1) == make<V>(0b11000, 0b10100, 0b1010, 0b11110));
					CLAL_CHECK((a >> 1) == make<V>(0b0110, 0b0101, 0b0010, 0b0111));
					CLAL_CHECK((a >> 0) == a);
					CLAL_CHECK((~V(T(0))) == V(static_cast<T>(~T(0))));
					CLAL_CHECK(~(~a) == a);

					// compound forms
					V c = a; c &= b;
					CLAL_CHECK(c == make<V>(0b1000, 0b0010, 0b0100, 0b0011));
					c = a; c |= b;
					CLAL_CHECK(c == make<V>(0b1110, 0b1110, 0b1101, 0b1111));
					c = a; c ^= b;
					CLAL_CHECK(c == make<V>(0b0110, 0b1100, 0b1001, 0b1100));
					c = a; c <<= 1;
					CLAL_CHECK(c == make<V>(0b11000, 0b10100, 0b1010, 0b11110));
					c = a; c >>= 1;
					CLAL_CHECK(c == make<V>(0b0110, 0b0101, 0b0010, 0b0111));

					// x ^ x == 0, x & ~x == 0, x | ~x == all ones
					CLAL_CHECK((a ^ a) == V(T(0)));
					CLAL_CHECK((a & ~a) == V(T(0)));
					CLAL_CHECK((a | ~a) == V(static_cast<T>(~T(0))));

					// 8-bit types promote to int during the shift, then narrow back
					if constexpr (sizeof(T) == 1 && std::is_unsigned_v<T>) {
						CLAL_CHECK((V(T(200), T(1), T(50), T(128)) << 1) == V(T(144), T(2), T(100), T(0)));
					}

					// arithmetic shift of negatives
					if constexpr (std::is_signed_v<T>) {
						CLAL_CHECK((make<V>(-8, -4, -6, -2) >> 1) == make<V>(-4, -2, -3, -1));
					}
				}
			}


			// floating point
			template <typename V>
			void testFloatBehaviour() {
				using T = ElementT<V>;

				if constexpr (std::is_floating_point_v<T>) {
					CLAL_CHECK(make<V>(7, 9, 11, 13) / T(2) == V(T(3.5), T(4.5), T(5.5), T(6.5)));

					constexpr T infinity = std::numeric_limits<T>::infinity();
					constexpr T nan = std::numeric_limits<T>::quiet_NaN();

					V z = V(T(1)) / V(T(0));
					CLAL_CHECK(z[0] == infinity && z[1] == infinity && z[2] == infinity && z[3] == infinity);

					V n = V(T(0)) / V(T(0));
					CLAL_CHECK(n[0] != n[0] && n[1] != n[1] && n[2] != n[2] && n[3] != n[3]);

					V withNan(nan, nan, nan, T(1));
					CLAL_CHECK(withNan != withNan);
					CLAL_CHECK(!(withNan == withNan));

					CLAL_CHECK(V(T(-0.0)) == V(T(0.0)));
				}
			}

			// casting
			template <typename V>
			void testCasting() {
				using T = ElementT<V>;

				V a = make<V>(1, 2, 3, 4);
				CLAL::Vector<double, 4> asDouble = a;
				CLAL_CHECK(asDouble[0] == 1.0 && asDouble[1] == 2.0 && asDouble[2] == 3.0 && asDouble[3] == 4.0);

				V back = static_cast<V>(asDouble);
				CLAL_CHECK(back == a);

				CLAL::Vector<int, 4> asInt = a;
				CLAL_CHECK(asInt[0] == 1 && asInt[1] == 2 && asInt[2] == 3 && asInt[3] == 4);
			}

			inline void testCastingSpecific() {
				CLAL::Vector<float, 4> asFloat(-1.7f, 2.9f, -3.8f, 4.6f);
				CLAL::Vector<int, 4> asInt = asFloat;
				CLAL_CHECK(asInt[0] == -1 && asInt[1] == 2 && asInt[2] == -3 && asInt[3] == 4);

				CLAL::Vector<int, 4> wide(300, -1, 400, -2);
				CLAL::Vector<int8_t, 4> narrow = wide;
				CLAL_CHECK(narrow[0] == 44 && narrow[1] == -1 && narrow[2] == -112 && narrow[3] == -2);

				asInt = CLAL::Vector<int, 4>(3, -4, 5, -6);
				asFloat = CLAL::Vector<float, 4>(asInt);
				CLAL_CHECK(asFloat[0] == 3.0f && asFloat[1] == -4.0f && asFloat[2] == 5.0f && asFloat[3] == -6.0f);
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

			runAll<CLAL::f32Vector4>("f32Vector4");
			runAll<CLAL::f64Vector4>("f64Vector4");

			runAll<CLAL::i8Vector4>("i8Vector4");
			runAll<CLAL::i16Vector4>("i16Vector4");
			runAll<CLAL::i32Vector4>("i32Vector4");
			runAll<CLAL::i64Vector4>("i64Vector4");

			runAll<CLAL::u8Vector4>("u8Vector4");
			runAll<CLAL::u16Vector4>("u16Vector4");
			runAll<CLAL::u32Vector4>("u32Vector4");
			runAll<CLAL::u64Vector4>("u64Vector4");

			g_currentType = "cast";
			testCastingSpecific();

			std::println("{} checks, {} failed", g_total, g_failed);
			return g_failed == 0;
		}
	}

#undef CLAL_CHECK
}