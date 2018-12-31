/****************************************************************************
 *
 *   Copyright (c) 2018 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#pragma once

#include <containers/List.hpp>
#include <containers/Queue.hpp>
#include <lib/perf/perf_counter.h>
#include <px4_defines.h>
#include <drivers/drv_hrt.h>

#define WQ_ITEM_PERF 0

namespace px4
{

class WorkQueue; // forward declaration

class WorkItem : public ListNode<WorkItem *>, public QueueNode<WorkItem *>
{

public:

	WorkItem();
	virtual ~WorkItem();

	bool Init();

	void ScheduleNow();

	virtual void Run() = 0;

	void pre_run()
	{
		_queued = false;
#if WQ_ITEM_PERF
		perf_set_elapsed(_perf_latency, hrt_elapsed_time(&_qtime));
		perf_count(_perf_interval);
		perf_begin(_perf_cycle_time);
#endif /* WQ_ITEM_PERF */
	}

	void post_run()
	{
#if WQ_ITEM_PERF
		perf_end(_perf_cycle_time);
#endif /* WQ_ITEM_PERF */
	}

	void print_status() const;

protected:

	uint64_t	_qtime{0};       // Time work queued

private:

	px4::WorkQueue	*_wq{nullptr};

	volatile bool	_queued{false};	// only allow a single item to be queued at a time

#if WQ_ITEM_PERF
	perf_counter_t	_perf_cycle_time;
	perf_counter_t	_perf_interval;
	perf_counter_t	_perf_latency;
#endif /* WQ_ITEM_PERF */
};

} // namespace px4