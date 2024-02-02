# Still working on Project Name

## Project Overview:

The application is a simulator designed to define a system of concurrent hybrid automata and simulate their behavior over time. The primary objective is to generate output files in .csv format containing the evolution of variables for these hybrid automatas.

## Compilation and Build:

#### Linux:

##### Prerequisites:

- cmake (version 3.5)
- make (version 4.3)
- g++ compiler (version 12.3.0)

#### Windows:

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

2. **Create Build Directory and Compile**
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ./main
   ```

### Windows

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

## JSON Tutorial

Follow the next example to define Systems in .json standard:

```bash
 {
     "system": {                                                 #everything needs to be inside the system object
         "global": {                                             #global variables
             "delta": "0.1",                                     #sampling interval
             "finaltime": "5"                                    #total time
         },
         "automata": [                                           #inside you can define all of your automatas
             {
                 "name": "A",                                    #the automata's name
                 "node": [                                       #inside you can define all the nodes
                     {                                           #for each node
                         "name": "Off",                          #its name
                         "description": "x >= 18",               #its description
                         "instructions": "x' = -0.1*x;",         #the instructions to execute (divided by ;)
                         "flag": "start",                        #start if starting node, none otherwise
                         "transitions": [                        #transitions to other nodes
                             {
                                 "to": "On",                     #the destination's name
                                 "condition": "(x<19)"           #the condition to check (use paranthesis to delimit)
                             }
                         ]
                     },
                     {
                         "name": "On",
                         "description": "x <= 22",
                         "instructions": "x' = 5-0.1*x;",
                         "flag": "none",
                         "transitions": [
                             {
                                 "to": "Off",
                                 "condition": "(x>21)"
                             }
                         ]
                     }
                 ],
                 "variables": [                                  #inside you need to define starting values for all the variables
                     {
                         "name": "x",
                         "value": "20"
                     }
                 ]
             }
         ]
     }
 }
```

#### Guide for Instructions

When you need to define instructions to execute inside a node, you need to follow these rules:

1. **Simple Assignment**
   x = 5; #no particular rules just remember the ; at the end
2. **Math Operation**
   x = 5 + a; #no particular rules just remember the ; at the end and make sure that the other variable exists
3. **Differential Equation**
   x' = 5\*x + a - 0.02; #the derivative is written with the ' after the variable and with no brackets such as "x'(t)", always remember the ; at the end

#### Guide for Transitions

Delimit with parenthesis and if you need to use the and/or operator you can write conditions like this:

```bash
(x<19) & (x+5>=4) #and operator
(x<19) | (x+5>=4) #or operator
```

Some more examples:

```bash
((x<19) & (x+5>=4)) & (x+x<6)
(((x<19) | (x+5>=4)) & (2*x>=5)) | (x+1==0)
```

## Stack

Written in C++

## Authors

Aldegheri Alessandro, Cipriani Michele, Venturi Davide, Vilotto Tommaso
