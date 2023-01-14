# Chromatic-Tuner

Lab 3b: The Chromatic Tuner

By: Benjamin Cruttenden

Partner: Jackson Newman


# **Purpose and Goals:** 

The purpose of this lab is to build a chromatic tuner.

The tuner will have a:



1. Detectable frequency range of 65Hz - 4200Hz with an error accuracy up to 5 - 10 cents. 
2. Clear main page showing, the current frequency, set octave, nearest note, and the error in cents to that nearest note along with an error bar showing the error as well. 
3. Settings page, allowing the user to change the octave, change the tune of the A note, and open a debug screen, showing a histogram of the FFT. 
4. Flawless execution, no freezing, artifacts, or bugs, no matter the input received. 
5. intuitive and readable UI, so any user can operate the machine, read the given outputs, change the settings, and is happy with the function of the tuner.

# **Methodology:**

There are two sets of tasks needed to be completed for the whole system to work. These sets are Hardware Tasks and Software Tasks.

**First, hardware tasks include:**



1. Taking input frequencies in through the FPGA’s onboard microphone.
2. Taking input twist controls through an external rotary encoder.
3. Taking input switch controls through the FPGA’s onboard buttons.
4. Giving output data through an external LCD by drawing and clearing the data.

**Secondly, software tasks include:**



1. Performing an FFT on the input frequencies that is both fast and accurate to get input Hz
2. Building a real-time system to handle the different screen displays and their functions (main tuning page, settings page, debug screen, octave, and a4 tune adjustment display)
3. Taking the input Hz and calculating the nearest octave, note, and the error in cents to it.
4. Customization, allows octaves to be set for extra accuracy and a4’s tune to be changed.
5. Building a Histogram of all input frequencies for debugging.

# **Assumptions:**  
After testing for proper operation of the LCD, rotary encoder, FPGA, and its onboard microphone and buttons it is assumed that the processing power of the FPGA is capable of performing all outlined tasks accurately enough to pass all guidelines discussed in the purpose and that it can also be quick enough to feel responsive to a user, assuming fully optimized code. As for software, assuming correct operation of the given LCD, QT, FONT, and MATH functions.

# **Observations:**
This project is great as it requires proper implementation of all the designs in the past labs. So, a lot of the framework needed has already been completed in past labs. We already have the correct Vivado block design after Lab 3A and will just be using that for the tuner. It already has GPIO set up for buttons, the rotary encoder, the LCD, and the microphone. Combining this with the interrupt initiation done in past labs gets all the hardware set up.

As for the systems software, my plan for it will be to build it all based on a hierarchical QT state machine. Each required screen will have its own state handler and then will have a case for each of the possible user inputs and one more case for when no inputs are given. For my machine, the only user inputs will be the rotary encoder twist and all 5 of the onboard buttons so the number of different cases should not be high. These cases will call a lot more code that we have used in previous labs, such as code for drawing different screens, values, and an error bar on the LCD. Also, it will call the FFT function which can be edited to only look for frequencies in a certain octave hopefully making it faster to get some more accuracy from it. As for new code needed a lot of data will be changed and stored within the action of the state machine including the octave, a4 tune, found the frequency, nearest note, error, and more.	

Everything running like, going between states, drawing to the LCD, and running the FFT will take time as I’ve learned in the past labs. Combining them all does worry me that I will have runtime issues either causing the system to be slow or just causing it to flat-out not run. Careful consideration must be taken when it comes to how many pixels are being drawn for every function, how accurate the FFT needs to be depending on the input frequencies and octave, and just the fact that the program needs to be efficient, there can be only very little unnecessary for loops or other functions taking up time from the core function of the tuner.

