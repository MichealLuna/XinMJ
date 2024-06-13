#ifndef XINMJ_BASE_THREAD_H
#define XINMJ_BASE_THREAD_H

#include <xin/base/noncopyable.h>

#include <atomic>
#include <functional>
#include <error.h>
#include <memory>
#include <string>
#include <pthread.h>

namespace xin
{
    class Thread : public noncopyable
    {
        public:
            typedef std::function<void()> ThreadFunc;

        public:
            explicit Thread(const ThreadFunc&,const std::string& name = std::string());
            explicit Thread(ThreadFunc&&,const std::string& name = std::string());
            ~Thread();

            void start();
            int join();
            
            bool started() const { return started_; }
            pid_t tid() const { return *tid_; }
            const std::string& name() const { return name_; }

            static int numCreated() { return numCreated_; }

        private:
            void setDefaultName();

            bool        started_;
            bool        joined_;
            pthread_t   pthreadId_;
            //using shared_ptr for thread-safety
            std::shared_ptr<pid_t> tid_;
            ThreadFunc  func_;
            std::string name_;

            static std::atomic<int32_t> numCreated_;
    };
}// namespace xin

#endif //!XINMJ_BASE_THREAD_H