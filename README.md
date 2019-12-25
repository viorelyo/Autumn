# Autumn
Autumn 🍂 is a Dependency Injection framework written in C++.
The project contains more modules examples:  
1. Minimal Taskmanager for monitoring `start/stop` of specific processes (implemented using WMI)
2. Client/Server using NamedPipe modules for math operations

## Features
* Reads configuration `.xml` files  
* Autumn uses configuration files to load modules by their name at runtime.
* Modules are stored in form of `.dll`

## Built with
* C++ 17
* WMI (Windows Management Instrumentation)
* Microsoft Visual Studio

## Usage
* **Autumn Framework**
1. Write names of the modules you want to use in an `.xml` configuration file
2. Use interfaces to instantiate modules by their names  
  
* **Client/Server module**  
1. Client module waits for user's input in form of:
> Operation: <op1 +/* op2>
2. Sends request to Server using NamedPipe
3. Server responds the result of the operation  
  
* **Taskmanager module**  
1. Module waits for user input. User can introduce name of multiple processes he wants to monitor.
2. Module subscribes to Creating/Stoping of specified processes when user's command is: `start`
3. Module notifies when one process from the set `starts/stops`