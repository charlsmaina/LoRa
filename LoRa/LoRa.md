- Gaussian distribution - A probability density function with a bell shape. Used to model events that appear random. Follows a central limit theorem
- Gaussian noise - Amplitude distribution of the noise follows the Gaussian probability distribution function. White noise is the one that has a uniform power spectral density
- Additive White Gaussian Noise - Gaussian amplitude distribution plus flat Power spectral density
- Amplitude probability Distribution - It gives you a probability structure. Amplitude is on the x axis and probability density is on the y axis. It answers the question. If i ignore when i measure a signal, what is the probability that i will get a signal of a given amplitude.
- The right intuition : if you pick am amplitude window and find the area under the probability density curve within that window, that area is the probability that the signal sits within that amplitude range at any given time.


$$
P(a < x < b) = integral of  p(x) w.r.t dx with limits from a to b
$$
- PSD - Power Spectra Density - A plot of the power density on the y axis and frequency on the x axis. It is a function S(f) that when integrated over a frequency interval , gives you the actual power contained within that frequency band.
- Density is the only mathematically consistent way to talk about how a continous signal is distributed. The problem comes when. For example you take power. it is continous and distribute d between many frequencies. You can have infinitely many frequency components under which the power is distributed. So to get the power of each frequency you divide the total power divide with the infinite frequency points. You get a zero
```
-In a continous domain the concept of 'power at an exact point is simply undefined
```
 
-  So this would produce a straight line with zero amplitude which is meaningless. With density, we get the rate at which power is accumulating with respect to frequency so the points between the samples and those are the  points on the y axis.
- 