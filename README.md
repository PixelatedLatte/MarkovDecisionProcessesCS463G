# MarkovDecisionProcessesCS463G
A path value and policy generator with a fully viewable decision and state tree environment using a simple Markov Decision Process.

## Variations

With probability 0.15, the car will end up in reverse and go backwards from the intended direction. With probability 0.15, it will stall out and go nowhere. With probability 0.7, it goes where you send it. If it hits an edge of the grid world, it stays in its square.

## Algorithms

- Value Iteration
- Policy Iteration

## Parameters

For all algorithms, the discount rate gamma will be 0.95. For Value Iteration (VI) and Policy Iteration (PI), run each of the algorithms with time horizons of 50 and 100.

## Using MSYS2 / MinGW-w64

This project uses MSYS2 / MinGW-w64 for building and running native tools on Windows. To ensure that shells and IDEs can find `g++`, `gcc`, and `make`, add the MSYS2 MinGW-w64 toolchain `mingw64\bin` to your PATH.

### Recommended path to add:

    C:\msys64\mingw64\bin

### Temporary (current shell only):

**CMD:**
```
set PATH=%PATH%;C:\msys64\mingw64\bin
```

**PowerShell:**
```
$env:Path += ";C:\msys64\mingw64\bin"
```

### Permanent (GUI):

1. Open Start ? type "Environment variables" ? select **Edit the system environment variables**.
2. Click **Environment Variables...**.
3. Under **User variables** (or **System variables**), select `Path` ? **Edit** ? **New** ? paste `C:\msys64\mingw64\bin` ? **OK**.

### Permanent (command-line):

**CMD/PowerShell (current user):**
```
setx PATH "%PATH%;C:\msys64\mingw64\bin"
```

### Notes:

- Adding `C:\msys64\msys2.exe` to PATH allows launching the MSYS2 launcher by name, but it does not expose the compiler toolchain. Prefer adding `mingw64\bin` for compilers.
- `setx` requires opening a new shell to take effect and can truncate very long PATH values on some systems.

### Visual Studio (project-only):

- Open __Project > Properties__ ? __Debugging__ ? set __Environment__ to:

    PATH=%PATH%;C:\msys64\mingw64\bin

### Verification in a new shell:
```
where g++
g++ --version
```

This setup will ensure that you can build and run the project successfully on a Windows environment using the MSYS2 / MinGW-w64 toolchain.