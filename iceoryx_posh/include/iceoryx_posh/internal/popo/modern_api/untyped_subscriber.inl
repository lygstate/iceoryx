// Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef IOX_POSH_POPO_UNTYPED_SUBSCRIBER_INL
#define IOX_POSH_POPO_UNTYPED_SUBSCRIBER_INL

namespace iox
{
namespace popo
{
template <template <typename, typename, typename> class base_subscriber_t>
inline UntypedSubscriberImpl<base_subscriber_t>::UntypedSubscriberImpl(const capro::ServiceDescription& service)
    : SubscriberParent(service)
{
}

template <template <typename, typename, typename> class base_subscriber_t>
inline uid_t UntypedSubscriberImpl<base_subscriber_t>::getUid() const noexcept
{
    return SubscriberParent::getUid();
}

template <template <typename, typename, typename> class base_subscriber_t>
inline capro::ServiceDescription UntypedSubscriberImpl<base_subscriber_t>::getServiceDescription() const noexcept
{
    return SubscriberParent::getServiceDescription();
}

template <template <typename, typename, typename> class base_subscriber_t>
inline void UntypedSubscriberImpl<base_subscriber_t>::subscribe(const uint64_t queueCapacity) noexcept
{
    SubscriberParent::subscribe(queueCapacity);
}

template <template <typename, typename, typename> class base_subscriber_t>
inline SubscribeState UntypedSubscriberImpl<base_subscriber_t>::getSubscriptionState() const noexcept
{
    return SubscriberParent::getSubscriptionState();
}

template <template <typename, typename, typename> class base_subscriber_t>
inline void UntypedSubscriberImpl<base_subscriber_t>::unsubscribe() noexcept
{
    return SubscriberParent::unsubscribe();
}

template <template <typename, typename, typename> class base_subscriber_t>
inline bool UntypedSubscriberImpl<base_subscriber_t>::hasNewSamples() const noexcept
{
    return SubscriberParent::hasNewSamples();
}

template <template <typename, typename, typename> class base_subscriber_t>
inline bool UntypedSubscriberImpl<base_subscriber_t>::hasMissedSamples() noexcept
{
    return SubscriberParent::hasMissedSamples();
}

template <template <typename, typename, typename> class base_subscriber_t>
inline cxx::expected<cxx::optional<Sample<const void>>, ChunkReceiveError>
UntypedSubscriberImpl<base_subscriber_t>::take() noexcept
{
    return SubscriberParent::take();
}

template <template <typename, typename, typename> class base_subscriber_t>
inline void UntypedSubscriberImpl<base_subscriber_t>::releaseQueuedSamples() noexcept
{
    SubscriberParent::releaseQueuedSamples();
}

} // namespace popo
} // namespace iox

#endif // IOX_POSH_POPO_UNTYPED_SUBSCRIBER_INL
