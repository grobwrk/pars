.. highlight:: cpp
   :linenothreshold: 1

.. contents:: Table of Contents
    :depth: 3

.. cpp:namespace:: pars

Client
------

In this example we're going to create a pars application by deriving from an :cpp:struct:`app::single` class that allows us to create an application that uses just one component, namely :cpp:struct:`comp::client` in this case.

If you're interested in building the library and examples as well as running the examples, take a look at the :ref:`building <building>` page.

Main
....

The main function is straightforward. We just instantiate :cpp:struct:`::pars_example::apps::client`, and then execute it passing argc and argv. Should any exception arise, we will print the exception message to standard output:

.. literalinclude::
   ../../../example/client.cpp
   :start-at: int main

Application
...........

We start by declaring such application:

.. literalinclude::
   ../../../example/client.cpp
   :start-at: class client
   :end-at: class client

Then we override the only functions that we are required to implement in order to have a runnable single application (ie: the startup function):

.. literalinclude::
   ../../../example/client.cpp
   :start-at: void startup
   :end-at: void startup

Startup
,,,,,,,

.. cpp:namespace:: pars_example

We are going to leverage the :cpp:struct:`pars::app::state_machine` to handle the application state, which can be in one of the cases defined in :cpp:enum:`resource::client_state`.

.. literalinclude::
   ../../../example/client.cpp
   :start-at: app::state_machine
   :lines: 1-2

As a first thing hence, we're going to ask for a transition from the actual expected state to the next state. This will check that the current application state is :cpp:enumerator:`resource::client_state::creating` (and will throws if it's not) and creates a :cpp:class:`pars::app::state_tx` object that initiate the transition on the :cpp:struct:`pars::app::state_machine`.

Once we're done, we're going to commit this state transition in order for the state to change to :cpp:enumerator:`resource::client_state::initializing`. Should any exception be raised after the creation of this :cpp:class:`pars::app::state_tx`, the initiated transition will be rolled back and the app state machine will remain in the initial state:

.. literalinclude::
   ../../../example/client.cpp
   :start-after: // 1
   :end-before: // 2

Next we're going to read the inputs:

.. literalinclude::
   ../../../example/client.cpp
   :start-after: // 2
   :end-before: // 3

Now we register the handler functions (ie: the ones that react to various kinds of events) for the events that we're intrested in:

.. literalinclude::
   ../../../example/client.cpp
   :start-after: // 3
   :end-before: // 4

Finally as anticipated, we just commit the state transition:

.. literalinclude::
   ../../../example/client.cpp
   :start-after: // 4
   :end-before: }

Handler Functions
,,,,,,,,,,,,,,,,,

We're going now to briefly comment all the handler functions that we registered during startup (see previous section).

This application is made out of a client component, which has a single nng_req socket. These handler functions are going to connect to the server, send the request and print the output or any error that could arise.

On ``fired<init>`` we're going to initialize the client component and connect to the specified address using the given mode:

.. literalinclude::
   ../../../example/client.cpp
   :start-at: void initialize
   :end-before: void send_work

On ``fired<pipe_created>`` (ie: connection established), we're going to start a send operation on the req socket:

.. literalinclude::
   ../../../example/client.cpp
   :start-at: void send_work
   :end-before: void recv_answer

On ``sent<event::fib_requested>`` we start a receive operation:

.. literalinclude::
   ../../../example/client.cpp
   :start-at: void recv_answer
   :end-before: void terminate

On ``received<event::fib_computed>`` we terminate the component and print the result:

.. literalinclude::
   ../../../example/client.cpp
   :start-at: void terminate
   :end-before: void terminate_print_ex

On ``fired<exception>`` (ie: an exception was thrown during an execution of some handler function), we just terminate and print the exception message:

.. literalinclude::
   ../../../example/client.cpp
   :start-at: void terminate_print_ex
   :end-before: void terminate_print_err

On ``fired<network_error>`` (ie: we got some error while executing a send or recv operation), we just terminate and print the error message:

.. literalinclude::
   ../../../example/client.cpp
   :start-at: void terminate_print_err
   :end-before: void terminate_print_msg

On ``fired<pipe_removed>`` (ie: connection gone), we just terminate and inform the client was disconnected:

.. literalinclude::
   ../../../example/client.cpp
   :start-at: void terminate_print_msg
   :end-before: };
