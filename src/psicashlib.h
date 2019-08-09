#pragma once

#pragma warning(disable: 4127)
#include "3rdParty/psicash/psicash.hpp"
#include "tstring.h"
#include "stopsignal.h"
#include "dispatch_queue.h"


namespace psicash {

/// Wraps the PsiCash library and provides the HTTP requester. 
/// Network calls are made on a separate thread.
/// Uses and respects the GlobalStopSignal.
class Lib : public PsiCash {
public:
    static Lib& _()
    {
        // Instantiated on first use.
        // Guaranteed to be destroyed.
        static Lib instance;
        return instance;
    }

private:
    Lib();
public:
    Lib(const Lib&) = delete;
    Lib& operator=(Lib const&) = delete;

    virtual ~Lib();

    /// Initialize the library. Error means all subsequent calls will fail.
    /// Must only be called once.
    error::Error Init();

    /// Update the client region (in the request metadata) as it's better known.
    error::Error UpdateClientRegion(const string& region);

    /// Makes a RefreshState request. 
    /// Network and callback will happen on a separate thread.
    void RefreshState(
        std::function<void(error::Result<Status>)> callback);

    /// Makes a NewExpiringPurchase request.
    /// Network and callback will happen on a separate thread.
    void NewExpiringPurchase(
        const std::string& transactionClass,
        const std::string& distinguisher,
        const int64_t expectedPrice,
        std::function<void(error::Result<NewExpiringPurchaseResponse>)> callback);

protected:
    /// If this returns true, the request has been made and requestTask has been moved.
    bool MakeLimitedRequest(std::packaged_task<void()>&& requestTask);

private:
    HANDLE m_mutex;
    const StopInfo m_requestStopInfo;

    dispatch_queue m_requestQueue;
};

} // namespace psicash
