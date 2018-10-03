/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_rewards/browser/media_fetcher.h"

#include <string>

#include "base/callback.h"
#include "base/files/file_path.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_avatar_icon_util.h"
#include "net/base/load_flags.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "ui/gfx/image/image.h"

namespace brave_rewards {

MediaFetcher::MediaFetcher(
    GURL url,
    const FetchCompleteCallback& callback)
    : callback_(callback) {
  DCHECK(!callback_.is_null());
  net::NetworkTrafficAnnotationTag traffic_annotation =
      net::DefineNetworkTrafficAnnotation("brave_rewards_favicon_fetcher", R"(
        semantics {
          sender:
            "Brave Rewards Media Fetcher"
          description:
            "Fetches favicon for media publishers in Rewards."
          trigger:
            "User visits a media publisher content."
          data: "Favicon for media publisher."
          destination: WEBSITE
        }
        policy {
          cookies_allowed: NO
          setting:
            "This feature cannot be disabled by settings."
          policy_exception_justification:
            "Not implemented."
        })");
  fetcher_.reset(new BitmapFetcher(url, this, traffic_annotation));
}

MediaFetcher::~MediaFetcher() {
}

void MediaFetcher::Start() {
  SystemNetworkContextManager* system_network_context_manager =
      g_browser_process->system_network_context_manager();
  // In unit tests, the browser process can return a NULL context manager
  if (!system_network_context_manager)
    return;
  network::mojom::URLLoaderFactory* loader_factory =
      system_network_context_manager->GetURLLoaderFactory();
  if (loader_factory) {
    fetcher_->Init(
        std::string(),
        net::URLRequest::REDUCE_REFERRER_GRANULARITY_ON_TRANSITION_CROSS_ORIGIN,
        net::LOAD_NORMAL);
    fetcher_->Start(loader_factory);
  }
}

// BitmapFetcherDelegate overrides.
void MediaFetcher::OnFetchComplete(const GURL& url,
                                   const SkBitmap* bitmap) {
  if (!bitmap) {
    return;
  }

  // Decode the downloaded bitmap.
  // Ownership of the image is taken by |cache_|.
  gfx::Image image = gfx::Image::CreateFrom1xBitmap(*bitmap);
  callback_.Run(&image);
}

}  // namespace brave_rewards