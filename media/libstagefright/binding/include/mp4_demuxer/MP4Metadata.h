/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MP4METADATA_H_
#define MP4METADATA_H_

#include "mozilla/UniquePtr.h"
#include "mp4_demuxer/DecoderData.h"
#include "mp4_demuxer/Index.h"
#include "MediaData.h"
#include "MediaInfo.h"
#include "Stream.h"
#include "mp4parse.h"

namespace mp4_demuxer
{

class MP4MetadataStagefright;
class MP4MetadataRust;

class IndiceWrapper {
public:
  virtual size_t Length() const = 0;

  // TODO: Index::Indice is from stagefright, we should use another struct once
  //       stagefrigth is removed.
  virtual bool GetIndice(size_t aIndex, Index::Indice& aIndice) const = 0;

  virtual ~IndiceWrapper() {}
};

class MP4Metadata
{
public:
  explicit MP4Metadata(Stream* aSource);
  ~MP4Metadata();

  static already_AddRefed<mozilla::MediaByteBuffer> Metadata(Stream* aSource);
  uint32_t GetNumberTracks(mozilla::TrackInfo::TrackType aType) const;
  mozilla::UniquePtr<mozilla::TrackInfo> GetTrackInfo(mozilla::TrackInfo::TrackType aType,
                                                      size_t aTrackNumber) const;
  bool CanSeek() const;

  const CryptoFile& Crypto() const;

  mozilla::UniquePtr<IndiceWrapper> GetTrackIndice(mozilla::TrackID aTrackID);

private:
  UniquePtr<MP4MetadataStagefright> mStagefright;
  UniquePtr<MP4MetadataRust> mRust;
  mutable bool mPreferRust;
  mutable bool mReportedAudioTrackTelemetry;
  mutable bool mReportedVideoTrackTelemetry;
#ifndef RELEASE_OR_BETA
  mutable bool mRustTestMode;
#endif
  bool ShouldPreferRust() const;
};

} // namespace mp4_demuxer

#endif // MP4METADATA_H_
