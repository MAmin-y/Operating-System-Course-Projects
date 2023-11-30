# Operating-System-Course-Projects
Operating System course projects/ University of Tehran/ Fall 2023-2024

## CA1: Socket Programming
A dining system implemented in C using the POSIX sockets API. Recipes and ingredients are shown in 'recipes.json'.
There can be many processes, and each takes a port to listen for UDP broadcasts of product changes:
`print(hello)`
There are three roles: Customer, Restaurant owner, and Supplier. A customer orders food from the restaurant, which its port is indicated after a suitable command. The restaurant checks whether it has enough ingredients or not. If it doesn't, it rejects the customer's order, but if it has, it can accept the order 90 seconds after receiving the order, and the customer is also blocked until the restaurant answers its order or their order becomes expired. Also, a restaurant can order ingredients from a supplier with the same mechanism.
Internally, messaging is implemented using TCP sockets. Answers to orders are sent using unique TCP ports.
