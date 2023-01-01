# C-Troll

![Build Status](http://dev.openspaceproject.com/buildStatus/icon?job=C-Toolbox%2FC-Troll%2Fmaster&style=flat-square)
[![GitHub Issues](https://img.shields.io/github/issues/C-Toolbox/C-Troll.svg)](https://github.com/C-Toolbox/C-Troll/issues)
[![Average time to resolve an issue](http://isitmaintained.com/badge/resolution/c-toolbox/c-troll.svg)](http://isitmaintained.com/project/c-toolbox/c-troll "Average time to resolve an issue")
[![GitHub Releases](https://img.shields.io/github/release/C-Toolbox/C-Troll.svg)](https://github.com/C-Toolbox/C-Troll/releases)
[![GitHub Downloads](https://img.shields.io/github/downloads/C-Toolbox/C-Troll/total)](https://github.com/C-Toolbox/C-Troll/releases)

C-Troll is a suite of Windows-only applications that make it possible to controls applications in a distributed computing environment, for example a planetarium dome.  It provides the ability for a central _Control_ computer to control a number of _Nodes_ and start configurable programs on these clusters.

The suite consists of four applications. The _Tray_, _C-Troll_, the _Editor_, and the _Starter_.

# Applications
## Tray
![Tray](/images/tray.png?raw=true "The Tray application")

The _Tray_ application is running on each Node on which you would want to be able to start applications.  The _Tray_ is opening a port and receives commands from the _C-Troll_ application via JSON-formatted messages.

## C-Troll
![C-Troll](/images/c-troll.png?raw=true "The main C-Troll application")

The _C-Troll_ application is running on a central _Control_ computer and controls an arbitrary number of _Tray_ applications.  From this user interface, the user selects the
applications and configurations that they want to start or stop on the _Nodes_.  _C-Troll_ acts as a central hub to communicate with the clusters that it connects to and can also optionally receive REST messages through an open port that report back information about the status of the cluster, start programs, stop programs, etc.

## Editor
![Editor](/images/editor.png?raw=true "The Editor application")

The _Editor_ application allows the quick editing and creation of JSON configuration files for the _C-Troll_ application.  These configuration files define which _Nodes_ are available, which _Nodes_ are combined to form _Clusters_, and which _Programs_ can be executed on which _Cluster_.

## Starter
The _Starter_ application is a simple commandline tool that will send a REST message to a C-Troll instance to start a specific program.  The program and its configuration are provided using commandline arguments.  This application is specifically useful to generate easy-to-use desktop shortcuts that cause an application startup on the cluster


# Getting started
The `example` folder contains a full working example including nodes, clusters, and programs.  If you want to create your own setup, the recommended way is to start with configuration the nodes, then grouping them into clusters, and then define programs to execute on the clusters.  The [Wiki](https://github.com/c-toolbox-C-Troll/wiki) contains additional information about allowed parameters for the various configuration and JSON files.  The _Editor_ application contains all tools necessary to create these configurations without manually editing the JSON configuration files.

1. On the `client` computers that should be able to start programs
  1. Install the _Tray_ application
  1. (*optional*) Configure the operating system to start the _Tray_ automatically at startup
1. On the `master` computer that is controlling the cluster
  1. Using the _Editor_ create "Node" configuration files for each of the clients that have the _Tray_ installed
  1. Using the _Editor_ create a "Cluster" configuration file
  1. Using the _Editor_ create as many "Program" configuration files as needed
