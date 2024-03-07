#ifndef __ANY_H__
#define __ANY_H__
#include <memory>

namespace selfUtils {
	class Any {
		struct DataBase {
			DataBase() = default;
			virtual ~DataBase() = default;
			virtual std::unique_ptr<DataBase> clone() const = 0;
			virtual const type_info& getType() const = 0;
		};
		template<typename T>
		struct DataImp : public DataBase {
			DataImp(const T& t) : data(t) {}
			DataImp(T&& t) :data(std::move(t)) {}
			auto clone() const -> std::unique_ptr<DataBase> override {
				return std::make_unique<DataImp<T>>(data);
			}
			auto getType() const -> const type_info & override {
				return typeid(T);
			}

			T data;
		};
	public:
		Any() = default;

		~Any() = default;

		Any(const Any& rhs) {
			if (rhs.dataPtr) {
				dataPtr = rhs.dataPtr->clone();
			}
		}

		auto operator=(const Any& rhs)->Any& {
			if (rhs.dataPtr) {
				dataPtr = rhs.dataPtr->clone();
			}
			return *this;

		}

		Any(Any&& rhs) noexcept : dataPtr(std::move(rhs.dataPtr)) {}

		auto operator=(Any&& rhs) noexcept ->Any& {
			dataPtr = std::move(rhs.dataPtr);
			return *this;
		}

		auto isNull() const ->bool { return dataPtr == nullptr; }

		auto getType() const -> const type_info& {
			if (isNull()) return typeid(void);
			else dataPtr->getType();
		}

		template<typename T>
		auto isType() const ->bool {
			if (getType().hash_code() == typeid(T).hash_code()) return true;
			else return false;
		}

		template<typename T, typename std::enable_if_t<!std::is_same<typename std::decay<T>::type, Any>::value, bool> = true>
		Any(T&& t) :dataPtr(new DataImp<typename std::decay<T>::type>(std::forward<T>(t))) {}

		template<typename T, typename std::enable_if_t<!std::is_same<typename std::decay<T>::type, Any>::value, bool> = true>
		auto operator=(T&& t)->Any& {
			dataPtr = std::make_unique<DataImp<typename std::decay<T>::type>>(std::forward<T>(t));
			return *this;
		}
	private:
		std::unique_ptr<DataBase> dataPtr;

		template<typename T>
		auto cast() ->T& {
			if (isNull()) {
				throw std::bad_cast();
			}
			if (!isType<T>()) {
				printf("error cast type %s to %s\n", getType().name(), typeid(T).name());
				throw std::bad_cast();
			}

			return static_cast<DataImp<T>*>(dataPtr.get())->data;
		}
		template<typename T>
		auto cast() const -> const T& {
			if (isNull()) {
				throw std::bad_cast();
			}
			if (!isType<T>()) {
				printf("error cast type %s to %s\n", getType().name(), typeid(T).name());
				throw std::bad_cast();
			}

			return static_cast<const DataImp<T>*>(dataPtr.get())->data;
		}

		template<typename T>
		friend const T& any_cast(const Any& any);

		template<typename T>
		friend T& any_cast(Any& any);
	};

	template<typename T>
	auto any_cast(const Any& any) -> const T& {
		return any.cast<T>();
	}

	template<typename T>
	auto any_cast(Any& any) -> T& {
		return any.cast<T>();
	}
}

#endif