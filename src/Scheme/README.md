# Explaination of the design pattern

The sets of classes in this directory allow the code of TeXMacs to interact with scheme whatever the scheme implementation is.

For that, we used the design pattern "Bridge" (see [Wikipedia](https://en.wikipedia.org/wiki/Bridge_pattern)).
The idea is to have an abstract class which represent a bridge between the code of TeXMacs and the scheme implementation. Each scheme implementation will inherit from this abstract bridge, allowing to :
 - have a single interface to interact with scheme, and to be able to change the scheme implementation without changing the code of TeXMacs.
 - change the scheme implementation without recompiling TeXMacs.


Each "bridge" classes have an associated "factory" class (see [Wikipedia](https://en.wikipedia.org/wiki/Factory_(object-oriented_programming))). The factories are registered from CMake.

# Scheme exemple

```cpp 
int main() {
    
    // List all available schemes 
    auto allSchemes = texmacs::get_scheme_factories();
    std::cout << "Available schemes:" << std::endl;
    for (const auto& scheme : allSchemes) {
        std::cout << " - " << scheme.name() << std::endl;
    }
    
    // Create a scheme instance from the first factory
    texmacs::abstract_scheme &scheme = allSchemes[0]->create();
    
    // Or create a scheme from a specific factory
    // texmacs::abstract_scheme &scheme = texmacs::make_scheme("guile");
    
    // Install a procedure
    scheme.install_procedure("my-proc", [](texmacs::abstract_scheme *scheme, texmacs::tmscm args) {
        return scheme->int_to_tmscm(42);
    }, 0);
    
    // Call the procedure
    tmscm result = scheme.eval_scheme("(my-proc)");
    
    // Notice that tmscm can contain any type of scheme object (S7, Guile, ...)
    
    // Convert the result to a C++ type
    int result_int = result.as_int();
    
    
}
```

# Compatibility with the old TeXmacs code

```cpp
int main() {
    
    // to ensure compatibility with the old code, we can use the old scheme interface by choosing which scheme implementation to use
    use_scheme("guile");
    // or use_scheme("s7");
    
    // from here, the old code can be used as usual
    tmscm_install_procedure ("url?", urlP, 1, 0, 0);
}
```
