/*
 * Copyright © 2017-2023 Synthstrom Audible Limited
 *
 * This file is part of The Synthstrom Audible Deluge Firmware.
 *
 * The Synthstrom Audible Deluge Firmware is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SAMPLEMANAGER_H_
#define SAMPLEMANAGER_H_
#include <AudioFileVector.h>
#include "r_typedefs.h"
#include "BidirectionalLinkedList.h"
#include "definitions.h"
#include "LoadedSampleChunkPriorityQueue.h"

extern "C" {
#include "fatfs/ff.h"
}

class Sample;
class Cluster;
class SampleCache;
class String;
class SampleRecorder;

#define ALTERNATE_LOAD_DIR_NONE_SET 0
#define ALTERNATE_LOAD_DIR_NOT_FOUND 1
#define ALTERNATE_LOAD_DIR_MIGHT_EXIST 2
#define ALTERNATE_LOAD_DIR_DOES_EXIST 3


class SampleManager {
public:
	SampleManager();

	AudioFileVector audioFiles;

	void init();
	AudioFile* getAudioFileFromFilename(String* fileName, bool mayReadCard, uint8_t* error, FilePointer* filePointer, int type, bool makeWaveTableWorkAtAllCosts = false);
	Cluster* allocateLoadedSampleChunk(int type = LOADED_SAMPLE_CHUNK_SAMPLE, bool shouldAddReasons = true, void* dontStealFromThing = NULL);
	int enqueueLoadedSampleChunk(Cluster* loadedSampleChunk, uint32_t priorityRating = 0xFFFFFFFF);
	bool loadSampleChunk(Cluster* loadedSampleChunk, int minNumReasonsAfter = 0);
	void loadAnyEnqueuedSampleChunks(int maxNum = 128, bool mayProcessUserActionsBetween = false);
	void addReasonToLoadedSampleChunk(Cluster* loadedSampleChunk);
	void removeReasonFromLoadedSampleChunk(Cluster* loadedSampleChunk, char const* errorCode);
	void testQueue();

	bool ensureEnoughMemoryForOneMoreAudioFile();

	void slowRoutine();
	void deallocateLoadedSampleChunk(Cluster* loadedSampleChunk);
	int setupAlternateAudioFilePath(String* newPath, int dirPathLength, String* oldPath);
	int setupAlternateAudioFileDir(String* newPath, char const* rootDir, String* songFilenameWithoutExtension);
	bool loadingQueueHasAnyLowestPriorityElements();
	int getUnusedAudioRecordingFilePath(String* filePath, String* tempFilePathForRecording, int folderID, uint32_t* getNumber);
	void deleteAnyTempRecordedSamplesFromMemory();
	void deleteUnusedAudioFileFromMemory(AudioFile* audioFile, int i);
	void deleteUnusedAudioFileFromMemoryIndexUnknown(AudioFile* audioFile);
	bool tryToDeleteAudioFileFromMemoryIfItExists(char const* filePath);

	void thingBeginningLoading(int newThingType);
	void thingFinishedLoading();

	LoadedSampleChunkPriorityQueue loadingQueue;

	uint32_t clusterSize;
	uint32_t clusterSizeAtBoot;
	int clusterSizeMagnitude;

	uint32_t loadedSampleChunkSize;

	bool cardEjected;
	bool cardDisabled;

	Cluster* loadedSampleChunkBeingLoaded;
	int minNumReasonsForLoadedSampleChunkBeingLoaded; // Only valid when loadedSampleChunkBeingLoaded is set. And this exists for bug hunting only.

	String alternateAudioFileLoadPath;
	uint8_t alternateLoadDirStatus;
	uint8_t thingTypeBeingLoaded;
	DIR alternateLoadDir;

	int32_t highestUsedAudioRecordingNumber[NUM_AUDIO_RECORDING_FOLDERS];
	bool highestUsedAudioRecordingNumberNeedsReChecking[NUM_AUDIO_RECORDING_FOLDERS];

private:
	int advanceChunksIfNecessary(int* byteIndexWithinChunk, Cluster** currentChunk, uint32_t* currentCluster, Sample* sample);
	void setClusterSize(uint32_t newSize);
	void cardReinserted();
	int readBytes(char* buffer, int num, int* byteIndexWithinChunk, Cluster** currentChunk, uint32_t* currentChunkIndex, uint32_t fileSize, Sample* sample);
	int loadAiff(Sample* newSample, uint32_t fileSize, Cluster** currentChunk, uint32_t* currentChunkIndex);
	int loadWav(Sample* newSample, uint32_t fileSize, Cluster** currentChunk, uint32_t* currentChunkIndex);

};

extern SampleManager sampleManager;

#endif /* SAMPLEMANAGER_H_ */