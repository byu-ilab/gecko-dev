/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MOZILLA_SVGCONTEXT_H_
#define MOZILLA_SVGCONTEXT_H_

#include "mozilla/Maybe.h"
#include "mozilla/SVGContextPaint.h"
#include "SVGPreserveAspectRatio.h"
#include "Units.h"

class nsIFrame;

namespace mozilla {

// SVG image-specific rendering context. For imgIContainer::Draw.
// Used to pass information such as
//  - viewport information from CSS, and
//  - overridden attributes from an SVG <image> element
// to the image's internal SVG document when it's drawn.
class SVGImageContext
{
public:
  SVGImageContext()
    : mGlobalOpacity(1.0)
    , mIsPaintingSVGImageElement(false)
  { }

  /**
   * Currently it seems that the aViewportSize parameter ends up being used
   * for different things by different pieces of code, and probably in some
   * cases being used incorrectly (specifically in the case of pixel snapping
   * under the nsLayoutUtils::Draw*Image() methods).  An unfortunate result of
   * the messy code is that aViewportSize is currently a Maybe<T> since it
   * is difficult to create a utility function that consumers can use up
   * front to get the "correct" viewport size (i.e. which for compatibility
   * with the current code (bugs and all) would mean the size including all
   * the snapping and resizing magic that happens in various places under the
   * nsLayoutUtils::Draw*Image() methods on the way to DrawImageInternal
   * creating |fallbackContext|).  Using Maybe<T> allows code to pass Nothing()
   * in order to get the size that's created for |fallbackContext|.  At some
   * point we need to clean this code up, make our abstractions clear, create
   * that utility and stop using Maybe for this parameter.
   *
   * Note: 'aIsPaintingSVGImageElement' should be used to indicate whether
   * the SVG image in question is being painted for an SVG <image> element.
   */
  explicit SVGImageContext(const Maybe<CSSIntSize>& aViewportSize,
                           const Maybe<SVGPreserveAspectRatio>& aPreserveAspectRatio = Nothing(),
                           gfxFloat aOpacity = 1.0,
                           bool aIsPaintingSVGImageElement = false)
    : mViewportSize(aViewportSize)
    , mPreserveAspectRatio(aPreserveAspectRatio)
    , mGlobalOpacity(aOpacity)
    , mIsPaintingSVGImageElement(aIsPaintingSVGImageElement)
  { }

  bool MaybeStoreContextPaint(nsIFrame* aFromFrame);

  const Maybe<CSSIntSize>& GetViewportSize() const {
    return mViewportSize;
  }

  void SetViewportSize(const Maybe<CSSIntSize>& aSize) {
    mViewportSize = aSize;
  }

  const Maybe<SVGPreserveAspectRatio>& GetPreserveAspectRatio() const {
    return mPreserveAspectRatio;
  }

  void SetPreserveAspectRatio(const Maybe<SVGPreserveAspectRatio>& aPAR) {
    mPreserveAspectRatio = aPAR;
  }

  gfxFloat GetGlobalOpacity() const {
    return mGlobalOpacity;
  }

  const SVGContextPaint* GetContextPaint() const {
    return mContextPaint.get();
  }

  bool IsPaintingForSVGImageElement() const {
    return mIsPaintingSVGImageElement;
  }

  bool operator==(const SVGImageContext& aOther) const {
    return mViewportSize == aOther.mViewportSize &&
           mPreserveAspectRatio == aOther.mPreserveAspectRatio &&
           mGlobalOpacity == aOther.mGlobalOpacity &&
           mIsPaintingSVGImageElement == aOther.mIsPaintingSVGImageElement;
  }

  bool operator!=(const SVGImageContext& aOther) const {
    return !(*this == aOther);
  }

  uint32_t Hash() const {
    uint32_t hash = 0;
    if (mContextPaint) {
      hash = HashGeneric(hash, mContextPaint->Hash());
    }
    return HashGeneric(hash,
                       mViewportSize.map(HashSize).valueOr(0),
                       mPreserveAspectRatio.map(HashPAR).valueOr(0),
                       HashBytes(&mGlobalOpacity, sizeof(mGlobalOpacity)),
                       mIsPaintingSVGImageElement);
  }

private:
  static uint32_t HashSize(const CSSIntSize& aSize) {
    return HashGeneric(aSize.width, aSize.height);
  }
  static uint32_t HashPAR(const SVGPreserveAspectRatio& aPAR) {
    return aPAR.Hash();
  }

  // NOTE: When adding new member-vars, remember to update Hash() & operator==.
  RefPtr<SVGContextPaint>       mContextPaint;
  Maybe<CSSIntSize>             mViewportSize;
  Maybe<SVGPreserveAspectRatio> mPreserveAspectRatio;
  gfxFloat                      mGlobalOpacity;
  bool                          mIsPaintingSVGImageElement;
};

} // namespace mozilla

#endif // MOZILLA_SVGCONTEXT_H_
