/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_BRAVE_REWARDS_MEDIA_FETCHER_
#define BRAVE_BROWSER_BRAVE_REWARDS_MEDIA_FETCHER_

#include <stddef.h>

#include "base/callback_forward.h"
#include "chrome/browser/bitmap_fetcher/bitmap_fetcher.h"

namespace gfx {
  class Image;
}

namespace brave_rewards {

class MediaFetcher : public BitmapFetcherDelegate {
  public:
    using FetchCompleteCallback = base::Callback<void(const gfx::Image*)>;

    MediaFetcher(GURL url,
        const FetchCompleteCallback& callback);
    ~MediaFetcher() override;

    void Start();

    // BitmapFetcherDelegate:
    void OnFetchComplete(const GURL& url, const SkBitmap* bitmap) override;

  private:
    // Downloads the avatar image from a url.
    std::unique_ptr<BitmapFetcher> fetcher_;
    FetchCompleteCallback callback_;
};

}  // namespace brave_rewards

#endif //BRAVE_BROWSER_BRAVE_REWARDS_MEDIA_FETCHER_
