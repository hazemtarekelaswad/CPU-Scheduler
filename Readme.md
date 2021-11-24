# How to build and run the project
| Command | Description |
| ------- | ----------- |
| `make` | compiles all files included in the **Makefile** |
| `make run` | runs **process_generator.c** |
| `make generate` | generates random test cases in a file named **processes.txt** |
---
# Commonly used git commands:
### Install git
`$ sudo apt install git-all`
### Clone the repo from github to your local machine
`git clone <url>`
### Add and commit changes
```
git add .
git commit -m "commit msg"
```
### Push your changes
`git push origin <branch name>`
### Pull the changes
`git pull`
### Create new branch
`git branch <branch name>`
### Switch between branches
`git checkout <branch name>`

### Notes:
1. If you added a file to your project add it to the **build** section in the section in the Makefile.
2. Always start the line with a **tab** in Makefile, it is its syntax.
3. **headers.h** contains clk functions, it should be included anywhere the clock functions are used.
