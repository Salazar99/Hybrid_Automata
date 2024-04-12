# CHAOS

## Project Overview:

The application is a simulator designed to define a system of concurrent hybrid automata and simulate their behavior over time. The primary objective is to generate output files in .csv format containing the evolution of variables for these hybrid automatas.

## Compilation and Build:

#### Linux:

##### Prerequisites:

- cmake (version 3.5 or newer)
    ```sudo apt-get install cmake```
- make (version 4.3 or newer)
    ```sudo apt-get install make```
- g++ compiler (version 11.0.3 or newer)
- ninja-build
    ```sudo apt-get install ninja-build```
- qt (6 or superior)
    ```https://www.qt.io/download-qt-installer-oss?hsCtaTracking=99d9dd4f-5681-48d2-b096-470725510d34%7C074ddad0-fdef-4e53-8aa8-5e8a876d6ab4```

#### Windows (to be completed):

##### Prerequisites:

- cmake (version 3.28.0)
- make (version 3.81)
- MSVC (version 14.37.32822)

#### MacOS: Not tested yet

## Getting Started:

### Linux

1. **Clone the repository:**

   ```bash
   git clone https://github.com/AldeIT/Hybrid_Automata.git
   cd Hybrid_Automata
   ```

2. **Create and prepare Build Directory**
   ```bash
   mkdir build
   cd build
   export CMAKE_GENERATOR=Ninja
   export CMAKE_BUILD_TYPE=Debug
   export QT_QMAKE_EXECUTABLE=<path-to-qt-installation-directory>/Qt/<version>/gcc_64/bin/qmake
   export CMAKE_PREFIX_PATH=<path-to-qt-installation-directory>/Qt/<version>/gcc_64
   export CMAKE_C_COMPILER=/usr/bin/gcc-11 (choose your own version)
   export CMAKE_CXX_COMPILER=/usr/bin/x86_64-linux-gnu-g++-11 (choose your own version)
   export CMAKE_CXX_FLAGS_INIT=-DQT_QML_DEBUG
   cmake -S <project-directory> -B <build-directory>
   ```
   In case of an error regarding the "QT6Dir" variable or the "QT6ConfigCmake" file you will need to run the following command and then try again:
    ```
    sudo apt-get install build-essential libgl1-mesa-dev
    ```
3. **Build and Run**
    ```bash
   cmake --build <build-directory> --target all
   ./main
   ```
   

### Windows (to be completed)

1. **Clone the repository:**

   ```bash
   git clone https://github.com/AldeIT/Hybrid_Automata.git
   cd Hybrid_Automata
   ```

2. **Create Build Directory and Compile**
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build ./ --config Debug --target ALL_BUILD -j 10
   cd Debug
   ./main.exe
   ```

#### Guide for Instructions

When you need to define instructions to execute inside a node, you need to follow these rules:

1. **Simple Assignment**<br>
   ```plaintext
   x = 5; #no particular rules just remember the ; at the end
   ```
2. **Math Operation**<br>
   ```plaintext
   x = 5 + a; #no particular rules just remember the ; at the end and make sure that the other variable exists
   ```
3. **Differential Equation**<br>
   ```plaintext
   x' = 5*x + a - 0.02; #the derivative is written with the ' after the variable and with no brackets such as "x'(t)", always remember the ; at the end
   ```

#### Guide for Transitions

Delimit with parenthesis and if you need to use the and/or operator you can write conditions like this:

```plaintext
(x<19) & (x+5>=4) #and operator
(x<19) | (x+5>=4) #or operator
```

Some more examples:

```plaintext
((x<19) & (x+5>=4)) & (x+x<6)
(((x<19) | (x+5>=4)) & (2*x>=5)) | (x+1==0)
```

For a complete example with multiple automatas and various types of instructions and conditions you can read the [Watertanks Example](watertanks.json)

#### Output
You can find the csv output file in the main directory under the name "export.csv".
By default you will also find a "output.json" that contains the automata you have just run, be aware at every execution both the "output.json" and "export.csv" will be overwritten.
And for every execution you'll have to close the two files before hitting the "Run" button.

## Stack

Written in C++

## Authors

Aldegheri Alessandro, Cipriani Michele, Venturi Davide, Vilotto Tommaso
