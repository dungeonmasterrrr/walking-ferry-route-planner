# Walking and Ferry Route Planner

## Project Overview

This project is a C-based route planning program that finds the earliest arrival path between landmarks.

The program supports two types of transport connections:

- Walking links
- Ferry schedules

Users can enter a starting landmark, destination landmark, and departure time. The program then searches for a valid journey plan and prints the step-by-step route, including walking and ferry segments with departure and arrival times.

## Key Features

- Implemented in C.
- Supports multiple landmarks.
- Supports bidirectional walking links.
- Supports directed ferry schedules with fixed departure and arrival times.
- Accepts repeated user queries until the user enters `done`.
- Finds the earliest arrival route using a Dijkstra-like algorithm.
- Prints a detailed travel plan with transport type, duration, time, and location.
- Handles cases where no valid route is available.

## Technologies Used

- C
- Graph data structures
- Linked lists
- Dijkstra-like shortest path search
- Dynamic memory allocation
- Time conversion and formatting

## Project Structure

```text
walking-ferry-route-planner/
│
├── planz.c
└── README.md
```

## How It Works

The program first reads the number of landmarks and their names.

It then reads:

1. Walking links  
   Each walking link connects two landmarks and has a travel time in minutes. Walking links are treated as bidirectional.

2. Ferry schedules  
   Each ferry schedule has a departure landmark, departure time, arrival landmark, and arrival time. Ferry routes are treated as directed and can only be used if the user reaches the departure landmark before or at the ferry departure time.

For each query, the program calculates the earliest possible arrival time from the starting landmark to the destination landmark.

## Algorithm

The route search is based on a Dijkstra-like algorithm.

Each landmark is treated as a node in a graph.

The program stores:

- The earliest known arrival time for each landmark
- The previous landmark used to reach each node
- The transport type used for each step
- Departure and arrival times for each segment

Walking edges are relaxed by adding walking duration to the current time.

Ferry edges are only valid if the current time is earlier than or equal to the ferry departure time.

## Time Format

Times are entered and printed using `HHMM` format.

Example:

```text
0930
1545
```

This means:

```text
0930 = 9:30 AM
1545 = 3:45 PM
```

## Example Usage

Compile the program:

```bash
gcc -Wall -Werror -o planz planz.c
```

Run the program:

```bash
./planz
```

Example interaction:

```text
Number of landmarks: 3
A
B
C
Number of walking links: 1
A B 10
Number of ferry schedules: 1
B 0930 C 1000

From: A
To: C
Time of departure: 0915
```

Example output:

```text
Walk 10 minute(s):
  0915 A
  0925 B
Ferry 30 minute(s):
  0930 B
  1000 C
```

If no route is available, the program prints:

```text
No planz available.
```

To exit the program:

```text
From: done
Enjoy your planz!
```

## Complexity

Let:

```text
L = number of landmarks
W = number of walking links
F = number of ferry schedules
```

Building the graph takes:

```text
O(L + W + F)
```

Each route query uses a simple Dijkstra-like search over the landmarks:

```text
O(L^2 + W + F)
```

This is suitable for small to medium-sized input sizes.

## Limitations

- The program assumes that input landmark names are valid.
- The search does not support trips that roll over to the next day.
- The Dijkstra implementation uses a simple `O(L^2)` scan instead of a priority queue.
- Ferry schedules are fixed and directed.
- Walking links are always bidirectional.

## Notes

This project was created for learning and portfolio purposes. It demonstrates graph modelling, route planning, time-based path search, and C programming with dynamic memory management.
