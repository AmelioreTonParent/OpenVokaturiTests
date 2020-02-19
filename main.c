/**
 * To check OpenVokaturi engine using ravdess audio files.
 * 
 * We are able to download files from https://zenodo.org/record/1188976/files/Audio_Song_Actors_01-24.zip?download=1
 * and https://zenodo.org/record/1188976/files/Audio_Speech_Actors_01-24.zip?download=1.
 * 
 * See details from https://zenodo.org/record/1188976.
 * 
 * Here is the naming convention:
 * 
 * File naming convention
 * 
 * Each of the 7356 RAVDESS files has a unique filename. The filename consists of a 7-part numerical
 * identifier (e.g., 02-01-06-01-02-01-12.mp4). These identifiers define the stimulus
 * characteristics:
 * 
 * Filename identifiers
 * 
 * Modality (01 = full-AV, 02 = video-only, 03 = audio-only). 
 * Vocal channel (01 = speech, 02 = song). 
 * Emotion (01 = neutral, 02 = calm, 03 = happy, 04 = sad, 05 = angry, 06 = fearful, 07 = disgust, 08 = surprised). 
 * Emotional intensity (01 = normal, 02 = strong). 
 * NOTE: There is no strong intensity for the 'neutral' emotion. 
 * Statement (01 = "Kids are talking by the door", 02 = "Dogs are sitting by the door"). 
 * Repetition (01 = 1st repetition, 02 = 2nd repetition). 
 * Actor (01 to 24. Odd numbered actors are male, even numbered actors are female).
 * 
 * 
 * Filename example: 02-01-06-01-02-01-12.mp4
 * 
 * Video-only (02) Speech (01) Fearful (06) Normal intensity (01) Statement "dogs" (02) 1st
 * Repetition (01) 12th Actor (12) Female, as the actor ID number is even.
 */

#include <math.h>
#include "VokaWav.h"

/**
 * Here, we will iterate over every audio file (*.wav) found recursively in provided folder. The
 * only thing we would like to check right now is anger. But it might be extended to other
 * emotions.
 * 
 * We have to check audio files which relates positively to anger, but also those which relates
 * negatively to anger. So we have two use cases, for audio files with anger, it might be not
 * detected (false negative). And for audio files without anger, it might be detected (false
 * negative).
 * 
 * And the only way to get the emotion, is to find third number in file name and check its
 * value: 05 = angry !
 */
int main (int argc, const char * argv[]) {
	if (argc < 2) {
		printf ("**********\nWAV files analyzed with:\n%s\n**********\n",
			Vokaturi_versionAndLicense ());
		printf ("Usage: %s [soundfilename.wav ...]\n", argv [0]);
		exit (1);
	}
	for (int ifile = 1; ifile < argc; ifile ++) {
		const char *fileName = argv [ifile];

		printf ("\nEmotion analysis of WAV file %s:\n", fileName);
		VokaWav wavFile;
		VokaWav_open (fileName, & wavFile);
		if (! VokaWav_valid (& wavFile)) {
			fprintf (stderr, "Error: WAV file not analyzed.\n");
			exit (1);
		}

		VokaturiVoice voice = VokaturiVoice_create (
			wavFile.samplingFrequency,
			wavFile.numberOfSamples
		);

		VokaWav_fillVoice (& wavFile, voice,
			0,   // the only or left channel
			0,   // starting from the first sample
			wavFile.numberOfSamples   // all samples
		);

		VokaturiQuality quality;
		VokaturiEmotionProbabilities emotionProbabilities;
		VokaturiVoice_extract (voice, & quality, & emotionProbabilities);

		if (quality.valid) {
			printf ("Neutrality %f\n", emotionProbabilities.neutrality);
			printf ("Happiness %f\n", emotionProbabilities.happiness);
			printf ("Sadness %f\n", emotionProbabilities.sadness);
			printf ("Anger %f\n", emotionProbabilities.anger);
			printf ("Fear %f\n", emotionProbabilities.fear);
		} else {
			printf ("Not enough sonorancy to determine emotions\n");
		}

		VokaturiVoice_destroy (voice);
		VokaWav_clear (& wavFile);
	}
}
