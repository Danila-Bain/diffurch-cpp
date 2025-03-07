# Plotting libraries in C++

There are several ways to plot data in C++. To name a few:
- matplotplusplus
    - native C++ library
    - api similar to matplotlib 
    - has convenience functions like linspace or logspace
    - for logarithmic plots, has precision issue due to internal float-to-string conversion
- matplotlib-cpp
    - c++ wrapper for python's matplotlib
    - last commit on github Apr 23, 2021
- gnuplot-iostream
    - defines iostream to send gnuplot code as strings from c++.
- implot
    - doesn't support exporting plots in vector format
    - not supposed for academic purposes


# Transfering data between C++ and Python

Both data computation and data visualization can take a lot of computational time. Because of that, it is desirable to separate those  processes, such that data visualization prosedure can be adjusted without recomputation of the data, or multiple visualizations can be made for the same set of data. C++ were chosen as the language to implement data computation in, because its template mechanisms allow making vastly generic code, while its compiling nature makes it possible to optimize away the abstractions to produce an efficient code. But for data plotting, C++ is not a popular choice, and REPL notebook environment provides much more convenient workflow for data visualization. Because of that, here we focus on the transfer of the data from C++ to a Python notebook or a Python interactive shell, althogh in place of Python there can be anything, including another C++ instance, that is focused on the data visualization.

The propossed ways to transfer C++ data to Python
- Write a binary file in C++, then read it in Python.
    - requires implementing some file type conventions and implement read-write 
      functions both in C++ and Python that adhere to that file type
    - requires storage i/o which can be slow
- Write a JSON or CSV file in C++, then read it in Python.
    - doesn't require custom read-write implementation, and it is very portable
    - requires storage i/o which can be slow
    - JSON and CSV memory wastefull as any human-readable format, I initially thought that it may become a bottle-neck for large amount of data-points; for example, the data that represents 1000x1000 image of doubles would take 10^6*8 bytes = 8 megabytes in bynary, but as a string, assuming 15 decimal digits of precision, it would take about twice the size to save this in human-readable format, which is not as bad, but it reduces biggest possible data set in half.
- Use something like pybind to compile c++ executable, callable from Python, such that in Python, we directly call C++ function that returns a tuple of numpy array or something
    - not portable to other languages
    - no extra copying of data (for file-writing solutions, three versions of the data are created)
    - requires some knowledge about pybind syntax
    - data exists only while Python lives
    - data can only be reused within REPL or interactive mode
- Somehow pipe C++ and Python the information such that C++ runs, outputs to some buffer, from which Python can read or something.



File writing vs direct passing
- File writing
 - Pros:
   - Once the data is computed: it stays as long as needed, even if interactive session is terminated. (In case you would like to adjust the plotting)
   - There is practically no limit in size, I guess. The programm can output to file more data than can fit in RAM, and for visualizing purposes, data can be read and aggregated in parts.
   - Conceptually is easy.
 - Cons:
   - Data is copied multiple times: from C++ to drive, and from drive to Python.
   - Files need to be managed, maybe occasionally deleted
   - There is a need to filename matching between c++ and python.
 - Neutral:
   - Human-readable formats can take more storage space, but it may not be relevant with respect of RAM usage.
- Direct passing of data
 - Pros:
   - Speed of transfer. Ideally no extra copies occur. But the effect might be neglegible because this is one-time operation, and overall computation of data and plotting usually takes significantly more time.
   - No files created. Clean storage.
   - No need to manage filenames, such that C++ outputs a file that python expects
   - Data computation parameters (ranges, precision, etc) can be controlled from the same environment as the plotting.
 - Cons:
   - Data is not stored, so can be used ones, or reused only during notebook kernel runtime. May require unnecessary recalculation.
   - Amount of data is limited by RAM. It is often a bottle-neck if large amount of data is produced.
   - The setup is most probably more complex.

So, the easiest and most flexible approach is to use files to transfer the data.

# Passing parameters from Python to C++

The first arpproach to use together C++ for data and Python for visualization, is to write a c++ code, that produces a file with that data. In that case, all parameters are specified in c++ source code, as well as the filename. Then in Python, filename has to be retyped. This approach is not really convenient, because involves working with multiple files, requires manual matching of file type, which is not the thing one want to pay attention to. Additionaly, parameters used in c++ are often used in the visalization, and even if not, it is useful to know which exact parameters were used for a plot, because such things tend to get forgotten.

So, there is a need to pass parameters to python with the data, which can be done by creating a json file with parameters alongside the file with the data. But, I can imagine, it may be bothersome to pack all the parameters into json from c++ (which can be helped with some variadic macros), and the burden of filename matching persists.

If we want to specify the filename only once, then it can only be in python. Additionaly, we can specify parameters in python, and pass them to c++, preferably through json file (not json string passed as a parameter), so the parameters are saved alongside the data (but not inside data, and not in filename, because filenames have length limitations). This approach is quite convenient, because it allows to write a c++ code once and then reuse it with different parameters, tinkering with the computation parameters in the same place as the visualization parameters. But this approach has an issue. If c++ code is rarely touched, then there is a templation to optimize the process, by 1) calling c++ from python, 2) compiling c++ from python, 3) pass not only runtime parameters, but also a compile time parameters via `-D` flags, by which we also can switch the equation studied completely, reusing the code for, for example, returning the time series of a solution. So in the end, the next logical step is to write a python only interface, that would provide a functions and classes, that accept right-hand-side in the string form, events, integration scheme, etc, then produces and compiles relevant c++ code, which can be then called with different parameters. And this interface would automatically manage filenames, existing parameters, etc.

In the design of such interface, there is a choice to make. Keep this interface similar to c++ interface (in regard of equation definition), or abstract it completely, such that all relevant information is taken in the form of strings.

Example, with the interface, close to a cpp:
```python
import diffurch

t = diffurch.state.time_variable
x, Dx = diffurch.state.variable[0:2]
k = diffurch.state.parameter()

eq = diffurch.equation((Dx, -k*k*x))

# output
x, Dx = eq(ic=(0,1), interval=(0,10), stepsize=diffurch.constant_stepsize(0.1), 
           events=diffurch.events(diffurch.step_event(save="all")))
```

More casual style:
```python
import diffurch

eq = diffurch.equation("x' = Dx; Dx' = -k*k*x")

# output call is the same
```

Parsing strings can be a scetchy process, but 

One cool thing that is available for string parsing is defining high order equations with `x''' = x - x' + x''`, which is not really possible for direct approach, because string parsing can hide the automatic definition of `x'` and `x''` as state variables.


