# Plant Growth Simulator Background

## Model Plants

Plants are modeled using "L-system" that were formulated by biologist Aristid Lidenmayer in 1968. This program is using the same system. The basic idea is to use a string rewriting mechanism. We start with a starting symbol and some rules on how the strings are rewritten and interpretted.

For example, consider the starting symbol: `L` and the rule `L -> F[+L][-L]`.  Here each `L` symbol draws a leaf (aka cone). Each `F` draws a stem. And the `+` and `-` are rotations.

## Parameters

Each time the rule is applied, the plant grows. However, it grows in a discrete and non-continuous way. We wish to animate the growth and thus we introduce the use of a parameter with each symbol. A lower value parameter means a leaf is drawn very small. Then, gradually the parameter increases and the result is that the leaf gets bigger.

## Randomization

Plants do not grow in perfect symmetry. Therefore, we also introduce some randomness to create new and unique plants every time. The challenge is that as the plant grows, the tree that represents the plant changes and if we are not careful, the plant will "pop" when that happens. 

# Implementation

## Build

The program is written in C++. To build it, you need openGL and Glut installed. Then, you can simply run:

`make`

Then, you can run:

`./plant-grow`

## Controls

You can use the numeric keypad so that "8" goes forward and "2" goes backwards. "4" and "6" turn you left and right respectively. Use "s" to reset back to start position and "k" to break. Finally, to grow the plant, type "g"

## Demo

![Demo of Growth of simple pinnate plant](docs/grow_pinnate.gif)
![Demo of Growth of a more complex plant](docs/grow_complex.gif)


# License

This code is provided under the "Creative Commons NonCommercial NoDerivatives" license. Aka [CC BY-NC-ND](https://github.com/Gibberlings3/GitHub-Templates/blob/master/License-Templates/CC-BY-NC-ND-4.0/LICENSE-CC-BY-NC-ND-4.0.md) license. 