# **Design Testing:**
Our completed tuner will be tested along the rubric of the in-person demo. We will use the website [https://www.szynalski.com/tone-generator/](https://www.szynalski.com/tone-generator/) to generate different tones out of headphones placed directly on top of the microphone. We will test with multiple pairs of headphones/earbuds to assure the testing is accurate. We will go through every note at octaves 2-7, being sure to set the octave to the one being tested and record the error for each note. Our goal is 5-10 cents error and until we have that much accuracy we plan on going back into the fft code and continuing to optimize in order to get the best accuracy at a fast enough speed. We will then play a certain percentage off of notes to see if the generated error is accurate to that same degree. On top of testing every octave, we will run through of test of all a4 tunes, 420 - 460Hz to assure all work hopefully once again within the maximum error. We will test a variety of inputs, getting the state machine to go through all possible cases to check for bugs and crashes. We will through impossible inputs in and try to throw as many inputs at one time as possible, by twisting the rotary encoder, pressing all the push buttons, and having the microphone pick up frequencies and output the FFT. Lastly, we will check our debug histogram with sources online to see if it is outputting an accurate model of the input frequencies. Upon all of this working, it would seem pretty likely our system would not break under all of the demo circumstances. 

Before completing the project each state handler upon implementation will be tested, beginning with the tuning page, then the settings page, octave selection, a4 tune, then the debug histogram. With each section possibly taking up more and more time it’s likely even though our FFT code ran in 19ms in Lab 3A, we will still need to optimize it and make it more accurate along the road, and this will just be tested based on the output Frequencies error given.

# **Results Pictures:**

<img width="926" alt="Screen Shot 2023-01-14 at 11 41 23 AM" src="https://user-images.githubusercontent.com/122656360/212493317-b8d26547-a0a4-48c7-830e-335fdf7ada0a.png">

# **Metric results:**

Our goals were to have the correct note and frequency being played while having an efficient UI/UX design. Our UI/UX goals were to make the navigation quick and consistent. We accomplished this by using consistent controls for each button and using the rotary encoder to select the numbers of each setting. This was a success and we made a very intuitive and fast experience for the user. Our screens transition faster than our lab2 due to the simpler background and the buttons/rotary encoder have no bouncing issues, things that we learned from the past labs. 

<img width="1003" alt="Screen Shot 2023-01-14 at 11 48 45 AM" src="https://user-images.githubusercontent.com/122656360/212493617-220667af-29c6-475b-8d99-8510e6c92164.png">

Looking at figure 1, we can see our results based on testing every note in each octave. As you can see, we made a sufficiently accurate design for about 85% of the total amount of notes. We accomplished this through trial and error and also our knowledge of how FFT is calculated. Our design correctly identified many of the on note notes, but is understandably less accurate specifically when we change the base frequency to 420 Hz. This is because it requires a greater accuracy especially in the lower octaves where our accuracy is not the best. Since it decreases our performance, we tried increasing our accuracy which led to varying results.



# **Design Structure:**
Our design was built off of a QT state machine running off interrupt-driven cases. With every state handler handling a different screen of the tuner. 

The program starts at a set octave of 4 and a4 tune of 440 Hz, these values are then used to adjust the fft function values such as sample size and sample frequencies, It then initializes input interrupts, the state machine, and the fsl values table by reading in 4096 samples. 

The state machine will automatically enter the main tuning page. The main tuning state handler has an IDLE case which when no interrupts are called will update the tuning screen. It does this by setting the sampling frequency based on the set octave. Then, calculate the frequency using the FFT function with a lookup table optimization (planning on implementing optimization based on the octave Hz ranges but haven’t gotten to it as of 12/5). Then, using the calculated frequency to find the nearest note with an error in cents, and then filling the samples up again in order to be ready if the IDLE stage is next. Upon getting this data the IDLE stage will then draw all the values on the screen using the LCD updating the note, frequency, and error bar/readout. 

This will continue to run and take in samples through the microphone until an interrupt BTN is pressed to send the system to the settings page. The state will be switched to a new handler and the condition to read in samples will be violated so the machine will pause mic inputs. The new screen will be drawn outlining instructions for the user to continue. Interrupts are available to change octave and a4 tune, which have been set up previously to update the sample reading, fft, and findnote functions to correspond to the new values. 

Lastly, from the settings page, an interrupt is available to debug the system. This will start up the reading in of samples again but instead of calculating frequency and note, it instead will draw all the input samples into a histogram. This is done by taking the samples and bin size from the fft function and just drawing the number of samples in each bin at a log scale for 4096 samples. It will then stop. At that point, the user has the option to restart the histogram and clear it for it to take in a different 4096 samples, or to go back to the main tuning page. This system is closed and has every possible input accounted for to minimize bugs and crashes. In a perfect scenario the system will never stop running, just be in a constant loop of updating its values.

# **Roadblocks:**
A big issue that took time to overcome was timing everything. When on the tuner page, our display is updated multiple times a second. This means that the values for frequency, note, and error all have to be updated and then drawn on the display every time. On testing many times the note or frequency would draw an invalid character. Primarily with note instead of drawing out A# what would happen is it would try to draw the value saved in our note variable before the frequency calculation and findnote were finished this would cause it to draw out random memory. This was happening even though we have it built to run the findnote just before drawing the note. It all came down to timing and actually reminds me of what happens in a pipeline error. To fix this we didn’t have to put in any buffers but instead just had to spread out the data fetching functions and the data drawing functions as much as possible. So in the IDLE state the first thing done is the samples (which have been grabbed from the previous IDLE state) are used to calculate frequency then the frequency is used to find notes. Then the buffer put in place is the pipeline grabbing 4096 samples for the next IDLE stage. Only after that will it clear the display and draw the new frequency, note, and error. Another issue was crashes. Trying to make the FFTs work as accurate as possible we wanted to fit as many samples in as possible. To many samples would result in a machine that would just crash. It’s our best guess that it would just take to long and the state machine wouldn’t wait for it but thats still up for debate. What we do know is that setting the octave to a higher value, we needed to average out the 4096 samples into 2048 or even 512 samples so it would work properly, this fixed our issue of crashing but took our accuracy and threw it in the trash can. This is a very important issue that we want to fix but may be limited by our design.

# **Shortcomings:**
The largest problem with my tuner is its accuracy. It has all the necessary functionality, looks great, and has a smooth UI. It is just not as accurate as I would’ve liked. It is at the point where it can correctly identify just about every single note, only getting a few in octave 2 wrong. To accurately tune a guitar it’s best to be less than 3 cents of every note as at that point our ears can’t quite distinguish the difference. As for this project our goal was to be less than 5-10 cents, but in reality we are having errors from 0 -  40 cents on the notes that are correctly detected. Our fft worked by taking in 4096 samples and averaging them down depending on the octave. The lower the octave the less it would be averaged down as the lower the octave the more accuracy was needed. This may have been our downfall as at higher octaves the accuracy is almost equally as bad as at lower octaves which should not be the case. Trying to lower the average samples at higher octaves only made the error worse. What makes it even worse is that it varies so much even within each octave. Going through octave 5 per se we will have half of the notes being within 5 cents error but then the other half will be between 10 - 40 cents error. So for almost all cases, the tuner is neither accurate nor precise. We are aware of error caused by noise and a bottlenecked mic but I am sure there are optimizations we could make to the accuracy, we just were not capable of figuring them out. A second unresolved issue was our histogram. We were never able to grab the correct data in order to draw it. We had the max frequency, bin spacing, but just couldn’t actually get the samples that was supposed to be in the bins. Due to this reason our histogram basically never worked and would fail.

# **Design / Aesthetics:**
Design inspiration was taken from some of the top-rated IOS app guitar tuners. It seemed that the most liked design was clean and minimalistic, with a dark solid background color and clear crisp white writing showing only the necessary data and nothing else. So we decided on colors to keep it simple with a dark gray background that has a black border. On the background we will use white writing in primarily Big Font For very important values they were printed in sevenseg font although this font is huge so it was only used for the most significant piece of data printed on each screen. The information from octave, note, frequency, and error are all evenly spaced apart and will be the only information found on the tuner page to avoid any distractions. Values and words were kept at a constant position even when they changed from 1 - 2 digits as this made them much easier to read when they are quickly changing. The error bar is drawn in the color red to stand out from the background and has a yellow line directly in the middle to discern between a positive and negative error. The settings page took a form over function approach. Each setting is put next to its correlating interrupt, for example, “Top: Octave” reads as the top button on the FPGA will send you to the Octave adjustment screen. While it is not the prettiest, it gets the job done and makes it very clear to the user, what button does what. The design slightly slows down the program as I am drawing over most of the screen forcing me to draw more pixels than if I made everything smaller. The performance hit was deemed worth it as it is still plenty fast enough and is a much more usable machine with bigger fonts and value outputs.

# **Conclusion:**
The project was one of perseverance. Between hundreds of crashes, thousands of lines of code, and dozens of hours of work. In the end, the project was great. It had a good-looking fluid UI. It was able to accurately show the correct note in 99% of situations, wasn’t quite able to meet the desired accuracy but was as good as we could get it is within 20 cents for 90% of notes. We were missing out on a histogram but were capable of setting the octave and tune of A4. It had a tuning screen, setting screen, A4 tuning screen, octave screen, and lastly the debug screen. Our main tuner showed the input frequency, closest note, octave, error in cents, and had an error bar to visualize it. It did all this in a large clear font making it very readable. It had a great contrast with white writing on a dark grey background with a black border. It was great, the project was great, I was able to build a machine that I was happy with and that’s that. 

**	**
