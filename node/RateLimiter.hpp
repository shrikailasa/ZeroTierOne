/*
 * ZeroTier One - Global Peer to Peer Ethernet
 * Copyright (C) 2012-2013  ZeroTier Networks LLC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * --
 *
 * ZeroTier may be used and distributed under the terms of the GPLv3, which
 * are available at: http://www.gnu.org/licenses/gpl-3.0.html
 *
 * If you would like to embed ZeroTier into a commercial application or
 * redistribute it in a modified binary form, please contact ZeroTier Networks
 * LLC. Start here: http://www.zerotier.com/
 */

#ifndef _ZT_RATELIMITER_HPP
#define _ZT_RATELIMITER_HPP

#include <math.h>

#include "Constants.hpp"
#include "Utils.hpp"

#ifdef __WINDOWS__
#define fmin(a,b) (((a) <= (b)) ? (a) : (b))
#define fmax(a,b) (((a) >= (b)) ? (a) : (b))
#endif

namespace ZeroTier {

/**
 * Data transfer accounting used for multicast groups
 *
 * This is used to apply a bank account model to multicast groups. Each
 * multicast packet counts against a balance, which accrues at a given
 * rate in bytes per second. Debt is possible. These parameters are
 * configurable.
 *
 * A bank account model permits bursting behavior, which correctly models
 * how OSes and apps typically use multicast. It's common for things to
 * spew lots of multicast messages at once, wait a while, then do it
 * again. A consistent bandwidth limit model doesn't fit.
 */
class RateLimiter
{
public:
	/**
	 * Rate and min/max to apply on rate limiter update
	 */
	struct Rate
	{
		/**
		 * Rate of balance accrual in bytes per second
		 */
		double bytesPerSecond;

		/**
		 * Maximum balance that can ever be accrued (should be > 0.0)
		 */
		double maxBalance;

		/**
		 * Minimum balance, or maximum allowable "debt" (should be <= 0.0)
		 */
		double minBalance;
	};

	/**
	 * Create an uninitialized rate limiter
	 *
	 * init() must be called before this is used.
	 */
	RateLimiter() throw() {}

	/**
	 * Create an initialize rate limiter
	 *
	 * @param preload Initial balance to place in account
	 */
	RateLimiter(double preload)
		throw()
	{
		init(preload);
	}

	/**
	 * Initialize or re-initialize rate limiter
	 *
	 * @param preload Initial balance to place in account
	 */
	inline void init(double preload)
		throw()
	{
		_lastTime = Utils::nowf();
		_balance = preload;
	}

	/**
	 * Update balance based on current clock and supplied rate
	 *
	 * @param lim Current limits in effect
	 * @param deduct Amount to deduct, or 0.0 to just update
	 * @return New balance with deduction applied
	 */
	inline double update(const Rate &r,double deduct)
		throw()
	{
		double lt = _lastTime;
		double now = _lastTime = Utils::nowf();
		return (_balance = fmax(r.minBalance,fmin(r.maxBalance,(_balance + (r.bytesPerSecond * (now - lt))) - deduct)));
	}

private:
	double _lastTime;
	double _balance;
};

} // namespace ZeroTier

#endif
