# What is  TeXmacs

[GNU TeXmacs](https://texmacs.org) is a free wysiwyw (what you see is what you want) editing platform with special features for scientists. The software aims to provide a unified and user friendly framework for editing structured documents with different types of content (text, graphics, mathematics, interactive content, etc.). The rendering engine uses high-quality typesetting algorithms so as to produce professionally looking documents, which can either be printed out or presented from a laptop.

The software includes a text editor with support for mathematical formulas, a small technical picture editor and a tool for making presentations from a laptop. Moreover, TeXmacs can be used as an interface for many external systems for computer algebra, numerical analysis, statistics, etc. New presentation styles can be written by the user and new features can be added to the editor using the Scheme extension language. A native spreadsheet and tools for collaborative authoring are planned for later.

## This repository

This repository is a fork of TeXMacs that aims to : 
 - Be dependent of only Qt 5 or 6
 - Be compatible with Android

To clone it, use the following command : 
```bash
git clone --recursive https://github.com/lizabelos/texmacs-android
```
Don't forget the `--recursive` option to clone the submodules.

## Compilation

### Linux, Mac OS X, Windows

Into the `texmacs-android` directory, run the following commands :
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Android

For the moment, we advise you to use Qt Creator to compile the project.

