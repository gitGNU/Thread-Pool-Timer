/*
 * Licensed to Bradley J. Snyder under one or more contributor 
 * license agreements.  See the NOTICE file distributed with 
 * this work for additional information regarding copyright 
 * ownership.  The ASF licenses this file to you under the 
 * Apache License, Version 2.0 (the "License"); you may not use 
 * this file except in compliance with the License.  You may 
 * obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#ifndef _TIMER_IMPL_SOURCE_
#define _TIMER_IMPL_SOURCE_

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "tp_timer.hpp"
#include "timer_impl.hpp"
#include "timer_run_loop.hpp"

namespace ThreadPoolTimer
{

TimerImpl::TimerImpl( std::function<void()> callback, const std::chrono::nanoseconds &interval ) :
    _callback(callback),
    _enabled(false),
    _interval(interval),
    _runLoop( TimerRunLoop::getSingleton() )
{
}

TimerImpl::~TimerImpl()
{
}

void TimerImpl::stopRunLoop()
{
    TimerRunLoop::getSingleton().stop();
}

void TimerImpl::startRunLoop()
{
    TimerRunLoop::getSingleton().start();
}
        
void TimerImpl::setInterval( const std::chrono::nanoseconds &interval )
{
    bool reset = false;
    if( _interval.count() < 0 )
    {
        throw TimerException("Invalid timer interval");
    }
    if( _enabled )
    {
        setEnabled(false);
        reset = true;
    }
    _interval = interval;
    if( reset && _interval.count() > 0 )
    {
        setEnabled(true);
    }
}

const std::chrono::nanoseconds &TimerImpl::getInterval()
{
    return _interval;
}

void TimerImpl::setEnabled( bool enabled )
{
    if( enabled != _enabled )
    {
        if( enabled && _interval.count() == 0 )
        {
            throw TimerException("Cannot enable timer with a zero interval");
        }
        _enabled = enabled;
        if( _enabled )
        {
            _runLoop.registerTimer(*this);
        }
        else
        {
            _runLoop.unRegisterTimer(*this);
        }
    }
}

bool TimerImpl::getEnabled()
{
    return _enabled;
}

void TimerImpl::setNextTrigger( const std::chrono::steady_clock::time_point &trigger )
{
    _nextTrigger = trigger;
}

std::chrono::steady_clock::time_point &TimerImpl::getNextTrigger()
{
    return _nextTrigger;
}
        
void TimerImpl::trigger()
{
    _callback();
}

}

#endif
