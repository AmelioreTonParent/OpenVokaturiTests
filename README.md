# OpenVokaturi Tests

To check results of [OpenVokaturi engine](https://vokaturi.com/) to analyze emotions in voice samples.

I have reused the [Ryerson Audio-Visual Database of Emotional Speech and Song (RAVDESS)](https://zenodo.org/record/1188976) to qualify such engine. 

Disclaimer: in case of stereo samples, I have reused only left channel. 
And it concerns only wave files which provide a basic 16 bits PCM format.

# Numbers

- Audio files analyzed =  2453 files.
- Audio files with expected anger analyzed =   373 files.
- Audio files with other emotions analyzed =  1822 files.
- Invalid files = 258 files.
- Correct anger answers =  41 %
- False negative answers =  58 %
- False negative answers with enough anger (more than 10 %) =  29 %
- Correct other emotions answers =  65 %
- False positive answers =  34 %
- It took 65.541223 seconds to execute

# Conclusion

So in conclusion, ``our rate for correct answers is around 61%.``
And our rate for incorrect answers is around 38%.
Not so good for a professional engine, but ``good enough for our prototype !``

Note: you have noticed that correct + incorrect answers is 99%. Seems that I have rounded too much digitals in results :-)
