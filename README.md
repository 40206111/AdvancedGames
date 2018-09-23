# AdvancedGames
Repository for class to use for joint Advanced Game Engineering project

[![Build status](https://ci.appveyor.com/api/projects/status/jf9e1m0ctk4fko3t/branch/master?svg=true)](https://ci.appveyor.com/project/40206111/advancedgames/branch/master)

Branching Rules
-
Before begining please branch from dev to your own branch.
Prefix any branch you make with your own name incase people name branches similarly and get confused. (e.g. Emma-dev)

Large Files (Images/Objects)
-
We are using gitLFS to store large files like images and objs. This should work automatically provided you keep those kind of files in the resources folder as this has been whitelisted.

When Commiting
-
Write your commits in this format

> Title of Commit
> 
> \- More information
> 
> \- Further information

for example

> Fixed Generation
> 
> \- Fixed math in procedural calculation
> 
> \- Added correct objects for generating

When Merging
-
DON'T merge into dev or master.
If you think your code is ready to be merged for other people to work with create a pull request.
Each pull request needs to pass continous integration and be reviewed by at least one other person.

Project not working?
-
Make sure you've retargeted the project and you're running x86.
