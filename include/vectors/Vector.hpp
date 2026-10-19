#pragma once

#include <array>
#include <print>
#include <compare>
#include <exception>
#include <stdexcept>
#include <format>
#include <type_traits>
#include <concepts>

namespace CLAL {
	template <typename T, size_t N>
	class Vector {
	public:
		template <typename U, size_t M>
		friend class Vector;

		Vector() = default;
		~Vector() = default;
		explicit Vector(T initializer) {
			m_data.fill(initializer);
		}

		template<size_t S, typename... Args>
		requires (S <= N && S + sizeof...(Args) == N && (std::convertible_to<Args, T> && ...))
		Vector(const Vector<T, S>& other, Args&&... args) {
			for (size_t i = 0; i < S; i++) {
				m_data[i] = other.m_data[i];
			}

			if constexpr (sizeof...(Args) > 0) {
				size_t index = S;
				((m_data[index++] = static_cast<T>(std::forward<Args>(args))), ...);
			}
		}

		template <typename... Args>
			requires (N > 1 && sizeof...(Args) == N && (std::convertible_to<Args, T> && ...))
		Vector(Args&&... args) : m_data{ static_cast<T>(std::forward<Args>(args))... } {}

		template <typename U>
			requires (!std::same_as<U, T>)
		Vector(const Vector<U, N>& other) {
			for (size_t i = 0; i < N; i++) {
				m_data[i] = static_cast<T>(other.m_data[i]);
			}
		}

		Vector<T, N>(const Vector<T, N>& other) = default;
		Vector<T, N>& operator=(const Vector<T, N>& other) = default;

		Vector<T, N>(Vector<T, N>&& other) = default;
		Vector<T, N>& operator=(Vector<T, N>&& other) = default;

		T& operator[](size_t index) {
			if (index >= N) {
				throw std::out_of_range(std::format("Index is too large, {} >= {}", index, N));
			}

			return m_data[index];
		}

		const T& operator[](size_t index) const {
			if (index >= N) {
				throw std::out_of_range(std::format("Index is too large, {} >= {}", index, N));
			}

			return m_data[index];
		}

	
		// arithmatic
		Vector<T, N>& operator+=(const Vector<T, N>& other) {
			for (size_t i = 0; i < N; i++) {
				m_data[i] += other.m_data[i];
			}
			return *this;
		}

		Vector<T, N>& operator+= (std::type_identity_t<T> scalar) {
			for (size_t i = 0; i < N; i++) {
				m_data[i] += scalar;
			}
			return *this;
		}

		Vector<T, N>& operator-=(const Vector<T, N>& other) {
			for (size_t i = 0; i < N; i++) {
				m_data[i] -= other.m_data[i];
			}
			return *this;
		}


		Vector<T, N>& operator-= (std::type_identity_t<T> scalar) {
			for (size_t i = 0; i < N; i++) {
				m_data[i] -= scalar;
			}
			return *this;
		}

		Vector<T, N>& operator*=(const Vector<T, N>& other) {
			for (size_t i = 0; i < N; i++) {
				m_data[i] *= other.m_data[i];
			}
			return *this;
		}

		Vector<T, N>& operator*=(std::type_identity_t<T> scalar) {
			for (size_t i = 0; i < N; i++) {
				m_data[i] *= scalar;
			}
			return *this;
		}

		Vector<T, N>& operator/=(const Vector<T, N>& other) {
			for (size_t i = 0; i < N; i++) {
				m_data[i] /= other.m_data[i];
			}
			return *this;
		}

		Vector<T, N>& operator/=(std::type_identity_t<T> scalar) {
			for (size_t i = 0; i < N; i++) {
				m_data[i] /= scalar;
			}
			return *this;
		}

		Vector<T, N> operator-() const {
			Vector<T, N> out{};
			for (size_t i = 0; i < N; i++) {
				out[i] = -m_data[i];
			}
			return out;
		}

		// comparison
		auto operator<=>(const Vector<T, N>&) const = default;

		// bitwise 
		
		Vector<T, N>& operator&=(const Vector<T, N>& other) requires std::is_integral_v<T> {
			for (size_t i = 0; i < N; i++) {
				m_data[i] &= other.m_data[i];
			}
			return *this;
		}

