C-Troll is a suite of applications that make it possible to controls applications in a distributed computing environment, for example a planetarium dome.  It provides the ability for a central _Control_ computer to control a number of _Node_ and start configurable programs on these clusters.

The suite consists of three applications:
 1. The _Tray_ application is running on each Node on which you would want to be able to start applications.  The _Tray_ is opening a port and receives commands from the _C-Troll_ application via Json-formatted messages.
 2. The _C-Troll_ application is running on a central _Control_ computer and controls an arbitrary number of _Tray_ applications.  From this user interface, the user selects the applications and configurations that they want to start or stop on the _Nodes_. 
 3. The _Editor_ application allows the quick editing and creation of JSON configuration files for the _C-Troll_ application.  These configuration files define which _Nodes_ are available, which _Nodes_ are combined to form _Clusters_, and which _Programs_ can be executed on which _Cluster_.

 ![Tray](/images/tray.png?raw=true "The Tray application)

 ![C-Troll](/images/c-troll.png?raw=true "The main C-Troll application)

 ![Editor](/images/editor.png?raw=true "The Editor application)
