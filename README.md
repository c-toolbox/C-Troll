C-Troll is a suite of applications that make it possible to controls applications in a distributed computing environment, for example a planetarium dome.  It provides the ability for a central _Control_ computer to control a number of _Node_ and start configurable programs on these clusters.

The suite consists of three applications. The _Tray_, _C-Troll_, and the _Editor_.

# Applications
## Tray
![Tray](/images/tray.png?raw=true "The Tray application")
The _Tray_ application is running on each Node on which you would want to be able to start applications.  The _Tray_ is opening a port and receives commands from the _C-Troll_ application via Json-formatted messages.

## C-Troll
![C-Troll](/images/c-troll.png?raw=true "The main C-Troll application")
The _C-Troll_ application is running on a central _Control_ computer and controls an arbitrary number of _Tray_ applications.  From this user interface, the user selects the 
applications and configurations that they want to start or stop on the _Nodes_. 

## Editor
![Editor](/images/editor.png?raw=true "The Editor application")
The _Editor_ application allows the quick editing and creation of JSON configuration files for the _C-Troll_ application.  These configuration files define which _Nodes_ are available, which _Nodes_ are combined to form _Clusters_, and which _Programs_ can be executed on which _Cluster_.

# Getting started
The `example` folder contains a  full working example including nodes, clusters, and programs.  If you want to create your own setup, the recommended way is to start with configuration the nodes, then grouping them into clusters, and then define programs to execute on the clusters.  The [Wiki](https://github.com/c-toolbox-C-Troll/wiki) contains additional information about allowed parameters for the various configuration and JSON files.

Configuration a node:
 1. Copy the _Tray_ application onto the computer that should be able to start applications
 1. (*optional*) Setup the operating system to start the _Tray_ automatically at startup
 1. Create a JSON configuration file on the machine that will run _C-Troll_ which contains at least the `name` and the `ipAddress` 
 1. Customize the configuration of the _Tray_ by editing the `config.json` file that gets created the first time the _Tray_ application runs.  Each change in the configuration file must be reflected in the JSON file on the _C-Troll_ machine
 
Create cluster:
 1. Create a JSON configuration file on the _C-Troll_ machine that contains at leats a `name` and the `nodes` attributes. `nodes` is a list of node names as specified in their respective JSON files
 
Assign programs:
 1. Create a JSON configuration file on the _C-Troll_ machine.  The `name` is a user-facing name of the application, the `executable` is the path to the file that gets executed on the _Nodes_.  The `clusters` is an array of cluster names as specified in their respective configurations.
