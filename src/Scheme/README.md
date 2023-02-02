# Explaination of the design pattern

The sets of classes in this directory allow the code of TeXMacs to interact with scheme whatever the scheme implementation is.


For that, we use the design pattern "Bridge" (see [Wikipedia](https://en.wikipedia.org/wiki/Bridge_pattern)).
The idea is to have an abstract class Scheme which is the bridge between the code of TeXMacs and the scheme implementation. 

This allows : 
 - to have a single interface to interact with scheme, and to be able to change the scheme implementation without changing the code of TeXMacs.
 - to change the scheme implementation without recompiling TeXMacs.
