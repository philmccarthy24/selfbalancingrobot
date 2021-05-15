#include "ThreadHelper.h"
#include "spdlog/spdlog.h"
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

namespace sbrcontroller {
    namespace utility {

        ThreadHelper::ThreadHelper()
        {}

        ThreadHelper::~ThreadHelper()
        {}

        void ThreadHelper::SetRealtimePriority(std::shared_ptr<spdlog::logger> pLogger)
        {
            int ret;
        
            // We'll operate on the currently running thread.
            pthread_t this_thread = pthread_self();
            // struct sched_param is used to store the scheduling priority
            struct sched_param params;
        
            // We'll set the priority to the maximum.
            params.sched_priority = sched_get_priority_max(SCHED_FIFO);

            pLogger->debug("Trying to set thread realtime prio = {}", params.sched_priority);
        
            // Attempt to set thread real-time priority to the SCHED_FIFO policy
            ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);
            if (ret != 0) {
                // Print the error
                pLogger->error("Unsuccessful in setting thread realtime prio");
                return;     
            }

            // Now verify the change in thread priority
            int policy = 0;
            ret = pthread_getschedparam(this_thread, &policy, &params);
            if (ret != 0) {
                pLogger->error("Couldn't retrieve real-time scheduling paramers");
                return;
            }
        
            // Check the correct policy was applied
            if (policy != SCHED_FIFO) {
                pLogger->error("Scheduling is NOT SCHED_FIFO!");
            } else {
                pLogger->debug("SCHED_FIFO OK");
            }
        
            // Print thread scheduling priority
            pLogger->debug("Thread priority is {}", params.sched_priority);
        }
    }
}
        