		Vector<T, N>& operator|=(const Vector<T, N>& other) requires std::is_integral_v<T> {
			for (size_t i = 0; i < N; i++) {
				m_data[i] |= other.m_data[i];
			}
			return *this;
		}

		Vector<T, N>& operator^=(const Vector<T, N>& other) requires std::is_integral_v<T> {
			for (size_t i = 0; i < N; i++) {
				m_data[i] ^= other.m_data[i];
			}
			return *this;
		}

		Vector<T, N>& operator<<=(size_t shift) requires std::is_integral_v<T> {
			for (size_t i = 0; i < N; i++) {
				m_data[i] <<= shift;
			}
			return *this;
		}

		Vector<T, N>& operator>>=(size_t shift) requires std::is_integral_v<T> {
			for (size_t i = 0; i < N; i++) {
				m_data[i] >>= shift;
			}
			return *this;
		}
	private:
		std::array<T, N> m_data{};
	};

	//arithmatic
	template<typename T, size_t N>
	Vector<T, N> operator+(const Vector<T, N>& a, const Vector<T, N>& b) {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] + b[i];
		}
		return out;
	}

	template<typename T, size_t N>
	Vector<T, N> operator+(const Vector<T, N>& a, std::type_identity_t<T> scalar) {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] + scalar;
		}
		return out;
	}

	template<typename T, size_t N>
	Vector<T, N> operator+(std::type_identity_t<T> scalar, const Vector<T, N>& a) {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] + scalar;
		}
		return out;
	}

	template<typename T, size_t N>
	Vector<T, N> operator-(const Vector<T, N>& a, const Vector<T, N>& b) {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] - b[i];
		}
		return out;
	}

	template<typename T, size_t N>
	Vector<T, N> operator-(const Vector<T, N>& a, std::type_identity_t<T> scalar) {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] - scalar;
		}
		return out;
	}

	template<typename T, size_t N>
	Vector<T, N> operator-(std::type_identity_t<T> scalar, const Vector<T, N>& a) {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = scalar - a[i];
		}
		return out;
	}

	template<typename T, size_t N>
	Vector<T, N> operator*(const Vector<T, N>& a, const Vector<T, N>& b) {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] * b[i];
		}
		return out;
	}

	template<typename T, size_t N>
	Vector<T, N> operator/(const Vector<T, N>& a, const Vector<T, N>& b) {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] / b[i];
		}
		return out;
	}

	template<typename T, size_t N>
	Vector<T, N> operator*(const Vector<T, N>& a, std::type_identity_t<T> scalar) {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] * scalar;
		}
		return out;
	}

	template<typename T, size_t N>
	Vector<T, N> operator*(std::type_identity_t<T> scalar, const Vector<T, N>& a) {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] * scalar;
		}
		return out;
	}

	template<typename T, size_t N>
	Vector<T, N> operator/(const Vector<T, N>& a, std::type_identity_t<T> scalar) {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] / scalar;
		}
		return out;
	}
	

	// bitwise 
	template<typename T, size_t N>
	Vector<T, N> operator&(const Vector<T, N>& a, const Vector<T, N>& b) requires std::is_integral_v<T> {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] & b[i];
		}
		return out;
	}

	template<typename T, size_t N>
	Vector<T, N> operator|(const Vector<T, N>& a, const Vector<T, N>& b) requires std::is_integral_v<T> {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] | b[i];
		}
		return out;
	}


	template<typename T, size_t N>
	Vector<T, N> operator^(const Vector<T, N>& a, const Vector<T, N>& b) requires std::is_integral_v<T> {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] ^ b[i];
		}
		return out;
	}

	template<typename T, size_t N>
	Vector<T, N> operator~(const Vector<T, N>& a) requires std::is_integral_v<T> {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = ~a[i];
		}
		return out;
	}
	
	template<typename T, size_t N>
	Vector<T, N> operator<<(const Vector<T, N>& a, size_t shift) requires std::is_integral_v<T> {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] << shift;
		}
		return out;
	}

	template<typename T, size_t N>
	Vector<T, N> operator>>(const Vector<T, N>& a, size_t shift) requires std::is_integral_v<T> {
		Vector<T, N> out{};
		for (size_t i = 0; i < N; i++) {
			out[i] = a[i] >> shift;
		}
		return out;
	}
}