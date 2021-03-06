/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012 - 2019 m0slevin, all rights reserved.
See license.txt for more information
===========================================================================*/
/**
    @file manual.h

    /brief Ascii-format documentation, used by doxygen to create various
           printable and viewable forms.
*/
/**
    @mainpage The Mark3 Realtime Kernel

    @image html mark3.png width=480
    @image latex mark3.png width=10cm

    The Mark3 Realtime Kernel is a completely free, open-source, real-time operating
    system aimed at bringing powerful, easy-to-use multitasking to microcontroller
    systems without MMUs.

    @image html banner2.png width=640
    @image latex banner2.png width=15cm

    The RTOS is written using a super portable design that scales to many common processor
    architectures, including a variety of 8, 16, 32, and 64-bit targets.  The flexible
    CMake-based build system facilitates compiling the kernel, tests, examples, and
    user-application code for any supported target with a consistent interface.

    The API is rich and surprisingly simple – with six function calls, you can set up
    the kernel, initialize two threads, and start the scheduler.

    Written in modern C++, Mark3 makes use of modern language features that improve code
    quality, reduces duplication, and simplifies API usage.  C++ isn't your thing? No
    problem! there's a complete set of C-language bindings available to facilitate the
    use of Mark3 in a wider variety of environments.

    The Mark3 kernel releases contain zero compiler warnings, zero compiler errors, and have zero
    unit test failures.  The build and test process can be automated through
    the Mark3-docker project, allowing for easy integration with continuous integration
    environments.  The kernel is also run through static analysis tools, automated profiling
    and documentation tools.

    The source is fully-documented with doxygen, and example code is provided to illustrate
    core concepts.  The result is a performant RTOS, which is easy to read, easy to
    understand, and easy to extend to fit your needs.
*/
/**
    @page LICENSE License

    @section LIC0 License

    @verbatim

    Copyright (c) 2012 - 2019, m0slevin
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        - Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        - Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in the
          documentation and/or other materials provided with the distribution.
        - Neither the names m0slevin, Mark Slevinsky, nor the names of its
          contributors may be used to endorse or promote products derived
          from this software without specific prior written permission.
        .

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL FUNKENSTEIN SOFTWARE AND/OR ITS CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    @endverbatim
*/
/**
    @page CONFIG0 Configuring The Kernel

    @section CONFIG_I Overview

    Configuration is done through setting options in mark3cfg.h , and portcfg.h.

    mark3cfg.h contains global kernel configuration options, which determine
    specific kernel behaviors, and enable certain kernel APIs independent of the
    any target architecture.  Previous to the R7 release, all kernel configuration
    options were set from mark3cfg.h, and there was an incredible amount of granularity
    in the configuration options.

    One main motivating factor behind removing the
    granular configuration in mark3cfg.h is that it added a ton of #ifdefs throughout
    the code, which made it look less clean.  It was also difficult to maintain since
    there were too many permutations and combinations of configuration options to
    reasonably test.

    Another motivation for removing the vast array of configuration options from mark3cfg.h
    is that there's limited benefit to code size.  With the advent of modern compiler
    optimizations such as section-based garbage collection and link-time optimizations,
    compilers to a remarkable job of optimizing out unused code.  Mark3 supports these
    optimizations, allowing for nearly the same level of performance benefit as feature
    specific #ifdefs.  In short - you still only pay for what you use, without having to
    select the features you want ahead of time.

    @subsection CONFIG_MARK3CFG Kernel Configuration Options

    Kernel configuration is performed by setting #define's in mark3cfg.h to values of 0
    or 1.

    <b>KERNEL_DEBUG</b>

    When enabled, assert statements within the kernel are checked at runtime.  This is
    useful for tracking kernel-breaking changes, memory corruption, etc. in debug builds.

    Should be disabled in release builds for performance reasons.

    <b>KERNEL_STACK_CHECK</b>

    Perform stack-depth checks on threads at each context switch, which is useful
    in detecting stack overflows / near overflows.  Near-overflow detection uses
    thresholds defined in the target's portcfg.h.  Enabling this also adds the
    Thread::GetStackSlack() method, which allows a thread's stack to be profiled
    on-demand.

    Note:  When enabled, the additional stack checks result in a performance hit
    to context switches and thread initialization.

    <b>KERNEL_NAMED_THREADS</b>

    Enabling this provides the Thread::SetName() and Thread::GetName() methods,
    allowing for each thread to be named with a null-terminated const char* string.

    Note: the string passed to Thread::SetName() must persist for the lifetime of
    the thread

    <b>KERNEL_EVENT_FLAGS</b>

    This flag enables the event-flags synchronization object.  This feature allows
    threads to be blocked, waiting on specific condition bits to be set or cleared
    on an EventFlag object.

    While other synchronization objects are enabled by default, this one is configurable
    because it impacts the Thread object's member data.

    <b>KERNEL_CONTEXT_SWITCH_CALLOUT</b>

    When enabled, this feature allows a user to define a callback to be executed
    whenever a context switch occurs.  Enabling this provides a means for a user
    to track thread statistics, but it does result in additional overhead during
    a context switch.

    <b>KERNEL_THREAD_CREATE_CALLOUT</b>

    This feature provides a user-defined kernel callback that is executed whenever
    a thread is started.

    <b>KERNEL_THREAD_EXIT_CALLOUT</b>

    This feature provides a user-defined kernel callback that is executed whenever
    a thread is terminated.

    <b>KERNEL_ROUND_ROBIN</b>

    Enable round-robin scheduling within each priority level.  When selected, this
    results in a small performance hit during context switching and in the system
    tick handler, as a special software timer is used to manage the running thread's
    quantum.  Can be disabled to optimize performance if not required.

    <b>KERNEL_EXTENDED_CONTEXT</b>

    Provide a special data pointer in the thread object, which may be used to add
    additional context to a thread.  Typically this would be used to implement
    thread-local-storage.

    @subsection CONFIG_PORTCFG Port Configuration Options

    The bulk of kernel configuration options reside in the target's portcfg.h file.
    These options determine various sizes, priorities, and default values related
    to registers, timers, and threads.  Some ports may define their own configuration
    options used locally by its kerneltimer/kernelswi/threadport modules; these are
    not shown here.  The common configuration options are described below.

    <b>KERNEL_NUM_PRIORITIES</b>

    Define the number of thread priorities that the kernel's scheduler will
    support.  The number of thread priorities is limited only by the memory
    of the host CPU, as a ThreadList object is statically-allocated for each
    thread priority.

    In practice, systems rarely need more than 32 priority levels, with the
    most complex having the capacity for 256.

    <b>KERNEL_TIMERS_THREAD_PRIORITY</b>

    Define the priority at which the kernel timer thread runs.  Typically,
    this needs to be one of the highest

    Note:  Other threads at or above the timer thread's priority are not permitted
    to use the kernel's Timer API, as the thread relies on strict priority scheduling
    to manage the global timer list without requiring additional/excessive
    critical sections.

    <b>THREAD_QUANTUM_DEFAULT</b>

    Number of milliseconds to set as the default epoch for round-robin scheduling
    when multiple ready threads are active within the same priority.

    <b>KERNEL_STACK_GUARD_DEFAULT</b>

    Set the minimum number of words of margin that each thread's stack must maintain.
    If a thread's stack grows into theis margin, a kernel assert will be generated in
    debug builds.  This is useful for ensuring that threads are not in danger of
    overflowing their stacks during development/verification.

    <b>K_WORD</b>

    Define the size of a data word (in bytes) on the target

    <b>K_ADDR</b>

    Define the size of an address (in bytes) on the target.  This typically only differs
    from K_WORD on Harvard-architecture CPUs.

    <b>K_INT</b>

    Define a type to be used as an integer by the kernel.

    <b>PORT_PRIO_TYPE</b>

    Set a base datatype used to represent each element of the scheduler's
    priority bitmap.

    <b>PORT_PRIO_MAP_WORD_SIZE</b>

    Size of PORT_PRIO_TYPE in bytes

    <b>PORT_SYSTEM_FREQ </b>

    Define the running CPU frequency.  This may be an integer constant, or an alias
    for another variable which holds the CPU's current running frequency.

    <b>PORT_TIMER_FREQ</b>

    Set the timer tick frequency.  This is the frequency at which the fixed-frequency
    kernel tick interrupt occurs.

    <b>PORT_KERNEL_DEFAULT_STACK_SIZE</b>

    Define the default thread stack size (in bytes)

    <b>PORT_KERNEL_TIMERS_THREAD_STACK</b>

    Define the Timer thread's stack size (in bytes)

    <b>PORT_TIMER_COUNT_TYPE</b>

    Define the native type corresponding to the target timer hardware's counter register.

    <b>PORT_MIN_TIMER_TICKS</b>

    Minimum number of timer ticks for any delay or sleep, required to ensure that a timer cannot
    be initialized to a negative value.

    <b>PORT_OVERLOAD_NEW</b>

    Set this to 1 to overload the system's New/Free functions with the kernel's allocator functions.
    A user must configure the Kernel's allocator functions to point to a real heap implementation
    backed with real memory in order to use dynamic object creation.

    <b>PORT_STACK_GROWS_DOWN</b>

    Set this to 1 if the stack grows down in the target architecture, or 0 if the stack grows up

    <b>PORT_USE_HW_CLZ</b>

    Set this to 1 if the target CPU/toolchain supports an optimized Count-leading-zeros instruction,
    or count-leading-zeros intrinsic.  If such functionality is not available, a general-purpose
    implementation will be used.

    @sa portcfg.h
    @sa mark3cfg.h
*/
/**
    @page BUILD0 Building The Kernel

    Mark3 contains its own build system and support scripts, based on CMake and Ninja.

    @section BUILDPRE Prerequisites

    To build via CMake, a user requires a suitable, supported toolchain (i.e. gcc-avr, arm-none-eabi-gcc),
    CMake 3.4.2 or higher, and a backend supported by CMake (i.e. Ninja build).

    For example, on debian-based distributions, such as Ubuntu, the avr toolchain can be installed using:

            apt-get install avr-libc gcc-avr cmake ninja-build

    Once a sane build environment has been created, the kernel, libraries, examples and tests can be built
    by running `ninja` from the target's build directory (/out/<target>/).  By default, Mark3 builds for the
    atmega328p target, but the target can be selected by manually configuring environment variables, or
    by running the ./build/set_target.sh script as follows:

            ./build/set_target.sh <architecture> <variant> <toolchain>

        Where:
             <architecture> is the target CPU architecture(i.e. avr, msp430, cm0, cm3, cm4f)
             <variant>      is the part name (i.e. atmega328p, msp430f2274, generic)
             <toolchain>    is the build toolchain (i.e. gcc)

    This script is a thin wrapper for the cmake configuration commands, and clears the output directory before
    re-initializing cmake for the selected target.

    @section BUILDBuiLD Building

    To build the Mark3 kernel and middleware libraries for a generic ARM Cortex-M0 using a pre-configured
    arm-none-eabi-gcc toolchain, one would run the following commands:

            ./build/set_target.sh cm0 generic gcc
            ./build/build.sh

    To perform an incremental build, go into the target's cmake build directory (/out/<target/) and simply run 'ninja'.

    To perform a clean build, go into the target's cmake build directory (/out/<target/) and run 'ninja clean', then 'ninja'.

    Note that not all libraries/tests/examples will build in all kernel configurations.  The default kernel
    configuration may need adjustment/tweaking to support a specific part.  See the documentation for details.

    @section BUILDTARG Supported Targets

    Currently, Mark3 supports GCC toolchains for the following targets:

            atmega328p
            atmega644
            atmega1284p
            atxmega256a3
            atmega1280
            atmega2560
            msp430f2274
            ARM Cortex-M0
            ARM Cortex-M3 (Note: Also supports Cortex-M4)
            ARM Cortex-M4F (Note: Also supports Cortex-M7)

    @section BUILDMORE More Info

    The Mark3 project also has a ready-made docker image to simplify the process.  Please
    see the Mark3 Docker project here for more details:

    https://github.com/moslevin/mark3-docker

*/
/**
    @page START The Mark3 API

    @section START0 Kernel Setup

    This section details the process of defining threads, initializing the
    kernel, and adding threads to the scheduler.

    If you're at all familiar with real-time operating systems, then these
    setup and initialization steps should be familiar.  I've tried very hard
    to ensure that as much of the heavy lifting is hidden from the user, so
    that only the bare minimum of calls are required to get things started.

    The examples presented in this chapter are real, working examples taken
    from the ATmega328p port.

    First, you'll need to create the necessary data structures and functions
    for the threads:
     -# Create a Thread object for all of the "root" or "initial" tasks.
     -# Allocate stacks for each of the Threads
     -# Define an entry-point function for each Thread

    This is shown in the example code below:

    @code
    //---------------------------------------------------------------------------
    #include "thread.h"
    #include "kernel.h"

    //1) Create a thread object for all of the "root" or "initial" tasks
    static Thread AppThread;
    static Thread IdleThread;

    //2) Allocate stacks for each thread - in bytes
    #define STACK_SIZE_APP      (192)
    #define STACK_SIZE_IDLE     (128)

    static K_WORD awAppStack[STACK_SIZE_APP / sizeof(K_WORD)];
    static K_WORD awIdleStack[STACK_SIZE_IDLE / sizeof(K_WORD)];

    //3) Define entry point functions for each thread
    void AppThread(void);
    void IdleThread(void);

    @endcode

    Next, we'll need to add the required kernel initialization code to main.
    This consists of running the Kernel's init routine, initializing all of
    the threads we defined, adding the threads to the scheduler, and finally
    calling Kernel::Start(), which transfers control of the system to the
    RTOS.

    These steps are illustrated in the following example.

    @code

    int main(void)
    {
        //1) Initialize the kernel prior to use
        Kernel::Init();				// MUST be before other kernel ops

        //2) Initialize all of the threads we've defined
        AppThread.Init(	awAppStack,             // Pointer to the stack
                        sizeof(awAppStack),		// Size of the stack
                        1,                      // Thread priority
                        (void*)AppEntry,        // Entry function
                        nullptr );              // Entry function argument

        IdleThread.Init( awIdleStack,           // Pointer to the stack
                         sizeof(awIdleStack),	// Size of the stack
                         0,                     // Thread priority
                         (void*)IdleEntry,      // Entry function
                         nullptr );             // Entry function argument

        //3) Add the threads to the scheduler
        AppThread.Start();			// Actively schedule the threads
        IdleThread.Start();

        //4) Give control of the system to the kernel
        Kernel::Start();			// Start the kernel!
    }
    @endcode

    Not much to it, is there?
    There are a few noteworthy points in this code, though.

    In order for the kernel to work properly, a system must always contain
    an idle thread; that is, a thread at priority level 0 that never
    blocks.  This thread is responsible for performing any of the low-level
    power management on the CPU in order to maximize battery life in an
    embedded device.  The idle thread must also never block, and it
    must never exit.  Either of these operations will cause undefined
    behavior in the system.

    The App thread is at a priority level greater-than 0.  This ensures that
    as long as the App thread has something useful to do, it will be given
    control of the CPU.  In this case, if the app thread blocks, control
    will be given back to the Idle thread, which will put the CPU into
    a power-saving mode until an interrupt occurs.

    Stack sizes must be large enough to accommodate not only the requirements
    of the threads, but also the requirements of interrupts - up to the
    maximum interrupt-nesting level used.  Stack overflows are super-easy
    to run into in an embedded system; if you encounter strange and
    unexplained behavior in your code, chances are good that one of your
    threads is blowing its stack.

    @section THREADS Threads

    Mark3 Threads act as independent tasks in the system.  While they share
    the same address-space, global data, device-drivers, and system
    peripherals, each thread has its own set of CPU registers and stack,
    collectively known as the thread's <b>context</b>.  The context
    is what allows the RTOS kernel to rapidly switch between threads
    at a high rate, giving the illusion that multiple things are happening
    in a system, when really, only one thread is executing at a time.

    @subsection TH_SETUP Thread Setup

    Each instance of the Thread class represents a thread, its stack,
    its CPU context, and all of the state and metadata maintained by the
    kernel.  Before a Thread will be scheduled to run, it must first be
    initialized with the necessary configuration data.

    The Init function gives the user the opportunity to set the stack,
    stack size, thread priority, entry-point function, entry-function
    argument, and round-robin time quantum:

    Thread stacks are pointers to blobs of memory (usually char arrays)
    carved out of the system's address space.  Each thread must have a
    stack defined that's large enough to handle not only the requirements
    of local variables in the thread's code path, but also the maximum
    depth of the ISR stack.

    Priorities should be chosen carefully such that the shortest tasks
    with the most strict determinism requirements are executed first -
    and are thus located in the highest priorities.  Tasks that take the
    longest to execute (and require the least degree of responsiveness)
    must occupy the lower thread priorities.  The idle thread must be
    the only thread occupying the lowest priority level.

    The thread quantum only aplies when there are multiple threads in
    the ready queue at the same priority level.  This interval is used
    to kick-off a timer that will cycle execution between the threads
    in the priority list so that they each get a fair chance to execute.

    The entry function is the function that the kernel calls first when
    the thread instance is first started.  Entry functions have at most
    one argument - a pointer to a data-object specified by the user
    during initialization.

    An example thread initailization is shown below:

    @code
    Thread clMyThread;
    K_WORD awStack[192];

    void AppEntry(void)
    {
        while(1) {
            // Do something!
        }
    }

    ...
    {
        clMyThread.Init(awStack,             // Pointer to the stack to use by this thread
                        sizeof(awStack),	 // Size of the stack in bytes
                        1,                   // Thread priority (0 = idle, 7 = max)
                        (void*)AppEntry,     // Function where the thread starts executing
                        nullptr );           // Argument passed into the entry function

    }

    @endcode

    Once a thread has been initialized, it can be added to the scheduler by calling:

    @code

    clMyThread.Start();

    @endcode

    The thread will be placed into the Scheduler's queue at the designated
    priority, where it will wait its turn for execution.

    @subsection TH_ENTRY Entry Functions

    Mark3 Threads should not run-to-completion - they should execute as
    infinite loops that perform a series of tasks, appropriately partitioned
    to provide the responsiveness characteristics desired in the system.

    The most basic Thread loop is shown below:

    @code

    void Thread( void *param )
    {
        while(1) {
            // Do Something
        }
    }

    @endcode

    Threads can interact with eachother in the system by means of
    synchronization objects (Semaphore), mutual-exclusion objects (Mutex),
    Inter-process messaging (MessageQueue), and timers (Timer).

    Threads can suspend their own execution for a predetermined period of
    time by using the static Thread::Sleep() method.  Calling this will
    block the Thread's executin until the amount of time specified has
    ellapsed.  Upon expiry, the thread will be placed back into the ready
    queue for its priority level, where it awaits its next turn to run.

    @section TIMERS Timers

    Timer objects are used to trigger callback events periodic or on a one-shot
    (alarm) basis.

    While extremely simple to use, they provide one of the most powerful
    execution contexts in the system.  The timer callbacks execute from within
    a timer thread, as a result of a semaphore posted in a timer interrupt.
    Timer callbacks are executed from a high-priority thread -- typically at the
    highest priority thread in the system.  Care must be taken to ensure
    that timer callbacks execute as quickly as possible to minimize the
    impact of processing on the throughput of tasks in the system.
    Wherever possible, heavy-lifting should be deferred to lower-priroity
    threads by way of semaphores or messages.

    Below is an example showing how to start a periodic system timer
    which will trigger every second:

    @code
    {
        Timer clTimer;
        clTimer.Init();

        clTimer.Start( 1000,              //!< Time in ms to use as the interval
                     1,                   //!< 1 = repeat, 0 = one-shot
                     MyCallback,          //!< Function that gets called on expiry
                     (void*)&my_data );   //!< Data passed to the timer callback

        ... // Keep doing work in the thread
    }

    // Callback function, executed from the timer-expiry context.
    void MyCallBack( Thread *pclOwner_, void *pvData_ )
    {
        LED.Flash(); // Flash an LED.
    }

    @endcode

    @section SEM Semaphores

    Semaphores are used to synchronized execution of threads based on the
    availability (and quantity) of application-specific resources in the
    system.  They are extremely useful for solving producer-consumer
    problems, and are the method-of-choice for creating efficient, low
    latency systems, where ISRs post semaphores that are handled
    from within the context of individual threads.  (Yes, Semaphores
    can be posted - but not pended - from the interrupt context).

    The following is an example of the producer-consumer usage of a binary
    semaphore:

    @code

    Semaphore clSemaphore; // Declare a semaphore shared between a producer and a consumer thread.

    void Producer()
    {
        clSemaphore.Init(0, 1);  //!< Intialize as a binary semaphore, with an initial value of 0, maximum value of 1.
        while(1) {
            // Do some work, create something to be consumed

            // Post a semaphore, allowing another thread to consume the data
            clSemaphore.Post();
        }
    }

    void Consumer()
    {
        // Assumes semaphore initialized before use...
        While(1) {
            // Wait for new data from the producer thread
            clSemaphore.Pend();

            // Consume the data!
        }
    }
    @endcode

    And an example of using semaphores from the ISR context to perform event-
    driven processing.

    @code

    Semaphore clSemaphore;

    __interrupt__ MyISR()
    {
        clSemaphore.Post(); // Post the interrupt.  Lightweight when uncontested.
    }

    void MyThread()
    {
        clSemaphore.Init(0, 1); // Ensure this is initialized before the MyISR interrupt is enabled.
        while(1) {
            // Wait until we get notification from the interrupt
            clSemaphore.Pend();

            // Interrupt has fired, do the necessary work in this thread's context
            HeavyLifting();
        }
    }

    @endcode

    @section MUT Mutexes

    Mutexes (Mutual exclusion objects) are provided as a means of creating
    "protected sections" around a particular resource, allowing for access
    of these objects to be serialized. Only one thread can hold the mutex at
    a time - other threads have to wait until the region is released by the
    owner thread before they can take their turn operating on the protected
    resource. Note that mutexes can only be owned by threads - they are not
    available to other contexts (i.e. interrupts). Calling the mutex APIs
    from an interrupt will cause catastrophic system failures.

    Note that these objects are also not recursive- that is, the owner
    thread can not attempt to claim a mutex more than once.

    Prioritiy inheritence is provided with these objects as a means to
    avoid prioritiy inversions. Whenever a thread at a priority than the
    mutex owner blocks on a mutex, the priority of the current thread is
    boosted to the highest-priority waiter to ensure that other tasks at
    intermediate priorities cannot artificically prevent progress from being
    made.

    Mutex objects are very easy to use, as there are only three operations
    supported: Initialize, Claim and Release. An example is shown below.

    @code

    Mutex clMutex;	// Create a mutex globally.

    void Init()
    {
        // Initialize the mutex before use.
        clMutex.Init();
    }

    // Some function called from a thread
    void Thread1Function()
    {
        clMutex.Claim();

        // Once the mutex is owned, no other thread can
        // enter a block protect by the same mutex

        my_protected_resource.do_something();
        my_protected_resource.do_something_else();

        clMutex.Release();
    }

    // Some function called from another thread
    void Thread2Function()
    {
        clMutex.Claim();

        // Once the mutex is owned, no other thread can
        // enter a block protect by the same mutex

        my_protected_resource.do_something();
        my_protected_resource.do_different_things();

        clMutex.Release();
    }

    @endcode

    @section EVF Event Flags

    Event Flags are another synchronization object, conceptually similar
    to a semaphore.

    Unlike a semaphore, however, the condition on which threads are unblocked
    is determined by a more complex set of rules.  Each Event Flag object
    contains a 16-bit field, and threads block, waiting for combinations of
    bits within this field to become set.

    A thread can wait on any pattern of bits from this field to be set, and
    any number of threads can wait on any number of different patterns.  Threads
    can wait on a single bit, multiple bits, or bits from within a subset of
    bits within the field.

    As a result, setting a single value in the flag can result in any number of
    threads becoming unblocked simultaneously.  This mechanism is extremely
    powerful, allowing for all sorts of complex, yet efficient, thread
    synchronization schemes that can be created using a single shared object.

    Note that Event Flags can be set from interrupts, but you cannot wait on
    an event flag from within an interrupt.

    Examples demonstrating the use of event flags are shown below.

    @code

        // Simple example showing a thread blocking on a multiple bits in the
        // fields within an event flag.

        EventFlag clEventFlag;

        int main()
        {
            ...
            clEventFlag.Init(); // Initialize event flag prior to use
            ...
        }

        void MyInterrupt()
        {
            // Some interrupt corresponds to event 0x0020
            clEventFlag.Set(0x0020);
        }

        void MyThreadFunc()
        {
            ...
            while(1) {
                ...
                uint16_t u16WakeCondition;

                // Allow this thread to block on multiple flags
                u16WakeCondition = clEventFlag.Wait(0x00FF, EventFlagOperation::Any_Set);

                // Clear the event condition that caused the thread to wake (in this case,
                // u16WakeCondtion will equal 0x20 when triggered from the interrupt above)
                clEventFlag.Clear(u16WakeCondition);

                // <do something>
            }
        }

    @endcode

    @section MSG Messages

    Sending messages between threads is the key means of synchronizing
    access to data, and the primary mechanism to perform asynchronous data
    processing operations.

    Sending a message consists of the following operations:

    - Obtain a Message object from the global message pool
    - Set the message data and event fields
    - Send the message to the destination message queue
    .

    While receiving a message consists of the following steps:

    - Wait for a messages in the destination message queue
    - Process the message data
    - Return the message back to the global message pool
    .

    These operations, and the various data objects involved are discussed in
    more detail in the following section.

    @subsection MSGO Message Objects

    Message objects are used to communicate arbitrary data between threads
    in a safe and synchronous way.

    The message object consists of an event code field and a data field.
    The event code is used to provide context to the message object, while
    the data field (essentially a void * data pointer) is used to provide a
    payload of data corresponding to the particular event.

    Access to these fields is marshalled by accessors - the transmitting
    thread uses the SetData() and SetCode() methods to seed the data, while
    the receiving thread uses the GetData() and GetCode() methods to
    retrieve it.

    By providing the data as a void data pointer instead of a fixed-size
    message, we achieve an unprecedented measure of simplicity and
    flexibility. Data can be either statically or dynamically allocated, and
    sized appropriately for the event without having to format and reformat
    data by both sending and receiving threads. The choices here are left to
    the user - and the kernel doesn't get in the way of efficiency.

    It is worth noting that you can send messages to message queues from
    within ISR context.  This helps maintain consistency, since the same
    APIs can be used to provide event-driven programming facilities
    throughout the whole of the OS.

    @subsection MSGQ Message Queues

    Message objects specify data with context, but do not specify where the
    messages will be sent. For this purpose we have a MessageQueue object.
    Sending an object to a message queue involves calling the
    MessageQueue::Send() method, passing in a pointer to the Message object
    as an argument.

    When a message is sent to the queue, the first thread blocked on the
    queue (as a result of calling the MessageQueue Receive() method) will
    wake up, with a pointer to the Message object returned.

    It's worth noting that multiple threads can block on the same message
    queue, providing a means for multiple threads to share work in parallel.

    @subsection MSGQS Messaging Example

    @code

    // Message queue object shared between threads
    MessageQueue clMsgQ;

    // Function that initializes the shared message queue
    void MsgQInit()
    {
        clMsgQ.Init();
    }

    // Function called by one thread to send message data to
    // another
    void TxMessage()
    {
        // Get a message, initialize its data
        Message *pclMesg = MyMessagePool.Pop();

        pclMesg->SetCode(0xAB);
        pclMesg->SetData((void*)some_data);

        // Send the data to the message queue
        clMsgQ.Send(pclMesg);
    }

    // Function called in the other thread to block until
    // a message is received in the message queue.
    void RxMessage()
    {
        Message *pclMesg;

        // Block until we have a message in the queue
        pclMesg = clMsgQ.Receive();

        // Do something with the data once the message is received
        pclMesg->GetCode();

        // Free the message once we're done with it.
        MyMessagePool.Push(pclMesg);
    }
    @endcode

    @section MBX Mailboxes

    Another form of IPC is provided by Mark3, in the form of Mailboxes and Envelopes.

    Mailboxes are similar to message queues in that they provide a synchronized interface
    by which data can be transmitted between threads.

    Where Message Queues rely on linked lists of lightweight message objects (containing
    only message code and a void* data-pointer), which are inherently abstract,
    Mailboxes use a dedicated blob of memory, which is carved up into fixed-size
    chunks called Envelopes (defined by the user), which are sent and received.  Unlike
    message queues, mailbox data is copied to and from the mailboxes dedicated pool.

    Mailboxes also differ in that they provide not only a blocking "receive" call, but
    also a blocking "send" call, providing the opportunity for threads to block on
    "mailbox full" as well as "mailbox empty" conditions.

    All send/receive APIs support an optional timeout parameter if the KERNEL_USE_TIMEOUTS
    option has been configured in mark3cfg.h

    @subsection MBXE Mailbox Example

    @code

    // Create a mailbox object, and define a buffer that will be used to store the
    // mailbox' envelopes.
    static Mailbox clMbox;
    static uint8_t aucMBoxBuffer[128];

    ...
    void InitMailbox(void)
    {
        // Initialize our mailbox, telling it to use our defined buffer for envelope
        // storage.  Pass in the size of the buffer, and set the size of each
        // envelope to 16 bytes.  This gives u16 a mailbox capacity of (128 / 16) = 8
        // envelopes.
        clMbox.Init((void*)aucMBoxBuffer, 128, 16);

    }

    ...
    void SendThread(void)
    {
        // Define a buffer that we'll eventually send to the
        // mailbox.  Note the size is the same as that of an
        // envelope.
        uint8_t aucTxBuf[16];

        while(1) {
            // Copy some data into aucTxBuf, a 16-byte buffer, the
            // same size as a mailbox envelope.
            ...

            // Deliver the envelope (our buffer) into the mailbox
            clMbox.Send((void*)aucTxBuf);
        }
    }

    ...
    void RecvThred(void)
    {
        uint8_t aucRxBuf[16];

        while(1) {
            // Wait until there's a message in our mailbox.  Once
            // there is a message, read it into our local buffer.
            cmMbox.Receive((void*)aucRxBuf);

            // Do something with the contents of aucRxBuf, which now
            // contains an envelope of data read from the mailbox.
            ...
        }
    }

    @endcode

    @section NTF Notification Objects

    Notification objects are the most lightweight of all blocking objects supplied
    by Mark3.

    using this blocking primative, one or more threads wait for the notification
    object to be signalled by code elsewhere in the system (i.e. another
    thread or interrupt).  Once the the notification has been signalled, all
    threads currently blocked on the object become unblocked.

    @subsection NTFE Notification Example

    @code NOTIFIER

    static Notify clNotifier;

    ...
    void MyThread(void *unused_)
    {
        // Initialize our notification object before use
        clNotifier.Init();

        while (1)
        {
            // Wait until our thread has been notified that it
            // can wake up.
            clNotify.Wait();

            ...
            // Thread has woken up now -- do something!
        }
    }

    ...
    void SignalCallback(void)
    {
        // Something in the system (interrupt, thread event, IPC,
        // etc.,) has called this function.  As a result, we need
        // our other thread to wake up.  Call the Notify object's
        // Signal() method to wake the thread up.  Note that this
        // will have no effect if the thread is not presently
        // blocked.

        clNotify.Signal();
    }

    @endcode

    @section CONDVAR Condition Variables

    Condition Variables, implemented in Mark3 with the ConditionVariable class,
    provide an implementation of the classic Monitor pattern.  This object allows
    a thread to wait for a specific condition to occur, claiming a shared lock
    once the condition is met.  Threads may also choose to signal a single blocking
    thread to indicate a condition has changed, or broadcast condition changes to
    all waiting threads.

    @subsection CONDVAREX Condition Variable Example

    @code

    // Define a condition variable object, a shared lock, and
    // a piece of common data shared between threads to represent
    // a condition.

    // Assume Mutex and ConditionVariable are initialized
    // prior to use.
    static ConditionVariable clCondVar;
    static Mutex clSharedLock;
    static volatile int iCondition = 0;

    ...

    void CondThread1(void *unused_)
    {
        while (1)
        {
            // Wait until
            clCondVar.Wait(&clSharedLock);

            // Only act on a specific condition
            if (iCondition == 1337) {
                // Do something
            }

            clSharedLock.Release();
        }
    }

    void CondThread2(void *unused_)
    {
        // Assume Mutex and ConditionVariable are initialized
        // prior to use.
        while (1)
        {
            // Wait until
            clCondVar.Wait(&clSharedLock);

            // Act on a *different* condition than the other thread
            if (iCondition == 5454) {
                // Do something
            }

            clSharedLock.Release();
        }
    }

    void SignalThread(void* unused)
    {
        while (1) {
            // Sleep for a while
            Thread::Sleep(100);

            // Update the condition in a thread-safe manner
            clSharedLock.Claim();
            iCondition = 1337;
            clSharedLock.Release();

            // Wake one thread to check for the updated condition
            clCondVar.Signal();

            // Sleep for a while
            Thread::Sleep(100);

            // Update the condition in a thread-safe manner
            clSharedLock.Claim();
            iCondition = 1337;
            clSharedLock.Release();

            // Wake all threads to check for the updated condition
            clCondVar.Broadcast();
        }
    }

    @endcode

    @section RWLOCKS Reader-Write Locks

    Reader-Writer locks are provided in Mark3 to provide an efficient way for
    multiple threads to share concurrent, non-destructive access to a resource,
    while preventing concurrent destructive/non-destructive accesses.  A single
    "writer" may hold the lock, or 1-or-more "readers" may hold the lock.  In the
    case that readers hold the lock, writers will block until all readers have
    relinquished their access to the resource.  In the case that a writer holds the
    lock, all other readers and writers must wait until the lock is relinquished.

    @subsection RWLOCKEX Reader-Write Lock Example

    @code

    void WriterTask(void* param)
    {
        auto pclRWLock = static_cast<ReaderWriterLock*>(param);

        pclRWLock->AcquireWriter();
        // All other readers and writers will have to wait until
        // the lock is released.
        iNumWrites++;
        ...
        pclRWLock->ReleaseWriter();
    }

    void ReaderTask(void* param)
    {
        auto pclRWLock = static_cast<ReaderWriterLock*>(param);

        pclRWLock->AcquireReader();
        // Any number of reader threads can also acquire the lock
        // without having to block, waiting for this task to release it.
        // Writers must block until all readers have released their references
        // to the lock.
        iNumReads++;
        ...
        pclRWLock->ReleaseReader();
    }

    @endcode

    @section SLP Sleep

    There are instances where it may be necessary for a thread to poll a
    resource, or wait a specific amount of time before proceeding to operate
    on a peripheral or volatile piece of data.

    While the Timer object is generally a better choice for performing
    time-sensitive operations (and certainly a better choice for periodic
    operations), the Thread::Sleep() method provides a convenient (and
    efficient) mechanism that allows for a thread to suspend its execution
    for a specified interval.

    Note that when a thread is sleeping it is blocked, during which other
    threads can operate, or the system can enter its idle state.

    @code
    int GetPeripheralData();
    {
        int value;
        // The hardware manual for a peripheral specifies that
        // the "foo()" method will result in data being generated
        // that can be captured using the "bar()" method.
        // However, the value only becomes valid after 10ms

        peripheral.foo();
        Thread::Sleep(10);	// Wait 10ms for data to become valid
        value = peripheral.bar();
        return value;
    }
    @endcode

    @section RR Round-Robin Quantum

    Threads at the same thread priority are scheduled using a round-robin
    scheme.  Each thread is given a timeslice (which can be configured)
    of which it shares time amongst ready threads in the group.  Once
    a thread's timeslice has expired, the next thread in the priority
    group is chosen to run until its quantum has expired - the cycle
    continues over and over so long as each thread has work to be done.

    By default, the round-robin interval is set at 4ms.

    This value can be overridden by calling the thread's SetQuantum()
    with a new interval specified in milliseconds.

    @section COROU Coroutines

    Mark3 implements a coroutine scheduler, capable of managing a set
    of prioritized run-to-completion tasks.  This is a simple and lightweight
    cooperative scheduling mechanism, that trades the preemption and
    synchonization capabilities of threads for simplicity.  It is an
    ideal mechanism to use for background processes in a system, or
    for performing a coordinating a group of tasks where the relative
    priority of task execution is important, but the duration of individual
    tasks is less important.

    Like the Mark3 thread scheduler, the coroutine scheduler supports
    multiple priorities of tasks.  Multiple coroutines activated at the
    same priority level are executed in first-in first-out order.

    Coroutines are activated by interrupts, threads, or from within other
    co-routines.  Once activated, the co-routine is able to be scheuduled.

    The coroutine scheduler is called by the application from a thread
    priority.  So long as there are activated tasks to be scheduled, the
    scheduler will return a pointer to the highest priority active coroutine
    to be run.

    Running a co-routine de-activates the co-routine, meaning that coroutines
    must be re-activated every time they are run.

    @code

    Coroutine cr1;
    Coroutine cr2;
    Coroutine cr3;

    void CoRoutineInit()
    {
        CoScheduler::Init();

        cr1.Init(<priority>, <handler function>, <data passed to handler function>);
        cr2.Init(<priority>, <handler function>, <data passed to handler function>);
        cr3.Init(<priority>, <handler function>, <data passed to handler function>);
    }

    void AsyncEvent1()
    {
        // Some event occurred requiring us to run cr1.  Activating a task does not
        // cause the coroutine to be run immediately, but schedules it to be returned by
        // CoScheduler::Schedule() when there are no higher-priroity active tasks to
        // run.
        cr1.Activate();
    }

    void AsyncEvent2()
    {
        // Some event occurred requiring us to run cr2.
        cr2.Activate();
    }

    void AsyncEvent3()
    {
        // Some event occurred requiring us to run cr3.
        cr3.Activate();
    }

    ...
    void CoRoutineSchedulerTask()
    {
        while (1) {
            auto* coroutineToRun = CoScheduler::Schedule();
            if (coroutineToRun) {
                // Scheduled task is removed from the scheduler once run.  It must
                // be reactivated to be run again
                coroutineToRun->Run();
            }
            // If run from idle thread, could go into sleep mode instead of running in
            // a tight loop.
        }
    }

    @endcode

    @section CRG Critical Guards

    Often times, it is useful in a real-time multi-threaded system to place a
    critical section around a block of code to protect it against concurrent access,
    or to protect global data from access from interrupts.  In Mark3 there are a
    few different ways of implementing critical sections.  Historically, the CS_ENTER()
    and CS_EXIT() macros were used for this purpose; however, the usage of matching
    entry/exit macros can often-times be cumbersome and error-prone.

    The CriticalGuard object allows a user to wrap a block of code in a critical
    section, where the critical section is entered when the critical guard object
    is declared, and the critical section is exited when the object goes out-of-scope.

    It is essentially an RAII-style critical section object, that provides the benefit
    of critical sections without the hassle of having to carefully match enter/exit
    statements.

    @code
    void MyFunc()
    {
        // operations outside of critical section
        {
            auto cg = CriticalGuard{};
            // operations protected by critical section
            // critical section ends when CriticalGuard object goes out of scope
        }
        // Operations outside of critical section
    }
    @endcode

    @section LKG Lock Guards

    The modern C++ standard library provides RAII-style mutex locking.  Unfortunately
    such an implementation is not usable in the context of Mark3, because the STL
    implementation supplied for an embedded C++ toolchain would not be aware of
    Mark3's threading model or synchronization primatives.  Mark3 provides its own
    RAII mutex locking mechanism in the form of LockGuard objects.  When a LockGuard
    object is declared (referencing a valid and initialized mutex object at construction),
    the lock is claimed upon declaration, and released when the object goes out-of-scope.

    @code
    Mutex clMutex;
    void MyFunc()
    {
        // operations outside of mutex-locked context
        {
            auto lg = LockGuard{&clMutex};
            // operations inside mutex-locked context
            // mutex automatically unlocked when LockGuard goes out of scope
        }
        // Operations outside of mutex-locked context
    }
    @endcode

    @section SCG Scheduler Guards

    Similar to the LockGuard and CriticalGuard objects, the SchedulerGuard object
    provides a coped scheduler-disabled blocks.  This essentially gives the executing
    thread exclusive control of the CPU - except for interrupts - for the duration
    of the block wrapped in the SchedulerGuard.  The scheduler is disabled when the
    object is declared, and scheduler state is restored when the SchedulerGuard object
    goes out-of-scope.  This is yet another form of RAII-based resource locking in Mark3.

    @code
    void MyFunc()
    {
        // thread scheduler enabled
        {
            auto sg = SchedulerGuard{};
            // thread-safety guaranteed - scheduler disabled
            // thread scheduler re-enabled when SchedulerGuard object goes out of scope
        }
        // thread scheduler enabled
    }
    @endcode

*/
/**
    @page WHYMARK3 Why Mark3?

    My first job after graduating from university in 2005 was with a small company
    that had a very old-school, low-budget philosophy when it came to software
    development. Every make-or-buy decision ended with "make" when it came to
    tools. It was the kind of environment where vendors cost us money, but manpower
    was free. In retrospect, we didn't have a ton of business during the time
    that I worked there, and that may have had something to do with the fact that
    we were constantly short on ready cash for things we could code ourselves.

    Early on, I asked why we didn't use industry-standard tools - like JTAG debuggers
    or IDEs. One senior engineer scoffed that debuggers were tools for wimps - and
    something that a good programmer should be able to do without. After all - we
    had serial ports, GPIOs, and a bi-color LED on our boards. Since these were
    built into the hardware, they didn't cost us a thing. We also had a single
    software "build" server that took 5 minutes to build a 32k binary on its best
    days, so when we had to debug code, it was a painful process of trial and error,
    with lots of Youtube between iterations. We complained that tens of thousands
    of dollars of productivity was being flushed away that could have been solved
    by implementing a proper build server - and while we eventually got our wish,
    it took far more time than it should have.

    Needless to say, software development was painful at that company. We made
    life hard on ourselves purely out of pride, and for the right to say that we
    walked "up-hills both ways through 3 feet of snow, everyday". Our code was
    tied ever-so-tightly to our hardware platform, and the system code was
    indistinguishable from the application. While we didn't use an RTOS, we had
    effectively implemented a 3-priority threading scheme using a carefully
    designed interrupt nesting scheme with event flags and a while(1) superloop
    running as a background thread. Nothing was abstracted, and the code was
    always optimized for the platform, presumably in an effort to save on code
    size and wasted cycles. I asked why we didn't use an RTOS in any of our systems
    and received dismissive scoffs - the overhead from thread switching and
    maintaining multiple threads could not be tolerated in our systems according
    to our chief engineers. In retrospect, our ad-hoc system was likely as large
    as my smallest kernel, and had just as much context switching (althrough it
    was hidden by the compiler).

    And every time a new iteration of our product was developed, the firmware took
    far too long to bring up, because the algorithms and data structures had to
    be re-tooled to work with the peripherals and sensors attached to the new
    boards. We worked very hard in an attempt to reinvent the wheel, all in the
    name of producing "efficient" code.

    Regardless, I learned a lot about embedded software development.

    Most important, I learned that good design is the key to good software; and
    good design doesn't have to come at a price. In all but the smallest of
    projects, the well-designed, well-abstracted code is not only more portable,
    but it's usually smaller, easier to read, and easier to reuse.

    Also, since we had all the time in the world to invest in developing our own
    tools, I gained a lot of experience building them, and making use of good,
    free PC tools that could be used to develop and debug a large portion of our
    code. I ended up writing PC-based device and peripheral simulators,
    state-machine frameworks, and abstractions for our horrible ad-hoc system
    code. At the end of the day, I had developed enough tools that I could solve
    a lot of our development problems without having to re-inventing the wheel
    at each turn. Gaining a background in how these tools worked gave me a better
    understanding of how to use them - making me more productive at the jobs that
    I've had since.

    I am convinced that designing good software takes honest effort up-front,
    and that good application code cannot be written unless it is based on a
    solid framework. Just as the wise man builds his house on rocks, and not on
    sand, wise developers write applications based on a well-defined platforms.
    And while you can probably build a house using nothing but a hammer and sheer
    will, you can certainly build one a lot faster with all the right tools.

    This conviction lead me to development my first RTOS kernel in 2009 - FunkOS.
    It is a small, yet surprisingly full-featured kernel. It has all the basics
    (semaphores, mutexes, round-robin and preemptive scheduling), and some pretty
    advanced features as well (device drivers and other middleware). However, it
    had two major problems - it doesn't scale well, and it doesn't support many
    devices.

    While I had modest success with this kernel (it has been featured on some
    blogs, and still gets around 125 downloads a month), it was nothing like the
    success of other RTOS kernels like uC/OS-II and FreeRTOS. To be honest, as a
    one-man show, I just don't have the resources to support all of the devices,
    toolchains, and evaluation boards that a real vendor can. I had never expected
    my kernel to compete with the likes of them, and I don't expect Mark3 to
    change the embedded landscape either.

    My main goal with Mark3 was to solve the technical shortfalls in the FunkOS
    kernel by applying my experience in kernel development. As a result, Mark3 is
    better than FunkOS in almost every way; it scales better, has lower interrupt
    latency, and is generally more thoughtfully designed (all at a small cost to
    code size).

    Another goal I had was to create something easy to understand, that could be
    documented and serve as a good introduction to RTOS kernel design. The end
    result of these goals is the kernel as presented in this book - a full source
    listing of a working OS kernel, with each module completely documented and
    explained in detail.

    Finally, I wanted to prove that a kernel written entirely in C++ could
    perform just as well as one written in C.  Mark3 is fully benchmarked and
    profiled – you can see exactly how much it costs to call certain APIs or
    include various features in the kernel.

    And in addition, the code is more readable and easier to understand as a
    result of making use of object-oriented concepts provided by C++.
    Applications are easier to write because common concepts are encapsulated
    into objects (Threads, Semaphores, Mutexes, etc.) with their own methods and
    data, as opposed to APIs which rely on lots of explicit pointer or
    handle-passing, type casting, and other operations that are typically
    considered "unsafe" or "advaned" topics in C.
*/
/**
    @page WHENRTOS When to use an RTOS?

    @section WHENREAL The reality of system code

    System code can be defined as the program logic required to manage, synchronize,
    and schedule all of the resources (CPU time, memory, peripherals, etc.) used
    by the application running on the CPU.  And it's true that a significant
    portion of the code running on an embedded system will be system code.  No
    matter how simple a system is, whether or not this logic is embedded directly
    into the application (bare-metal system), or included as part of a well-defined
    stack on which an application is written (RTOS-based); system code is still
    present, and it comes with a cost.

    As an embedded systems is being designed, engineers have to decide which
    approach to take:  Bare-metal, or RTOS.  There are advantages and
    disadvantages to each – and a reasonable engineer should always perform a
    thorough analysis of the pros and cons of each - in the context of the given
    application - before choosing a path.

    The following figure demonstrates the differences between the architecture of
    a bare-metal system and RTOS based system at a high level:

    @image html Mark3_arch.png
    @image latex Mark3_arch.png "Arch" width=10cm

    As can be seen, the RTOS (And associated middleware + libraries) captures a
    certain fixed size.

    As a generalization, bare-metal systems typically have the advantage in that
    the system code overhead is small to start – but grows significantly as the
    application grows in complexity.  At a certain point, it becomes extremely
    difficult and error-prone to add more functionality to an application running
    on such a system.  There's a tipping point, where the cost of the code used
    to work-around the limitations of a bare-metal system outweigh the cost of a
    capable RTOS.  Bare-metal systems also generally take longer to implement,
    because the system code has to be written from scratch (or derived from
    existing code) for the application.  The resulting code also tend to be less
    portable, as it takes serious discipline to keep the system-specific elements
    of the code separated – in an RTOS-based system, once the kernel and drivers
    are ported, the application code is generally platform agnostic.

    Conversely, an RTOS-based system incurs a slightly higher fixed cost up-front,
    but scales infinitely better than a bare-metal system as application's
    complexity increases.  Using an RTOS for simple systems reduces application
    development time, but may cause an application not to fit into some extremely
    size-constrained microcontroller.  An RTOS can also cause the size of an
    application to grow more slowly relative to a bare-metal system – especially
    as a result of applying synchronization mechanisms and judicious IPC.  As a
    result, an RTOS makes it significantly easier to “go agile” with an
    application – iteratively adding features and functionality, without having
    to consider refactoring the underlying system at each turn.

    Some of these factors may be more important than others.  Requirements,
    specifications, schedules, chip-selection, and volume projections for a
    project should all be used to feed into the discussions to decide whether or
    to go bare-metal or RTOS as a result.

    Consider the following questions when making that decision:

    - What is the application?
    - How efficient is efficient enough?
    - How fast is fast enough?
    - How small is small enough?
    - How responsive is responsive enough?
    - How much code space/RAM/etc is available on the target system?
    - How much code space/RAM do I need for an RTOS?
    - How much code space/RAM do I think I'll need for my application?
    - How much time do I have to deliver my system?
    - How many units do we plan to sell?
    .

    @section WHENSUPER Superloops, and their limitations

    @subsection WHENSUPERINTRO Intro to Superloops

    Before we start taking a look at designing a real-time operating system, it's
    worthwhile taking a look through one of the most-common design patterns that
    developers use to manage task execution in bare-metal embedded systems -
    Superloops.

    Systems based on superloops favor the system control logic baked directly into
    the application code, usually under the guise of simplicity, or memory (code
    and RAM) efficiency. For simple systems, superloops can definitely get the
    job done. However, they have some serious limitations, and are not suitable
    for every kind of project. In a lot of cases you can squeak by using
    superloops - especially in extremely constrained systems, but in general they
    are not a solid basis for reusable, portable code.

    Nonetheless, a variety of examples are presented here- from the extremely
    simple, to cooperative and liimted-preemptive multitasking systems, all of
    which are examples are representative of real-world systems that I've either
    written the firmware for, or have seen in my experience.

    @subsection WHENSUPERSIMPLE The simplest loop

    Let's start with the simplest embedded system design possible - an infinite
    loop that performs a single task repeatedly:

    @code{.cpp}
    int main()
    {
        while(1)
        {
            Do_Something();
        }
    }
    @endcode

    Here, the code inside the loop will run a single function forever and ever.
    Not much to it, is there? But you might be surprised at just how much embedded
    system firmware is implemented using essentially the same mechanism - there
    isn't anything wrong with that, but it's just not that interesting.

    Despite its simplicity we can see the beginnings of some core OS concepts.
    Here, the while(1) statement can be logically seen as the he operating system
    kernel - this one control statement determines what tasks can run in the
    system, and defines the constraints that could modify their execution. But
    at the end of the day, that's a big part of what a kernel is - a mechanism
    that controls the execution of application code.

    The second concept here is the task. This is application code provided by the
    user to perform some useful purpose in a system. In this case Do_something()
    represents that task - it could be monitoring blood pressure, reading a
    sensor and writing its data to a terminal, or playing an MP3; anything you
    can think of for an embedded system to do. A simple round-robin multi-tasking
    system can be built off of this example by simply adding additional tasks in
    sequence in the main while-loop. Note that in this example the CPU is always
    busy running tasks - at no time is the CPU idle, meaning that it is likely
    burning a lot of power.

    While we conceptually have two separate pieces of code involved here (an
    operating system kernel and a set of running tasks), they are not logically
    separate. The OS code is indistinguishable from the application. It's like a
    single-celled organism - everything is crammed together within the walls of
    an indivisible unit; and specialized to perform its given function relying
    solely on instinct.

    @subsection WHENSUPERINTER Interrupt-Driven Super-loop

    In the previous example, we had a system without any way to control the
    execution of the task- it just runs forever. There's no way to control when
    the task can (or more importantly can't) run, which greatly limits the
    usefulness of the system. Say you only want your task to run every 100
    miliseconds - in the previous code, you have to add a hard-coded delay at the
    end of your task's execution to ensure your code runs only when it should.

    Fortunately, there is a much more elegant way to do this. In this example,
    we introduce the concept of the synchronization object. A Synchronization
    object is some data structure which works within the bounds of the operating
    system to tell tasks when they can run, and in many cases includes special
    data unique to the synchronization event.

    There are a whole family of synchronization objects, which we'll get into
    later. In this example, we make use of the simplest synchronization primitive
    - the global flag.

    With the addition of synchronization brings the addition of event-driven
    systems. If you're programming a microcontroller system, you generally have
    scores of peripherals available to you - timers, GPIOs, ADCs, UARTs, ethernet,
    USB, etc. All of which can be configured to provide a stimulus to your system
    by means of interrupts. This stimulus gives us the ability not only to program
    our micros to do_something(), but to do_something() if-and-only-if a
    corresponding trigger has occurred.

    The following concepts are shown in the example below:

    @code{.cpp}
    volatile K_BOOL something_to_do = false;

    __interrupt__ My_Interrupt_Source(void)
    {
        something_to_do = true;
    }

    int main()
    {
        while (1)
        {
            if (something_to_do)
            {
                Do_something();
                something_to_do = false;
            }
            else
            {
                Idle();
            }
        }
    }
    @endcode

    So there you have it - an event driven system which uses a global variable
    to synchronize the execution of our task based on the occurrence of an
    interrupt. It's still just a bare-metal, OS-baked-into-the-application system,
    but it's introduced a whole bunch of added complexity (and control!) into
    the system.

    The first thing to notice in the source is that the global variable,
    something_to_do, is used as a synchronization object. When an interrupt occurs
    from some external event, triggering the My_Interrupt_Source() ISR, program
    flow in main() is interrupted, the interrupt handler is run, and
    something_to_do is set to true, letting us know that when we get back to
    main(), that we should run our Do_something() task.

    Another new concept at play here is that of the idle function. In general,
    when running an event driven system, there are times when the CPU has no
    application tasks to run. In order to minimize power consumption, CPUs
    usually contain instructions or registers that can be set up to disable
    non-essential subsets of the system when there's nothing to do. In general,
    the sleeping system can be re-activated quickly as a result of an interrupt
    or other external stimulus, allowing normal processing to resume.

    Now, we could just call Do_something() from the interrupt itself - but that's
    generally not a great solution. In general, the more time we spend inside an
    interrupt, the more time we spend with at least some interrupts disabled. As
    a result, we end up with interrupt latency. Now, in this system, with only
    one interrupt source and only one task this might not be a big deal, but say
    that Do_something() takes several seconds to complete, and in that time
    several other interrupts occur from other sources. While executing in our
    long-running interrupt, no other interrupts can be processed - in many cases,
    if two interrupts of the same type occur before the first is processed, one
    of these interrupt events will be lost. This can be utterly disastrous in a
    real-time system and should be avoided at all costs. As a result, it's
    generally preferable to use synchronization objects whenever possible to
    defer processing outside of the ISR.

    Another OS concept that is implicitly introduced in this example is that of
    task priority. When an interrupt occurs, the normal execution of code in
    main() is preempted: control is swapped over to the ISR (which runs to
    completion), and then control is given back to main() where it left off. The
    very fact that interrupts take precedence over what's running shows that main
    is conceptually a "low-priority" task, and that all ISRs are "high-priority"
    tasks. In this example, our "high-priority" task is setting a variable to
    tell our "low-priority" task that it can do something useful. We will
    investigate the concept of task priority further in the next example.

    Preemption is another key principle in embedded systems. This is the notion
    that whatever the CPU is doing when an interrupt occurs, it should stop, cache
    its current state (referred to as its context), and allow the high-priority
    event to be processed. The context of the previous task is then restored its
    state before the interrupt, and resumes processing. We'll come back to
    preemption frequently, since the concept comes up frequently in RTOS-based systems.

    @subsection WHENSUPERCOOP Cooperative multi-tasking

    Our next example takes the previous example one step further by introducing
    cooperative multi-tasking:

    @code{.cpp}
    // Bitfield values used to represent three distinct tasks
    #define TASK_1_EVENT (0x01)
    #define TASK_2_EVENT (0x02)
    #define TASK_3_EVENT (0x04)

    volatile K_UCHAR event_flags = 0;

    // Interrupt sources used to trigger event execution

    __interrupt__  My_Interrupt_1(void)
    {
        event_flags |= TASK_1_EVENT;
    }

    __interrupt__ My_Interrupt_2(void)
    {
        event_flags |= TASK_2_EVENT;
    }

    __interrupt__ My_Interrupt_3(void)
    {
        event_flags |= TASK_3_EVENT;
    }

    // Main tasks
    int main(void)
    {
        while(1)
        {
            while(event_flags)
            {
                if( event_flags & TASK_1_EVENT)
                {
                    Do_Task_1();
                    event_flags &= ~TASK_1_EVENT;
                } else if( event_flags & TASK_2_EVENT) {
                    Do_Task_2();
                    event_flags &= ~TASK_2_EVENT;
                } else if( event_flags & TASK_3_EVENT) {
                    Do_Task_3();
                    event_flags &= ~TASK_3_EVENT;
                }
            }
            Idle();
        }
    }
    @endcode

    This system is very similar to what we had before - however the differences
    are worth discussing. First, we have stimulus from multiple interrupt sources:
    each ISR is responsible for setting a single bit in our global event flag,
    which is then used to control execution of individual tasks from within main().

    Next, we can see that tasks are explicitly given priorities inside the main
    loop based on the logic of the if/else if structure. As long as there is
    something set in the event flag, we will always try to execute Task1 first,
    and only when Task1 isn't set will we attempt to execute Task2, and then
    Task3. This added logic provides the notion of priority. However, because
    each of these tasks exist within the same context (they're just different
    functions called from our main control loop), we don't have the same notion
    of preemption that we have when dealing with interrupts.

    That means that even through we may be running Task2 and an event flag for
    Task1 is set by an interrupt, the CPU still has to finish processing Task2
    to completion before Task1 can be run. And that's why this kind of scheduling
    is referred to as cooperative multitasking: we can have as many tasks as we
    want, but unless they cooperate by means of returning back to main, the
    system can end up with high-priority tasks getting starved for CPU time by
    lower-priority, long-running tasks.

    This is one of the more popular Os-baked-into-the-application approaches, and
    is widely used in a variety of real-time embedded systems.

    @subsection WHENSUPERHYBRID Hybrid cooperative/preemptive multi-tasking

    The final variation on the superloop design utilizes software-triggered
    interrupts to simulate a hybrid cooperative/preemptive multitasking system.
    Consider the example code below.

    @code{.cpp}
    // Bitfields used to represent high-priority tasks.  Tasks in this group
    // can preempt tasks in the group below - but not eachother.
    #define HP_TASK_1(0x01)
    #define HP_TASK_2(0x02)

    volatile K_UCHAR hp_tasks = 0;

    // Bitfields used to represent low-priority tasks.
    #define LP_TASK_1(0x01)
    #define LP_TASK_2(0x02)

    volatile K_UCHAR lp_tasks = 0;

    // Interrupt sources, used to trigger both high and low priority tasks.
    __interrupt__ System_Interrupt_1(void)
    {
        // Set any of the other tasks from here...
        hp_tasks |= HP_TASK_1;
        // Trigger the SWI that calls the High_Priority_Tasks interrupt handler
        SWI();
    }

    __interrupt__ System_Interrupt_n...(void)
    {
    // Set any of the other tasks from here...
    }


    // Interrupt handler that is used to implement the high-priority event context
    __interrupt__ High_Priority_Tasks(void)
    {
        // Enabled every interrupt except this one
        Disable_My_Interrupt();
        Enable_Interrupts();
        while( hp_tasks)
        {
            if( hp_tasks & HP_TASK_1)
            {
                HP_Task1();
                hp_tasks &= ~HP_TASK_1;
            }
            else if (hp_tasks & HP_TASK_2)
            {
                HP_Task2();
                hp_tasks &= ~HP_TASK_2;
            }
        }
        Restore_Interrupts();
        Enable_My_Interrupt();
    }

    // Main loop, used to implement the low-priority events
    int main(void)
    {
        // Set the function to run when a SWI is triggered
        Set_SWI(High_Priority_Tasks);

        // Run our super-loop
        while(1)
        {
            while (lp_tasks)
            {
                if (lp_tasks & LP_TASK_1)
                {
                    LP_Task1();
                    lp_tasks &= ~LP_TASK_1;
                }
                else if (lp_tasks & LP_TASK_2)
                {
                    LP_Task2();
                    lp_tasks &= ~LP_TASK_2;
                }
            }
            Idle();
        }
    }
    @endcode

    In this example, High_Priority_Tasks() can be triggered at any time as a
    result of a software interrupt (SWI),. When a high-priority event is set, the
    code that sets the event calls the SWI as well, which instantly preempts
    whatever is happening in main, switching to the high-priority interrupt
    handler. If the CPU is executing in an interrupt handler already, the current
    ISR completes, at which point control is given to the high priority interrupt
    handler.

    Once inside the HP ISR, all interrupts (except the software interrupt) are
    re-enabled, which allows this interrupt to be preempted by other interrupt
    sources, which is called interrupt nesting. As a result, we end up with two
    distinct execution contexts (main and HighPriorityTasks()), in which all tasks
    in the high-priority group are guaranteed to preempt main() tasks, and will
    run to completion before returning control back to tasks in main(). This is
    a very basic preemptive multitasking scenario, approximating a "real" RTOS
    system with two threads of different priorities.

    @section WHENPROB Problems with superloops

    As mentioned earlier, a lot of real-world systems are implemented using a
    superloop design; and while they are simple to understand due to the limited
    and obvious control logic involved, they are not without their problems.

    @subsection WHENPROBHIDDEN Hidden Costs

    It's difficult to calculate the overhead of the superloop and the code
    required to implement workarounds for blocking calls, scheduling, and
    preemption. There's a cost in both the logic used to implement workarounds
    (usually involving state machines), as well as a cost to maintainability
    that comes with breaking up into chunks based on execution time instead of
    logical operations. In moderate firmware systems, this size cost can exceed
    the overhead of a reasonably well-featured RTOS, and the deficit in
    maintainability is something that is measurable in terms of lost
    productivity through debugging and profiling.

    @subsection WHENPROBTIGHTLY Tightly-coupled code

    Because the control logic is integrated so closely with the application
    logic, a lot of care must be taken not to compromise the separation between
    application and system code. The timing loops, state machines, and
    architecture-specific control mechanisms used to avoid (or simulate)
    preemption can all contribute to the problem. As a result, a lot of superloop
    code ends up being difficult to port without effectively simulating or
    replicating the underlying system for which the application was written.
    Abstraction layers can mitigate the risks, but a lot of care should be taken
    to fully decouple the application code from the system code.

    @subsection WHENPROBBLOCK No blocking Calls

    In a super-loop environment, there's no such thing as a blocking call or
    blocking objects. Tasks cannot stop mid-execution for event-driven I/O from
    other contexts - they must always run to completion. If busy-waiting and
    polling are used as a substitute, it increases latency and wastes cycles. As
    a result, extra code complexity is often times necessary to work-around this
    lack of blocking objects, often times through implementing additional state
    machines. In a large enough system, the added overhead in code size and
    cycles can add up.

    @subsection WHENPROBDIFF Difficult to guarantee responsiveness

    Without multiple levels of priority, it may be difficult to guarantee a
    certain degree of real-time responsiveness without added profiling and
    tweaking. The latency of a given task in a priority-based cooperative
    multitasking system is the length of the longest task. Care must be taken to
    break tasks up into appropriate sized chunks in order to ensure that higher-
    priority tasks can run in a timely fashion - a manual process that must be
    repeated as new tasks are added in the system. Once again, this adds extra
    complexity that makes code larger, more difficult to understand and maintain
    due to the artificial subdivision of tasks into time-based components.

    @subsection WHENPROBLIM Limited preemption capability

    As shown in the example code, the way to gain preemption in a superloop is
    through the use of nested interrupts. While this isn't unwiedly for two
    levels of priority, adding more levels beyond this is becomes complicated.
    In this case, it becomes necessary to track interrupt nesting manually, and
    separate sets of tasks that can run within given priority loops - and
    deadlock becomes more difficult to avoid.
*/
/**
    @page CANRTOS Can you afford an RTOS?

    @section CANRTOSINTRO Intro

    Of course, since you're reading the manual for an RTOS that I've been
    developing over the course of the several years, you can guess that the
    conclusion that I draw.

    If your code is of any sort of non-trivial complexity (say, at least a few-
    thousand lines), then a more appropriate question would be "can you afford
    *not* to use an RTOS in your system?".

    In short, there are simply too many benefits of an RTOS to ignore, the most
    important being:

    Threading, along with priority and time-based scheduling
    Sophisticated synchronization objects and IPC
    Flexible, powerful Software Timers
    Ability to write more portable, decoupled code

    Sure, these features have a cost in code space and RAM, but from my experience
    the cost of trying to code around a lack of these features will cost you as
    much - if not more.  The results are often far less maintainable, error
    prone, and complex.  And that simply adds time and cost.  Real developers
    ship, and the RTOS is quickly becoming one of the standard tools that help
    keep developers shipping.

    One of the main arguments against using an RTOS in an embedded project is
    that the overhead incurred is too great to be justified. Concerns over
    "wasted" RAM caused by using multiple stacks, added CPU utilization, and the
    "large" code footprint from the kernel cause a large number of developers to
    shun using a preemptive RTOS, instead favoring a non-preemptive,
    application-specific solution.

    I believe that not only is the impact negligible in most cases, but that the
    benefits of writing an application with an RTOS can lead to savings around
    the board (code size, quality, reliability, and development time). While
    these other benefits provide the most compelling case for using an RTOS, they
    are far more challenging to demonstrate in a quantitative way, and are
    clearly documented in numerous industry-based case studies.

    While there is some overhead associated with an RTOS, the typical arguments
    are largely unfounded when an RTOS is correctly implemented in a system. By
    measuring the true overhead of a preemptive RTOS in a typical application,
    we will demonstrate that the impact to code space, RAM, and CPU usage is
    minimal, and indeed acceptable for a wide range of CPU targets.


    To illustrate just how little an RTOS impacts the size of an embedded
    software design we will look at a typical microcontroller project and analyze
    the various types of overhead associated with using a pre-emptive realtime
    kernel versus a similar non-preemptive event-based framework.

    RTOS overhead can be broken into three distinct areas:

    - Code space: The amount of code space eaten up by the kernel (static)
    - Memory overhead: The RAM associated wtih running the kernel and application
                       threads.
    - Runtime overhead: The CPU cycles required for the kernel's functionality
                       (primarily scheduling and thread switching)
    .

    While there are other notable reasons to include or avoid the use of an RTOS
    in certain applications (determinism, responsiveness, and interrupt latency
    among others), these are not considered in this discussion - as they are
    difficult to consider for the scope of our "canned" application.

    @section CANRTOSAPP Application description

    For the purpose of this comparison, we first create an application using the
    standard preemptive Mark3 kernel with 2 system threads running: A foreground
    thread and a background thread. This gives three total priority levels in
    the system - the interrupt level (high), and two application priority
    threads (medium and low), which is quite a common paradigm for microcontroller
    firmware designs. The foreground thread processes a variety of time-critical
    events at a fixed frequency, while the background thread processes lower
    priority, aperiodic events. When there are no background thread events to
    process, the processor enters its low-power mode until the next interrupt is
    acknowledged.

    The contents of the threads themselves are unimportant for this comparison,
    but we can assume they perform a variety of realtime I/O functions. As a
    result, a number of device drivers are also implemented.

    Code Space and Memory Overhead:

    The application is compiled for an ATMega328p processor which contains 32kB
    of code space in flash, and 2kB of RAM, which is a lower-mid-range
    microcontroller in Atmel's 8-bit AVR line of microcontrollers. Using the AVR
    GCC compiler with -Os level optimizations, an executable is produced with the
    following code/RAM utilization:

    @verbatim
    Program:   27914 bytes
    Data:       1313 bytes
    @endverbatim

    An alternate version of this project is created using a custom "super-loop"
    kernel, which uses a single application thread and provides 2 levels of
    priority (interrupt and application). In this case, the event handler
    processes the different priority application events to completion from
    highest to lowest priority.

    This approach leaves the application itself largely unchanged. Using the same
    optimization levels as the preemptive kernel, the code compiles as follows:

    @verbatim
    Program:   24886 bytes
    Data:        750 bytes
    @endverbatim

    At first glance, the difference in RAM utilization seems quite a lot higher
    for the preemptive mode version of the application, but the raw numbers don't
    tell the whole story.

    The first issue is that the cooperative-mode total does not take into account
    the system stack - whereas these values are included in the totals for RTOS
    version of the project. As a result, some further analysis is required to
    determine how the stack sizes truly compare.

    In cooperative mode, there is only one thread of execution - so considering
    that multiple event handlers are executed in turn, the stack requirements for
    cooperative mode is simply determined by those of the most stack-intensive
    event handler (ignoring stack use contributions due to interrupts).

    In contrast, the preemptive kernel requires a separate stack for each active
    thread, and as a result the stack usage of the system is the sum of the stacks
    for all threads.

    Since the application and idle events are the same for both preemptive and
    cooperative mode, we know that their (independent) stack requirements will
    be the same in both cases.

    For cooperative mode, we see that the idle thread stack utilization is lower
    than that of the application thread, and so the application thread's determines
    the stack size requirement. Again, with the preemptive kernel the stack
    utilization is the sum of the stacks defined for both threads.

    As a result, the difference in overhead between the two cases becomes the
    extra stack required for the idle thread - which in our case is (a somewhat
    generous) 128 bytes.

    The numbers still don't add up completely, but looking into the linker output
    we see that the rest of the difference comes from the extra data structures
    used to manage the kernel in preemptive mode, and the kernel data itself.

    Fixed kernel data costs:
    @verbatim
    --- 134 Bytes Kernel data
    --- 26 Bytes Kernel Vtables
    @endverbatim

    Application (Variable) data costs:
    @verbatim
    --- 24 Bytes Driver Vtables
    --- 123 Bytes – statically-allocated kernel objects (semaphores, timers, etc.)
    @endverbatim

    With this taken into account, the true memory cost of a 2-thread system ends
    up being around 428 bytes of RAM - which is about 20% of the total memory
    available on this particular microcontroller. Whether or not this is
    reasonable certainly depends on the application, but more importantly, it is
    not so unreasonable as to eliminate an RTOS-based solution from being
    considered. Also note that by using the “simulated idle” feature provided in
    Mark3 R3 and onward, the idle thread (and its associated stack) can be
    eliminated altogether to reduce the cost in constrained devices.

    The difference in code space overhead between the preemptive and cooperative
    mode solutions is less of an issue. Part of this reason is that both the
    preemptive and cooperative kernels are relatively small, and even an average
    target device (like the Atmega328 we've chosen) has plenty of room.

    Mark3 can be configured so that only features necessary for the application
    are included in the RTOS - you only pay for the parts of the system that you
    use. In this way, we can measure the overhead on a feature-by-feature basis,
    which is shown below for the kernel as configured for this application:

    @verbatim
    Kernel ................. 2563 Bytes
    Synchronization Objects.  644 Bytes
    Port ...................  974 Bytes
    Features ...............  871 Bytes
    @endverbatim

    The configuration tested in this comparison uses the thread/port module with
    timers, drivers, and semaphores, and mutexes, for a total kernel size of
    5052 Bytes, with the rest of the code space occupied by the application.

    As can be seen from the compiler's output, the difference in code space
    between the two versions of the application is 3028 bytes - or about 9% of
    the available code space on the selected processor.  While nearly all of
    this comes from the added overhead of the kernel, the rest of the difference
    comes the changes to the application necessary to facilitate the different
    frameworks.  This also demonstrates that the system-software code size in the
    cooperative case is about 2024 bytes.

    @section CANRTOSOVERHEAD Runtime Overhead

    On the cooperative kernel, the overhead associated with running the thread
    is the time it takes the kernel to notice a pending event flag and launch the
    appropriate event handler, plus the timer interrupt execution time.

    Similarly, on the preemptive kernel, the overhead is the time it takes to
    switch contexts to the application thread, plus the timer interrupt execution
    time.

    The timer interrupt overhead is similar for both cases, so the overhead then
    becomes the difference between the following:

    Preemptive mode:
    - Posting the semaphore that wakes the high-priority thread
    - Performing a context switch to the high-priority thread
    .

    Cooperative mode:
    - Setting the event flag from the timer interrupt
    - Acknowledging the event from the event loop
    .

    coop – 438 cycles
    preempt – 764 cycles

    Using a cycle-accurate AVR simulator (flAVR) running with a simulated speed
    of 16MHz, we find the end-to-end event sequence time to be 27us for the
    cooperative mode scheduler and 48us for the preemptive, and a raw difference
    of 20us.

    With a fixed high-priority event frequency of 30Hz, we achieve a runtime
    overhead of 611us per second, or 0.06% of the total available CPU time. Now,
    obviously this value would expand at higher event frequencies and/or slower
    CPU frequencies, but for this typical application we find the difference in
    runtime overhead to be neglible for a preemptive system.

    @section CANRTOSANALYSIS Analysis

    For the selected test application and platform, including a preemptive RTOS
    is entirely reasonable, as the costs are low relative to a non-preemptive
    kernel solution. But these costs scale relative to the speed, memory and
    code space of the target processor. Because of these variables, there is no
    "magic bullet" environment suitable for every application, but Mark3 attempts
    to provide a framework suitable for a wide range of targets.

    On the one hand, if these tests had been performed on a higher-end
    microcontroller such as the ATMega1284p (containing 128kB of code space and
    16kB of RAM), the overhead would be in the noise. For this type of
    resource-rich microcontroller, there would be no reason to avoid using
    the Mark3 preemptive kernel.

    Conversely, using a lower-end microcontroller like an ATMega88pa (which has
    only 8kB of code space and 1kB of RAM), the added overhead would likely be
    prohibitive for including a preemptive kernel. In this case, the
    cooperative-mode kernel would be a better choice.

    As a rule of thumb, if one budgets 25% of a microcontroller's code space/RAM
    for system code, you should only require at minimum a microcontroller with
    16k of code space and 2kB of RAM as a base platform for an RTOS. Unless there
    are serious constraints on the system that require much better latency or
    responsiveness than can be achieved with RTOS overhead, almost any modern
    platform is sufficient for hosting a kernel. In the event you find yourself
    with a microprocessor with external memory, there should be no reason to
    avoid using an RTOS at all.
*/
/**
    @page INSIDEMARK3 Mark3 Design Goals

    @section INSIDEOVERVIEW Overview

    @subsection INSIDEOVERSERVICES Services Provided by an RTOS Kernel

    At its lowest-levels, an operating system kernel is responsible for managing
    and scheduling resources within a system according to the application.  In a
    typical thread-based RTOS, the resources involved is CPU time, and the kernel
    manages this by scheduling threads and timers.  But capable RTOS kernels
    provide much more than just threading and timers.

    In the following section, we discuss the Mark3 kernel architecture, all of
    its features, and a thorough discussion of how the pieces all work together
    to make an awesome RTOS kernel.

    @subsection INSIDEOVERPRINCIPLES Guiding Principles of Mark3

    Mark3 was designed with a number of over-arching principles, coming from
    years of experience designing, implementing, refining, and experimenting with
    RTOS kernels.  Through that process I not only discovered what features I
    wanted in an RTOS, but how I wanted to build those features to look, work,
    and “feel”.   With that understanding, I started with a clean slate and
    began designing a new RTOS.  Mark3 is the result of that process, and its
    design goals can be summarized in the following guiding principles.

    @subsection INSIDEOVERBFC Be feature competitive

    To truly be taken seriously as more than just a toy or educational tool, an
    RTOS needs to have a certain feature suite.  While Mark3 isn't a clone of any
    existing RTOS, it should at least attempt parity with the most common
    software in its class.

    Looking at its competitors, Mark3 as a kernel supports most, if not all of
    the compelling features found in modern RTOS kernels, including dynamic
    threads, dynamic timers, efficient message passing, and multiple types of
    synchronization primatives.

    @subsection INSIDEOVERNED No external dependencies, no non-language features

    To maximize portability and promote adoption to new platforms, Mark3 is
    written in a widely supported subset of C++ that lends itself to embedded
    applications.  It avoids RTTI, exceptions, and libraries (C standard, STL,
    boost, etc.), with all fundamental data structures and types implemented
    completely for use by the kernel.  As a result, the portable parts of Mark3
    should compile for any capable C++ toolchain.

    @subsection INSIDEOVERHPA Target the most popular hobbyist platforms available

    Realistically, this means supporting the various Arduino-compatible target
    CPUs, including AVR and ARM Cortex-M series microcontrollers.  As a result,
    the current default target for Mark3 is the atmega328p, which has 32KB of
    flash and 2KB of RAM.  All decisions regarding default features, code size,
    and performance need to take that target system into account.

    Mark3 integrates cleanly as a library into the Arduino IDE to support
    atmega328-based targets.  Other AVR and Cortex-M targets can be supported
    using the port code provided in the source package.

    @subsection INSIDEOVERMAXDET Maximize determinism – but be pragmatic

    Guaranteeing deterministic and predictable behavior is tough to do in an
    embedded system, and often comes with a heavy price tag in either RAM or
    code-space.  With Mark3, we strive to keep the core kernel APIs and features
    as lightweight as possible, while avoiding algorithms that don't scale to
    large numbers of threads.  We also achieve minimal latency by keeping
    interrupts enabled (operating out of the critical section) wherever possible.

    In Mark3, the most important parts of the kernel are fixed-time, including
    thread scheduling and context switching.  Operations that are not fixed time
    can be characterized as a function of their dependent data data.  For
    instances, the Mutex and Semaphore APIs operate in fixed time in the
    uncontested case, and execute in linear time for the contested case – where
    the speed of execution is dependent on the number of threads currently
    waiting on that object.

    The caveat here is that while we want to minimize latency and time spent in
    critical sections, that has to be balanced against increases in code size,
    and uncontested-case performance.

    @subsection INSIDEOVERENG Apply engineering principles – and that means discipline, measurement and verification

    My previous RTOS, FunkOS, was designed to be very ad-hoc.  The usage
    instructions were along the lines of “drag and drop the source files into
    your IDE and compile”.  There was no regression/unit testing, no code
    size/speed profiling, and all documentation was done manually.  It worked,
    but the process was a bit of a mess, and resulted in a lot of re-spins of
    the software, and a lot of time spent stepping through emulators to measure
    parameters.

    We take a different approach in Mark3.  Here, we've designed not only the
    kernel-code, but the build system, unit tests, profiling code, documentation
    and reporting that supports the kernel.  Each release is built and tested
    using automation in order to ensure quality and correctness, with supporting
    documentation containing all critical metrics.  Only code that passes
    testing is submitted to the repos and public forums for distribution.  These
    metrics can be traced from build-to-build to ensure that performance remains
    consistent from one drop to the next, and that no regressions are introduced
    by new/refactored code.

    And while the kernel code can still be exported into an IDE directly, that
    takes place with the knowledge that the kernel code has already been
    rigorously tested and profiled.  Exporting source in Mark3 is also supported
    by scripting to ensure reliable, reproducible results without the possibility
    for human-error.

    @subsection INSIDEOVERFLAVR Use Virtualization For Verification

    Mark3 was designed to work with automated simulation tools as the primary
    means to validate changes to the kernel, due to the power and flexibility
    of automatic tests on virtual hardware.  I was also intrigued by the thought
    of extending the virtual target to support functionality useful for a kernel,
    but not found on real hardware.

    When the project was started, simavr was the tool of choice- however, its
    simulation was found to be incorrect compared to execution on a real MCU, and
    it did not provide the degree of extension that I desired for use with kernel
    development.

    The flAVR AVR simulator was written to replace the dependency on that tool,
    and overcome those limitations.  It also provides a GDB interface, as well as
    its own built-in debugger, profilers, and trace tools.

    flAVR is hosted on sourceforge at http://www.sourceforge.net/projects/flavr/ .
    In its basic configuration, it builds with minimal external dependencies.

    - On linux, it requires only pthreads.
    - On Windows, it rquires pthreads and ws2_32, both satisfied via MinGW.
    - Optional SDL builds for both targets (featuring graphics and simulated joystick
      input) can be built, and rely on libSDL.
    .

*/
/**
    @page MARK3KA Mark3 Kernel Architecture

    @section MARK3KAOVER Overview

    At a high level, the Mark3 RTOS is organized into the following features, and
    layered as shown below:

    @image html Mark3_arch4.png
    @image latex Mark3_arch4.png "Overview" width=10cm

    Everything in the “green” layer represents the Mark3 public API and classes,
    beneath which lives all hardware abstraction and CPU-specific porting and
    driver code, which runs on a given target CPU.

    The features and concepts introduced in this diagram can be described as follows:

    <b>Threads:</b>
    The ability to multiplex the CPU between multiple tasks to give the perception
    that multiple programs are running simultaneously.  Each thread runs in its
    own context with its own stack.

    <b>Scheduler:</b>
    Algorithm which determines the thread that gets to run on the CPU at any
    given time.  This algorithm takes into account the priorites (and other
    execution parameters) associated with the threads in the system.

    <b>IPC:</b>
    Inter-process-communications.  Message-passing and Mailbox interfaces used
    to communicate between threads synchronously or asynchronously.

    <b>Synchronization Objects:</b>
    Ability to schedule thread execution relative to system conditions and events,
    allowing for sharing global data and resources safely and effectively.

    <b>Timers:</b>
    High-resolution software timers that allow for actions to be triggered on a
    periodic or one-shot basis.

    <b>Profiler:</b>
    Special timer used to measure the performance of arbitrary blocks of code.

    <b>Debugging:</b>
    Realitme logging and trace functionality, facilitating simplified debugging
    of systems using the OS.

    <b>Atomics:</b>
    Support for UN-interruptble arithmatic operations.

    <b>Driver API:</b>
    Hardware abstraction interface allowing for device drivers to be written in
    a consistent, portable manner.

    <b>Hardware Abstraction Layer:</b>
    Class interface definitions to represent threading, context-switching, and
    timers in a generic, abstracted manner.

    <b>Porting Layer:</b>
    Class interface implementation to support threading, context-switching, and
    timers for a given CPU.

    <b>User Drivers:</b>
    Code written by the user to implement device-specific peripheral drivers,
    built to make use of the Mark3 driver API.

    Each of these features will be described in more detail in the following
    sections of this chapter.

    The concepts introduced in the above architecture are implemented in a
    variety of source modules, which are logically broken down into classes (or
    in some cases, groups of functions/macros).  The relationship between objects
    in the Mark3 kernel is shown below:

    @image html Mark3_arch5.png
    @image latex Mark3_arch5.png "Overview" width=10cm

    The objects shown in the preceding table can be grouped together by feature.
    In the table below, we group each feature by object, referencing the source
    module in which they can be found in the Mark3 source tree.

    <b>Atomic Operations</b><br>
    Atomic  - atomic.cpp/.h<br>

    <b>Memory Allocators</b><br>
    AutoAlloc - autoalloc.cpp/.h<br>

    <b>CoRoutines</b><br>
    CoScheduler - cosched.cpp/.h<br>
    CoList - colist.cpp/.h<br>
    Coroutine - coroutine.cpp/.h<br>

    <b>Data Structures</b><br>
    LinkList - ll.cpp/.h<br>
    PriorityMapL1 - priomapl1.h<br>
    PriorityMapL2 - priomapl2.h<br>

    <b>Threads + Scheduling</b><br>
    Thread - thread.cpp/.h<br>
    Scheduler - scheduler.cpp/.h<br>
    Quantum - quantum.cpp/.h<br>
    ThreadPort - threadport.cpp/.h **<br>
    KernelSWI - kernelswi.cpp/.h **<br>
    ThreadList - threadlist.cpp/.h<br>
    ThreadListList - threadlistlist.cpp/.h<br>

    <b>Profiling</b><br>
    ProfileTimer - profile.cpp/.h<br>

    <b>Timers</b><br>
    Timer - timer.h/timer.cpp<br>
    TimerScheduler - timerscheduler.h<br>
    TimerList - timerlist.h/cpp<br>
    KernelTimer - kerneltimer.cpp/.h **<br>

    <b>Synchronization</b><br>
    BlockingObject - blocking.cpp/.h<br>
    Semaphore - ksemaphore.cpp/.h<br>
    EventFlag - eventflag.cpp/.h<br>
    Mutex - mutex.cpp/.h<br>
    Notify - notify.cpp/.h<br>
    ConditionVariable - condvar.cpp/.h<br>
    ReaderWriterLock - readerwriter.cpp/.h<br>
    CriticalGuard - criticalguard.h<br>
    CriticalSection - criticalsection.h<br>
    LockGuard - lockguard.h<br>
    SchedGuard - schedguard.h<br>

    <b>IPC/Message-passing</b><br>
    Mailbox - mailbox.cpp/.h<br>
    MessageQueue - message.cpp/.h<br>

    <b>Debugging</b><br>
    Miscellaneous Macros - kerneldebug.h<br>

    <b>Kernel</b><br>
    Kernel - kernel.cpp/.h<br>

    @verbatim
    ** implementation is platform-dependent, and located under the kernel's
    ** /cpu/<arch>/<variant>/<toolchain> folder in the source tree
    @endverbatim

    @section THREADSCHED Threads and Scheduling

    The classes involved in threading and scheudling in Mark3 are highlighted
    in the following diagram, and are discussed in detail in this chapter:

    @image html Mark3_arch24.png
    @image latex Mark3_arch24.png "Threads and Scheduling" width=10cm

    @subsection BITTHREAD A Bit About Threads

    Before we get started talking about the internals of the Mark3 scheduler,
    it's necessary to go over some background material - starting with:  what is
    a thread, anyway?

    Let's look at a very basic CPU without any sort of special multi-threading
    hardware, and without interrupts.  When the CPU is powered up, the program
    counter is loaded with some default location, at which point the processor
    core will start executing instructions sequentially - running forever and
    ever according to whatever has been loaded into program memory.  This single
    instance of a simple program sequence is the only thing that runs on the
    processor, and the execution of the program can be predicted entirely by
    looking at the CPU's current register state, its program, and any affected
    system memory (the CPU's "context").

    It's simple enough, and that's exactly the definition we have for a thread
    in an RTOS.

    Each thread contains an instance of a CPU's register context, its own stack,
    and any other bookkeeping information necessary to define the minimum unique
    execution state of a system at runtime.  It is the job of a RTOS to multiplex
    the execution of multiple threads on a single physical CPU, thereby creating
    the illusion that many programs are being executed simultaneously.  In
    reality there can only ever be one thread truly executing at any given moment
    on a CPU core, so it's up to the scheduler to set and enforce rules about
    what thread gets to run when, for how long, and under what conditions.  As
    mentioned earlier, any system without an RTOS exeuctes as a single thread,
    so at least two threads are required for an RTOS to serve any useful purpose.

    Note that all of this information is is common to pretty well every RTOS in
    existence - the implementation details, including the scheduler rules, are
    all part of what differentiates one RTOS from another.

    @subsection THREADLISTS Thread States and ThreadLists

    Since only one thread can run on a CPU at a time, the scheduler relies on
    thread information to make its decisions.  Mark3's scheduler relies on a
    variety of such information, including:

    - The thread's current priority
    - Round-Robin execution quanta
    - Whether or not the thread is blocked on a synchronization object, such as a mutex or semaphore
    - Whether or not the thread is currently suspended
    .

    The scheduler further uses this information to logically place each thread
    into 1 of 4 possible states:

        - Ready - The thread is currently running
        - Running - The thread is able to run
        - Blocked - The thread cannot run until a system condition is met
        - Stopped - The thread cannot run because its execution has been suspended
        .

    In order to determine a thread's state, threads are placed in "buckets"
    corresponding to these states.  Ready and running threads exist in the
    scheduler's buckets, blocked threads exist in a bucket belonging to the
    object they're blocked on, and stopped threads exist in a separate bucket
    containing all stopped threads.

    In reality, the various buckets are just doubly-linked lists of Thread
    objects - implemented in something called the ThreadList class.  To
    facilitate this, the Thread class directly inherits from a LinkListNode class,
    which contains the node pointers required to implement a doubly-linked list.
    As a result, Threads may be effortlessly moved from one state to another
    using efficient linked-list operations built into the ThreadList class.

    @subsection BLOCKUNBLOCK Blocking and Unblocking

    While many developers new to the concept of an RTOS assume that all threads
    in a system are entirely separate from eachother, the reality is that
    practical systems typically involve multiple threads working together, or
    at the very least sharing resources.  In order to synchronize the execution
    of threads for that purpose, a number of synchronization primatives (blocking
    objects) are implemented to create specific sets of conditions under which
    threads can continue execution.  The concept of "blocking" a thread until a
    specific condition is met is fundamental to understanding RTOS applications
    design, as well as any highly-multithreaded applications.

    @subsection BLOCKOBJ Blocking Objects

    Blocking objects and primatives provided by Mark3 include:

    - Semaphores (binary and counting)
    - Mutexes
    - Event Flags
    - Thread Notification Objects
    - Thread Sleep
    - Message Queues
    - Mailboxes
    .

    The relationship between these objects in the system are shown below:

    @image html Mark3_arch21.png
    @image latex Mark3_arch21.png "Blocking Objects" width=10cm

    Each of these objects inherit from the BlockingObject class, which itself
    contains a ThreadList object.  This class contains methods to Block() a
    thread (remove it from the Scheduler's "Ready" or "Running" ThreadLists),
    as well as UnBlock() a thread (move a thread back to the "Ready" lists). This
    object handles transitioning threads from list-to-list (and state-to-state),
    as well as taking care of any other Scheduler bookkeeping required in the
    process.  While each of the Blocking types implement a different condition,
    they are effectively variations on the same theme.  Many simple Blocking
    objects are also used to build complex blocking objects - for instance, the
    Thread Sleep mechanism is essentially a binary semaphore and a timer object,
    while a message queue is a linked-list of message objects combined with a
    semaphore.

    @section INSIDESCHED Inside the Mark3 Scheduler

    At this point we've covered the following concepts:

    - Threads
    - Thread States and Thread Lists
    - Blocking and Un-Blocking Threads
    .

    Thankfully, this is all the background required to understand how the Mark3
    Scheduler works.  In technical terms, Mark3 implements "strict priority
    scheduling, with round-robin scheduling among threads in each priority group".
    In plain English, this boils down to a scheduler which follows a few simple
    rules:

    @verbatim
        Find the highest-priority "Ready" list that has at least one Thread.
        Select the next thread to run as the first thread in that list
    @endverbatim

    Since context switching is one of the most common and frequent operation
    performed by an RTOS, this needs to be as fast and deterministic as possible.
    While the logic is simple, a lot of care must be put into optimizing the
    scheduler to achieve those goals.  In the section below we discuss the
    optimization approaches taken in Mark3.

    There are a number of ways to find the highest-priority thread.  The naive
    approach would be to simply iterate through the scheduler's array of
    ThreadLists from highest to lowest, stopping when the first non-empty list
    is found, such as in the following block of code:

    @code{.cpp}
    for (prio = num_prio - 1; prio >= 0; prio--)
    {
        if (thread_list[prio].get_head() != nullptr)
        {
            break;
        }
    }
    @endcode

    While that would certainly work and be sufficient for a variety of systems,
    it's a non-deterministic approach (complexity O(n)) whose cost varies
    substantially based on how many priorities have to be evaluated.  It's simple
    to read and understand, but it's non-optimal.

    Fortunatley, a functionally-equivalent and more deterministic approach can
    be implemented with a few tricks.

    In addition to maintaining an array of ThreadLists, Mark3 also maintains a
    bitmap (one bit per priority level) that indicates which thread lists have
    ready threads.  This bitmap is maintained automatically by the ThreadList
    class, and is updated every time a thread is moved to/from the Scheduler's
    ready lists.

    By inspecting this bitmap using a technique to count the leading zero bits
    in the bitmap, we determine which threadlist to choose in fixed time.

    Now, to implement the leading-zeros check, this can once again be performed
    iteratively using bitshifts and compares (which isn't any more efficient
    than the raw list traversal), but it can also be evaluated using either a
    lookup table, or via a special CPU instruction to count the leading zeros in
    a value.  In Mark3, we use all approaches.  In the event a target architecture
    or toolchain has intrinsic support for a count-leading-zeroes (PORT_CLZ) instruction,
    that implementation is used.  Otherwise, a software-based implementation is
    provided -- either using a lookup table, or a bitshift-and-compare algorithm.

    (As a sidenote - this is actually a very common approach used in OS schedulers.
    It's actually part of the reason why modern ARM cores implement a dedicated
    count-leading-zeros [PORT_CLZ] instruction!)

    For the lookup-table approach - a 4-bit lookup table can be used with an 8-bit
    priority-level bitmap would look something like this:

    @code{.cpp}
    // Check the highest 4 priority levels, represented in the
    // upper 4 bits in the bitmap
    priority = priority_lookup_table[(priority_bitmap >> 4)];

    // priority is non-zero if we found something there
    if( priority )
    {
        // Add 4 because we were looking at the higher levels
        priority += 4;
    }
    else
    {
        // Nothing in the upper 4, look at the lowest 4 priority levels
        // represented by the lowest 4 bits in the bitmap
        priority = priority_lookup_table[priority_bitmap & 0x0F];
    }
    @endcode

    Deconstructing this algorithm, you can see that the priority lookup will have
    on O(1) complexity - and is extremely low-cost.

    This operation is thus fully deterministic and time bound - no matter how
    many threads are scheduled, the operation will always be time-bound to the
    most expensive of these two code paths.  Even with only 8 priority levels,
    this is still much faster than iteratively checking the thread lists manually,
    compared with the previous example implementation.

    Once the priority level has been found, selecting the next thread to run is
    trivial, consisting of something like this:

    next_thread = thread_list[prio].get_head();

    In the case of the get_head() calls, this evaluates to an inline-load of the
    "head" pointer in the particular thread list.

    One important thing to take away from this analysis is that the scheduler is
    only responsible for selecting the next-to-run thread.  In fact, these two
    operations are totally decoupled - no context switching is performed by the
    scheduler, and the scheduler isn't called from the context switch.  The
    scheduler simply produces new "next thread" values that are consumed from
    within the context switch code.

    @subsection RRSCHED Considerations for Round-Robin Scheduling

    One thing that isn't considered directly from the scheduler algorithm is the
    problem of dealing with multiple threads within a single priority group; all
    of the alorithms that have been explored above simply look at the first
    Thread in each group.

    Mark3 addresses this issue indirectly, using an optimized software timer to
    manage round-robin scheduling, as follows.

    In some instances where the scheduler is run by the kernel directly
    (typically as a result of calling Thread::Yield()), the kernel will perfom
    an additional check after running the Scheduler to determine whether or
    there are multiple ready Threads in the priority of the next ready thread.

    If there are multiple threads within that priority, the kernel starts a
    one-shot software timer which is programmed to expire at the next Thread's
    configured quantum.  When this timer expires, a timer callback function
    executes to perform two simple operations:

    "Pivot" the current Thread's priority list.
    Set a flag telling the kernel to trigger a Yield after exiting the main
    TimerScheduler processing loop

    Pivoting the thread list basically moves the head of a circular-linked-list
    to its next value, which in our case ensures that a new thread will be
    chosen the next time the scheduler is run (the scheduler only looks at the
    head node of the priority lists).  And by calling Yield, the system forces
    the scheduler to run, a new round-robin software timer to be installed (if
    necssary), and triggers a context switch SWI to load the newly-chosen thread.
    Note that if the thread attached to the round-robin timer is pre-empted, the
    kernel will take steps to abort and invalidate that round-robin software
    timer, installing a new one tied to the next thread to run if necessary.

    Because the round-robin software timer is dynamically installed when there
    are multiple ready threads at the highest ready priority level, there is no
    CPU overhead with this feature unless that condition is met.  The cost of
    round-robin scheduling is also fixed - no matter how many threads there are,
    and the cost is identical to any other one-shot software timer in the system.

    @subsection CONSWITCH Context Switching

    There's really not much to say about the actual context switch operation at
    a high level.  Context switches are triggered whenever it has been determined
    that a new thread needs to be swapped into the CPU core when the scheduler
    is run.  Mark3 implements also context switches as a call to a software
    interrupt - on AVR platforms, we typically use INT0 or INT2 for this (although
    any pin-change GPIO interrupt can be used), and on ARM we achieve this by
    triggering a PendSV exception.

    However, regardless of the architecture, the contex-switch ISR will perform
    the following three operations:

    Save the current Thread's context to the current Thread stack
    Make the "next to run" thread the "currently running" thread
    Restore the context of the next Thread from the Thread stack

    The code to implement the context switch is entirely architecture-specific,
    so it won't be discussed in detail here.  It's almost always gory
    inline-assembly which is used to load and store various CPU registers, and
    is highly-optimized for speed.  We dive into an example implementation for
    the ARM Cortex-M0 microcontroller in a later section of this book.

    @subsection ALLTOGETHER Putting It All Together

    In short, we can say that the Mark3 scheduler works as follows:

    - The scheduler is run whenever a Thread::Yield() is called by a user, as part
      of blocking calls, or whenever a new thread is started
    - The Mark3 scheduler is deterministic, selecting the next thread to run in
      fixed-time
    - The scheduler only chooses the next thread to run, the context switch SWI
      consumes that information to get that thread running
    - Where there are multiple ready threads in the highest populated priority
      level, a software timer is used to manage round-robin scheduling
    .

    While we've covered a lot of ground in this section, there's not a whole lot
    of code involved.  However, the code that performs these operations is
    nuanced and subtle.  If you're interested in seeing how this all works in
    practice, I suggest reading through the Mark3 source code (which is heavily
    annotated), and stepping through the code with a simulator/emulator.

    @section ARCHTIMERS Timers

    Mark3 implements one-shot and periodic software-timers via the Timer class.
    The user configures the timer for duration, repetition, and action, at which
    point the timer can be activated.  When an active timer expires, the kernel calls
    a user-specified callback function, and then reloads the timer in the case of
    periodic timers.  The same timer objects exposed to the user are also used
    within the kernel to implement round-robin scheduling, and timeout-based APIs
    for seamphores, mutexes, events, and messages.

    Timers are implemented using the following components in the Mark3 Kernel:

    @image html Mark3_arch20.png
    @image latex Mark3_arch20.png "Timers" width=10cm

    The Timer class provides the basic periodic and one-shot timer functionality
    used by applicaiton code, blocking objects, and IPC.

    The TimerList class implements a doubly-linked list of Timer objects, and
    the logic required to implement a timer tick (tick-based kernel) or timer
    expiry (tickless kernel) event.

    The TimerScheduler class contains a single TimerList object, implementing a
    single, system-wide list of Timer objects within the kernel.  It also
    provides hooks for the hardware timer, such that when a timer tick or expiry
    event occurs, the TimerList expiry handler is run.

    The KernelTimer class (kerneltimer.cpp/.h) implements the CPU specific hardware
    timer driver that is used by the kernel and the TimerScheduler to implement
    software timers.

    While extremely simple to use, they provide one of the most powerful
    execution contexts in the system.

    The software timers implemented in Mark3 use interrupt-nesting within the
    kernel timer's interrupt handler.  This context is be considered
    higher-priority than the highest priority user thread, but lower-priority
    than other interrupts in the system.  As a result, this minimizes critical
    interrupt latency in the system, albeit at the expense of responsiveness of
    the user-threads.

    For this reason, it's critical to ensure that all timer callback events are
    kept as short as possible to prevent adding thread-level latency.  All
    heavy-lifting should be left to the threads, so the callback should only
    implement signalling via IPC or synchronization object.

    The time spent in this interrupt context is also dependent on the number of
    active timers at any given time.  However, Mark3 also can be used to minimize
    the frequency of these interrupts wakeups, by using an optional “tolerance”
    parameter in the timer API calls.  In this way, periodic tasks that have
    less rigorous real-time constraints can all be grouped together – executing
    as a group instead of one-after-another.

    Mark3 also contains two different timer implementations that can be
    configured at build-time, each with their own advantages.

    @subsection TICKBASEDTIMERS Tick-based Timers

    In a tick-based timing scheme, the kernel relies on a system-timer interrupt
    to fire at a relatively-high frequency, on which all kernel timer events are
    derived.  On modern CPUs and microcontrollers, a 1kHz system tick is common,
    although quite often lower frequencies such as 60Hz, 100Hz, or 120Hz are used.
    The resolution of this timer also defines the maximum resolution of timer
    objects as a result.  That is, if the timer frequency is 1kHz, a user cannot
    specify a timer resolution lowerthan 1ms.

    The advantage of a tick-based timer is its sheer simplicity.  It typically
    doesn't take much to set up a timer to trigger an interrupt at a
    fixed-interval, at which point, all system timer intervals are decremented
    by 1 count.  When each system timer interval reaches zero, a callback is
    called for the event, and the events are either reset and restarted (repeated
    timers) or cleared (1-shot).

    Unfortunately, that simplicity comes at a cost of increased interrupt count,
    which cause frequent CPU wakeups and utilization, and power consumption.

    @subsection TICKLESSTIMERS Tickless Timers

    Note:  Tickless timers are removed as of the R7 release.  The below documentation
    is preserved for historical information only.  The reason for removing
    tickless timers include the overhead associated with managing those timers
    (significantly more math and management is involved).  In practice, there
    are few scenarios where purely tickless timers add benefit - beyond the most
    constrained devices.  Also, it is entirely possible to disable software timers
    from the idle task when lower power/fewer interrupts are desired in most cases
    where a tickless timer would be of value.

    ---

    In a tickless system, the kernel timer only runs when there are active
    timers pending expiry, and even then, the timer module only generates
    interrupts when a timer expires, or a timer reaches its maximum count value.
    Additionally, when there are no active timer objects, the timer can is
    completely disabled – saving even more cycles, power, and CPU wakeups.
    These factors make the tickless timer approach a highly-optimal solution,
    suitable for a wide array of low-power applications.

    Also, since tickless timers do not rely on a fixed, periodic clock, they can
    potentially be higher resolution.  The only limitation in timer resolution
    is the precision of the underlying hardware timer as configured.  For
    example, if a 32kHz hardware timer is being used to drive the timer scheduler,
    the resolution of timer objects would be in the ~33us range.

    The only downside of the tickless timer system is an added complexity to the
    timer code, requiring more code space, and slightly longer execution of the
    timer routines when the timer interrupt is executed.

    @subsection TIMERPA Timer Processing Algorithm

    Timer interrupts occur at either a fixed-frequency (tick-based), or at the
    next timer expiry interval (tickless), at which point the timer processing
    algorithm runs.  While the timer count is reset by the timer-interrupt, it is
    still allowed to accumulate ticks while this algorithm is executed in order
    to ensure that timer-accuracy is kept in real-time.  It is also important to
    note that round-robin scheduling changes are disabled during the execution
    of this algorithm to prevent race conditions, as the round-robin code also
    relies on timer objects.

    All active timer objects are stored in a doubly-linked list within the
    timer-scheduler, and this list is processed in two passes by the alogirthm
    which runs from the timer-interrupt (with interrupt nesting enabled).  The
    first pass determines which timers have expired and the next timer interval,
    while the second pass deals with executing the timer callbacks themselves.
    Both phases are discussed in more detail below.

    In the first pass, the active timers are decremented by either 1 tick
    (tick-based), or by the duration of the last elapsed timer interval (tickless).
    Timers that have zero (or less-than-zero) time remaining have a “callback”
    flag set, telling the algorithm to call the timer's callback function in the
    second pass of the loop.  In the event of a periodic timer, the timer's
    interval is reset to its starting value.

    For the tickless case, the next timer interval is also computed in the
    first-pass by looking for the active timer with the least amount of time
    remaining in its interval.  Note that this calculation is irrelevant in the
    tick-based timer code, as the timer interrupt fires at a fixed-frequency.

    In the second pass, the algorithms loops through the list of active timers,
    looking for those with their  “callback” flag set in the first pass.  The
    callback function is then executed for each expired timer, and the “callback”
    flag cleared.  In the event that a non-periodic (one-shot) timer expires, the
    timer is also removed from the timer scheduler at this time.

    In a tickless system, once the second pass of the loop has been completed,
    the hardware timer is checked to see if the next timer interval has expired
    while processing the expired timer callbacks.  In that event, the complete
    algorithm is re-run to ensure that no expired timers are missed.  Once the
    algorithm has completed without the next timer expiring during processing,
    the expiry time is programmed into the hardware timer.  Round-robin scheduling
    is re-enabled, and if a new thread has been scheduled as a result of action
    taken during a timer callback, a context switch takes place on return from
    the timer interrupt.

    @section SYNCHIPC Synchronization and IPC

    @image html Mark3_arch21.png
    @image latex Mark3_arch21.png "Synchronization and IPC" width=10cm

    @section BLOCKINGOBJECTS Blocking Objects

    A Blocking object in Mark3 is essentially a thread list.  Any blocking object
    implementation (being a semaphore, mutex, event flag, etc.) canbe built on
    top of this class, utilizing the provided functions to manipulate thread
    location within the Kernel.

    Blocking a thread results in that thread becoming de-scheduled, placed in
    the blocking object's own private list of threads which are waiting on the
    object.

    Unblocking a thread results in the reverse:  The thread is moved back to its
    original location from the blocking list.

    The only difference between a blocking object based on this class is the
    logic used to determine what consitutes a Block or Unblock condition.

    For instance, a semaphore Pend operation may result in a call to the Block()
    method with the currently-executing thread in order to make that thread wait
    for a semaphore Post.  That operation would then invoke the UnBlock() method,
    removing the blocking thread from the semaphore's list, and back into the
    appropriate thread inside the scheduler.

    Care must be taken when implementing blocking objects to ensure that critical
    sections are used judiciously, otherwise asynchronous events like timers and
    interrupts could result in non-deterministic and often catastrophic behavior.

    Mark3 implements a variety of blocking objects including semaphores, mutexes,
    event flags, and IPC mechanisms that all inherit from the basic Blocking-object
    class found in blocking.h/cpp, ensuring consistency and a high degree of
    code-reuse between components.

    @subsection SEMAPHORES Semaphores

    Semaphores are used to synchronized execution of threads based on the
    availability (and quantity) of application-specific resources in the system.
    They are extremely useful for solving producer-consumer     problems, and are
    the method-of-choice for creating efficient, low latency systems, where ISRs
    post semaphores that are handled from within the context of individual
    threads.  Semaphores can also be posted (but not pended) from within the
    interrupt context.

    @subsection MUTEX Mutex

    Mutexes (Mutual exclusion objects) are provided as a means of creating
    "protected sections" around a particular resource, allowing for access of
    these objects to be serialized. Only one thread can hold the mutex at a time
    - other threads have to wait until the region is released by the owner thread
    before they can take their turn operating on the protected resource. Note
    that mutexes can only be owned by threads - they are not available to other
    contexts (i.e. interrupts). Calling the mutex APIs 	from an interrupt will cause
    catastrophic system failures.

    Note that these objects are recursive in Mark3 - that is, the owner thread
    can claim a mutex more than once.  The caveat here is that a recursively-held
    mutex will not be released until a matching “release” call is made for each
    “claim” call.

    Prioritiy inheritence is provided with these objects as a means to avoid
    prioritiy inversions. Whenever a thread at a priority than the mutex owner
    blocks on a mutex, the priority of the current thread is boosted to the
    highest-priority waiter to ensure that other tasks at intermediate priorities
    cannot artificically prevent progress from being made.

    @subsection EVENTFLAG Event Flags

    Event Flags are another synchronization object, conceptually similar to a
    semaphore.

    Unlike a semaphore, however, the condition on which threads are unblocked is
    determined by a more complex set of rules.  Each Event Flag object contains
    a 16-bit field, and threads block, waiting for combinations of bits within
    this field to become set.

    A thread can wait on any pattern of bits from this field to be set, and any
    number of threads can wait on any number of different patterns.  Threads can
    wait on a single bit, multiple bits, or bits from within a subset of bits
    within the field.

    As a result, setting a single value in the flag can result in any number of
    threads becoming unblocked simultaneously.  This mechanism is extremely
    powerful, allowing for all sorts of complex, yet efficient, thread
    synchronization schemes that can be created using a single shared object.

    Note that Event Flags can be set from interrupts, but you cannot wait on an
    event flag from within an interrupt.

    @subsection NOTIFICATION Notification Objects

    Notification objects are the most lightweight of all blocking objects
    supplied by Mark3.

    using this blocking primative, one or more threads wait for the notification
    object to be signalled by code elsewhere in the system (i.e. another thread
    or interrupt).  Once the notification has been signalled, all threads
    currently blocked on the object become unblocked and moved into the ready list.

    Signalling a notification object that has no actively-waiting threads has no
    effect.

    @section MSGMSGQ Messages and Message Queues

    @subsection MESSAGES Messages

    Sending messages between threads is the key means of synchronizing access to
    data, and the primary mechanism to perform asynchronous data processing
    operations.

    Sending a message consists of the following operations:

    - Obtain a Message object from a source message pool
    - Set the message data and event fields
    - Send the message to the destination message queue
    .

    While receiving a message consists of the following steps:

    - Wait for a messages in the destination message queue
    - Process the message data
    - Return the message back to the source message pool
    .

    These operations, and the various data objects involved are discussed in
    more detail in the following section.

    @subsection MESSAGEOBJ Message Objects

    Message objects are used to communicate arbitrary data between threads in a
    safe and synchronous way.

    The message object consists of an event code field and a data field. The
    event code is used to provide context to the message object, while the data
    field (essentially a void * data pointer) is used to provide a payload of
    data corresponding to the particular event.

    Access to these fields is marshalled by accessors - the transmitting thread
    uses the SetData() and SetCode() methods to seed the data, while the
    receiving thread uses the GetData() and GetCode() methods to retrieve it.

    By providing the data as a void data pointer instead of a fixed-size message,
    we achieve an unprecedented measure of simplicity and flexibility. Data can
    be either statically or dynamically allocated, and sized appropriately for
    the event without having to format and reformat data by both sending and
    receiving threads. The choices here are left to the user - and the kernel
    doesn't get in the way of efficiency.

    It is worth noting that you can send messages to message queues from within
    ISR context.  This helps maintain consistency, since the same APIs can be
    used to provide event-driven programming facilities throughout the whole of
    the OS.

    @subsection MESSAGEQUEUES Message Queues

    Message objects specify data with context, but do not specify where the
    messages will be sent. For this purpose we have a MessageQueue object.
    Sending an object to a message queue involves calling the  MessageQueue::Send()
    method, passing in a pointer to the Message object as an argument.

    When a message is sent to the queue, the first thread blocked on the queue
    (as a result of calling the MessageQueue Receive() method) will wake up,
    with a pointer to the Message object returned.

    It's worth noting that multiple threads can block on the same message queue,
    providing a means for multiple threads to share work in parallel.

    @subsection MAILBOXES Mailboxes

    Another form of IPC is provided by Mark3, in the form of Mailboxes and Envelopes.
    Mailboxes are similar to message queues in that they provide a synchronized
    interface by which data can be transmitted between threads.

    Where Message Queues rely on linked lists of lightweight message objects
    (containing only message code and a void* data-pointer), which are inherently
    abstract, Mailboxes use a dedicated blob of memory, which is carved up into
    fixed-size chunks called Envelopes (defined by the user), which are sent and
    received.  Unlike message queues, mailbox data is copied to and from the
    mailboxes dedicated pool.

    Mailboxes also differ in that they provide not only a blocking "receive" call,
    but also a blocking "send" call, providing the opportunity for threads to
    block on "mailbox full" as well as "mailbox empty" conditions.

    All send/receive APIs support an optional timeout parameter if the
    KERNEL_USE_TIMEOUTS option has been configured in mark3cfg.h

    @subsection ATOMICOPS Atomic Operations

    @image html Mark3_arch25.png
    @image latex Mark3_arch25.png "Atomic operations" width=10cm

    This utility class provides primatives for atomic operations - that is,
    operations that are guaranteed to execute uninterrupted.  Basic atomic
    primatives provided here include Set/Add/Delete for 8, 16, and 32-bit integer
    types, as well as an atomic test-and-set.

    @subsection DRIVERS Drivers

    @image html Mark3_arch18.png
    @image latex Mark3_arch18.png "Drivers" width=10cm

    This is the basis of the driver framework.  In the context of Mark3, drivers
    don't necessarily have to be based on physical hardware peripherals.  They
    can be used to represent algorithms (such as random number generators), files,
    or protocol stacks.  Unlike FunkOS, where driver IO is protected automatically
    by a mutex, we do not use this kind of protection - we leave it up to the
    driver implementor to do what's right in its own context.  This also frees
    up the driver to implement all sorts of other neat stuff, like sending
    messages to threads associated with the driver.  Drivers are implemented as
    character devices, with the standard array of posix-style accessor methods
    for reading, writing, and general driver control.

    A global driver list is provided as a convenient and minimal "filesystem"
    structure, in which devices can be accessed by name.

    <b>Driver Design</b>

    A device driver needs to be able to perform the following operations:

    - Initialize a peripheral
    - Start/stop a peripheral
    - Handle I/O control operations
    - Perform various read/write operations
    .

    At the end of the day, that's pretty much all a device driver has to do, and
    all of the functionality that needs to be presented to the developer.

    We abstract all device drivers using a base-class which implements the
    following methods:

    - Start/Open
    - Stop/Close
    - Control
    - Read
    - Write
    .

    A basic driver framework and API can thus be implemented in five function
    calls - that's it! You could even reduce that further by handling the
    initialize, start, and stop operations inside the "control" operation.

    <b>Driver API</b>

    In C++, we can implement this as a class to abstract these event handlers,
    with virtual void functions in the base class overridden by the inherited
    objects.

    To add and remove device drivers from the global table, we use the following
    methods:
    @code
        void DriverList::Add( Driver *pclDriver_ );
        void DriverList::Remove( Driver *pclDriver_ );
    @endcode

    DriverList::Add()/Remove() takes a single argument - the pointer to the object
    to operate on.

    Once a driver has been added to the table, drivers are opened by NAME using
    DriverList::FindByName("/dev/name"). This function returns a pointer to the
    specified driver if successful, or to a built in /dev/null device if the path
    name is invalid.  After a driver is open, that pointer is used for all other
    driver access functions.

    This abstraction is incredibly useful - any peripheral or service can be
    accessed through a consistent set of APIs, that make it easy to substitute
    implementations from one platform to another.  Portability is ensured, the
    overhead is negligible, and it emphasizes the reuse of both driver and
    application code as separate entities.

    Consider a system with drivers for I2C, SPI, and UART peripherals - under
    our driver framework, an application can initialize these peripherals and
    write a greeting to each using the same simple API functions for all drivers:

    @code{.cpp}
        pclI2C  = DriverList::FindByName("/dev/i2c");
        pclUART = DriverList::FindByName("/dev/tty0");
        pclSPI  = DriverList::FindByName("/dev/spi");

        pclI2C->Write(12,"Hello World!");
        pclUART->Write(12, "Hello World!");
        pclSPI->Write(12, "Hello World!");
    @endcode

    @section KRNLPROPER Kernel Proper and Porting

    @image html Mark3_arch26.png
    @image latex Mark3_arch26.png "Kernel Proper and Porting" width=10cm

    The Kernel class is a static class with methods to handle the initialization
    and startup of the RTOS, manage errors, and provide user-hooks for fatal
    error handling (functions called when Kernel::Panic() conditions are
    encountered), or when the Idle function is run.

    Internally, Kernel::Init() calls the initialization routines for various
    kernel objects, providing a single interface by which all RTOS-related system
    initialization takes place.

    Kernel::Start() is called to begin running OS funcitonality, and does not
    return.  Control of the CPU is handed over to the scheduler, and the
    highest-priority ready thread begins execution in the RTOS environment.

    <b>Harware Abstraction Layer</b>

    Almost all of the Mark3 kernel (and middleware) is completely platform
    independent, and should compile cleanly on any platform with a modern C++
    compiler.  However, there are a few areas within Mark3 that can only be
    implemented by touching hardware directly.

    These interfaces generally cover four features:
    - Thread initializaiton and context-switching logic
    - Software interrupt control (used to generate context switches)
    - Hardware timer control (support for time-based functionlity, such as Sleep())
    - Code-execution profiling timer (not necessary to port if code-profiling is
      not compiled into the kernel)
    .

    The hardware abstraction layer in Mark3 provides a consistent interface for
    each of these four features.  Mark3 is ported to new target architectures by
    providing an implementation for all of the interfaces declared in the
    abstraction layer.  In the following section, we will explore how this was
    used to port the kernel to ARM Cortex-M0.

    <b>Real-world  Porting Example – Cortex M0</b>

    This section serves as a real-world example of how Mark3 can be ported to
    new architectures, how the Mark3 abstraction layer works, and as a practical
    reference for using the RTOS support functionality baked in modern ARM
    Cortex-M series microcontrollers.  Most of this documentation here is taken
    directly from the source code found in the /kernel/cpu/cm0/ ports directory,
    with additional annotations to explain the port in more detail.  Note that a
    familiarity with Cortex-M series parts will go a long way to understanding
    the subject matter presented, especially a basic understanding of the ARM
    CPU registers, exception models, and OS support features (PendSV, SysTick
    and SVC).  If you're unfamiliar with ARM architecture, pay attention to the
    comments more than the source itself to illustrate the concepts.

    Porting Mark3 to a new architecture consists of a few basic pieces; for
    developers familiar with the target architecture and the porting process,
    it's not a tremendously onerous endeavour to get Mark3 up-and-running
    somewhere new.  For starters, all non-portable components are completely
    isolated in the source-tree under:

    /embedded/kernel/CPU/VARIANT/TOOLCHAIN/,

    where CPU is the architecture, VARIANT is the vendor/part, and TOOLCHAIN
    is the compiler tool suite used to build the code.

    From within the specific port folder, a developer needs only implement a few
    classes and headers that define the port-specific behavior of Mark3:

    - KernelSWI (kernelswi.cpp/kernelswi.h) - Provides a maskable software-triggered
      interrupt used to perform context switching.
    - KernelTimer (kerneltimer.cpp/kerneltimer.h) - Provides either a fixed-frequency
      or programmable-interval timer, which triggers an interrupt on expiry.
      This is used for implementing round-robin scheduling, thread-sleeps, and
      generic software timers.
    - Profiler (kprofile.cpp/kprofile.h) - Contains code for runtime
      code-profiling.  This is optional and may be stubbed out if left
      unimplemented (we won't cover profiling timers here).
    - ThreadPort (threadport.cpp/threadport.h) - The meat-and-potatoes of the
      port code lives here.  This class contains architecture/part-specific code
      used to initialize threads, implement critical-sections, perform
      context-switching, and start the kernel.  Most of the time spent in this
      article focuses on the code found here.
    .

    Summarizing the above, these modules provide the following list of functionality:

        - Thread stack initialization
        - Kernel startup and first thread entry
        - Context switch and SWI
        - Kernel timers
        - Critical Sections
        .

    The implementation of each of these pieces will be analyzed in detail in the
    sections that follow.

    <b>Thread Stack Initialization</b>

    Before a thread can be used, its stack must first be initialized to its default
    state.  This default state ensures that when the thread is scheduled for the
    first time and its context restored, that it will cause the CPU to jump to
    the user's specified entry-point function.

    All of the platform independent thread setup is handled by the generic kernel
    code.  However, since every CPU architecture has its own register set, and
    stacks different information as part of an interrupt/exception, we have to
    implement this thread setup code for each platform we want the kernel to
    support (Combination of Architecture + Variant + Toolchain).

    In the ARM Cortex-M0 architecture, the stack frame consists of the following
    information:

    a) Exception Stack Frame

    Contains the 8 registers which the ARM Cortex-M0 CPU automatically pushes to
    the stack when entering an exception.  The following registers are included
    (in stack'd order):

    @verbatim
        [ XPSR ] <-- Highest address in context
        [ PC   ]
        [ LR   ]
        [ R12  ]
        [ R3   ]
        [ R2   ]
        [ R1   ]
        [ R0   ]
    @endverbatim

    XPSR – This is the CPU's status register. We need to set this to 0x01000000
    (the "T" bit), which indicates that the CPU is executing in “thumb” mode.
    Note that ARMv6m and ARMv7m processors only run thumb2 instructions, so an
    exception is liable to occur if this bit ever gets cleared.

    PC – Program Counter.  This should be set with the initial entry point
    (function pointer) for that the user wishes to start executing with this thread.

    LR - The base link register.  Normally, this register contains the return
    address of the calling function, which is where the CPU jumps when a function
    returns.  However, our threads generally don't return (and if they do, they're
    placed into the stop state).  As a result we can leave this as 0.

    The other registers in the stack frame are generic working registers, and have
    no special meaning, with the exception that R0 will hold the user's argument
    value passed into the entrypoint.

    b) Complimentary CPU Register Context

    @verbatim
        [ R11   ]
        ...
        [ R4    ] <-- Lowest address in context
    @endverbatim

    These are the other general-purpose CPU registers that need to be backed up/
    restored on a context switch, but aren't stacked by default on a Cortex-M0
    exception.  If there were any additional hardware registers to back up, then
    we'd also have to include them in this part of the context as well.

    As a result, these registers all need to be manually pushed to the stack on
    stack creation, and will need to be explicitly pushed and pop as part of a
    normal context switch.

    With this default exception state in mind, the following code is used to
    initialize a thread's stack for a Cortex-M0.

    @code{.cpp}
    void ThreadPort::InitStack(Thread *pclThread_)
    {
        K_ULONG *pulStack;
        K_ULONG *pulTemp;
        K_ULONG ulAddr;
        K_USHORT i;

        // Get the entrypoint for the thread
        ulAddr = (K_ULONG)(pclThread_->m_pfEntryPoint);

        // Get the top-of-stack pointer for the thread
        pulStack = (K_ULONG*)pclThread_->m_pwStackTop;

        // Initialize the stack to all FF's to aid in stack depth checking
        pulTemp = (K_ULONG*)pclThread_->m_pwStack;
        for (i = 0; i < pclThread_->m_usStackSize / sizeof(K_ULONG); i++)
        {
            pulTemp[i] = 0xFFFFFFFF;
        }

        PORT_PUSH_TO_STACK(pulStack, 0);             // Apply one word of padding

        //-- Simulated Exception Stack Frame --
        PORT_PUSH_TO_STACK(pulStack, 0x01000000);    // XSPR;set "T" bit for thumb-mode
        PORT_PUSH_TO_STACK(pulStack, ulAddr);        // PC
        PORT_PUSH_TO_STACK(pulStack, 0);             // LR
        PORT_PUSH_TO_STACK(pulStack, 0x12);
        PORT_PUSH_TO_STACK(pulStack, 0x3);
        PORT_PUSH_TO_STACK(pulStack, 0x2);
        PORT_PUSH_TO_STACK(pulStack, 0x1);
        PORT_PUSH_TO_STACK(pulStack, (K_ULONG)pclThread_->m_pvArg);    // R0 = argument

        //-- Simulated Manually-Stacked Registers --
        PORT_PUSH_TO_STACK(pulStack, 0x11);
        PORT_PUSH_TO_STACK(pulStack, 0x10);
        PORT_PUSH_TO_STACK(pulStack, 0x09);
        PORT_PUSH_TO_STACK(pulStack, 0x08);
        PORT_PUSH_TO_STACK(pulStack, 0x07);
        PORT_PUSH_TO_STACK(pulStack, 0x06);
        PORT_PUSH_TO_STACK(pulStack, 0x05);
        PORT_PUSH_TO_STACK(pulStack, 0x04);
        pulStack++;

        pclThread_->m_pwStackTop = pulStack;
    }
    @endcode

    <b>Kernel Startup</b>

    The same general process applies to starting the kernel on an ARM Cortex-M0
    as on other platforms.  Here, we initialize and start the platform specific
    timer and software-interrupt modules, find the first thread to run, and then
    jump to that first thread.

    Now, to perform that last step, we have two options:

    1) Simulate a return from an exception manually to start the first thread, or..
    2) Use a software interrupt to trigger the first "Context Restore/Return from
       Interrupt"

    For 1), we basically have to restore the whole stack manually, not relying
    on the CPU to do any of this for us.  That's certainly doable, but not all
    Cortex parts support this (other members of the family support privileged
    modes, etc.).  That, and the code required to do this is generally more
    complex due to all of the exception-state simulation.  So, we will opt for
    the second option instead.

    To implement a software to start our first thread, we will use the SVC
    instruction to generate an exception.  From that exception, we can then
    restore the context from our first thread, set the CPU up to use the right
    “process” stack, and return-from-exception back to our first thread.  We'll
    explore the code for that later.

    But, before we can call the SVC exception, we're going to do a couple of things.

    First, we're going to reset the default MSP stack pointer to its original
    top-of-stack value.  The rationale here is that we no longer care about the
    data on the MSP stack, since calling the SVC instruction triggers a chain of
    events from which we never return.  The MSP is also used by all
    exception-handling, so regaining a few words of stack here can be useful.
    We'll also enable all maskable exceptions at this point, since this code
    results in the kernel being started with the CPU executing the RTOS threads,
    at which point a user would expect interrupts to be enabled.

    Note, the default stack pointer location is stored at address 0x00000000 on
    all ARM Cortex M0 parts.  That explains the code below...

    @code{.cpp}
    void ThreadPort_StartFirstThread( void )
    {
        asm(
            " ldr r1, [r0] \n" // Reset the MSP to the default base address
            " msr msp, r1 \n"
            " cpsie i \n"      // Enable interrupts
            " svc 0 \n"        // Jump to SVC Call
            );
    }
    @endcode

    <b>First Thread Entry</b>

    This handler has the job of taking the first thread object's stack, and
    restoring the default state data in a way that ensures that the thread starts
    executing when returning from the call.

    We also keep in mind that there's an 8-byte offset from the beginning of the
    thread object to the location of the thread stack pointer.  This offset is a
    result of the thread object inheriting from the linked-list node class, which
    has 8-bytes of data.  This is stored first in the object, before the first
    element of the class, which is the "stack top" pointer.

    The following assembly code shows how the SVC call is implemented in Mark3
    for the purpose of starting the first thread.

    @code
    get_thread_stack:
        ; Get the stack pointer for the current thread
        ldr r0, g_pstCurrent
        ldr r1, [r0]
        add r1, #8
        ldr r2, [r1]         ; r2 contains the current stack-top

    load_manually_placed_context_r11_r8:
        ; Handle the bottom 32-bytes of the stack frame
        ; Start with r11-r8, because only r0-r7 can be used
        ; with ldmia on CM0.
        add r2, #16
        ldmia r2!, {r4-r7}
        mov r11, r7
        mov r10, r6
        mov r9, r5
        mov r8, r4

    set_psp:
        ; Since r2 is coincidentally back to where the stack pointer should be,
        ; Set the program stack pointer such that returning from the exception handler
        msr psp, r2

    load_manually_placed_context_r7_r4:
        ; Get back to the bottom of the manually stacked registers and pop.
        sub r2, #32
        ldmia r2!, {r4-r7}  ; Register r4-r11 are restored.

    set_thread_and_privilege_modes:
        ; Also modify the control register to force use of thread mode as well
        ; For CM3 forward-compatibility, also set user mode.
        mrs r0, control
        mov r1, #0x03
        orr r0, r1
        control, r0

    set_lr:
        ; Set up the link register such that on return, the code operates
        ; in thread mode using the PSP. To do this, we or 0x0D to the value stored
        ; in the lr by the exception hardware EXC_RETURN. Alternately, we could
        ; just force lr to be 0xFFFFFFFD (we know that's what we want from the
        ; hardware, anyway)
        mov  r0, #0x0D
        mov  r1, lr
        orr r0, r1

    exit_exception:
        ; Return from the exception handler.
        ; The CPU will automagically unstack R0-R3, R12, PC, LR, and xPSR
        ; for us.  If all goes well, our thread will start execution at the
        ; entrypoint, with the us-specified argument.
        bx r0
    @endcode

    On ARM Cortex parts, there's dedicated hardware that's used primarily to
    support RTOS (or RTOS-like) funcationlity.  This functionality includes the
    SysTick timer, and the PendSV Exception.  SysTick is used for a tick-based
    kernel timer, while the PendSV exception is used for performing context
    switches.  In reality, it's a "special SVC" call that's designed to be
    lower-overhead, in that it isn't mux'd with a bunch of other system or
    application functionality.

    So how do we go about actually implementing a context switch here? There are
    a lot of different parts involved, but it essentially comes down to 3 steps:

    1) Saving the context.

        Thread's top-of-stack value is stored, all registers are stacked.  We're
        good to go!

    2)  Swap threads

        We swap the Scheduler's “next” thread with the “current” thread.

    3)  Restore Context

        This is more or less identical to what we did when restoring the first
        context.  Some operations may be optimized for data already stored in
        registers.

    The code used to implement these steps on Cortex-M0 is presented below:

    @code{.cpp}
    void PendSV_Handler(void)
    {
        ASM(
        // Thread_SaveContext()
        " ldr r1, CURR_ \n"
        " ldr r1, [r1] \n "
        " mov r3, r1 \n "
        " add r3, #8 \n "

        //  Grab the psp and adjust it by 32 based on extra registers we're going
        // to be manually stacking.
        " mrs r2, psp \n "
        " sub r2, #32 \n "

        // While we're here, store the new top-of-stack value
        " str r2, [r3] \n "

        // And, while r2 is at the bottom of the stack frame, stack r7-r4
        " stmia r2!, {r4-r7} \n "

        // Stack r11-r8
        " mov r7, r11 \n "
        " mov r6, r10 \n "
        " mov r5, r9 \n "
        " mov r4, r8 \n "
        " stmia r2!, {r4-r7} \n "

        // Equivalent of Thread_Swap() - performs g_pstCurrent = g_pstNext
        " ldr r1, CURR_ \n"
        " ldr r0, NEXT_ \n"
        " ldr r0, [r0] \n"
        " str r0, [r1] \n"

        // Thread_RestoreContext()
        // Get the pointer to the next thread's stack
        " add r0, #8 \n "
        " ldr r2, [r0] \n "

        // Stack pointer is in r2, start loading registers from
        // the "manually-stacked" set
        // Start with r11-r8, since these can't be accessed directly.
        " add r2, #16 \n "
        " ldmia r2!, {r4-r7} \n "
        " mov r11, r7 \n "
        " mov r10, r6 \n "
        " mov r9, r5 \n "
        " mov r8, r4 \n "

        // After subbing R2 #16 manually, and #16 through ldmia, our PSP is where it
        // needs to be when we return from the exception handler
        " msr psp, r2 \n "

        // Pop manually-stacked R4-R7
        " sub r2, #32 \n "
        " ldmia r2!, {r4-r7} \n "

        // lr contains the proper EXC_RETURN value
        // we're done with the exception, so return back to newly-chosen thread
        " bx lr \n "
        " nop \n "

        // Must be 4-byte aligned.
        " NEXT_: .word g_pstNext \n"
        " CURR_: .word g_pstCurrent \n"
        );
    }
    @endcode

    <b>Kernel Timers</b>

    ARM Cortex-M series microcontrollers each contain a SysTick timer, which was
    designed to facilitate a fixed-interval RTOS timer-tick.  This timer is a
    precise 24-bit down-count timer, run at the main CPU clock frequency, that
    can be programmed to trigger an exception when the timer expires.  The
    handler for this exception can thus be used to drive software timers
    throughout the system on a fixed interval.

    Unfortunately, this hardware is extremely simple, and does not offer the
    flexibility of other timer hardware commonly implemented by MCU vendors -
    specifically a suitable timer prescalar that can be used to generate efficient,
    long-counting intervals.  As a result, while the "generic" port of Mark3 for
    Cortex-M0 leverages the common SysTick timer interface, it only supports the
    tick-based version of the kernel's timer (note that specific Cortex-M0 ports
    such as the Atmel SAMD20 do have tickless timers).

    Setting up a tick-based KernelTimer class to use the SysTick timer is,
    however, extremely easy, as is illustrated below:

    @code{.cpp}
    void KernelTimer::Start(void)
    {
        SysTick_Config(PORT_SYSTEM_FREQ / 1000); // 1KHz fixed clock...
        NVIC_EnableIRQ(SysTick_IRQn);
    }

    In this instance, the call to SysTick_Config() generates a 1kHz system-tick
    signal, and the NVIC_EnableIRQ() call ensures that a SysTick exception is
    generated for each tick.  All other functions in the Cortex version of the
    KernelTimer class are essentially stubbed out (see the source for more details).

    Note that the functions used in this call are part of the ARM Cortex
    Microcontroller Software Interface Standard (cmsis), and are supplied by all
    parts vendors selling Cortex hardware.  This greatly simplifies the design
    of our port-code, since we can be reasonably assured that these APIs will
    work the same on all devices.

    The handler code called when a SysTick exception occurs is basically the
    same as on other platforms (such as AVR), except that we explicitly clear the
    "exception pending" bit before returning.  This is implemented in the
    following code:

    @code{.cpp}
    void SysTick_Handler(void)
    {
    #if KERNEL_USE_TIMERS
        TimerScheduler::Process();
    #endif
    #if KERNEL_USE_QUANTUM
        Quantum::UpdateTimer();
    #endif

        // Clear the systick interrupt pending bit.
        SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
    }
    @endcode

    <b>Critical Sections</b>

    A "critical section" is a block of code whose execution cannot be interrupted
    by means of context switches or an interrupt.  In a traditional single-core
    operating system, it is typically implemented as a block of code where the
    interrupts are disabled - this is also the approach taken by Mark3.  Given
    that every CPU has its own means of disabling/enabling interrupts, the
    implementation of the critical section APIs is also non-portable.

    In the Cortex-M0 port, we implement the two critical section APIs (CriticalSection::Enter()
    and CriticalSection::Exit()) as function-like macros containing inline assembly.  All uses
    of these calls are called in pairs within a function and must take place at
    the same level-of-scope.  Also, as nesting may occur (critical section within
    a critical section), this must be taken into account in the code.

    In general, CriticalSection::Enter() performs the following tasks:

        - Cache the current interrupt-enabled state within a local variable in the
        thread's state
        - Disable interrupts
        .

    Conversely, CriticalSection::Exit() performs the following tasks:

        - Read the original interrupt-enabled state from the cached value
        - Restore interrupts to the original value
        .

    On Cortex-M series micrcontrollers, the PRIMASK special register contains a
    single status bit which can be used to enable/disable all maskable interrupts
    at once.  This register can be read directly to examine or modify its state.
    For convenience, ARMv6m provides two instructions to enable/disable interrupts
    - cpsid (disable interrupts) and cpsie (enable interrupts).  Mark3 Implements
    these steps according to the following code:

    @code{.cpp}
    //------------------------------------------------------------------------
    //! Enter critical section (copy current PRIMASK register value, disable interrupts)
    #define CriticalSection::Enter()    \
    {    \
        K_ULONG __ulRegState;    \
        asm    ( \
        " mrs r0, PRIMASK \n"    \
        " mov %[STATUS], r0 \n" \
        " cpsid i \n "    \
        : [STATUS] "=r" (__ulRegState) \
        );

    //------------------------------------------------------------------------
    //! Exit critical section (restore previous PRIMASK status register value)
    #define CriticalSection::Exit() \
        asm    ( \
        " mov r0, %[STATUS] \n" \
        " msr primask, r0 \n"    \
        : \
        : [STATUS] "r" (__ulRegState) \
        ); \
    }
    @endcode

    <b>Summary</b>

    In this section we have investigated how the main non-portable areas of the
    Mark3 RTOS are implemented on a Cortex-M0 microcontroller.  Mark3 leverages
    all of the hardware blocks designed to enable RTOS functionality on ARM
    Cortex-M series microcontrollers: the SVC call provides the mechanism by
    which we start the kernel, the PendSV exception provides the necessary
    software interrupt, and the SysTick timer provides an RTOS tick. As a result,
    Mark3 is a perfect fit for these devices - and as a result of this approach,
    the same RTOS port code should work with little to no modification on all
    ARM Cortex-M parts.

    We have discussed what functionality in the RTOS is not portable, and what
    interfaces must be implemented in order to complete a fully-functional port.
    The five specific areas which are non-portable (stack initialization, kernel
    startup/entry, kernel timers, context switching, and critical sections)  have
    been discussed in detail, with the platform-specifc source provided as a
    practical reference to ARM-specific OS features, as well as Mark3's porting
    infrastructure.  From this example (and the accompanying source), it should
    be possible for an experienced developers to create a port Mark3 to other
    microcontroller targets.
*/
/**
    @page MARK3C C-language bindings

    Mark3 now includes an optional additional library with C language bindings
    for all core kernel APIs, known as Mark3C.  This library alllows applications
    to be written in C, while still enjoying all of the benefits of the
    clean, modular design of the core RTOS kernel.

    The C-language Mark3C APIs map directly to their Mark3 counterparts using
    a simple set of conventions, documented below.  As a result, explicit API
    documentation for Mark3C is not necessary, as the functions map 1-1 to their
    C++ counterparts.

    @section MARK3CAPI API Conventions

    1) Static Methods:

    @verbatim
    <ClassName>::<MethodName>()   Becomes    <ClassName>_<MethodName>()
    i.e. Kernel::Start()          Becomes    Kernel_Start()
    @endverbatim

    2) Kernel Object Methods:

    In short, any class instance is represented using an object handle, and is
    always passed into the relevant APIs as the first argument.  Further, any
    method that returns a pointer to an object in the C++ implementation now
    returns a handle to that object.

    @verbatim
    <Object>.<MethodName>(<args>) Becomes    <ClassName>_<MethoodName>(<ObjectHandle>, <args>)

    i.e. clAppThread.Start()      Becomes    Thread_Start(hAppThread)
    @endverbatim

    3) Overloaded Methods:

    a) Methods overloaded with a Timeout parameter:

    @verbatim
    <Object>.<MethodName>(<args>) Becomes    <ClassName>_Timed<MethodName>(<ObjectHandle>, <args>)

    i.e. clSemaphore.Wait(1000)   Becomes    Semaphore_Wait(hSemaphore, 1000)
    @endverbatim
    b) Methods overloaded based on number of arguments:

    @verbatim
    <Object>.<MethodName>()                   Becomes     <ClassName>_<MethodName>(<ObjectHandle>)
    <Object>.<MethodName>(<arg1>)             Becomes     <ClassName>_<MethodName>1(<ObjectHandle>, <arg1>)
    <Object>.<MethodName>(<arg1>, <arg2>)     Becomes     <ClassName>_<MethodName>2(<ObjectHandle>, <arg1>, <arg2>)

    <ClassName>::<MethodName>()               Becomes     <ClassName>_<MethodName>(<ObjectHandle>)
    <ClassName>::<MethodName>(<arg1>)         Becomes     <ClassName>_<MethodName>1(<ObjectHandle>, <arg1>)
    <ClassName>::<MethodName>(<arg1>, <arg2>) Becomes     <ClassName>_<MethodName>2(<ObjectHandle>, <arg1>, <arg2>)
    @endverbatim

    c) Methods overloaded base on parameter types:

    @verbatim
    <Object>.<MethodName>(<arg type_a>)       Becomes     <ClassName>_<MethodName><type_a>(<ObjectHandle>, <arg type a>)
    <Object>.<MethodName>(<arg type_b>)       Becomes     <ClassName>_<MethodName><type_b>(<ObjectHandle>, <arg type b>)
    <ClassName>::<MethodName>(<arg type_a>)   Becomes     <ClassName>_<MethodName><type_a>(<arg type a>)
    <ClassName>::<MethodName>(<arg type_b>)   Becomes     <ClassName>_<MethodName><type_b>(<arg type b>)
    @endverbatim

    d) Allocate-once memory allocation APIs

    @verbatim
    AutoAlloc::New<ObjectName>                Becomes      Alloc_<ObjectName>
    AutoAlloc::Allocate(uint16_t u16Size_)    Becomes      AutoAlloc(uint16_t u16Size_)
    @endverbatim

    @section MARK3CALLOC Allocating Objects

    Aside from the API name translations, the object allocation scheme is the
    major different between Mark3C and Mark3.  Instead of instantiating objects
    of the various kernel types, kernel objects must be declared using Declaration
    macros, which serve the purpose of reserving memory for the kernel object, and
    provide an opaque handle to that object memory.  This is the case for
    statically-allocated objects, and objects allocated on the stack.

    Example: Declaring a thread

    @verbatim

    #include "mark3c.h"

    // Statically-allocated
    DECLARE_THREAD(hMyThread1);
    ...

    // On stack
    int main(void)
    {
        DECLARE_THREAD(hMyThread2);
        ...
    }

    @endverbatim


    Where:

        hMyThread1 - is a handle to a statically-allocated thread
        hMyThread2 - is a handle to a thread allocated from the main stack.

    Alternatively, the AutoAlloc APIs can be used to dynamically allocate objects,
    as demonstrated in the following example.

    @verbatim

    void Allocate_Example(void)
    {
        Thread_t hMyThread = AutoAlloc_Thread();

        Thread_Init(hMyThread, awMyStack, sizeof(awMyStack), 1, MyFunction, 0);
    }

    @endverbatim

    Note that the relevant kernel-object Init() function *must* be called prior to using
    any kernel object, whether or not they have been allocated statically, or dynamically.
*/
/**
    @page RELEASE Release Notes

    @section RELR10 R10 Release
    - New: Coroutines + Cooperative scheduler (with examples + unit tests)
    - New: Critical section APIs defined in kernel lib
    - New: RAII critical section (CriticalGuard object)
    - New: RAII scheduler-disabled context (SchedulerGuard object)
    - New: Thread::GetStackTop() method, useful for implementing stack-dump features
    - Support for thread-local "errno", for use with the mark3-libc library
    - Removed a bunch of functions from memutil that are better-implemented in libc
    - Kernel code updated to use RAII critical sections instead of CS_ENTER/CS_EXIT macros
    - Updated documentation
    - Minor bugfixes
    .

    @section RELR9 R9 Release
    - New: templated linked-lists to avoid explicit casting used in list traversal
    - New: ThreadListList class to efficiently track all threads in the system 
    - Remove use of C-style casts in kernel
    - Remove use of 0 as nullptr in kernel
    - Refactor code to use constexpr instead of C-style preprocessor defines where possible
    - Refactor priority-map class as a set of template classes, reducing use of macros and defines
    - Fix a "disappearing thread" bug where an inopportune context switch could cause a thread to get lost
    - Docs no longer build by default
    .

    @section RELR8 R8 Release
    - Structural changes to separate the kernel from the rest of Mark3-repo
    - Cleanup and reformatting
    .

    @section RELR7 R7 (Full Throttle) Release
    - Re-focusing project on kernel, integrating with 3rd party code instead of 1st party middleware
    - Re-focusing on atmega1284p and cortex-m as default targets
    - New: Refactored codebase to C++14 standard
    - New: Moved non-kernel code, drivers, libs, and BSPs to separate repos from kernel
    - New: Modular repository-based structure, managed via Android's Repo tool
    - New: ConditionVariable kernel API
    - New: ReaderWriterLock kernel API
    - New: AutoAlloc redirects to user-defined allocators
    - New: Global new() and delete() overrides redirect to AutoAlloc APIs
    - New: RAII Mutex Locking APIs
    - New: Support for cortex-a53 (aarch64) targets
    - New: Doxygen builds as part of cmake process
    - Updated Mark3c for new APIs
    - Moved driver layer out of the kernel
    - Moved all non-essential libraries out of the kernel (into other repos)
    - Build system supports modular BSP architecture
    - Removed fake idle-thread feature, since it doesn't support all targets
    - Unit tests and examples will run on any target with a BSP
    - Moved AVR-specific code out of the kernel (kernelaware debugging support)
    - Remove most build-time configuration flags from mark3cfg.h, and remove associated ifdefs throughout the code.
    - Support qemu-system-arm's lm3s6965 evb target, with semihosting
    - Support qemu-system-arm's rasbpi3 evb target, with semihosting
    - Incrase test coverage
    - Various bugfixes and improvements
    .

    @section RELR6 R6 Release
    - New: Replace recursive-make build system with CMake and Ninja
    - New: Transitioned version control to Git from Subversion.
    - New: Socket library, implementing named "domain-socket" style IPC
    - New: State Machine framework library
    - New: Software I2C library completed, with demo app
    - New: Kernel Timer loop can optionally be run within its own thread instead of a nested interrupt
    - New: UART drivers are all now abstracted throught UartDriver base class for portability
    - Experimental: Process library, allowing for the creation of resource-isolated processes
    - Removed: Bare-metal support for Atmel SAMD20 (generic port still works)
    - Cleanup all compiler warnings on atmega328p
    - Various Bugfixes and optimizations
    - Various Script changes related to automating the build + release process
    .

    @section RELR5 R5 Release
    - New: Shell library for creating responsive CLIs for embedded applications (M3Shell)
    - New: Stream library for creating thread-safe buffered streams (streamer)
    - New: Blocking UART implementation for AVR (drvUARTplus)
    - New: "Extended context" kernel feature, which is used to implement thread-local storage
    - New: "Extra Checks" kernel feature, which enforces safe API usage under pain of Kernel Panic
    - New: Realtime clock library
    - New: Example application + bsp for the open-hardware Mark3no development board (mark3no)
    - New: Kernel objects descoped/destroyed while still in active use will now cause kernel panic
    - New: Kernel callouts for thread creation/destruction/context switching, used for time tracking
    - New: Simple power management class
    - New: WIP software-based I2C + SPI drivers
    - Optimized thread scheduling via target-optimized "count-leading-zero" macros
    - Expanded memutil library
    - Various optimizations of ARM Cortex-M assembly code
    - Various bugfixes to Timer code
    - Improved stack overflow checking + warning (stack guard kernel feature)
    - AVR bootloader now supports targets with more than 64K of flash
    - Moved some port configuration out of platform.mak into header files in the kernel port code
    - The usual minor bugfixes and "gentle refactoring"
    .

    @section RELR4 R4 Release

    - New: C-language bindings for Mark3 kernel (mark3c library)
    - New: Support for ARM Cortex-M3 and Cortex-M4 (floating point) targets
    - New: Support for Atmel AVR atmega2560 and arduino pro mega
    - New: Full-featured, lightweight heap implementation
    - New: Mailbox IPC class
    - New: Notification object class
    - New: lighweight tracelogger/instrumentation implementation (buffalogger), with sample parser
    - New: High-performance AVR Software UART implementation
    - New: Allocate-once "AutoAlloc" memory allocator
    - New: Fixed-time blocking/unblocking operations added to ThreadList/Blocking class
    - Placement-new supported for all kernel objects
    - Scheduler now supports up to 1024 levels of thread priority, up from 8 (configurable at build-time)
    - Kernel now uses stdint.h types for standard integers (instead of K_CHAR, K_ULONG, etc.)
    - Greatly expanded documentation, with many new examples covering all key kernel features
    - Expanded unit test coverage on AVR
    - Updated build system and scripts for easier kernel configuration
    - Updated builds to only attempt to build tests for supported platforms
    .

    @section RELR3 R3 Release

    - New: Added support for MSP430 microcontrollers
    - New: Added Kernel Idle-Function hook to eliminate the need for a dedicated idle-thread (where supported)
    - New: Support for kernel-aware simulation and testing via flAVR AVR simulator
    - Updated AVR driver selection
    - General bugfixes and maintenance
    - Expanded documentation and test coverage
    .

    @section RELR2  R2

    - Experimental release, using a "kernel transaction queue" for serializing kernel calls
    - Works as a proof-of-concept, but abandoned due to overhead of the transaction mechanism in the general case.
    .

    @section RELR12 R1 - 2nd Release Candidate

    - New: Added support for ARM Cortex-M0 targets
    - New: Added support for various AVR targets
    - New: Timers now support a "tolerance" parameter for grouping timers with close expiry times
    - Expanded scripts and auotmation used in build/test
    - Updated and expanded graphics APIs
    - Large number of bugfixes
    .

    @section RELR1 R1 - 1st Release Candidate

    - Initial release, with support for AVR microcontrollers
    .

 */
/**
    @example lab1_kernel_setup/main.cpp
    This example demonstrates basic kernel setup with two threads.

    @example lab2_coroutines/main.cpp
    This example demonstrates usage of the coroutine scheduler run from the idle thread.

    @example lab3_round_robin/main.cpp
    This example demonstrates how to use round-robin thread scheduling with
    multiple threads of the same priority.

    @example lab4_semaphores/main.cpp
    This example demonstrates how to use semaphores for Thread synchronization.

    @example lab5_mutexes/main.cpp
    This example demonstrates how to use mutexes to protect against concurrent
    access to resources.

    @example lab6_timers/main.cpp
    This example demonstrates how to create and use software timers.

    @example lab7_events/main.cpp
    This example demonstrates how to create and use event groups

    @example lab8_messages/main.cpp
    This example demonstrates how to pass data between threads using message
    passing.

    @example lab9_dynamic_threads/main.cpp
    This example demonstrates how to create and destroy threads dynamically at
    runtime.

    @example lab10_notifications/main.cpp
    This examples demonstrates how to use notifcation objects as a thread
    synchronization mechanism.

    @example lab11_mailboxes/main.cpp
    This examples shows how to use mailboxes to deliver data between threads in
    a synchronized way.
*/
