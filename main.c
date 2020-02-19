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

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "./api/Vokaturi.h"   /* or wherever you put the header file */

#define GREEN  "\x1B[32m"
#define BLUE  "\x1B[34m"

extern VokaturiEmotionProbabilities extractEmotionProbabilities(const char * fileName);

// recursive definition to list every files in folder and subfolders
void listFilesRecursively(char *path);
bool isAudioFileWithAnger(char *path);
double getDominantEmotion(VokaturiEmotionProbabilities emotionProbabilities);
void analyzeEmotionProbabilities(VokaturiEmotionProbabilities emotionProbabilities, char *localPath);

// declare inner variables to compute accuracy of emotion engine
int angerCorrectlyDetected = 0;
int angerNotDetected = 0;
int falseNegativeAnger = 0;
int falseNegativeAngerWithEnoughAnger = 0;
int falsePositiveAnger = 0;
int audioFilesWithAnger = 0;
int audioFilesWithoutAnger = 0;
int invalidFiles = 0;

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
int main ()
{
    // start measure of time spent to check accuracy.
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    // populate our inner variables using solutions provided by our emotion engine
    listFilesRecursively("./data");

    // display accuracy results
    // we will log result of analysis. Is our emotion analysis engine accurate for anger ?

    // Correct answers in percentile
    int correctAngerAnswers = angerCorrectlyDetected * 100 / audioFilesWithAnger;
    int falseNegativeAnswers = falseNegativeAnger * 100 / audioFilesWithAnger;
    int falseNegativeWithEnoughAngerAnswers = falseNegativeAngerWithEnoughAnger * 100 / falseNegativeAnger;
    int correctOtherEmotionsAnswer =
        angerNotDetected * 100 / audioFilesWithoutAnger;
    int falsePositiveAnswers = falsePositiveAnger * 100 / audioFilesWithoutAnger;

    printf ("Audio files analyzed =  %d files.\n", audioFilesWithAnger +
            audioFilesWithoutAnger + invalidFiles);
    printf ("Audio files with expected anger analyzed =   %d files.\n", audioFilesWithAnger);
    printf ("Audio files with other emotions analyzed =  %d files.\n",
            audioFilesWithoutAnger);
    printf ("Invalid files = %d files.\n", invalidFiles);

    printf ("Correct anger answers =  %d %% \n", correctAngerAnswers);
    printf ("False negative answers =  %d %% \n", falseNegativeAnswers);
    printf ("False negative answers with enough anger (more than 10 %%) =  %d %% \n", falseNegativeWithEnoughAngerAnswers);
    printf ("Correct other emotions answers =  %d %% \n", correctOtherEmotionsAnswer);
    printf ("False positive answers =  %d %% \n", falsePositiveAnswers);

    // display time spent
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("It took %f seconds to execute \n", cpu_time_used);

    // default return value
    return(0);
}

/**
 * Lists all files and sub-directories recursively
 * considering path as base path.
 */
void listFilesRecursively(char *basePath)
{
    struct dirent *dp; // for the directory entries
    DIR *dir = opendir(basePath); // open the path provided

    // Unable to open directory stream
    if (!dir)
    {
        return;
    }

    while ((dp = readdir(dir)) != NULL) // if we were able to read something from the directory
    {
        char * localPath = dp->d_name;
        if (strcmp(localPath, ".") != 0 && strcmp(localPath, "..") != 0)
        {
            // Construct new path from our base path
            char path[1000]; // to store full path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, localPath);
            //printf("%s\n", path);
            if (dp->d_type == DT_DIR) // if it is a directory
            {
                // start recursion
                listFilesRecursively(path);
            }
            else // if it is a file
            {
                // analyze emotions probabilities.
                VokaturiEmotionProbabilities emotionProbabilities = extractEmotionProbabilities(path);
                if (emotionProbabilities.valid == 1) {
                    analyzeEmotionProbabilities(emotionProbabilities, localPath);
                } else {
                    invalidFiles++;
                }
            }
        }
    }

    closedir(dir); // finally close the directory
}

/**
 * File naming convention
 *
 * Each of the 7356 RAVDESS files has a unique filename. The filename consists of a 7-part
 * numerical  identifier (e.g., 02-01-06-01-02-01-12.mp4). These identifiers define the stimulus
 * characteristics:
 *
 * Filename identifiers
 *
 * Modality (01 = full-AV, 02 = video-only, 03 = audio-only). Vocal channel (01 = speech, 02 =
 * song). Emotion (01 = neutral, 02 = calm, 03 = happy, 04 = sad, 05 = angry, 06 = fearful, 07 =
 * disgust, 08 = surprised). Emotional intensity (01 = normal, 02 = strong). NOTE: There is no
 * strong intensity for the 'neutral' emotion. Statement (01 = "Kids are talking by the door",
 * 02 = "Dogs are sitting by the door"). Repetition (01 = 1st repetition, 02 = 2nd repetition).
 * Actor (01 to 24. Odd numbered actors are male, even numbered actors are female).
 *
 * Filename example: 02-01-06-01-02-01-12.mp4
 *
 * Video-only (02) Speech (01) Fearful (06) Normal intensity (01) Statement "dogs" (02) 1st
 * Repetition (01) 12th Actor (12) Female, as the actor ID number is even.
 *
 * @param filename audio file name.
 * @return true if filename contains 05 (=angry) in value of third identifier. False otherwise.
 */
bool isAudioFileWithAnger(char *path)
{
    bool result = false;
    int length = strlen(path);
    int ravdessFilenameLength = 24;
    if (ravdessFilenameLength == length)
    {
        // check value of character at 8th position
        char emotion[1] = {path[7]}; // don't forget index starts at 0
        char *anger = "5";
        if (strcmp(anger, emotion) == 0)
        {
            // we have a file with anger as main emotion.
            result = true;
        }
    }
    return result;
}

double getDominantEmotion(VokaturiEmotionProbabilities emotionProbabilities)
{
    double result = emotionProbabilities.anger;
    if (result < emotionProbabilities.happiness) {
        result = emotionProbabilities.happiness;
    }
    if (result < emotionProbabilities.fear) {
        result = emotionProbabilities.fear;
    }
    if (result < emotionProbabilities.neutrality) {
        result = emotionProbabilities.neutrality;
    }
    if (result < emotionProbabilities.sadness) {
        result = emotionProbabilities.sadness;
    }
    return result;
}

void analyzeEmotionProbabilities(VokaturiEmotionProbabilities emotionProbabilities, char *localPath) {
    if (isAudioFileWithAnger(localPath))
    {
        audioFilesWithAnger++;
        // check that anger is the dominant emotion
        double dominantEmotion = getDominantEmotion(emotionProbabilities);
        if (dominantEmotion == emotionProbabilities.anger) {
            angerCorrectlyDetected++;
        } else {
            falseNegativeAnger++;
            // and in false negative answers
            // check if anger is at least detected to 50%, so 0.5 value
            if (emotionProbabilities.anger > 0.1) {
                falseNegativeAngerWithEnoughAnger++;
            }
        }
    }
    else
    {
        audioFilesWithoutAnger++;
        // check that anger is not the dominant emotion
        double dominantEmotion = getDominantEmotion(emotionProbabilities);
        if (dominantEmotion == emotionProbabilities.anger) {
            falsePositiveAnger++;
        } else {
            angerNotDetected++;
        }
    }
}
