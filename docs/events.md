# Events        

## Introduction

```Event``` is a template class that is used to perform any kinds of operations during numerical integration, including saving equation state or changing the state. The constructor has the following signature:

```
template <typename DetectHandler, typename SaveHandler, typename SetHandler>
Event(DetectHandler = nullptr, SaveHandler = nullptr, SetHandler = nullptr);
```

The ```DetectHandler``` argument is used to define the event function, that decides when event is triggered. For example, if we want the event to trigger, when the x variable crosses zero, we can pass ```x == 0``` as a detect handler, where ```x``` is ```State::Variable<0>()```. There are special events, that do not use detection handler and are triggered in specific times during integration. (Link?) They include ```StepEvent```, ```RejectEvent```, ```CallEvent```, ```StartEvent```, and ```StopEvent```.

The ```SaveHandler``` specifies the values that have to be saved when event is triggered. In particular, it can be used in conjunction with ```StepEvent``` to save the time series of the solution. It can be used to save zero crossing times of the solution with something like ```Event(x == 0, t)```, which can be read like "when x equals zero, save t", where ```x``` is ```State::Variable<0>()``` and ```t``` is ```State::TimeVariable()```.

The ```SetHandler``` is used either to modify the state of the equation or some external things like counters. For example, it can be used to implement the bouncing ball behavior for equation "x'' = - g" by adding ```Event(x==0, nullptr, dx << -0.9 * dx)```, where ```dx``` is ```State::Variable<1>()``` and denotes the derivative of x.

Since it is desirable to be able to define multiple events for one equation, and different event types (like ```Event``` with detection or ```StepEvent```) must be differentiated because they are triggered during different parts of integration, the container type ```Events``` is used. Type ```Events``` has the following constructor signature:
```
template <typename... EventTypes>
Events(EventTypes...);
```

## Detect Handler structure

```DetectHandler``` class must implement the following methods:

```
bool detect(const auto &state); 
double locate(const auto &state);
```


## Save Handler structure

There are two alternatives:
- ```SaveHandler``` is a class that implements ```double operator()(const auto &state)``` that returns the value to save, or
- ```SaveHandler``` is a tuple of such classes.

## Set Handler structure

```SetHandler``` must implement either
- `void operator()(void)`, or
- `void operator()(const auto &state)`, or
- `void operator()(auto &state)`.

If multiple defined, only one of them is used, in that order. Behavior differs for `void`, `const auto&`, and `auto&` argument signature. For `void` and `const auto&`, it is guaranteed, that state itself is not modified, hence the set handler is just called when event triggers. The situation is different for `auto&` argument, which indicated that the state is modified. In that case, state is forced to make a zero step, such that previous state equals to the current state, then the current step is modified by the set handler, reading only the previous state data. 

For that reason, it is not yet possible to modify state for a CallEvent, because it is triggered mid-step, usually multiple times, and modifying the state in that context would ruin the whole integration procedure.

## Special event types

There are several special types of events, like ```StepEvent```, for which the constructor signature is

```
template <typename SaveHandler, typename SetHandler>
StepEvent(SaveHandler = nullptr, SetHandler = nullptr);
```

Other event types from the following list have similar constructor signature, and they differ only by the time they are called:
- ```StepEvent``` is called before integration (after ```StartEvent```) and after each succeeded step that were not rejected due to stepsize controller or other event (with detection). At the point of the call, ```state.t_prev``` or ```state.x_prev``` are not yet reset.
- ```RejectEvent``` is called after a step that were rejected by a stepsize controller. In terms of order, the detection for events is processed only for steps that were not rejected by a stepsize controller.
- ```CallEvent``` is called after each call of the right hand side function. Its intended use is for counting such calls or exploring at which values the rhs was avaluated.
- ```StartEvent``` is called before integration starts. Can be used to set up some counters, time-measuring, or initialization of descrete variables. At the point of the call, the state of equation is initialized by the initial condition.
- ```StopEvent``` is called after integration loop terminates.


