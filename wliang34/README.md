# CSE 13S, Winter 2023

## Assignment directories

All of the files for a particular assignment *must* be committed in the appropriate directory. Files in the wrong assignment's directory or in the root directory of the repository will not be graded.

You might not need all of the directories in the repository. It's OK to have blank directories for assignments you haven't done yet (or might never be assigned).

## "Extra" files in the root directory of your repository

The following files *must* remain in the root directory of your repository, and must not be modified:

- `.gitignore`: This file limits warnings about files that exist on your computer but are *not* in the repository. This means that `git status` won't list `.o` files and other files that shouldn't be in your repository, which is good since you *don't* want to add them.
- `README.md`
- `.gitlab-ci.yml`

A note about `.gitlab-ci.yml`: this is the file that controls automated requirements checking and testing. We've made this file read-only, so you can't "accidentally" modify it. The file contains references to secrets provided by GitLab, allowing us to run tests without revealing them. ***DO NOT ATTEMPT TO MODIFY THIS FILE TO GAIN THESE SECRETS!*** Doing so will be considered a [Level 2 academic misconduct violation](https://ue.ucsc.edu/academic-misconduct.html), resulting in a 1--3 term suspension ***for a first offense***.

If you accidentally modify any of these files, **don't commit your changes**. Instead, delete the file (_e.g._, `rm .gitlab-ci.yml`) and then recover the last committed version with, _e.g._, `git checkout .gitlab-ci.yml`. As long as you don't commit (and push) a changed version of the file, you're fine.

Even if you *do* push a changed version of the file, we'll consider the type of changes in deciding on whether to charge you with academic misconduct. If it's clearly an accident, we won't do anything. But woe unto you if you try *anything* deliberate, which would be a clear and intentional violation of Sections 102.011, 102.014, 102.015, and 102.05 of the [UCSC Policy on Student Conduct and Discipline](https://deanofstudents.ucsc.edu/student-conduct/student-handbook/100.003.pdf).
