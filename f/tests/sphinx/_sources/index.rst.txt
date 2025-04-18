Index
=====

.. cpp:namespace:: pars

Welcome to the **pars** C++23 framework documentation.

This framework leverages libnng to create distributed applications by combining one or more reusable components. A component is an aggregate of nng_socket(s). You implement a pars application by combining one or more components.

.. note::
   At the moment, we support only applications that uses a single component, implemented by extending from :cpp:struct:`app::single`. Take a look at the :ref:`milestones <milestones>` page to see coming features.

A pars application is created by defining handler functions for certain kinds of events, and registering them into the :cpp:class:`ev::dispatcher`. The events are defined by you, while the kind of events are provided by pars and are: :cpp:struct:`ev::fired`, :cpp:struct:`ev::sent` and :cpp:struct:`ev::received`.

.. toctree::
   :maxdepth: 2
   :caption: Table of Contents:

   tutorial/index
   examples/index
   library/index
   contribute/index
