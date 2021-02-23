#ifndef OUTCOME_H

#include <cassert>
#include <utility>
#include "errors.h"

namespace util{

template<typename R> // Result
class outcome
{
public:

	outcome() : success(false)
	{
	}

	outcome(const error& e) : err(e), success(false)
	{
	} 

	outcome(const R& r) : result(r), success(true)
	{
	}

	outcome(R&& r) : result(std::forward<R>(r)), success(true)
	{
	}

	outcome(const outcome& o) :
		result(o.result),
		err(o.err),
		success(o.success)
	{
	}

	outcome& operator=(const outcome& o)
	{
		if (this != &o)
		{
			result = o.result;
			err = o.err;
			success = o.success;
		}

		return *this;
	}

	outcome(outcome&& o) : // Required to force Move Constructor
		result(std::move(o.result)),
		err(std::move(o.err)),
		success(o.success)
	{
	}

	outcome& operator=(outcome&& o)
	{
		if (this != &o)
		{
			result = std::move(o.result);
			err = std::move(o.err);
			success = o.success;
		}

		return *this;
	}

	inline const R& get_result() const
	{
		return result;
	}

	inline R& get_result()
	{
		return result;
	}

	/**
	 * casts the underlying result to an r-value so that caller can take ownership of underlying resources.
	 * this is necessary when streams are involved.
	 */
	inline R&& get_result_with_ownership()
	{
		return std::move(result);
	}

	inline const error& get_error() const
	{
		return err;
	}

	inline bool is_success() const
	{
		return this->success;
	}

private:
	R result;
	error err;
	bool success;
};

}

#endif
